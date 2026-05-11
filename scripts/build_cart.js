#!/usr/bin/env node

// Compile all examples to wasm/zip
// Outputs each example's wasm to dist/carts/<name>.(wasm|zip) for the web dev server.

import { execFile as execFileCb } from 'node:child_process'
import { promisify } from 'node:util'
import { readdir, stat, mkdir, unlink, readFile, writeFile } from 'node:fs/promises'
import { join, basename } from 'node:path'
import { fileURLToPath } from 'node:url'
import { zipSync } from 'fflate'

const execFile = promisify(execFileCb)

const ROOT = join(fileURLToPath(import.meta.url), '../..')
const WASI = process.env.WASI_SDK ?? '/opt/wasi-sdk'
const CLANG = join(WASI, 'bin/clang')
const INCLUDE = join(ROOT, 'src')
const OUTDIR = join(ROOT, 'web/carts')
const CFLAGS = [`-I${INCLUDE}`, '-O3', '-lm']
const LDFLAGS = [
  '-Wl,--export=CartPreload',
  '-Wl,--export=CartInit',
  '-Wl,--export=CartUpdate',
  '-Wl,--export=malloc',
  '-Wl,--export=free',
  '-Wl,--allow-undefined',
  '-Wl,--import-memory', // share emscripten's wasm memory — no cross-heap copies
  '-Wl,--global-base=33554432', // cart starts at 32MB, past emscripten's initial heap
  '-Wl,--export=memory' // re-export imported memory so WASI shim can see it
]

async function getWasmInfo(filePath) {
  const buffer = await readFile(filePath)
  let pos = 8 // Skip preamble (0-3) and version (4-7)

  let types = []
  let funcTypeIndices = []
  let exports = []
  let imports = []

  while (pos < buffer.length) {
    const sectionId = buffer[pos++]
    const sectionLen = readVarUint32()
    const sectionEnd = pos + sectionLen

    if (sectionId === 1) {
      // TYPE SECTION
      const numTypes = readVarUint32()
      for (let i = 0; i < numTypes; i++) {
        pos++ // Skip 0x60 (func type indicator)
        const params = Array.from({ length: readVarUint32() }, () => getTypeName(buffer[pos++]))
        const results = Array.from({ length: readVarUint32() }, () => getTypeName(buffer[pos++]))
        types.push({ params, results })
      }
    } else if (sectionId === 2) {
      // IMPORT SECTION
      const numImports = readVarUint32()
      for (let i = 0; i < numImports; i++) {
        const mod = readString()
        const field = readString()
        const kind = buffer[pos++]
        if (kind === 0) {
          // function — type index
          const typeIdx = readVarUint32()
          imports.push({ module: mod, field, typeIdx })
          funcTypeIndices.push(typeIdx)
        } else if (kind === 1) {
          // table — reftype (1 byte) + limits
          pos++ // reftype
          const flags = readVarUint32()
          readVarUint32() // initial
          if (flags & 1) readVarUint32() // max
        } else if (kind === 2) {
          // memory — limits
          const flags = readVarUint32()
          readVarUint32() // initial
          if (flags & 1) readVarUint32() // max
        } else if (kind === 3) {
          // global — valtype (1 byte) + mutability (1 byte)
          pos += 2
        }
      }
    } else if (sectionId === 3) {
      // FUNCTION SECTION
      const numFuncs = readVarUint32()
      for (let i = 0; i < numFuncs; i++) {
        funcTypeIndices.push(readVarUint32())
      }
    } else if (sectionId === 7) {
      // EXPORT SECTION
      const numExports = readVarUint32()
      for (let i = 0; i < numExports; i++) {
        const name = readString()
        const kind = buffer[pos++]
        const index = readVarUint32()
        if (kind === 0) exports.push({ name, funcIdx: index }) // functions only
      }
    }
    pos = sectionEnd
  }

  // Map everything together
  return {
    imports: imports.map((i) => ({ ...i, signature: types[i.typeIdx] })),
    exports: exports.map((e) => ({ ...e, signature: types[funcTypeIndices[e.funcIdx]] }))
  }

  // --- Helpers for LEB128 parsing ---
  function readVarUint32() {
    let res = 0,
      shift = 0,
      byte
    do {
      byte = buffer[pos++]
      res |= (byte & 0x7f) << shift
      shift += 7
    } while (byte & 0x80)
    return res
  }
  function readString() {
    const len = readVarUint32()
    const str = buffer.toString('utf8', pos, pos + len)
    pos += len
    return str
  }
  function getTypeName(b) {
    return { 0x7f: 'i32', 0x7e: 'i64', 0x7d: 'f32', 0x7c: 'f64' }[b] || 'unknown'
  }
}

async function buildCart(dir) {
  const name = basename(dir)
  console.log(`Building cart: ${name}`)
  const s = await stat(dir)
  if (!s.isDirectory()) {
    throw new Error(`${dir} does not exist.`)
  }
  const files = await readdir(dir)
  const assets = files.filter((f) => !f.endsWith('.c') && !f.endsWith('.h') && !f.endsWith('.nelua'))

  const outWasm = join(OUTDIR, `${name}.wasm`)
  const outZip = join(OUTDIR, `${name}.zip`)

  let zipFiles

  if (assets.length) {
    zipFiles = {}
    for (const f of assets) {
      zipFiles[f] = new Uint8Array(await readFile(join(dir, f)))
    }
  }

  // TODO: add other detection-types, like main.js or main.nelua
  if (files.includes('main.c')) {
    const src = join(dir, 'main.c')
    console.log(`  ${src} -> ${outWasm}`)
    await execFile(CLANG, [...CFLAGS, src, '-o', outWasm, ...LDFLAGS], { stdio: 'inherit' })
  }

  const info = await getWasmInfo(outWasm)
  console.log('  Imports')
  for (const f of Object.values(info.imports)) {
    console.log(`    ${f.module}.${f.field}(${f.signature.params.join(', ')}) => ${f.signature.results[0] || 'null'}`)
  }
  console.log('  Exports')
  for (const f of Object.values(info.exports)) {
    console.log(`    ${f.name}(${f.signature.params.join(', ')}) => ${f.signature.results[0] || 'null'}`)
  }

  if (zipFiles) {
    try {
      const s = await stat(outWasm)
      zipFiles['main.wasm'] = new Uint8Array(await readFile(outWasm))
      await unlink(outWasm)
    } catch {}

    await writeFile(outZip, zipSync(zipFiles))
  }
}

await mkdir(OUTDIR, { recursive: true })
const [, , ...cartdirs] = process.argv
for (const dir of cartdirs) {
  await buildCart(dir)
}
