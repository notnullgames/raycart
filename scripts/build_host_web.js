#!/usr/bin/env node

import { execFileSync, execSync } from 'node:child_process'
import { writeFileSync, mkdirSync } from 'node:fs'
import { readFile, stat, mkdir } from 'node:fs/promises'
import { join, dirname } from 'node:path'
import { fileURLToPath } from 'node:url'
import { unzipSync } from 'fflate'

const ROOT = join(fileURLToPath(import.meta.url), '../..')
const RAYLIB_DIR = join(ROOT, 'raylib-6.0_webassembly')
const RAYLIB_URL = 'https://github.com/raysan5/raylib/releases/download/6.0/raylib-6.0_webassembly.zip'

const api = JSON.parse(await readFile(join(ROOT, 'raylib_api.json'), 'utf8'))

async function ensureRaylib() {
  try {
    await stat(RAYLIB_DIR)
  } catch {
    console.log('Downloading raylib-6.0_webassembly...')
    const res = await fetch(RAYLIB_URL)
    if (!res.ok) throw new Error(`Download failed: ${res.status} ${res.statusText}`)
    const buf = await res.arrayBuffer()
    const files = unzipSync(new Uint8Array(buf))
    for (const [name, data] of Object.entries(files)) {
      const dest = join(ROOT, name)
      mkdirSync(dirname(dest), { recursive: true })
      writeFileSync(dest, data)
    }
    console.log('Extracted raylib-6.0_webassembly')
  }
}

function genExports() {
  const libPath = join(ROOT, 'raylib-6.0_webassembly/lib/libraylib.web.a')
  const libSymbols = new Set(
    execSync(`nm "${libPath}"`)
      .toString()
      .split('\n')
      .filter((l) => l.includes(' T '))
      .map((l) => l.replace(/.*T /, '').trim())
  )
  const EXCLUDE = new Set(['TraceLog', 'SetTraceLogCallback', 'SetLoadFileDataCallback', 'SetSaveFileDataCallback', 'SetLoadFileTextCallback', 'SetSaveFileTextCallback', 'SetAudioStreamCallback', 'AttachAudioStreamProcessor', 'DetachAudioStreamProcessor', 'AttachAudioMixedProcessor', 'DetachAudioMixedProcessor'])
  const names = api.functions.filter((f) => !EXCLUDE.has(f.name) && libSymbols.has(f.name)).map((f) => `_${f.name}`)
  const skipped = api.functions.filter((f) => !EXCLUDE.has(f.name) && !libSymbols.has(f.name)).map((f) => f.name)
  if (skipped.length) console.warn(`Skipped (not in lib): ${skipped.join(', ')}`)
  console.log(`Generated ${names.length} exported symbols`)
  return names
}

async function buildHost(exportedFns) {
  const src = await readFile(join(ROOT, 'host_web.c'), 'utf8')
  const firstLine = src.split('\n')[0]
  if (!firstLine.startsWith('// emcc ')) throw new Error('host_web.c line 1 must be the emcc command comment')
  const [, ...args] = firstLine.replace(/^\/\/\s*/, '').split(/\s+/)

  // Replace the @file placeholder with the inline generated list
  const idx = args.findIndex((a) => a.startsWith('-sEXPORTED_FUNCTIONS='))
  if (idx !== -1) args[idx] = `-sEXPORTED_FUNCTIONS=${exportedFns.join(',')}`

  await mkdir(join(ROOT, 'host/web'), { recursive: true })
  console.log('Building host_web.c...')
  execFileSync('emcc', args, { cwd: ROOT, stdio: 'inherit' })
  console.log('Built host/web/raylib_em.mjs')
}

await ensureRaylib()
const exportedFns = genExports()
await buildHost(exportedFns)
