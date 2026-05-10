import createRaylib from './raylib_em.mjs'
import WasiPreview1 from '@easywasm/wasi'
import JSZip from 'jszip'

const status = document.getElementById('status')
const canvas = document.getElementById('canvas')

// Init emscripten raylib — creates the shared wasm memory (32MB initial, growable)
status.textContent = 'loading raylib…'
const rl = await createRaylib({ canvas })

// Shared memory: cart imports this so pointers are valid across both wasms, no copies
const sharedMemory = rl.wasmMemory

const dec = new TextDecoder()
function readStr(ptr) {
  const mem = new Uint8Array(sharedMemory.buffer)
  let end = ptr
  while (mem[end]) end++
  return dec.decode(mem.subarray(ptr, end))
}

// Remap _InitWindow → InitWindow etc. for the cart's "raylib" import namespace
const raylibImports = {
  ...Object.fromEntries(
    Object.entries(rl)
      .filter(([k]) => k.startsWith('_') && typeof rl[k] === 'function')
      .map(([k, v]) => [k.slice(1), v])
  ),
  // Cart formats TraceLog varargs itself and calls this with a plain string ptr
  TraceLog: (level, ptr) => {
    const msg = readStr(ptr)
    ;[console.trace, console.trace, console.debug, console.info, console.warn, console.error, console.error][level]?.(`[raylib] ${msg}`) ?? console.log(`[raylib:${level}] ${msg}`)
  },
}

const wasi = new WasiPreview1()

const cartName = new URLSearchParams(location.search).get('cart') ?? 'basic.wasm'
const wasmUrl = new URL(`./carts/${cartName}`, import.meta.url)

status.textContent = 'fetching cart…'
const cartBytes = new Uint8Array(await fetch(wasmUrl).then((r) => r.arrayBuffer()))
let wasmBytes

let zip
if (wasmUrl.pathname.endsWith('.zip')) {
  zip = await JSZip.loadAsync(cartBytes)
  wasmBytes = await zip.files['main.wasm'].async('uint8array')
} else {
  wasmBytes = cartBytes
}

let exports
try {
  ;({
    instance: { exports }
  } = await WebAssembly.instantiate(wasmBytes, {
    env: { memory: sharedMemory }, // cart imports memory from "env"
    raylib: raylibImports,
    wasi_snapshot_preview1: wasi,
  }))
} catch (e) {
  status.textContent = 'error loading wasm: ' + e.message
  console.error(e)
  throw e
}

if (zip) {
  for (const [filename, fileData] of Object.entries(zip.files)) {
    if (!fileData.dir && filename !== 'main.wasm') {
      const content = await fileData.async('uint8array')
      try { wasi.fs.writeFileSync(filename, content) } catch {}
      // Also populate emscripten's FS so raylib file-loading functions work
      try {
        const parts = filename.split('/')
        for (let i = 1; i < parts.length; i++) {
          const dir = parts.slice(0, i).join('/')
          try { rl.FS.mkdir(dir) } catch {}
        }
        rl.FS.writeFile(filename, content)
      } catch {}
    }
  }
}

try {
  wasi.start(exports)
} catch (e) {
  if (e?.code !== 0) {
    status.textContent = 'wasm error: ' + e.message
    console.error(e)
    throw e
  }
}

status.textContent = ''

const CartPreload = exports.CartPreload ?? null
const CartInit = exports.CartInit ?? null
const CartUpdate = exports.CartUpdate ?? null

if (CartPreload && zip) {
  const files = Object.keys(zip.files)
  for (let i = 0; i < files.length; i++) {
    CartPreload(i / files.length)
  }
}

if (CartInit) CartInit()

if (CartUpdate) {
  function loop() {
    CartUpdate()
    requestAnimationFrame(loop)
  }
  requestAnimationFrame(loop)
}
