import { Raylib } from './raylib.js'
import WasiPreview1 from '@easywasm/wasi'
import JSZip from 'jszip'

const status = document.getElementById('status')
const canvas = document.getElementById('canvas')

canvas.width = 800
canvas.height = 450

const rl = new Raylib({ canvas })
const wasi = new WasiPreview1()

// ?cart=foo.wasm loads host/web/carts/foo.wasm; default is "basic.wasm"
const cartName = new URLSearchParams(location.search).get('cart') ?? 'basic.wasm'
const wasmUrl = new URL(`./carts/${cartName}`, import.meta.url)

let exports
let zip
status.textContent = 'fetching cart…'
const cartBytes = new Uint8Array(await fetch(wasmUrl).then((r) => r.arrayBuffer()))
let wasmBytes

if (wasmUrl.pathname.endsWith('.zip')) {
  zip = await JSZip.loadAsync(cartBytes)
  console.log(zip.files)
  wasmBytes = await zip.files['main.wasm'].async('uint8array')
} else {
  wasmBytes = cartBytes
}

try {
  ;({
    instance: { exports }
  } = await WebAssembly.instantiate(wasmBytes, {
    raylib: rl,
    wasi_snapshot_preview1: wasi
  }))
} catch (e) {
  status.textContent = 'error loading wasm: ' + e.message
  console.error(e)
  throw e
}

rl.start(exports)

// wasi.start() calls _start → main() → proc_exit(0), which most WASI shims
// signal by throwing. Code 0 is a clean exit from main() — not an error.
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

// Lifecycle — all phases are optional except CartUpdate
//
//   main()       — runs via _start; call InitWindow (minimal graphics ready)
//   CartPreload  — called each frame; draw loading screen;
//   CartInit     — called once after preload completes; set up game state
//   CartUpdate   — called every frame; draw game
//

const CartPreload = exports.CartPreload ?? null
const CartInit = exports.CartInit ?? null
const CartUpdate = exports.CartUpdate ?? null

if (CartPreload) {
  const files = Object.keys(zip.files)
  const totalFiles = files.length
  let processedFiles = 0

  for (const filename of files) {
    const fileData = zip.files[filename]
    if (!fileData.dir && filename !== 'main.wasm') {
      const content = await fileData.async('uint8array')
      wasi.fs.writeFileSync(filename, content)
    }
    processedFiles++
    const progress = processedFiles / totalFiles
    CartPreload(progress)
  }
}

if (CartInit) {
  CartInit()
}

if (CartUpdate) {
  function loop(timestamp) {
    rl._tickStart(timestamp)
    CartUpdate()
    requestAnimationFrame(loop)
  }
  requestAnimationFrame(loop)
}
