import WasiPreview1 from '@easywasm/wasi'
import { unzipSync } from 'fflate'

import createRaylib from './raylib_em.mjs'
import createEmFs from './emfs.js'

// Load and run a raycart wasm/zip cart.
// canvas defaults to #canvas, status defaults to #status (accepts element or callback).
export default async function loadRaycart(url, { canvas, status } = {}) {
  canvas ??= document.getElementById('canvas')
  const setStatus =
    typeof status === 'function'
      ? status
      : (s) => {
          if ((status ??= document.getElementById('status'))) status.textContent = s
        }

  setStatus('loading raylib…')
  const rl = await createRaylib({ canvas })
  const sharedMemory = rl.wasmMemory

  const dec = new TextDecoder()
  function readStr(ptr) {
    const mem = new Uint8Array(sharedMemory.buffer)
    let end = ptr
    while (mem[end]) end++
    return dec.decode(mem.subarray(ptr, end))
  }

  const raylibImports = {
    ...Object.fromEntries(
      Object.entries(rl)
        .filter(([k]) => k.startsWith('_') && typeof rl[k] === 'function')
        .map(([k, v]) => [k.slice(1), v])
    ),
    TraceLog: (level, ptr) => {
      const msg = readStr(ptr)
      ;[console.trace, console.trace, console.debug, console.info, console.warn, console.error, console.error][level]?.(`[raylib] ${msg}`) ?? console.log(`[raylib:${level}] ${msg}`)
    }
  }

  const emFs = createEmFs(rl.FS)
  const wasi = new WasiPreview1({ fs: emFs })

  setStatus('fetching cart…')
  const cartBytes = new Uint8Array(await fetch(url).then((r) => r.arrayBuffer()))
  let wasmBytes, zip

  if (String(url).endsWith('.zip')) {
    zip = unzipSync(cartBytes)
    wasmBytes = zip['main.wasm']
  } else {
    wasmBytes = cartBytes
  }

  let exports
  try {
    ;({
      instance: { exports }
    } = await WebAssembly.instantiate(wasmBytes, {
      env: { memory: sharedMemory },
      raylib: raylibImports,
      wasi_snapshot_preview1: wasi
    }))
  } catch (e) {
    setStatus('error loading wasm: ' + e.message)
    throw e
  }

  if (zip) {
    for (const [filename, data] of Object.entries(zip)) {
      if (filename !== 'main.wasm') emFs.writeFileSync(filename, data)
    }
  }

  try {
    wasi.start(exports)
  } catch (e) {
    if (e?.code !== 0) {
      setStatus('wasm error: ' + e.message)
      throw e
    }
  }

  setStatus('')

  const CartPreload = exports.CartPreload ?? null
  const CartInit = exports.CartInit ?? null
  const CartUpdate = exports.CartUpdate ?? null

  if (CartPreload && zip) {
    const files = Object.keys(zip)
    for (let i = 0; i < files.length; i++) CartPreload(i / files.length)
  }

  if (CartInit) CartInit()

  if (CartUpdate) {
    function loop() {
      CartUpdate()
      requestAnimationFrame(loop)
    }
    requestAnimationFrame(loop)
  }

  return { rl, exports, emFs }
}
