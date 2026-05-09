// Web host implementation of the raylib API for WebAssembly modules compiled
// with wasi-sdk.  All raylib functions are imported from module "raylib" in
// the wasm binary, so pass this class's methods as the "raylib" import object:
//
//   const rl = new Raylib({ canvas })
//   const wasi = new WasiPreview1()
//   const { instance } = await WebAssembly.instantiateStreaming(fetch('game.wasm'), {
//     raylib: rl,
//     wasi_snapshot_preview1: wasi,
//   })
//   rl.start(instance.exports)
//   wasi.start(instance.exports)
//
// ABI conventions (matching gen_header.js transforms):
//   - Struct params arrive as i32 wasm memory offsets; read fields directly.
//   - Struct returns use sret: first param is result ptr; write fields into it.
//   - Strings arrive as i32 wasm memory offsets; read null-terminated bytes.

const dec = new TextDecoder()
const enc = new TextEncoder()

// Keyboard: DOM KeyboardEvent.code → raylib KEY_* value (matches GLFW / ASCII)
const KEY_MAP = {
  Space: 32,
  Quote: 39,
  Comma: 44,
  Minus: 45,
  Period: 46,
  Slash: 47,
  Digit0: 48,
  Digit1: 49,
  Digit2: 50,
  Digit3: 51,
  Digit4: 52,
  Digit5: 53,
  Digit6: 54,
  Digit7: 55,
  Digit8: 56,
  Digit9: 57,
  Semicolon: 59,
  Equal: 61,
  KeyA: 65,
  KeyB: 66,
  KeyC: 67,
  KeyD: 68,
  KeyE: 69,
  KeyF: 70,
  KeyG: 71,
  KeyH: 72,
  KeyI: 73,
  KeyJ: 74,
  KeyK: 75,
  KeyL: 76,
  KeyM: 77,
  KeyN: 78,
  KeyO: 79,
  KeyP: 80,
  KeyQ: 81,
  KeyR: 82,
  KeyS: 83,
  KeyT: 84,
  KeyU: 85,
  KeyV: 86,
  KeyW: 87,
  KeyX: 88,
  KeyY: 89,
  KeyZ: 90,
  BracketLeft: 91,
  Backslash: 92,
  BracketRight: 93,
  Backquote: 96,
  Escape: 256,
  Enter: 257,
  Tab: 258,
  Backspace: 259,
  Insert: 260,
  Delete: 261,
  ArrowRight: 262,
  ArrowLeft: 263,
  ArrowDown: 264,
  ArrowUp: 265,
  PageUp: 266,
  PageDown: 267,
  Home: 268,
  End: 269,
  F1: 290,
  F2: 291,
  F3: 292,
  F4: 293,
  F5: 294,
  F6: 295,
  F7: 296,
  F8: 297,
  F9: 298,
  F10: 299,
  F11: 300,
  F12: 301,
  ShiftLeft: 340,
  ControlLeft: 341,
  AltLeft: 342,
  ShiftRight: 344,
  ControlRight: 345,
  AltRight: 346
}

// Raylib default font bitmap: 512 uint32s encoding a 128×128 1-bpp atlas
// Source: raylib/src/rtext.c defaultFontData[]
const _FONT_DATA = [
  0x00000000,0x00000000,0x00000000,0x00000000,0x00200020,0x0001b000,0x00000000,0x00000000,
  0x8ef92520,0x00020a00,0x7dbe8000,0x1f7df45f,0x4a2bf2a0,0x0852091e,0x41224000,0x10041450,
  0x2e292020,0x08220812,0x41222000,0x10041450,0x10f92020,0x3efa084c,0x7d22103c,0x107df7de,
  0xe8a12020,0x08220832,0x05220800,0x10450410,0xa4a3f000,0x08520832,0x05220400,0x10450410,
  0xe2f92020,0x0002085e,0x7d3e0281,0x107df41f,0x00200000,0x8001b000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xc0000fbe,0xfbf7e00f,0x5fbf7e7d,0x0050bee8,
  0x440808a2,0x0a142fe8,0x50810285,0x0050a048,0x49e428a2,0x0a142828,0x40810284,0x0048a048,
  0x10020fbe,0x09f7ebaf,0xd89f3e84,0x0047a04f,0x09e48822,0x0a142aa1,0x50810284,0x0048a048,
  0x04082822,0x0a142fa0,0x50810285,0x0050a248,0x00008fbe,0xfbf42021,0x5f817e7d,0x07d09ce8,
  0x00008000,0x00000fe0,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x000c0180,
  0xdfbf4282,0x0bfbf7ef,0x42850505,0x004804bf,0x50a142c6,0x08401428,0x42852505,0x00a808a0,
  0x50a146aa,0x08401428,0x42852505,0x00081090,0x5fa14a92,0x0843f7e8,0x7e792505,0x00082088,
  0x40a15282,0x08420128,0x40852489,0x00084084,0x40a16282,0x0842022a,0x40852451,0x00088082,
  0xc0bf4282,0xf843f42f,0x7e85fc21,0x3e0900bf,0x00000000,0x00000004,0x00000000,0x000c0180,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x04000402,0x41482000,0x00000000,0x00000800,
  0x04000404,0x4100203c,0x00000000,0x00000800,0xf7df7df0,0x514bef85,0xbefbefbe,0x04513bef,
  0x14414500,0x494a2885,0xa28a28aa,0x04510820,0xf44145f0,0x474a289d,0xa28a28aa,0x04510be0,
  0x14414510,0x494a2884,0xa28a28aa,0x02910a00,0xf7df7df0,0xd14a2f85,0xbefbe8aa,0x011f7be0,
  0x00000000,0x00400804,0x20080000,0x00000000,0x00000000,0x00600f84,0x20080000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xac000000,0x00000f01,0x00000000,0x00000000,
  0x24000000,0x00000f01,0x00000000,0x06000000,0x24000000,0x00000f01,0x00000000,0x09108000,
  0x24fa28a2,0x00000f01,0x00000000,0x013e0000,0x2242252a,0x00000f52,0x00000000,0x038a8000,
  0x2422222a,0x00000f29,0x00000000,0x010a8000,0x2412252a,0x00000f01,0x00000000,0x010a8000,
  0x24fbe8be,0x00000f01,0x00000000,0x0ebe8000,0xac020000,0x00000f01,0x00000000,0x00048000,
  0x0003e000,0x00000f00,0x00000000,0x00008000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000038,0x8443b80e,0x00203a03,0x02bea080,0xf0000020,0xc452208a,0x04202b02,
  0xf8029122,0x07f0003b,0xe44b388e,0x02203a02,0x081e8a1c,0x0411e92a,0xf4420be0,0x01248202,
  0xe8140414,0x05d104ba,0xe7c3b880,0x00893a0a,0x283c0e1c,0x04500902,0xc4400080,0x00448002,
  0xe8208422,0x04500002,0x80400000,0x05200002,0x083e8e00,0x04100002,0x804003e0,0x07000042,
  0xf8008400,0x07f00003,0x80400000,0x04000022,0x00000000,0x00000000,0x80400000,0x04000002,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00800702,0x1848a0c2,0x84010000,0x02920921,
  0x01042642,0x00005121,0x42023f7f,0x00291002,0xefc01422,0x7efdfbf7,0xefdfa109,0x03bbbbf7,
  0x28440f12,0x42850a14,0x20408109,0x01111010,0x28440408,0x42850a14,0x2040817f,0x01111010,
  0xefc78204,0x7efdfbf7,0xe7cf8109,0x011111f3,0x2850a932,0x42850a14,0x2040a109,0x01111010,
  0x2850b840,0x42850a14,0xefdfbf79,0x03bbbbf7,0x001fa020,0x00000000,0x00001000,0x00000000,
  0x00002070,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x08022800,0x00012283,0x02430802,0x01010001,0x8404147c,0x20000144,0x80048404,0x00823f08,
  0xdfbf4284,0x7e03f7ef,0x142850a1,0x0000210a,0x50a14684,0x528a1428,0x142850a1,0x03efa17a,
  0x50a14a9e,0x52521428,0x142850a1,0x02081f4a,0x50a15284,0x4a221428,0xf42850a1,0x03efa14b,
  0x50a16284,0x4a521428,0x042850a1,0x0228a17a,0xdfbf427c,0x7e8bf7ef,0xf7efdfbf,0x03efbd0b,
  0x00000000,0x04000000,0x00000000,0x00000008,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00200508,0x00840400,0x11458122,0x00014210,
  0x00514294,0x51420800,0x20a22a94,0x0050a508,0x00200000,0x00000000,0x00050000,0x08000000,
  0xfefbefbe,0xfbefbefb,0xfbeb9114,0x00fbefbe,0x20820820,0x8a28a20a,0x8a289114,0x3e8a28a2,
  0xfefbefbe,0xfbefbe0b,0x8a289114,0x008a28a2,0x228a28a2,0x08208208,0x8a289114,0x088a28a2,
  0xfefbefbe,0xfbefbefb,0xfa2f9114,0x00fbefbe,0x00000000,0x00000040,0x00000000,0x00000000,
  0x00000000,0x00000020,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00210100,0x00000004,0x00000000,0x00000000,0x14508200,0x00001402,0x00000000,0x00000000,
  0x00000010,0x00000020,0x00000000,0x00000000,0xa28a28be,0x00002228,0x00000000,0x00000000,
  0xa28a28aa,0x000022e8,0x00000000,0x00000000,0xa28a28aa,0x000022a8,0x00000000,0x00000000,
  0xa28a28aa,0x000022e8,0x00000000,0x00000000,0xbefbefbe,0x00003e2f,0x00000000,0x00000000,
  0x00000004,0x00002028,0x00000000,0x00000000,0x80000000,0x00003e0f,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
]
// Per-character pixel widths for the 224 default font glyphs (codepoints 32-255)
const _FONT_WIDTHS = [
  3,1,4,6,5,7,6,2,3,3,5,5,2,4,1,7,5,2,5,5,5,5,5,5,5,5,1,1,3,4,3,6,
  7,6,6,6,6,6,6,6,6,3,5,6,5,7,6,6,6,6,6,6,7,6,7,7,6,6,6,2,7,2,3,5,
  2,5,5,5,5,5,4,5,5,1,2,5,2,5,5,5,5,5,5,5,4,5,5,5,5,5,5,3,1,3,4,4,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,5,5,5,7,1,5,3,7,3,5,4,1,7,4,3,5,3,3,2,5,6,1,2,2,3,5,6,6,6,6,
  6,6,6,6,6,6,7,6,6,6,6,6,3,3,3,3,7,6,6,6,6,6,6,5,6,6,6,6,6,6,4,6,
  5,5,5,5,5,5,9,5,5,5,5,5,2,2,3,3,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,5,
]

export class Raylib {
  constructor({ canvas, fs } = {}) {
    this._canvas = canvas || document.createElement('canvas')
    this._ctx = this._canvas.getContext('2d')
    this._memory = null
    this._exports = null

    this._width = 800
    this._height = 450
    this._shouldClose = false
    this._targetFPS = 60

    // Timing
    this._frameStart = performance.now()
    this._frameTime = 0 // seconds, last frame
    this._fps = 0

    // Input state: key → 0 (up) | 1 (pressed this frame) | 2 (held) | 3 (released this frame)
    this._keys = new Map()
    this._keysNew = [] // keys pressed this frame (for GetKeyPressed)
    this._charsNew = [] // char codepoints pressed this frame
    this._mouse = { x: 0, y: 0, dx: 0, dy: 0, buttons: new Map(), scroll: 0, scrollV: { x: 0, y: 0 } }
    this._touch = [] // [{id, x, y}]

    // Texture / image / render-target system
    this._textures = new Map()       // id → OffscreenCanvas | ImageBitmap | null
    this._images = new Map()         // id → { width, height, data: ImageData, format }
    this._renderTextures = new Map() // id → OffscreenCanvas
    this._fileBitmaps = new Map()    // filename → ImageBitmap (pre-loaded from zip)
    this._fileData = new Map()       // filename → Uint8Array
    this._fs = fs || null            // optional WASI-compatible fs for file writes
    this._nextId = 1

    // Font system
    this._fonts = new Map()          // id → { face: FontFace|null, baseSize, family, glyphs }
    this._textLineSpacing = 15
    this._defaultFontAtlas = null
    this._defaultFontRecs = null
    this._buildDefaultFont()

    // Audio system
    this._audioCtx = null
    this._masterGain = null
    this._masterVolume = 1.0
    this._audioReady = false
    this._sounds = new Map()         // id → { buffer: AudioBuffer|null, sources: Set }
    this._music = new Map()          // id → { source, gainNode, looping, offset, startTime, duration }
    this._audioStreams = new Map()   // id → { gainNode, processor }

    // Rendering context stack (for BeginTextureMode)
    this._ctxStack = []

    // Blend mode
    this._blendMode = 0

    // Window flags
    this._configFlags = 0
    this._windowHidden = false
    this._windowOpacity = 1.0

    // bind all methods so they can be spread as the import object
    const proto = Object.getPrototypeOf(this)
    for (const key of Object.getOwnPropertyNames(proto)) {
      if (key === 'constructor') continue
      const desc = Object.getOwnPropertyDescriptor(proto, key)
      if (typeof desc?.value === 'function') this[key] = this[key].bind(this)
    }

    this._attachEvents()
  }

  // Call after wasm instantiation with the exports object
  start(exports) {
    this._exports = exports
    if (exports.memory) this._memory = exports.memory
  }

  // Pre-load assets from a zip (call before wasi.start() so LoadTexture is synchronous)
  async preloadAssets(files) {
    const promises = []
    for (const [name, data] of files) {
      this._fileData.set(name, data)
      const ext = name.split('.').pop().toLowerCase()
      if (['png', 'jpg', 'jpeg', 'gif', 'bmp', 'webp', 'qoi'].includes(ext)) {
        promises.push(
          createImageBitmap(new Blob([data])).then(bmp => this._fileBitmaps.set(name, bmp)).catch(() => {})
        )
      }
    }
    await Promise.all(promises)
  }

  // ── Memory helpers ────────────────────────────────────────────────────────

  get _view() {
    return new DataView(this._memory.buffer)
  }

  _readStr(ptr) {
    if (!ptr) return ''
    const buf = new Uint8Array(this._memory.buffer)
    let end = ptr
    while (buf[end]) end++
    return dec.decode(buf.subarray(ptr, end))
  }

  _writeStr(str) {
    const bytes = enc.encode(str + '\0')
    const ptr = this._exports.malloc(bytes.length)
    new Uint8Array(this._memory.buffer, ptr, bytes.length).set(bytes)
    return ptr
  }

  // Read Color struct: 4 bytes r,g,b,a at ptr
  _readColor(ptr) {
    const b = new Uint8Array(this._memory.buffer, ptr, 4)
    return { r: b[0], g: b[1], b: b[2], a: b[3] }
  }

  // CSS color string from Color ptr
  _css(ptr) {
    const c = this._readColor(ptr)
    return `rgba(${c.r},${c.g},${c.b},${c.a / 255})`
  }

  // Read Vector2 struct: 2 floats at ptr
  _readVec2(ptr) {
    const v = this._view
    return { x: v.getFloat32(ptr, true), y: v.getFloat32(ptr + 4, true) }
  }

  // Write Vector2 into wasm memory at ptr (sret)
  _writeVec2(ptr, x, y) {
    const v = this._view
    v.setFloat32(ptr, x, true)
    v.setFloat32(ptr + 4, y, true)
  }

  _writeVec3(ptr, x, y, z) {
    const v = this._view
    v.setFloat32(ptr, x, true)
    v.setFloat32(ptr + 4, y, true)
    v.setFloat32(ptr + 8, z, true)
  }

  _writeVec4(ptr, x, y, z, w) {
    const v = this._view
    v.setFloat32(ptr, x, true)
    v.setFloat32(ptr + 4, y, true)
    v.setFloat32(ptr + 8, z, true)
    v.setFloat32(ptr + 12, w, true)
  }

  _readVec3(ptr) {
    const v = this._view
    return { x: v.getFloat32(ptr, true), y: v.getFloat32(ptr + 4, true), z: v.getFloat32(ptr + 8, true) }
  }

  _readRect(ptr) {
    const v = this._view
    return { x: v.getFloat32(ptr, true), y: v.getFloat32(ptr + 4, true), width: v.getFloat32(ptr + 8, true), height: v.getFloat32(ptr + 12, true) }
  }

  _writeRect(ptr, x, y, w, h) {
    const v = this._view
    v.setFloat32(ptr, x, true); v.setFloat32(ptr + 4, y, true)
    v.setFloat32(ptr + 8, w, true); v.setFloat32(ptr + 12, h, true)
  }

  _writeColor(ptr, r, g, b, a) {
    const b2 = new Uint8Array(this._memory.buffer, ptr, 4)
    b2[0] = r; b2[1] = g; b2[2] = b; b2[3] = a
  }

  // Texture struct: id(u32), width(i32), height(i32), mipmaps(i32), format(i32) = 20 bytes
  _writeTexture(ptr, id, w, h, mipmaps = 1, format = 7) {
    const v = this._view
    v.setUint32(ptr, id, true)
    v.setInt32(ptr + 4, w, true)
    v.setInt32(ptr + 8, h, true)
    v.setInt32(ptr + 12, mipmaps, true)
    v.setInt32(ptr + 16, format, true)
  }

  _readTexture(ptr) {
    const v = this._view
    return { id: v.getUint32(ptr, true), width: v.getInt32(ptr + 4, true), height: v.getInt32(ptr + 8, true) }
  }

  // Image struct: data(u32), width(i32), height(i32), mipmaps(i32), format(i32) = 20 bytes
  _writeImage(ptr, dataPtr, w, h, mipmaps = 1, format = 7) {
    const v = this._view
    v.setUint32(ptr, dataPtr, true)
    v.setInt32(ptr + 4, w, true)
    v.setInt32(ptr + 8, h, true)
    v.setInt32(ptr + 12, mipmaps, true)
    v.setInt32(ptr + 16, format, true)
  }

  // Camera2D: offset(Vec2), target(Vec2), rotation(f32), zoom(f32) = 24 bytes
  _readCamera2D(ptr) {
    const v = this._view
    return {
      offset: { x: v.getFloat32(ptr, true), y: v.getFloat32(ptr + 4, true) },
      target: { x: v.getFloat32(ptr + 8, true), y: v.getFloat32(ptr + 12, true) },
      rotation: v.getFloat32(ptr + 16, true),
      zoom: v.getFloat32(ptr + 20, true)
    }
  }

  // Allocate a null-terminated string in wasm memory; caller must free
  _mallocStr(str) {
    if (!this._exports?.malloc) return 0
    const bytes = enc.encode(str + '\0')
    const ptr = this._exports.malloc(bytes.length)
    new Uint8Array(this._memory.buffer, ptr, bytes.length).set(bytes)
    return ptr
  }

  // Get or lazily init AudioContext
  get _audio() {
    if (!this._audioCtx) {
      this._audioCtx = new AudioContext()
      this._masterGain = this._audioCtx.createGain()
      this._masterGain.gain.value = this._masterVolume
      this._masterGain.connect(this._audioCtx.destination)
    }
    return this._audioCtx
  }

  // Draw bitmap with optional source rect, dest rect, rotation, origin, tint
  _drawBitmapPro(bmp, sx, sy, sw, sh, dx, dy, dw, dh, ox, oy, rotation, tintPtr) {
    if (!bmp) return
    const ctx = this._ctx
    const tint = tintPtr ? this._readColor(tintPtr) : { r: 255, g: 255, b: 255, a: 255 }
    ctx.save()
    ctx.globalAlpha = tint.a / 255
    ctx.translate(dx, dy)
    if (rotation) ctx.rotate(rotation * Math.PI / 180)
    // Handle flipped source (negative dimensions)
    const flipX = sw < 0, flipY = sh < 0
    if (flipX) { ctx.scale(-1, 1) }
    if (flipY) { ctx.scale(1, -1) }
    ctx.drawImage(bmp, Math.abs(sx), Math.abs(sy), Math.abs(sw), Math.abs(sh),
      -ox * (flipX ? -1 : 1), -oy * (flipY ? -1 : 1), Math.abs(dw), Math.abs(dh))
    ctx.restore()
  }

  // Get the next unique handle ID
  _newId() { return this._nextId++ }

  // Sound struct base: stream.buffer(u32) is our handle ID at offset 0
  _getSoundId(ptr) { return this._view.getUint32(ptr, true) }

  // Write Sound struct (24 bytes): AudioStream(20) + frameCount(4)
  _writeSound(ptr, id, sampleRate = 44100, sampleSize = 16, channels = 2, frameCount = 0) {
    const v = this._view
    v.setUint32(ptr, id, true)       // stream.buffer = id
    v.setUint32(ptr + 4, 0, true)    // stream.processor
    v.setUint32(ptr + 8, sampleRate, true)
    v.setUint32(ptr + 12, sampleSize, true)
    v.setUint32(ptr + 16, channels, true)
    v.setUint32(ptr + 20, frameCount, true)
  }

  // Write Wave struct (20 bytes): frameCount, sampleRate, sampleSize, channels, data
  _writeWave(ptr, id, frameCount = 0, sampleRate = 44100, sampleSize = 16, channels = 2) {
    const v = this._view
    v.setUint32(ptr, frameCount, true)
    v.setUint32(ptr + 4, sampleRate, true)
    v.setUint32(ptr + 8, sampleSize, true)
    v.setUint32(ptr + 12, channels, true)
    v.setUint32(ptr + 16, id, true)  // data ptr = our id
  }

  // Font struct (40 bytes): baseSize(4), glyphCount(4), glyphPadding(4), texture(20), recs(4), glyphs(4)
  _writeFont(ptr, id, baseSize, glyphCount = 95) {
    const v = this._view
    v.setInt32(ptr, baseSize, true)
    v.setInt32(ptr + 4, glyphCount, true)
    v.setInt32(ptr + 8, 0, true)          // glyphPadding
    this._writeTexture(ptr + 12, id, 0, 0) // texture (empty)
    v.setUint32(ptr + 32, 0, true)         // recs ptr
    v.setUint32(ptr + 36, 0, true)         // glyphs ptr
  }

  _getFontId(ptr) { return this._view.getUint32(ptr + 12, true) } // texture.id at offset 12

  // ── Window ────────────────────────────────────────────────────────────────

  InitWindow(width, height, titlePtr) {
    this._width = width
    this._height = height
    this._canvas.width = width
    this._canvas.height = height
    document.title = this._readStr(titlePtr)
  }

  CloseWindow() {
    this._shouldClose = true
  }

  WindowShouldClose() {
    return this._shouldClose ? 1 : 0
  }

  IsWindowReady() {
    return 1
  }
  IsWindowMinimized() { return this._windowMinimized ? 1 : 0 }
  IsWindowMaximized() { return !!document.fullscreenElement ? 1 : 0 }
  IsWindowFocused() { return document.hasFocus() ? 1 : 0 }
  IsWindowResized() { const r = this._windowResized ? 1 : 0; this._windowResized = false; return r }
  IsWindowFullscreen() { return !!document.fullscreenElement ? 1 : 0 }

  GetScreenWidth() {
    return this._canvas.width
  }
  GetScreenHeight() {
    return this._canvas.height
  }
  GetRenderWidth() {
    return (this._canvas.width * (window.devicePixelRatio || 1)) | 0
  }
  GetRenderHeight() {
    return (this._canvas.height * (window.devicePixelRatio || 1)) | 0
  }
  GetMonitorWidth(_monitor) { return screen.width }
  GetMonitorHeight(_monitor) { return screen.height }
  GetMonitorPhysicalWidth(_monitor) { return screen.width }
  GetMonitorPhysicalHeight(_monitor) { return screen.height }
  GetMonitorRefreshRate(_monitor) { return 60 }
  GetMonitorCount() { return 1 }
  GetCurrentMonitor() { return 0 }
  GetMonitorName(resultPtr, _monitor) { return this._mallocStr('Browser') }
  GetMonitorPosition(resultPtr, _monitor) { this._writeVec2(resultPtr, 0, 0) }
  GetWindowPosition(resultPtr) { this._writeVec2(resultPtr, 0, 0) }
  GetWindowScaleDPI(resultPtr) {
    const r = window.devicePixelRatio || 1
    this._writeVec2(resultPtr, r, r)
  }
  GetWindowHandle() { return 0 }

  SetWindowTitle(titlePtr) {
    document.title = this._readStr(titlePtr)
  }
  SetWindowSize(w, h) {
    this._canvas.width = w
    this._canvas.height = h
    this._width = w
    this._height = h
  }
  SetWindowMinSize(w, h) { this._winMinSize = { w, h } }
  SetWindowMaxSize(w, h) { this._winMaxSize = { w, h } }
  SetWindowPosition(x, y) { /* browser windows can't be repositioned */ }
  SetWindowState(flags) { this._configFlags |= flags }
  ClearWindowState(flags) { this._configFlags &= ~flags }
  IsWindowState(flag) { return (this._configFlags & flag) ? 1 : 0 }
  IsWindowHidden() { return this._windowHidden ? 1 : 0 }
  MaximizeWindow() { this._canvas.requestFullscreen?.().catch(() => {}); this._windowMinimized = false }
  MinimizeWindow() { this._windowMinimized = true; this._canvas.style.display = 'none' }
  RestoreWindow() { this._windowMinimized = false; this._canvas.style.display = '' }
  ToggleFullscreen() {
    if (!document.fullscreenElement) this._canvas.requestFullscreen?.().catch(() => {})
    else document.exitFullscreen?.()
  }
  ToggleBorderlessWindowed() { this.ToggleFullscreen() }
  SetWindowFocused() { this._canvas.focus() }
  SetWindowOpacity(alpha) { this._canvas.style.opacity = alpha }
  SetWindowMonitor() {}
  SetWindowIcon(imagePtr) {
    const id = this._view.getUint32(imagePtr, true), img = this._images.get(id)
    if (!img?.data) return
    const oc = new OffscreenCanvas(img.width, img.height)
    oc.getContext('2d').putImageData(img.data, 0, 0)
    oc.convertToBlob({ type: 'image/png' }).then(blob => {
      const link = document.querySelector("link[rel='icon']") || document.createElement('link')
      link.rel = 'icon'; link.href = URL.createObjectURL(blob); document.head.appendChild(link)
    })
  }
  SetWindowIcons(imagesPtr, count) { if (count > 0) this.SetWindowIcon(imagesPtr) }
  SetExitKey(key) { this._exitKey = key }
  SetConfigFlags(flags) { this._configFlags = flags }
  SwapScreenBuffer() {}

  // ── Cursor ────────────────────────────────────────────────────────────────

  ShowCursor() {
    this._canvas.style.cursor = ''
  }
  HideCursor() {
    this._canvas.style.cursor = 'none'
  }
  IsCursorHidden() {
    return this._canvas.style.cursor === 'none' ? 1 : 0
  }
  IsCursorOnScreen() { return this._cursorOnScreen ? 1 : 0 }
  EnableCursor() { this._canvas.style.cursor = ''; document.exitPointerLock?.() }
  DisableCursor() { this._canvas.style.cursor = 'none'; this._canvas.requestPointerLock?.() }
  SetMouseCursor(cursor) {
    const cursors = ['default','default','crosshair','text','pointer','all-scroll','ns-resize','ew-resize','nwse-resize','nesw-resize','nesw-resize','not-allowed']
    this._canvas.style.cursor = cursors[cursor] ?? 'default'
  }

  // ── Timing ────────────────────────────────────────────────────────────────

  SetTargetFPS(fps) {
    this._targetFPS = fps
  }

  GetFPS() {
    return this._fps | 0
  }

  GetFrameTime() {
    return this._frameTime
  }

  GetTime() {
    return performance.now() / 1000
  }

  // Called by the host loop with the requestAnimationFrame timestamp
  _tickStart(rafTimestamp = performance.now()) {
    const dt = (rafTimestamp - this._frameStart) / 1000
    this._frameStart = rafTimestamp
    if (dt > 0) {
      this._frameTime = dt
      this._fps = this._fps * 0.9 + (1 / dt) * 0.1
    }

    for (const [k, s] of this._keys) {
      if (s === 1) this._keys.set(k, 2)
      else if (s === 3) this._keys.delete(k)
    }
    for (const [k, s] of this._mouse.buttons) {
      if (s === 1) this._mouse.buttons.set(k, 2)
      else if (s === 3) this._mouse.buttons.delete(k)
    }
    this._keysNew = []
    this._charsNew = []
    this._mouse.dx = 0; this._mouse.dy = 0
    this._mouse.scroll = 0
  }

  // ── Drawing ───────────────────────────────────────────────────────────────

  BeginDrawing() {}
  EndDrawing() {}

  BeginMode2D(cameraPtr) {
    const cam = this._readCamera2D(cameraPtr)
    const ctx = this._ctx
    ctx.save()
    ctx.translate(cam.offset.x, cam.offset.y)
    ctx.rotate(cam.rotation * Math.PI / 180)
    ctx.scale(cam.zoom, cam.zoom)
    ctx.translate(-cam.target.x, -cam.target.y)
  }
  EndMode2D() { this._ctx.restore() }
  BeginMode3D() {}
  EndMode3D() {}
  BeginTextureMode(renderTexPtr) {
    const id = this._view.getUint32(renderTexPtr, true)
    const oc = this._renderTextures.get(id)
    if (!oc) return
    this._ctxStack.push(this._ctx)
    this._ctx = oc.getContext('2d')
  }
  EndTextureMode() {
    if (this._ctxStack.length) this._ctx = this._ctxStack.pop()
  }
  BeginShaderMode() {}
  EndShaderMode() {}
  BeginBlendMode(mode) {
    this._blendMode = mode
    const ops = ['source-over','lighter','multiply','add','source-over']
    this._ctx.globalCompositeOperation = ops[mode] || 'source-over'
  }
  EndBlendMode() {
    this._blendMode = 0
    this._ctx.globalCompositeOperation = 'source-over'
  }
  BeginScissorMode(x, y, w, h) {
    this._ctx.save()
    this._ctx.beginPath()
    this._ctx.rect(x, y, w, h)
    this._ctx.clip()
  }
  EndScissorMode() { this._ctx.restore() }
  BeginVrStereoMode() {}
  EndVrStereoMode() {}

  ClearBackground(colorPtr) {
    this._ctx.fillStyle = this._css(colorPtr)
    this._ctx.fillRect(0, 0, this._canvas.width, this._canvas.height)
  }

  // ── 2D Shapes ─────────────────────────────────────────────────────────────

  DrawPixel(x, y, colorPtr) {
    this._ctx.fillStyle = this._css(colorPtr)
    this._ctx.fillRect(x, y, 1, 1)
  }

  DrawLine(x1, y1, x2, y2, colorPtr) {
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr)
    ctx.lineWidth = 1
    ctx.beginPath()
    ctx.moveTo(x1, y1)
    ctx.lineTo(x2, y2)
    ctx.stroke()
  }

  DrawLineEx(startPtr, endPtr, thick, colorPtr) {
    const s = this._readVec2(startPtr),
      e = this._readVec2(endPtr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr)
    ctx.lineWidth = thick
    ctx.beginPath()
    ctx.moveTo(s.x, s.y)
    ctx.lineTo(e.x, e.y)
    ctx.stroke()
  }

  DrawCircle(cx, cy, radius, colorPtr) {
    const ctx = this._ctx
    ctx.fillStyle = this._css(colorPtr)
    ctx.beginPath()
    ctx.arc(cx, cy, radius, 0, Math.PI * 2)
    ctx.fill()
  }

  DrawCircleV(centerPtr, radius, colorPtr) {
    const c = this._readVec2(centerPtr)
    this.DrawCircle(c.x, c.y, radius, colorPtr)
  }

  DrawCircleLines(cx, cy, radius, colorPtr) {
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr)
    ctx.lineWidth = 1
    ctx.beginPath()
    ctx.arc(cx, cy, radius, 0, Math.PI * 2)
    ctx.stroke()
  }

  DrawCircleLinesV(centerPtr, radius, colorPtr) {
    const c = this._readVec2(centerPtr)
    this.DrawCircleLines(c.x, c.y, radius, colorPtr)
  }

  DrawEllipse(cx, cy, rx, ry, colorPtr) {
    const ctx = this._ctx
    ctx.fillStyle = this._css(colorPtr)
    ctx.beginPath()
    ctx.ellipse(cx, cy, rx, ry, 0, 0, Math.PI * 2)
    ctx.fill()
  }

  DrawEllipseLines(cx, cy, rx, ry, colorPtr) {
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr)
    ctx.lineWidth = 1
    ctx.beginPath()
    ctx.ellipse(cx, cy, rx, ry, 0, 0, Math.PI * 2)
    ctx.stroke()
  }

  DrawRectangle(x, y, w, h, colorPtr) {
    this._ctx.fillStyle = this._css(colorPtr)
    this._ctx.fillRect(x, y, w, h)
  }

  DrawRectangleV(posPtr, sizePtr, colorPtr) {
    const p = this._readVec2(posPtr),
      s = this._readVec2(sizePtr)
    this.DrawRectangle(p.x, p.y, s.x, s.y, colorPtr)
  }

  DrawRectangleRec(recPtr, colorPtr) {
    // Rectangle: float x,y,width,height
    const v = this._view
    const x = v.getFloat32(recPtr, true)
    const y = v.getFloat32(recPtr + 4, true)
    const w = v.getFloat32(recPtr + 8, true)
    const h = v.getFloat32(recPtr + 12, true)
    this.DrawRectangle(x, y, w, h, colorPtr)
  }

  DrawRectangleLines(x, y, w, h, colorPtr) {
    this._ctx.strokeStyle = this._css(colorPtr)
    this._ctx.lineWidth = 1
    this._ctx.strokeRect(x, y, w, h)
  }

  DrawRectangleLinesEx(recPtr, thick, colorPtr) {
    const v = this._view
    const x = v.getFloat32(recPtr, true)
    const y = v.getFloat32(recPtr + 4, true)
    const w = v.getFloat32(recPtr + 8, true)
    const h = v.getFloat32(recPtr + 12, true)
    this._ctx.strokeStyle = this._css(colorPtr)
    this._ctx.lineWidth = thick
    this._ctx.strokeRect(x, y, w, h)
  }

  DrawRectangleRounded(recPtr, roundness, segments, colorPtr) {
    const v = this._view
    const x = v.getFloat32(recPtr, true)
    const y = v.getFloat32(recPtr + 4, true)
    const w = v.getFloat32(recPtr + 8, true)
    const h = v.getFloat32(recPtr + 12, true)
    const r = Math.min(w, h) * roundness * 0.5
    const ctx = this._ctx
    ctx.fillStyle = this._css(colorPtr)
    ctx.beginPath()
    ctx.roundRect(x, y, w, h, r)
    ctx.fill()
  }

  DrawTriangle(v1Ptr, v2Ptr, v3Ptr, colorPtr) {
    const p1 = this._readVec2(v1Ptr)
    const p2 = this._readVec2(v2Ptr)
    const p3 = this._readVec2(v3Ptr)
    const ctx = this._ctx
    ctx.fillStyle = this._css(colorPtr)
    ctx.beginPath()
    ctx.moveTo(p1.x, p1.y)
    ctx.lineTo(p2.x, p2.y)
    ctx.lineTo(p3.x, p3.y)
    ctx.closePath()
    ctx.fill()
  }

  DrawTriangleLines(v1Ptr, v2Ptr, v3Ptr, colorPtr) {
    const p1 = this._readVec2(v1Ptr)
    const p2 = this._readVec2(v2Ptr)
    const p3 = this._readVec2(v3Ptr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr)
    ctx.lineWidth = 1
    ctx.beginPath()
    ctx.moveTo(p1.x, p1.y)
    ctx.lineTo(p2.x, p2.y)
    ctx.lineTo(p3.x, p3.y)
    ctx.closePath()
    ctx.stroke()
  }

  DrawTriangleFan(pointsPtr, pointCount, colorPtr) {
    if (pointCount < 3) return
    const v = this._view
    const p0x = v.getFloat32(pointsPtr, true)
    const p0y = v.getFloat32(pointsPtr + 4, true)
    const ctx = this._ctx
    ctx.fillStyle = this._css(colorPtr)
    ctx.beginPath()
    ctx.moveTo(p0x, p0y)
    for (let i = 1; i < pointCount; i++) {
      ctx.lineTo(v.getFloat32(pointsPtr + i * 8, true), v.getFloat32(pointsPtr + i * 8 + 4, true))
    }
    ctx.closePath()
    ctx.fill()
  }

  DrawTriangleStrip(pointsPtr, pointCount, colorPtr) {
    if (pointCount < 3) return
    const v = this._view
    const ctx = this._ctx
    ctx.fillStyle = this._css(colorPtr)
    for (let i = 0; i < pointCount - 2; i++) {
      const a = i % 2 === 0 ? [i, i+1, i+2] : [i+1, i, i+2]
      ctx.beginPath()
      for (let j = 0; j < 3; j++) {
        const off = pointsPtr + a[j] * 8
        const px = v.getFloat32(off, true), py = v.getFloat32(off + 4, true)
        j === 0 ? ctx.moveTo(px, py) : ctx.lineTo(px, py)
      }
      ctx.closePath()
      ctx.fill()
    }
  }

  DrawTriangleGradient(v1Ptr, v2Ptr, v3Ptr, c1Ptr, c2Ptr, c3Ptr) {
    // Canvas2D has no per-vertex gradient; average the colors as approximation
    const c1 = this._readColor(c1Ptr)
    const c2 = this._readColor(c2Ptr)
    const c3 = this._readColor(c3Ptr)
    const r = ((c1.r + c2.r + c3.r) / 3) | 0
    const g = ((c1.g + c2.g + c3.g) / 3) | 0
    const b = ((c1.b + c2.b + c3.b) / 3) | 0
    const a = ((c1.a + c2.a + c3.a) / 3) | 0
    const p1 = this._readVec2(v1Ptr), p2 = this._readVec2(v2Ptr), p3 = this._readVec2(v3Ptr)
    const ctx = this._ctx
    ctx.fillStyle = `rgba(${r},${g},${b},${a/255})`
    ctx.beginPath()
    ctx.moveTo(p1.x, p1.y); ctx.lineTo(p2.x, p2.y); ctx.lineTo(p3.x, p3.y)
    ctx.closePath(); ctx.fill()
  }

  DrawLineV(startPtr, endPtr, colorPtr) {
    const s = this._readVec2(startPtr), e = this._readVec2(endPtr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr)
    ctx.lineWidth = 1
    ctx.beginPath(); ctx.moveTo(s.x, s.y); ctx.lineTo(e.x, e.y); ctx.stroke()
  }

  DrawLineBezier(startPtr, endPtr, thick, colorPtr) {
    const s = this._readVec2(startPtr), e = this._readVec2(endPtr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr)
    ctx.lineWidth = thick
    const cx1 = s.x + (e.x - s.x) / 3, cy1 = s.y
    const cx2 = s.x + 2 * (e.x - s.x) / 3, cy2 = e.y
    ctx.beginPath(); ctx.moveTo(s.x, s.y); ctx.bezierCurveTo(cx1, cy1, cx2, cy2, e.x, e.y); ctx.stroke()
  }

  DrawLineDashed(startPtr, endPtr, thick, dash, colorPtr) {
    const s = this._readVec2(startPtr), e = this._readVec2(endPtr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr)
    ctx.lineWidth = thick
    ctx.setLineDash([dash, dash])
    ctx.beginPath(); ctx.moveTo(s.x, s.y); ctx.lineTo(e.x, e.y); ctx.stroke()
    ctx.setLineDash([])
  }

  DrawLineStrip(pointsPtr, pointCount, colorPtr) {
    if (pointCount < 2) return
    const v = this._view
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr)
    ctx.lineWidth = 1
    ctx.beginPath()
    ctx.moveTo(v.getFloat32(pointsPtr, true), v.getFloat32(pointsPtr + 4, true))
    for (let i = 1; i < pointCount; i++) {
      ctx.lineTo(v.getFloat32(pointsPtr + i * 8, true), v.getFloat32(pointsPtr + i * 8 + 4, true))
    }
    ctx.stroke()
  }

  DrawPixelV(posPtr, colorPtr) {
    const p = this._readVec2(posPtr)
    this._ctx.fillStyle = this._css(colorPtr)
    this._ctx.fillRect(p.x | 0, p.y | 0, 1, 1)
  }

  DrawEllipseV(centerPtr, radiusH, radiusV, colorPtr) {
    const c = this._readVec2(centerPtr)
    this.DrawEllipse(c.x, c.y, radiusH, radiusV, colorPtr)
  }

  DrawEllipseLinesV(centerPtr, radiusH, radiusV, colorPtr) {
    const c = this._readVec2(centerPtr)
    this.DrawEllipseLines(c.x, c.y, radiusH, radiusV, colorPtr)
  }

  DrawCircleGradient(cx, cy, radius, innerColorPtr, outerColorPtr) {
    const ctx = this._ctx
    const ci = this._readColor(innerColorPtr), co = this._readColor(outerColorPtr)
    const grad = ctx.createRadialGradient(cx, cy, 0, cx, cy, radius)
    grad.addColorStop(0, `rgba(${ci.r},${ci.g},${ci.b},${ci.a/255})`)
    grad.addColorStop(1, `rgba(${co.r},${co.g},${co.b},${co.a/255})`)
    ctx.fillStyle = grad
    ctx.beginPath(); ctx.arc(cx, cy, radius, 0, Math.PI * 2); ctx.fill()
  }

  DrawCircleSector(centerPtr, radius, startAngle, endAngle, segments, colorPtr) {
    const c = this._readVec2(centerPtr)
    const ctx = this._ctx
    ctx.fillStyle = this._css(colorPtr)
    const sa = startAngle * Math.PI / 180, ea = endAngle * Math.PI / 180
    ctx.beginPath(); ctx.moveTo(c.x, c.y)
    ctx.arc(c.x, c.y, radius, sa, ea); ctx.closePath(); ctx.fill()
  }

  DrawCircleSectorLines(centerPtr, radius, startAngle, endAngle, segments, colorPtr) {
    const c = this._readVec2(centerPtr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = 1
    const sa = startAngle * Math.PI / 180, ea = endAngle * Math.PI / 180
    ctx.beginPath(); ctx.moveTo(c.x, c.y)
    ctx.arc(c.x, c.y, radius, sa, ea); ctx.closePath(); ctx.stroke()
  }

  DrawPoly(centerPtr, sides, radius, rotation, colorPtr) {
    const c = this._readVec2(centerPtr)
    const ctx = this._ctx
    ctx.fillStyle = this._css(colorPtr)
    ctx.beginPath()
    const rot = rotation * Math.PI / 180
    for (let i = 0; i <= sides; i++) {
      const a = rot + (i / sides) * Math.PI * 2
      i === 0 ? ctx.moveTo(c.x + radius * Math.cos(a), c.y + radius * Math.sin(a))
               : ctx.lineTo(c.x + radius * Math.cos(a), c.y + radius * Math.sin(a))
    }
    ctx.closePath(); ctx.fill()
  }

  DrawPolyLines(centerPtr, sides, radius, rotation, colorPtr) {
    const c = this._readVec2(centerPtr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = 1
    ctx.beginPath()
    const rot = rotation * Math.PI / 180
    for (let i = 0; i <= sides; i++) {
      const a = rot + (i / sides) * Math.PI * 2
      i === 0 ? ctx.moveTo(c.x + radius * Math.cos(a), c.y + radius * Math.sin(a))
               : ctx.lineTo(c.x + radius * Math.cos(a), c.y + radius * Math.sin(a))
    }
    ctx.closePath(); ctx.stroke()
  }

  DrawPolyLinesEx(centerPtr, sides, radius, rotation, lineThick, colorPtr) {
    const c = this._readVec2(centerPtr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = lineThick
    ctx.beginPath()
    const rot = rotation * Math.PI / 180
    for (let i = 0; i <= sides; i++) {
      const a = rot + (i / sides) * Math.PI * 2
      i === 0 ? ctx.moveTo(c.x + radius * Math.cos(a), c.y + radius * Math.sin(a))
               : ctx.lineTo(c.x + radius * Math.cos(a), c.y + radius * Math.sin(a))
    }
    ctx.closePath(); ctx.stroke()
  }

  DrawRing(centerPtr, innerRadius, outerRadius, startAngle, endAngle, segments, colorPtr) {
    const c = this._readVec2(centerPtr)
    const ctx = this._ctx
    ctx.fillStyle = this._css(colorPtr)
    const sa = startAngle * Math.PI / 180, ea = endAngle * Math.PI / 180
    ctx.beginPath()
    ctx.arc(c.x, c.y, outerRadius, sa, ea)
    ctx.arc(c.x, c.y, innerRadius, ea, sa, true)
    ctx.closePath(); ctx.fill()
  }

  DrawRingLines(centerPtr, innerRadius, outerRadius, startAngle, endAngle, segments, colorPtr) {
    const c = this._readVec2(centerPtr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = 1
    const sa = startAngle * Math.PI / 180, ea = endAngle * Math.PI / 180
    ctx.beginPath(); ctx.arc(c.x, c.y, outerRadius, sa, ea); ctx.stroke()
    ctx.beginPath(); ctx.arc(c.x, c.y, innerRadius, sa, ea); ctx.stroke()
  }

  DrawRectanglePro(recPtr, originPtr, rotation, colorPtr) {
    const rec = this._readRect(recPtr), ori = this._readVec2(originPtr)
    const ctx = this._ctx
    ctx.save()
    ctx.translate(rec.x, rec.y)
    ctx.rotate(rotation * Math.PI / 180)
    ctx.fillStyle = this._css(colorPtr)
    ctx.fillRect(-ori.x, -ori.y, rec.width, rec.height)
    ctx.restore()
  }

  DrawRectangleGradientV(x, y, w, h, topColorPtr, bottomColorPtr) {
    const ctx = this._ctx
    const ct = this._readColor(topColorPtr), cb = this._readColor(bottomColorPtr)
    const grad = ctx.createLinearGradient(x, y, x, y + h)
    grad.addColorStop(0, `rgba(${ct.r},${ct.g},${ct.b},${ct.a/255})`)
    grad.addColorStop(1, `rgba(${cb.r},${cb.g},${cb.b},${cb.a/255})`)
    ctx.fillStyle = grad; ctx.fillRect(x, y, w, h)
  }

  DrawRectangleGradientH(x, y, w, h, leftColorPtr, rightColorPtr) {
    const ctx = this._ctx
    const cl = this._readColor(leftColorPtr), cr = this._readColor(rightColorPtr)
    const grad = ctx.createLinearGradient(x, y, x + w, y)
    grad.addColorStop(0, `rgba(${cl.r},${cl.g},${cl.b},${cl.a/255})`)
    grad.addColorStop(1, `rgba(${cr.r},${cr.g},${cr.b},${cr.a/255})`)
    ctx.fillStyle = grad; ctx.fillRect(x, y, w, h)
  }

  DrawRectangleGradientEx(recPtr, topLeftPtr, bottomLeftPtr, topRightPtr, bottomRightPtr) {
    // Canvas2D can only do linear gradients; use top-left→bottom-right diagonal approximation
    const rec = this._readRect(recPtr)
    const tl = this._readColor(topLeftPtr), br = this._readColor(bottomRightPtr)
    const ctx = this._ctx
    const grad = ctx.createLinearGradient(rec.x, rec.y, rec.x + rec.width, rec.y + rec.height)
    grad.addColorStop(0, `rgba(${tl.r},${tl.g},${tl.b},${tl.a/255})`)
    grad.addColorStop(1, `rgba(${br.r},${br.g},${br.b},${br.a/255})`)
    ctx.fillStyle = grad; ctx.fillRect(rec.x, rec.y, rec.width, rec.height)
  }

  DrawRectangleRoundedLines(recPtr, roundness, segments, colorPtr) {
    const v = this._view
    const x = v.getFloat32(recPtr, true), y = v.getFloat32(recPtr + 4, true)
    const w = v.getFloat32(recPtr + 8, true), h = v.getFloat32(recPtr + 12, true)
    const r = Math.min(w, h) * roundness * 0.5
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = 1
    ctx.beginPath(); ctx.roundRect(x, y, w, h, r); ctx.stroke()
  }

  DrawRectangleRoundedLinesEx(recPtr, roundness, segments, lineThick, colorPtr) {
    const v = this._view
    const x = v.getFloat32(recPtr, true), y = v.getFloat32(recPtr + 4, true)
    const w = v.getFloat32(recPtr + 8, true), h = v.getFloat32(recPtr + 12, true)
    const r = Math.min(w, h) * roundness * 0.5
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = lineThick
    ctx.beginPath(); ctx.roundRect(x, y, w, h, r); ctx.stroke()
  }

  // ── Splines ──────────────────────────────────────────────────────────────

  DrawSplineLinear(pointsPtr, pointCount, thick, colorPtr) {
    this.DrawLineStrip(pointsPtr, pointCount, colorPtr)
  }

  DrawSplineSegmentLinear(p1Ptr, p2Ptr, thick, colorPtr) {
    this.DrawLineEx(p1Ptr, p2Ptr, thick, colorPtr)
  }

  DrawSplineBezierQuadratic(pointsPtr, pointCount, thick, colorPtr) {
    if (pointCount < 3) return
    const v = this._view, ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = thick
    ctx.beginPath()
    ctx.moveTo(v.getFloat32(pointsPtr, true), v.getFloat32(pointsPtr + 4, true))
    for (let i = 0; i + 2 < pointCount; i += 2) {
      const cp = pointsPtr + i * 8
      ctx.quadraticCurveTo(
        v.getFloat32(cp + 8, true), v.getFloat32(cp + 12, true),
        v.getFloat32(cp + 16, true), v.getFloat32(cp + 20, true))
    }
    ctx.stroke()
  }

  DrawSplineSegmentBezierQuadratic(p1Ptr, c2Ptr, p3Ptr, thick, colorPtr) {
    const p1 = this._readVec2(p1Ptr), c2 = this._readVec2(c2Ptr), p3 = this._readVec2(p3Ptr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = thick
    ctx.beginPath(); ctx.moveTo(p1.x, p1.y)
    ctx.quadraticCurveTo(c2.x, c2.y, p3.x, p3.y); ctx.stroke()
  }

  DrawSplineBezierCubic(pointsPtr, pointCount, thick, colorPtr) {
    if (pointCount < 4) return
    const v = this._view, ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = thick
    ctx.beginPath()
    ctx.moveTo(v.getFloat32(pointsPtr, true), v.getFloat32(pointsPtr + 4, true))
    for (let i = 0; i + 3 < pointCount; i += 3) {
      const cp = pointsPtr + i * 8
      ctx.bezierCurveTo(
        v.getFloat32(cp + 8, true), v.getFloat32(cp + 12, true),
        v.getFloat32(cp + 16, true), v.getFloat32(cp + 20, true),
        v.getFloat32(cp + 24, true), v.getFloat32(cp + 28, true))
    }
    ctx.stroke()
  }

  DrawSplineSegmentBezierCubic(p1Ptr, c2Ptr, c3Ptr, p4Ptr, thick, colorPtr) {
    const p1 = this._readVec2(p1Ptr), c2 = this._readVec2(c2Ptr)
    const c3 = this._readVec2(c3Ptr), p4 = this._readVec2(p4Ptr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = thick
    ctx.beginPath(); ctx.moveTo(p1.x, p1.y)
    ctx.bezierCurveTo(c2.x, c2.y, c3.x, c3.y, p4.x, p4.y); ctx.stroke()
  }

  DrawSplineCatmullRom(pointsPtr, pointCount, thick, colorPtr) {
    if (pointCount < 4) return
    const v = this._view, pts = []
    for (let i = 0; i < pointCount; i++)
      pts.push({ x: v.getFloat32(pointsPtr + i * 8, true), y: v.getFloat32(pointsPtr + i * 8 + 4, true) })
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = thick
    ctx.beginPath(); ctx.moveTo(pts[0].x, pts[0].y)
    for (let i = 1; i < pointCount - 2; i++) {
      const p0 = pts[i-1], p1 = pts[i], p2 = pts[i+1], p3 = pts[i+2]
      for (let t = 0; t <= 1; t += 0.05) {
        const t2 = t*t, t3 = t2*t
        const x = 0.5*((2*p1.x)+(-p0.x+p2.x)*t+(2*p0.x-5*p1.x+4*p2.x-p3.x)*t2+(-p0.x+3*p1.x-3*p2.x+p3.x)*t3)
        const y = 0.5*((2*p1.y)+(-p0.y+p2.y)*t+(2*p0.y-5*p1.y+4*p2.y-p3.y)*t2+(-p0.y+3*p1.y-3*p2.y+p3.y)*t3)
        ctx.lineTo(x, y)
      }
    }
    ctx.stroke()
  }

  DrawSplineSegmentCatmullRom(p1Ptr, p2Ptr, p3Ptr, p4Ptr, thick, colorPtr) {
    const p0 = this._readVec2(p1Ptr), p1 = this._readVec2(p2Ptr)
    const p2 = this._readVec2(p3Ptr), p3 = this._readVec2(p4Ptr)
    const ctx = this._ctx
    ctx.strokeStyle = this._css(colorPtr); ctx.lineWidth = thick
    ctx.beginPath(); ctx.moveTo(p1.x, p1.y)
    for (let t = 0; t <= 1; t += 0.05) {
      const t2 = t*t, t3 = t2*t
      const x = 0.5*((2*p1.x)+(-p0.x+p2.x)*t+(2*p0.x-5*p1.x+4*p2.x-p3.x)*t2+(-p0.x+3*p1.x-3*p2.x+p3.x)*t3)
      const y = 0.5*((2*p1.y)+(-p0.y+p2.y)*t+(2*p0.y-5*p1.y+4*p2.y-p3.y)*t2+(-p0.y+3*p1.y-3*p2.y+p3.y)*t3)
      ctx.lineTo(x, y)
    }
    ctx.stroke()
  }

  DrawSplineBasis(pointsPtr, pointCount, thick, colorPtr) {
    // B-spline: use same path as Catmull-Rom as approximation
    this.DrawSplineCatmullRom(pointsPtr, pointCount, thick, colorPtr)
  }

  DrawSplineSegmentBasis(p1Ptr, p2Ptr, p3Ptr, p4Ptr, thick, colorPtr) {
    this.DrawSplineSegmentCatmullRom(p1Ptr, p2Ptr, p3Ptr, p4Ptr, thick, colorPtr)
  }

  // Spline point getters (sret)
  GetSplinePointLinear(resultPtr, startPtr, endPtr, t) {
    const s = this._readVec2(startPtr), e = this._readVec2(endPtr)
    this._writeVec2(resultPtr, s.x + (e.x - s.x) * t, s.y + (e.y - s.y) * t)
  }

  GetSplinePointBezierQuad(resultPtr, p1Ptr, c2Ptr, p3Ptr, t) {
    const p1 = this._readVec2(p1Ptr), c2 = this._readVec2(c2Ptr), p3 = this._readVec2(p3Ptr)
    const ti = 1 - t
    this._writeVec2(resultPtr, ti*ti*p1.x + 2*ti*t*c2.x + t*t*p3.x, ti*ti*p1.y + 2*ti*t*c2.y + t*t*p3.y)
  }

  GetSplinePointBezierCubic(resultPtr, p1Ptr, c2Ptr, c3Ptr, p4Ptr, t) {
    const p1 = this._readVec2(p1Ptr), c2 = this._readVec2(c2Ptr)
    const c3 = this._readVec2(c3Ptr), p4 = this._readVec2(p4Ptr)
    const ti = 1 - t
    this._writeVec2(resultPtr,
      ti*ti*ti*p1.x + 3*ti*ti*t*c2.x + 3*ti*t*t*c3.x + t*t*t*p4.x,
      ti*ti*ti*p1.y + 3*ti*ti*t*c2.y + 3*ti*t*t*c3.y + t*t*t*p4.y)
  }

  GetSplinePointCatmullRom(resultPtr, p1Ptr, p2Ptr, p3Ptr, p4Ptr, t) {
    const p0 = this._readVec2(p1Ptr), p1 = this._readVec2(p2Ptr)
    const p2 = this._readVec2(p3Ptr), p3 = this._readVec2(p4Ptr)
    const t2 = t*t, t3 = t2*t
    this._writeVec2(resultPtr,
      0.5*((2*p1.x)+(-p0.x+p2.x)*t+(2*p0.x-5*p1.x+4*p2.x-p3.x)*t2+(-p0.x+3*p1.x-3*p2.x+p3.x)*t3),
      0.5*((2*p1.y)+(-p0.y+p2.y)*t+(2*p0.y-5*p1.y+4*p2.y-p3.y)*t2+(-p0.y+3*p1.y-3*p2.y+p3.y)*t3))
  }

  GetSplinePointBasis(resultPtr, p1Ptr, p2Ptr, p3Ptr, p4Ptr, t) {
    this.GetSplinePointCatmullRom(resultPtr, p1Ptr, p2Ptr, p3Ptr, p4Ptr, t)
  }

  // ── Text ──────────────────────────────────────────────────────────────────

  _buildDefaultFont() {
    const atlas = new OffscreenCanvas(128, 128)
    const atx = atlas.getContext('2d')
    const img = atx.createImageData(128, 128)
    const px = img.data
    for (let p = 0; p < 128 * 128; p++) {
      const on = (_FONT_DATA[p >>> 5] >>> (p & 31)) & 1
      const i = p * 4
      px[i] = px[i+1] = px[i+2] = 255
      px[i+3] = on ? 255 : 0
    }
    atx.putImageData(img, 0, 0)
    this._defaultFontAtlas = atlas
    const recs = []
    let currentLine = 0, posX = 1, testX = 1
    for (let i = 0; i < 224; i++) {
      const rec = { x: posX, y: 1 + currentLine * 11, w: _FONT_WIDTHS[i], h: 10 }
      testX += _FONT_WIDTHS[i] + 1
      if (testX >= 128) {
        currentLine++
        posX = 2 + _FONT_WIDTHS[i]
        testX = posX
        rec.x = 1
        rec.y = 1 + currentLine * 11
      } else {
        posX = testX
      }
      recs.push(rec)
    }
    this._defaultFontRecs = recs
  }

  _measureDefaultText(text, fontSize, spacing) {
    fontSize = Math.max(10, fontSize)
    const scale = fontSize / 10
    const sp = spacing !== undefined ? spacing : ((fontSize / 10) | 0)
    let w = 0, count = 0
    for (const ch of text) {
      const cp = ch.codePointAt(0)
      if (cp === 10) continue  // newline not counted in width
      const idx = cp - 32
      if (idx >= 0 && idx < 224) { w += Math.ceil(_FONT_WIDTHS[idx] * scale) + sp; count++ }
    }
    return count > 0 ? w - sp : 0
  }

  _drawDefaultText(ctx, text, x, y, fontSize, r, g, b, a, spacing) {
    if (!this._defaultFontAtlas) return
    fontSize = Math.max(10, fontSize)
    const scale = fontSize / 10
    const sp = spacing !== undefined ? spacing : ((fontSize / 10) | 0)
    const glyphH = Math.ceil(10 * scale)
    const lineH = glyphH + this._textLineSpacing

    // Build per-char glyph list with newline markers
    const chars = []
    for (const ch of text) {
      const cp = ch.codePointAt(0)
      chars.push(cp === 10 ? -1 : cp - 32)
    }
    if (chars.length === 0) return

    // Measure max line width
    let lineW = 0, maxW = 0, lineCount = 1
    for (const idx of chars) {
      if (idx === -1) { if (lineW > maxW) maxW = lineW; lineW = 0; lineCount++; continue }
      if (idx >= 0 && idx < 224) lineW += Math.ceil(_FONT_WIDTHS[idx] * scale) + sp
    }
    if (lineW > maxW) maxW = lineW
    if (maxW === 0) return

    const totalH = lineCount * lineH - this._textLineSpacing
    const tmp = new OffscreenCanvas(maxW, Math.max(1, totalH))
    const tc = tmp.getContext('2d')
    tc.imageSmoothingEnabled = false

    let cx = 0, cy = 0
    for (const idx of chars) {
      if (idx === -1) { cx = 0; cy += lineH; continue }
      if (idx < 0 || idx >= 224) continue
      const rec = this._defaultFontRecs[idx]
      const dw = Math.ceil(rec.w * scale)
      tc.drawImage(this._defaultFontAtlas, rec.x, rec.y, rec.w, rec.h, cx, cy, dw, glyphH)
      cx += dw + sp
    }

    // Tint: fill color masked to glyph pixels
    tc.globalCompositeOperation = 'source-in'
    tc.fillStyle = `rgba(${r},${g},${b},${a / 255})`
    tc.fillRect(0, 0, maxW, totalH)

    ctx.drawImage(tmp, x | 0, y | 0)
  }

  _setFont(fontSize, fontPtr) {
    const family = fontPtr ? this._fontFamily(fontPtr) : 'monospace'
    this._ctx.font = `${fontSize}px ${family}`
  }

  _isDefaultFont(fontPtr) {
    if (!fontPtr) return true
    return this._getFontId(fontPtr) === 0
  }

  DrawText(textPtr, posX, posY, fontSize, colorPtr) {
    if (fontSize <= 0) return
    const v = this._view
    this._drawDefaultText(this._ctx, this._readStr(textPtr), posX, posY, fontSize,
      v.getUint8(colorPtr), v.getUint8(colorPtr+1), v.getUint8(colorPtr+2), v.getUint8(colorPtr+3))
  }

  DrawTextEx(fontPtr, textPtr, posPtr, fontSize, spacing, colorPtr) {
    if (fontSize <= 0) return
    const pos = this._readVec2(posPtr)
    if (this._isDefaultFont(fontPtr)) {
      const v = this._view
      this._drawDefaultText(this._ctx, this._readStr(textPtr), pos.x, pos.y, fontSize,
        v.getUint8(colorPtr), v.getUint8(colorPtr+1), v.getUint8(colorPtr+2), v.getUint8(colorPtr+3), spacing)
    } else {
      this._setFont(fontSize, fontPtr)
      this._ctx.fillStyle = this._css(colorPtr)
      const text = this._readStr(textPtr)
      if (spacing !== 0) {
        let x = pos.x
        for (const ch of text) {
          this._ctx.fillText(ch, x, pos.y + fontSize * 0.75)
          x += this._ctx.measureText(ch).width + spacing
        }
      } else {
        this._ctx.fillText(text, pos.x, pos.y + fontSize * 0.75)
      }
    }
  }

  DrawTextPro(fontPtr, textPtr, posPtr, originPtr, rotation, fontSize, spacing, colorPtr) {
    const pos = this._readVec2(posPtr), ori = this._readVec2(originPtr)
    const ctx = this._ctx
    ctx.save()
    ctx.translate(pos.x, pos.y)
    if (rotation) ctx.rotate(rotation * Math.PI / 180)
    ctx.translate(-ori.x, -ori.y)
    if (this._isDefaultFont(fontPtr)) {
      const v = this._view
      this._drawDefaultText(ctx, this._readStr(textPtr), 0, 0, fontSize,
        v.getUint8(colorPtr), v.getUint8(colorPtr+1), v.getUint8(colorPtr+2), v.getUint8(colorPtr+3), spacing)
    } else {
      this._setFont(fontSize, fontPtr)
      ctx.fillStyle = this._css(colorPtr)
      ctx.fillText(this._readStr(textPtr), 0, fontSize * 0.75)
    }
    ctx.restore()
  }

  DrawTextCodepoint(fontPtr, codepoint, posPtr, fontSize, colorPtr) {
    const pos = this._readVec2(posPtr)
    if (this._isDefaultFont(fontPtr)) {
      const v = this._view
      this._drawDefaultText(this._ctx, String.fromCodePoint(codepoint), pos.x, pos.y, fontSize,
        v.getUint8(colorPtr), v.getUint8(colorPtr+1), v.getUint8(colorPtr+2), v.getUint8(colorPtr+3))
    } else {
      this._setFont(fontSize, fontPtr)
      this._ctx.fillStyle = this._css(colorPtr)
      this._ctx.fillText(String.fromCodePoint(codepoint), pos.x, pos.y + fontSize * 0.75)
    }
  }

  DrawTextCodepoints(fontPtr, codepointsPtr, codepointCount, posPtr, fontSize, spacing, colorPtr) {
    const v = this._view, chars = []
    for (let i = 0; i < codepointCount; i++) chars.push(String.fromCodePoint(v.getInt32(codepointsPtr + i*4, true)))
    const pos = this._readVec2(posPtr)
    if (this._isDefaultFont(fontPtr)) {
      this._drawDefaultText(this._ctx, chars.join(''), pos.x, pos.y, fontSize,
        v.getUint8(colorPtr), v.getUint8(colorPtr+1), v.getUint8(colorPtr+2), v.getUint8(colorPtr+3), spacing)
    } else {
      this._setFont(fontSize, fontPtr)
      this._ctx.fillStyle = this._css(colorPtr)
      let x = pos.x
      for (const ch of chars) {
        this._ctx.fillText(ch, x, pos.y + fontSize * 0.75)
        x += this._ctx.measureText(ch).width + spacing
      }
    }
  }

  SetTextLineSpacing(spacing) { this._textLineSpacing = spacing }

  MeasureText(textPtr, fontSize) {
    return this._measureDefaultText(this._readStr(textPtr), fontSize)
  }

  MeasureTextEx(resultPtr, fontPtr, textPtr, fontSize, spacing) {
    const text = this._readStr(textPtr)
    if (this._isDefaultFont(fontPtr)) {
      const lines = text.split('\n')
      let maxW = 0
      for (const line of lines) {
        const w = this._measureDefaultText(line, fontSize, spacing)
        if (w > maxW) maxW = w
      }
      const scale = Math.max(10, fontSize) / 10
      const glyphH = Math.ceil(10 * scale)
      this._writeVec2(resultPtr, maxW, lines.length * (glyphH + this._textLineSpacing) - this._textLineSpacing)
    } else {
      this._setFont(fontSize, fontPtr)
      const lines = text.split('\n')
      let maxW = 0
      for (const line of lines) {
        const w = this._ctx.measureText(line).width + (line.length > 0 ? (line.length - 1) * spacing : 0)
        if (w > maxW) maxW = w
      }
      this._writeVec2(resultPtr, maxW, lines.length * (fontSize + this._textLineSpacing) - this._textLineSpacing)
    }
  }

  MeasureTextCodepoints(resultPtr, fontPtr, codepointsPtr, codepointCount, fontSize, spacing) {
    const v = this._view, chars = []
    for (let i = 0; i < codepointCount; i++) chars.push(String.fromCodePoint(v.getInt32(codepointsPtr + i*4, true)))
    const text = chars.join('')
    if (this._isDefaultFont(fontPtr)) {
      this._writeVec2(resultPtr, this._measureDefaultText(text, fontSize, spacing), Math.max(10, fontSize))
    } else {
      this._setFont(fontSize, fontPtr)
      this._writeVec2(resultPtr, this._ctx.measureText(text).width | 0, fontSize)
    }
  }

  DrawFPS(posX, posY) {
    const fps = this.GetFPS()
    const c = fps < 30 ? [255, 0, 0, 255] : [0, 228, 48, 255]
    this._drawDefaultText(this._ctx, `${fps} FPS`, posX, posY, 20, c[0], c[1], c[2], c[3])
  }

  GetGlyphIndex(fontPtr, codepoint) {
    if (this._isDefaultFont(fontPtr)) return codepoint >= 32 && codepoint < 32 + 224 ? codepoint - 32 : 0
    return codepoint
  }

  GetGlyphInfo(resultPtr, fontPtr, codepoint) {
    // GlyphInfo: value(4), offsetX(4), offsetY(4), advanceX(4), image(20) = 36 bytes
    const v = this._view
    v.setInt32(resultPtr, codepoint, true)
    v.setInt32(resultPtr + 4, 0, true)
    v.setInt32(resultPtr + 8, 0, true)
    if (this._isDefaultFont(fontPtr) && this._defaultFontRecs && codepoint >= 32 && codepoint < 32 + 224) {
      const rec = this._defaultFontRecs[codepoint - 32]
      v.setInt32(resultPtr + 12, rec.w, true)
      this._writeImage(resultPtr + 16, 0, rec.w, rec.h)
    } else {
      const fontSize = this._fontBaseSize(fontPtr)
      this._setFont(fontSize, fontPtr)
      const adv = this._ctx.measureText(String.fromCodePoint(codepoint)).width | 0
      v.setInt32(resultPtr + 12, adv, true)
      this._writeImage(resultPtr + 16, 0, adv, fontSize)
    }
  }

  GetGlyphAtlasRec(resultPtr, fontPtr, codepoint) {
    if (this._isDefaultFont(fontPtr) && this._defaultFontRecs && codepoint >= 32 && codepoint < 32 + 224) {
      const rec = this._defaultFontRecs[codepoint - 32]
      this._writeRect(resultPtr, rec.x, rec.y, rec.w, rec.h)
    } else {
      const fontSize = this._fontBaseSize(fontPtr)
      this._setFont(fontSize, fontPtr)
      const w = this._ctx.measureText(String.fromCodePoint(codepoint)).width | 0
      this._writeRect(resultPtr, 0, 0, w, fontSize)
    }
  }

  GetFontDefault(resultPtr) {
    this._writeFont(resultPtr, 0, 10)
  }

  LoadFont(resultPtr, fileNamePtr) {
    const fileName = this._readStr(fileNamePtr)
    this.LoadFontEx(resultPtr, fileNamePtr, 32, 0, 0)
  }

  LoadFontEx(resultPtr, fileNamePtr, fontSize, codepointsPtr, codepointCount) {
    const fileName = this._readStr(fileNamePtr)
    const id = this._newId()
    this._fonts.set(id, { face: null, baseSize: fontSize, family: `font_${id}`, loaded: false })
    this._writeFont(resultPtr, id, fontSize)
    // Async load
    const face = new FontFace(`font_${id}`, `url(${fileName})`)
    face.load().then(f => {
      document.fonts.add(f)
      const fnt = this._fonts.get(id)
      if (fnt) { fnt.face = f; fnt.loaded = true }
    }).catch(e => console.warn(`LoadFont: ${fileName}`, e))
  }

  LoadFontFromMemory(resultPtr, fileTypePtr, dataPtr, dataSize, fontSize, codepointsPtr, codepointCount) {
    const id = this._newId()
    this._fonts.set(id, { face: null, baseSize: fontSize, family: `font_${id}`, loaded: false })
    this._writeFont(resultPtr, id, fontSize)
    const bytes = new Uint8Array(this._memory.buffer, dataPtr, dataSize)
    const blob = new Blob([new Uint8Array(bytes)])
    const url = URL.createObjectURL(blob)
    const face = new FontFace(`font_${id}`, `url(${url})`)
    face.load().then(f => {
      document.fonts.add(f)
      const fnt = this._fonts.get(id); if (fnt) { fnt.face = f; fnt.loaded = true }
      URL.revokeObjectURL(url)
    }).catch(() => {})
  }

  LoadFontFromImage(resultPtr, imagePtr, colorKeyPtr, firstChar) {
    const id = this._newId()
    this._fonts.set(id, { face: null, baseSize: 10, family: 'monospace', loaded: true })
    this._writeFont(resultPtr, id, 10)
  }

  LoadFontData(resultPtr) { this._view.setUint32(resultPtr, 0, true) }
  UnloadFontData() {}

  IsFontValid(fontPtr) {
    const id = this._getFontId(fontPtr)
    return (id === 0 || this._fonts.has(id)) ? 1 : 0
  }

  UnloadFont(fontPtr) {
    const id = this._getFontId(fontPtr)
    this._fonts.delete(id)
  }

  _fontFamily(fontPtr) {
    if (!fontPtr) return 'monospace'
    const id = this._getFontId(fontPtr)
    if (id === 0) return 'monospace'
    return this._fonts.get(id)?.family || 'monospace'
  }

  _fontBaseSize(fontPtr) {
    if (!fontPtr) return 10
    const id = this._getFontId(fontPtr)
    if (id === 0) return 10
    return this._fonts.get(id)?.baseSize || 10
  }

  // ── Mouse ─────────────────────────────────────────────────────────────────

  _mouseXScaled() { return ((this._mouse.x + (this._mouse.offsetX || 0)) * (this._mouse.scaleX || 1)) | 0 }
  _mouseYScaled() { return ((this._mouse.y + (this._mouse.offsetY || 0)) * (this._mouse.scaleY || 1)) | 0 }

  GetMousePosition(resultPtr) {
    this._writeVec2(resultPtr, this._mouseXScaled(), this._mouseYScaled())
  }

  GetMouseX() { return this._mouseXScaled() }
  GetMouseY() { return this._mouseYScaled() }

  GetMouseDelta(resultPtr) {
    this._writeVec2(resultPtr, this._mouse.dx || 0, this._mouse.dy || 0)
  }

  SetMousePosition(x, y) {
    this._mouse.x = x
    this._mouse.y = y
  }
  SetMouseOffset(offsetX, offsetY) { this._mouse.offsetX = offsetX; this._mouse.offsetY = offsetY }
  SetMouseScale(scaleX, scaleY) { this._mouse.scaleX = scaleX; this._mouse.scaleY = scaleY }

  GetMouseWheelMove() {
    return this._mouse.scroll
  }
  GetMouseWheelMoveV(resultPtr) {
    this._writeVec2(resultPtr, 0, this.GetMouseWheelMove())
  }

  IsMouseButtonPressed(btn) {
    return this._mouse.buttons.get(btn) === 1 ? 1 : 0
  }
  IsMouseButtonDown(btn) {
    const s = this._mouse.buttons.get(btn)
    return s === 1 || s === 2 ? 1 : 0
  }
  IsMouseButtonReleased(btn) {
    return this._mouse.buttons.get(btn) === 3 ? 1 : 0
  }
  IsMouseButtonUp(btn) {
    const s = this._mouse.buttons.get(btn)
    return !s || s === 0 || s === 3 ? 1 : 0
  }

  // ── Keyboard ──────────────────────────────────────────────────────────────

  IsKeyPressed(key) {
    return this._keys.get(key) === 1 ? 1 : 0
  }
  IsKeyDown(key) {
    const s = this._keys.get(key)
    return s === 1 || s === 2 ? 1 : 0
  }
  IsKeyReleased(key) {
    return this._keys.get(key) === 3 ? 1 : 0
  }
  IsKeyUp(key) {
    const s = this._keys.get(key)
    return !s || s === 0 || s === 3 ? 1 : 0
  }
  IsKeyPressedRepeat(key) {
    const s = this._keys.get(key)
    return s === 1 || s === 2 ? 1 : 0
  }
  GetKeyPressed() {
    return this._keysNew.shift() || 0
  }
  GetCharPressed() {
    return this._keysNew.shift() || 0
  }

  // ── Color utils ───────────────────────────────────────────────────────────

  // ColorToInt: read Color from ptr, return packed RGBA int
  ColorToInt(colorPtr) {
    const c = this._readColor(colorPtr)
    return ((c.r << 24) | (c.g << 16) | (c.b << 8) | c.a) >>> 0
  }

  // GetColor: write Color from hex into result ptr (sret)
  GetColor(resultPtr, hexValue) {
    const b = new Uint8Array(this._memory.buffer, resultPtr, 4)
    b[0] = (hexValue >>> 24) & 0xff
    b[1] = (hexValue >>> 16) & 0xff
    b[2] = (hexValue >>> 8) & 0xff
    b[3] = hexValue & 0xff
  }

  // ColorNormalize: write Vector4 (sret)
  ColorNormalize(resultPtr, colorPtr) {
    const c = this._readColor(colorPtr)
    this._writeVec4(resultPtr, c.r / 255, c.g / 255, c.b / 255, c.a / 255)
  }

  // ColorAlpha: write Color (sret) — takes Color ptr + float alpha
  ColorAlpha(resultPtr, colorPtr, alpha) {
    const c = this._readColor(colorPtr)
    const b = new Uint8Array(this._memory.buffer, resultPtr, 4)
    b[0] = c.r
    b[1] = c.g
    b[2] = c.b
    b[3] = (alpha * 255) | 0
  }

  // ColorBrightness, ColorContrast, ColorTint, ColorInvert, ColorLerp — sret
  ColorBrightness(resultPtr, colorPtr, factor) {
    const c = this._readColor(colorPtr)
    const f = 1 + factor
    const b = new Uint8Array(this._memory.buffer, resultPtr, 4)
    b[0] = Math.min(255, c.r * f) | 0
    b[1] = Math.min(255, c.g * f) | 0
    b[2] = Math.min(255, c.b * f) | 0
    b[3] = c.a
  }

  Fade(resultPtr, colorPtr, alpha) {
    this.ColorAlpha(resultPtr, colorPtr, alpha)
  }

  ColorContrast(resultPtr, colorPtr, contrast) {
    const c = this._readColor(colorPtr)
    const f = (259 * (contrast * 255 + 255)) / (255 * (259 - contrast * 255))
    this._writeColor(resultPtr,
      Math.max(0, Math.min(255, f * (c.r - 128) + 128)) | 0,
      Math.max(0, Math.min(255, f * (c.g - 128) + 128)) | 0,
      Math.max(0, Math.min(255, f * (c.b - 128) + 128)) | 0,
      c.a)
  }

  ColorTint(resultPtr, colorPtr, tintPtr) {
    const c = this._readColor(colorPtr), t = this._readColor(tintPtr)
    this._writeColor(resultPtr, (c.r * t.r / 255) | 0, (c.g * t.g / 255) | 0, (c.b * t.b / 255) | 0, (c.a * t.a / 255) | 0)
  }

  ColorIsEqual(c1Ptr, c2Ptr) {
    const c1 = this._readColor(c1Ptr), c2 = this._readColor(c2Ptr)
    return (c1.r === c2.r && c1.g === c2.g && c1.b === c2.b && c1.a === c2.a) ? 1 : 0
  }

  ColorLerp(resultPtr, c1Ptr, c2Ptr, factor) {
    const c1 = this._readColor(c1Ptr), c2 = this._readColor(c2Ptr), f = Math.max(0, Math.min(1, factor))
    this._writeColor(resultPtr,
      (c1.r + (c2.r - c1.r) * f) | 0, (c1.g + (c2.g - c1.g) * f) | 0,
      (c1.b + (c2.b - c1.b) * f) | 0, (c1.a + (c2.a - c1.a) * f) | 0)
  }

  ColorAlphaBlend(resultPtr, dstPtr, srcPtr, tintPtr) {
    const dst = this._readColor(dstPtr), src = this._readColor(srcPtr), tint = this._readColor(tintPtr)
    const sa = (src.a / 255) * (tint.a / 255)
    const da = dst.a / 255
    const oa = sa + da * (1 - sa)
    if (oa <= 0) { this._writeColor(resultPtr, 0, 0, 0, 0); return }
    this._writeColor(resultPtr,
      ((src.r * (tint.r/255) * sa + dst.r * da * (1-sa)) / oa) | 0,
      ((src.g * (tint.g/255) * sa + dst.g * da * (1-sa)) / oa) | 0,
      ((src.b * (tint.b/255) * sa + dst.b * da * (1-sa)) / oa) | 0,
      (oa * 255) | 0)
  }

  ColorFromHSV(resultPtr, hue, saturation, value) {
    let r = 0, g = 0, b = 0
    if (saturation === 0) { r = g = b = value }
    else {
      const h = ((hue % 360) + 360) % 360 / 60
      const i = Math.floor(h), f = h - i
      const p = value * (1 - saturation)
      const q = value * (1 - saturation * f)
      const t = value * (1 - saturation * (1 - f))
      switch (i) {
        case 0: r=value; g=t; b=p; break; case 1: r=q; g=value; b=p; break
        case 2: r=p; g=value; b=t; break; case 3: r=p; g=q; b=value; break
        case 4: r=t; g=p; b=value; break; default: r=value; g=p; b=q
      }
    }
    this._writeColor(resultPtr, (r*255)|0, (g*255)|0, (b*255)|0, 255)
  }

  ColorToHSV(resultPtr, colorPtr) {
    const c = this._readColor(colorPtr)
    const r = c.r/255, g = c.g/255, b = c.b/255
    const max = Math.max(r,g,b), min = Math.min(r,g,b), delta = max - min
    let h = 0, s = max === 0 ? 0 : delta / max, v = max
    if (delta > 0) {
      if (max === r) h = 60 * (((g - b) / delta) % 6)
      else if (max === g) h = 60 * ((b - r) / delta + 2)
      else h = 60 * ((r - g) / delta + 4)
      if (h < 0) h += 360
    }
    this._writeVec3(resultPtr, h, s, v)
  }

  ColorFromNormalized(resultPtr, normalizedPtr) {
    const v = this._view
    const r = v.getFloat32(normalizedPtr, true), g = v.getFloat32(normalizedPtr+4, true)
    const b = v.getFloat32(normalizedPtr+8, true), a = v.getFloat32(normalizedPtr+12, true)
    this._writeColor(resultPtr, (r*255)|0, (g*255)|0, (b*255)|0, (a*255)|0)
  }

  // ── Collision detection ───────────────────────────────────────────────────

  CheckCollisionRecs(rec1Ptr, rec2Ptr) {
    const r1 = this._readRect(rec1Ptr), r2 = this._readRect(rec2Ptr)
    return (r1.x < r2.x + r2.width && r1.x + r1.width > r2.x &&
            r1.y < r2.y + r2.height && r1.y + r1.height > r2.y) ? 1 : 0
  }

  GetCollisionRec(resultPtr, rec1Ptr, rec2Ptr) {
    const r1 = this._readRect(rec1Ptr), r2 = this._readRect(rec2Ptr)
    const x = Math.max(r1.x, r2.x), y = Math.max(r1.y, r2.y)
    const ex = Math.min(r1.x + r1.width, r2.x + r2.width)
    const ey = Math.min(r1.y + r1.height, r2.y + r2.height)
    this._writeRect(resultPtr, x, y, Math.max(0, ex - x), Math.max(0, ey - y))
  }

  CheckCollisionCircles(center1Ptr, radius1, center2Ptr, radius2) {
    const c1 = this._readVec2(center1Ptr), c2 = this._readVec2(center2Ptr)
    const dx = c1.x - c2.x, dy = c1.y - c2.y
    return dx*dx + dy*dy <= (radius1 + radius2) * (radius1 + radius2) ? 1 : 0
  }

  CheckCollisionCircleRec(centerPtr, radius, recPtr) {
    const c = this._readVec2(centerPtr), r = this._readRect(recPtr)
    const cx = Math.max(r.x, Math.min(c.x, r.x + r.width))
    const cy = Math.max(r.y, Math.min(c.y, r.y + r.height))
    const dx = c.x - cx, dy = c.y - cy
    return dx*dx + dy*dy <= radius*radius ? 1 : 0
  }

  CheckCollisionCircleLine(centerPtr, radius, p1Ptr, p2Ptr) {
    const c = this._readVec2(centerPtr), p1 = this._readVec2(p1Ptr), p2 = this._readVec2(p2Ptr)
    const dx = p2.x - p1.x, dy = p2.y - p1.y
    const len2 = dx*dx + dy*dy
    if (len2 === 0) {
      const ex = c.x - p1.x, ey = c.y - p1.y
      return ex*ex + ey*ey <= radius*radius ? 1 : 0
    }
    const t = Math.max(0, Math.min(1, ((c.x - p1.x)*dx + (c.y - p1.y)*dy) / len2))
    const cx = p1.x + t*dx - c.x, cy = p1.y + t*dy - c.y
    return cx*cx + cy*cy <= radius*radius ? 1 : 0
  }

  CheckCollisionPointRec(pointPtr, recPtr) {
    const p = this._readVec2(pointPtr), r = this._readRect(recPtr)
    return (p.x >= r.x && p.x < r.x + r.width && p.y >= r.y && p.y < r.y + r.height) ? 1 : 0
  }

  CheckCollisionPointCircle(pointPtr, centerPtr, radius) {
    const p = this._readVec2(pointPtr), c = this._readVec2(centerPtr)
    const dx = p.x - c.x, dy = p.y - c.y
    return dx*dx + dy*dy <= radius*radius ? 1 : 0
  }

  CheckCollisionPointTriangle(pointPtr, p1Ptr, p2Ptr, p3Ptr) {
    const p = this._readVec2(pointPtr)
    const a = this._readVec2(p1Ptr), b = this._readVec2(p2Ptr), c = this._readVec2(p3Ptr)
    const d1 = (p.x - b.x)*(a.y - b.y) - (a.x - b.x)*(p.y - b.y)
    const d2 = (p.x - c.x)*(b.y - c.y) - (b.x - c.x)*(p.y - c.y)
    const d3 = (p.x - a.x)*(c.y - a.y) - (c.x - a.x)*(p.y - a.y)
    const neg = d1 < 0 || d2 < 0 || d3 < 0
    const pos = d1 > 0 || d2 > 0 || d3 > 0
    return !(neg && pos) ? 1 : 0
  }

  CheckCollisionPointPoly(pointPtr, polyPtr, pointCount) {
    const p = this._readVec2(pointPtr)
    const v = this._view
    let inside = false
    for (let i = 0, j = pointCount - 1; i < pointCount; j = i++) {
      const xi = v.getFloat32(polyPtr + i*8, true), yi = v.getFloat32(polyPtr + i*8 + 4, true)
      const xj = v.getFloat32(polyPtr + j*8, true), yj = v.getFloat32(polyPtr + j*8 + 4, true)
      if (((yi > p.y) !== (yj > p.y)) && (p.x < (xj - xi) * (p.y - yi) / (yj - yi) + xi)) inside = !inside
    }
    return inside ? 1 : 0
  }

  CheckCollisionLines(p1Ptr, p2Ptr, p3Ptr, p4Ptr, collisionPtPtr) {
    const a = this._readVec2(p1Ptr), b = this._readVec2(p2Ptr)
    const c = this._readVec2(p3Ptr), d = this._readVec2(p4Ptr)
    const denom = (a.x - b.x)*(c.y - d.y) - (a.y - b.y)*(c.x - d.x)
    if (Math.abs(denom) < 1e-10) return 0
    const t = ((a.x - c.x)*(c.y - d.y) - (a.y - c.y)*(c.x - d.x)) / denom
    const u = -((a.x - b.x)*(a.y - c.y) - (a.y - b.y)*(a.x - c.x)) / denom
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
      if (collisionPtPtr) this._writeVec2(collisionPtPtr, a.x + t*(b.x - a.x), a.y + t*(b.y - a.y))
      return 1
    }
    return 0
  }

  CheckCollisionPointLine(pointPtr, p1Ptr, p2Ptr, threshold) {
    const p = this._readVec2(pointPtr), a = this._readVec2(p1Ptr), b = this._readVec2(p2Ptr)
    const dx = b.x - a.x, dy = b.y - a.y, len2 = dx*dx + dy*dy
    if (len2 === 0) { const ex = p.x-a.x, ey = p.y-a.y; return ex*ex+ey*ey <= threshold*threshold ? 1 : 0 }
    const t = Math.max(0, Math.min(1, ((p.x-a.x)*dx + (p.y-a.y)*dy) / len2))
    const cx = a.x + t*dx - p.x, cy = a.y + t*dy - p.y
    return cx*cx + cy*cy <= threshold*threshold ? 1 : 0
  }

  // 3D collision — AABB and sphere math (no rendering needed)
  CheckCollisionBoxes(box1MinPtr, box1MaxPtr, box2MinPtr, box2MaxPtr) {
    const v = this._view
    const ax1 = v.getFloat32(box1MinPtr, true), ay1 = v.getFloat32(box1MinPtr+4, true), az1 = v.getFloat32(box1MinPtr+8, true)
    const ax2 = v.getFloat32(box1MaxPtr, true), ay2 = v.getFloat32(box1MaxPtr+4, true), az2 = v.getFloat32(box1MaxPtr+8, true)
    const bx1 = v.getFloat32(box2MinPtr, true), by1 = v.getFloat32(box2MinPtr+4, true), bz1 = v.getFloat32(box2MinPtr+8, true)
    const bx2 = v.getFloat32(box2MaxPtr, true), by2 = v.getFloat32(box2MaxPtr+4, true), bz2 = v.getFloat32(box2MaxPtr+8, true)
    return (ax1 <= bx2 && ax2 >= bx1 && ay1 <= by2 && ay2 >= by1 && az1 <= bz2 && az2 >= bz1) ? 1 : 0
  }

  CheckCollisionBoxSphere(boxMinPtr, boxMaxPtr, centerPtr, radius) {
    const v = this._view
    const bx1 = v.getFloat32(boxMinPtr, true), by1 = v.getFloat32(boxMinPtr+4, true), bz1 = v.getFloat32(boxMinPtr+8, true)
    const bx2 = v.getFloat32(boxMaxPtr, true), by2 = v.getFloat32(boxMaxPtr+4, true), bz2 = v.getFloat32(boxMaxPtr+8, true)
    const cx = v.getFloat32(centerPtr, true), cy = v.getFloat32(centerPtr+4, true), cz = v.getFloat32(centerPtr+8, true)
    const dx = cx - Math.max(bx1, Math.min(cx, bx2))
    const dy = cy - Math.max(by1, Math.min(cy, by2))
    const dz = cz - Math.max(bz1, Math.min(cz, bz2))
    return (dx*dx + dy*dy + dz*dz) <= radius*radius ? 1 : 0
  }

  CheckCollisionSpheres(center1Ptr, radius1, center2Ptr, radius2) {
    const v = this._view
    const dx = v.getFloat32(center1Ptr, true) - v.getFloat32(center2Ptr, true)
    const dy = v.getFloat32(center1Ptr+4, true) - v.getFloat32(center2Ptr+4, true)
    const dz = v.getFloat32(center1Ptr+8, true) - v.getFloat32(center2Ptr+8, true)
    const r = radius1 + radius2
    return (dx*dx + dy*dy + dz*dz) <= r*r ? 1 : 0
  }

  // ── Math (sret) ───────────────────────────────────────────────────────────

  Vector2Zero(resultPtr) {
    this._writeVec2(resultPtr, 0, 0)
  }
  Vector2One(resultPtr) {
    this._writeVec2(resultPtr, 1, 1)
  }
  Vector3Zero(resultPtr) {
    this._writeVec3(resultPtr, 0, 0, 0)
  }
  Vector3One(resultPtr) {
    this._writeVec3(resultPtr, 1, 1, 1)
  }

  // ── Camera ────────────────────────────────────────────────────────────────

  GetWorldToScreen2D(resultPtr, posPtr, cameraPtr) {
    const pos = this._readVec2(posPtr), cam = this._readCamera2D(cameraPtr)
    const cos = Math.cos(cam.rotation * Math.PI / 180)
    const sin = Math.sin(cam.rotation * Math.PI / 180)
    let x = pos.x - cam.target.x, y = pos.y - cam.target.y
    x *= cam.zoom; y *= cam.zoom
    this._writeVec2(resultPtr, x*cos - y*sin + cam.offset.x, x*sin + y*cos + cam.offset.y)
  }

  GetScreenToWorld2D(resultPtr, posPtr, cameraPtr) {
    const pos = this._readVec2(posPtr), cam = this._readCamera2D(cameraPtr)
    const cos = Math.cos(-cam.rotation * Math.PI / 180)
    const sin = Math.sin(-cam.rotation * Math.PI / 180)
    let x = pos.x - cam.offset.x, y = pos.y - cam.offset.y
    const rx = x*cos - y*sin, ry = x*sin + y*cos
    this._writeVec2(resultPtr, rx/cam.zoom + cam.target.x, ry/cam.zoom + cam.target.y)
  }

  GetCameraMatrix2D(resultPtr, cameraPtr) {
    // Camera2D matrix: scale by zoom, rotate, then translate
    const cam = this._readCamera2D(cameraPtr)
    const rad = cam.rotation * Math.PI / 180
    const cos = Math.cos(rad) * cam.zoom, sin = Math.sin(rad) * cam.zoom
    const tx = cam.offset.x - (cam.target.x * cos - cam.target.y * sin)
    const ty = cam.offset.y - (cam.target.x * sin + cam.target.y * cos)
    const v = this._view
    // Row-major 4x4: [ cos, -sin, 0, tx,  sin, cos, 0, ty,  0, 0, 1, 0,  0, 0, 0, 1 ]
    v.setFloat32(resultPtr, cos, true);       v.setFloat32(resultPtr+4, -sin, true)
    v.setFloat32(resultPtr+8, 0, true);       v.setFloat32(resultPtr+12, tx, true)
    v.setFloat32(resultPtr+16, sin, true);    v.setFloat32(resultPtr+20, cos, true)
    v.setFloat32(resultPtr+24, 0, true);      v.setFloat32(resultPtr+28, ty, true)
    v.setFloat32(resultPtr+32, 0, true);      v.setFloat32(resultPtr+36, 0, true)
    v.setFloat32(resultPtr+40, 1, true);      v.setFloat32(resultPtr+44, 0, true)
    v.setFloat32(resultPtr+48, 0, true);      v.setFloat32(resultPtr+52, 0, true)
    v.setFloat32(resultPtr+56, 0, true);      v.setFloat32(resultPtr+60, 1, true)
  }

  UpdateCamera() {}
  UpdateCameraPro() {}
  GetCameraMatrix(resultPtr) {
    const v = this._view
    for (let i = 0; i < 16; i++) v.setFloat32(resultPtr + i*4, i % 5 === 0 ? 1 : 0, true)
  }
  // ── Texture system ────────────────────────────────────────────────────────

  LoadTexture(resultPtr, fileNamePtr) {
    const fileName = this._readStr(fileNamePtr)
    const id = this._newId()
    const bmp = this._fileBitmaps.get(fileName) || null
    this._textures.set(id, bmp)
    const w = bmp?.width || 0, h = bmp?.height || 0
    this._writeTexture(resultPtr, id, w, h)
    if (!bmp) {
      // async fallback fetch
      this._loadTextureFetch(id, fileName, resultPtr)
    }
  }

  async _loadTextureFetch(id, url, resultPtr) {
    try {
      const data = this._fileData.get(url)
      const bmp = data
        ? await createImageBitmap(new Blob([data]))
        : await createImageBitmap(await (await fetch(url)).blob())
      this._textures.set(id, bmp)
    } catch (e) { console.warn(`LoadTexture: ${url}`, e) }
  }

  LoadTextureFromImage(resultPtr, imagePtr) {
    const img = this._images.get(this._view.getUint32(imagePtr, true))
    const id = this._newId()
    if (img) {
      const oc = new OffscreenCanvas(img.width, img.height)
      oc.getContext('2d').putImageData(img.data, 0, 0)
      this._textures.set(id, oc)
      this._writeTexture(resultPtr, id, img.width, img.height)
    } else {
      this._textures.set(id, null)
      this._writeTexture(resultPtr, id, 0, 0)
    }
  }

  LoadTextureCubemap(resultPtr) { this._writeTexture(resultPtr, 0, 0, 0) }

  LoadRenderTexture(resultPtr, w, h) {
    const id = this._newId()
    const oc = new OffscreenCanvas(w, h)
    this._renderTextures.set(id, oc)
    // RenderTexture struct (44 bytes): id(4), texture(20), depth(20)
    const v = this._view
    v.setUint32(resultPtr, id, true)
    this._writeTexture(resultPtr + 4, id, w, h)
    this._writeTexture(resultPtr + 24, 0, w, h)
  }

  UnloadTexture(texturePtr) {
    const id = this._view.getUint32(texturePtr, true)
    this._textures.delete(id)
  }

  UnloadRenderTexture(rtPtr) {
    const id = this._view.getUint32(rtPtr, true)
    this._renderTextures.delete(id)
  }

  IsTextureValid(texturePtr) {
    const id = this._view.getUint32(texturePtr, true)
    return this._textures.has(id) ? 1 : 0
  }

  IsRenderTextureValid(rtPtr) {
    const id = this._view.getUint32(rtPtr, true)
    return this._renderTextures.has(id) ? 1 : 0
  }

  GenTextureMipmaps(texturePtr) {} // no-op: canvas doesn't use mipmaps
  SetTextureFilter() {}
  SetTextureWrap() {}

  UpdateTexture(texturePtr, pixelsPtr) {
    const tex = this._readTexture(texturePtr)
    const bmp = this._textures.get(tex.id)
    if (!bmp || !tex.width || !tex.height) return
    const data = new Uint8ClampedArray(this._memory.buffer, pixelsPtr, tex.width * tex.height * 4)
    const imgData = new ImageData(new Uint8ClampedArray(data), tex.width, tex.height)
    const oc = new OffscreenCanvas(tex.width, tex.height)
    oc.getContext('2d').putImageData(imgData, 0, 0)
    this._textures.set(tex.id, oc)
  }

  UpdateTextureRec(texturePtr, recPtr, pixelsPtr) {
    const tex = this._readTexture(texturePtr)
    const bmp = this._textures.get(tex.id)
    if (!bmp) return
    const rec = this._readRect(recPtr)
    const data = new Uint8ClampedArray(this._memory.buffer, pixelsPtr, rec.width * rec.height * 4)
    const imgData = new ImageData(new Uint8ClampedArray(data), rec.width, rec.height)
    const oc = (bmp instanceof OffscreenCanvas) ? bmp : (() => {
      const o = new OffscreenCanvas(tex.width, tex.height)
      o.getContext('2d').drawImage(bmp, 0, 0)
      this._textures.set(tex.id, o)
      return o
    })()
    oc.getContext('2d').putImageData(imgData, rec.x, rec.y)
  }

  DrawTexture(texturePtr, posX, posY, tintPtr) {
    const tex = this._readTexture(texturePtr)
    const bmp = this._textures.get(tex.id)
    if (!bmp) return
    const tint = this._readColor(tintPtr)
    const ctx = this._ctx
    ctx.save()
    ctx.globalAlpha = tint.a / 255
    ctx.drawImage(bmp, posX, posY)
    ctx.restore()
  }

  DrawTextureV(texturePtr, posPtr, tintPtr) {
    const pos = this._readVec2(posPtr)
    this.DrawTexture(texturePtr, pos.x, pos.y, tintPtr)
  }

  DrawTextureEx(texturePtr, posPtr, rotation, scale, tintPtr) {
    const tex = this._readTexture(texturePtr)
    const bmp = this._textures.get(tex.id)
    if (!bmp) return
    const pos = this._readVec2(posPtr), tint = this._readColor(tintPtr)
    const w = (bmp.width || tex.width) * scale, h = (bmp.height || tex.height) * scale
    const ctx = this._ctx
    ctx.save()
    ctx.translate(pos.x, pos.y)
    ctx.rotate(rotation * Math.PI / 180)
    ctx.globalAlpha = tint.a / 255
    ctx.drawImage(bmp, 0, 0, w, h)
    ctx.restore()
  }

  DrawTextureRec(texturePtr, sourceRecPtr, posPtr, tintPtr) {
    const tex = this._readTexture(texturePtr)
    const bmp = this._textures.get(tex.id)
    if (!bmp) return
    const src = this._readRect(sourceRecPtr), pos = this._readVec2(posPtr), tint = this._readColor(tintPtr)
    const ctx = this._ctx
    ctx.save()
    ctx.globalAlpha = tint.a / 255
    if (src.width < 0) { ctx.scale(-1,1); ctx.translate(-pos.x - Math.abs(src.width), pos.y) }
    else ctx.translate(0, 0)
    ctx.drawImage(bmp, Math.abs(src.x), Math.abs(src.y), Math.abs(src.width), Math.abs(src.height),
      src.width < 0 ? 0 : pos.x, src.height < 0 ? -pos.y : pos.y, Math.abs(src.width), Math.abs(src.height))
    ctx.restore()
  }

  DrawTexturePro(texturePtr, sourceRecPtr, destRecPtr, originPtr, rotation, tintPtr) {
    const tex = this._readTexture(texturePtr)
    const bmp = this._textures.get(tex.id)
    if (!bmp) return
    const src = this._readRect(sourceRecPtr), dst = this._readRect(destRecPtr)
    const ori = this._readVec2(originPtr), tint = this._readColor(tintPtr)
    const ctx = this._ctx
    ctx.save()
    ctx.globalAlpha = tint.a / 255
    ctx.translate(dst.x, dst.y)
    if (rotation) ctx.rotate(rotation * Math.PI / 180)
    ctx.drawImage(bmp,
      Math.abs(src.x), Math.abs(src.y), Math.abs(src.width), Math.abs(src.height),
      -ori.x, -ori.y, Math.abs(dst.width), Math.abs(dst.height))
    ctx.restore()
  }

  DrawTextureNPatch(texturePtr, nPatchInfoPtr, destRecPtr, originPtr, rotation, tintPtr) {
    // NPatchInfo: source(16), left(4), top(4), right(4), bottom(4), layout(4) = 36 bytes
    const v = this._view
    const src = this._readRect(nPatchInfoPtr)
    const left = v.getInt32(nPatchInfoPtr + 16, true), top = v.getInt32(nPatchInfoPtr + 20, true)
    const right = v.getInt32(nPatchInfoPtr + 24, true), bottom = v.getInt32(nPatchInfoPtr + 28, true)
    const texId = v.getUint32(texturePtr, true), bmp = this._textures.get(texId)
    if (!bmp) return
    const dst = this._readRect(destRecPtr), ori = this._readVec2(originPtr)
    const tint = tintPtr ? this._readColor(tintPtr) : { r: 255, g: 255, b: 255, a: 255 }
    const ctx = this._ctx
    ctx.save(); ctx.globalAlpha = tint.a / 255
    ctx.translate(dst.x - ori.x, dst.y - ori.y)
    if (rotation) ctx.rotate(rotation * Math.PI / 180)
    const dw = dst.width, dh = dst.height
    const midDW = dw - left - right, midDH = dh - top - bottom
    const midSW = src.width - left - right, midSH = src.height - top - bottom
    // Draw 9 patches: TL, TM, TR, ML, MM, MR, BL, BM, BR
    const patches = [
      [src.x, src.y, left, top, 0, 0, left, top],
      [src.x+left, src.y, midSW, top, left, 0, midDW, top],
      [src.x+left+midSW, src.y, right, top, left+midDW, 0, right, top],
      [src.x, src.y+top, left, midSH, 0, top, left, midDH],
      [src.x+left, src.y+top, midSW, midSH, left, top, midDW, midDH],
      [src.x+left+midSW, src.y+top, right, midSH, left+midDW, top, right, midDH],
      [src.x, src.y+top+midSH, left, bottom, 0, top+midDH, left, bottom],
      [src.x+left, src.y+top+midSH, midSW, bottom, left, top+midDH, midDW, bottom],
      [src.x+left+midSW, src.y+top+midSH, right, bottom, left+midDW, top+midDH, right, bottom],
    ]
    for (const [sx,sy,sw,sh,dx,dy,dw2,dh2] of patches) {
      if (sw > 0 && sh > 0 && dw2 > 0 && dh2 > 0)
        ctx.drawImage(bmp, sx, sy, sw, sh, dx, dy, dw2, dh2)
    }
    ctx.restore()
  }

  GetShapesTexture(resultPtr) { this._writeTexture(resultPtr, 0, 0, 0) }
  GetShapesTextureRectangle(resultPtr) { this._writeRect(resultPtr, 0, 0, 0, 0) }
  SetShapesTexture() {}

  // ── Image system ──────────────────────────────────────────────────────────

  LoadImage(resultPtr, fileNamePtr) {
    const fileName = this._readStr(fileNamePtr)
    const id = this._newId()
    this._images.set(id, null)
    this._writeImage(resultPtr, id, 0, 0)
    const bmp = this._fileBitmaps.get(fileName)
    if (bmp) {
      this._bitmapToImage(id, bmp)
      this._writeImage(resultPtr, id, bmp.width, bmp.height)
    } else {
      this._loadImageFetch(id, fileName, resultPtr)
    }
  }

  async _loadImageFetch(id, url, resultPtr) {
    try {
      const data = this._fileData.get(url)
      const bmp = data
        ? await createImageBitmap(new Blob([data]))
        : await createImageBitmap(await (await fetch(url)).blob())
      this._bitmapToImage(id, bmp)
    } catch (e) { console.warn(`LoadImage: ${url}`, e) }
  }

  _bitmapToImage(id, bmp) {
    const oc = new OffscreenCanvas(bmp.width, bmp.height)
    const ctx2 = oc.getContext('2d')
    ctx2.drawImage(bmp, 0, 0)
    const imgData = ctx2.getImageData(0, 0, bmp.width, bmp.height)
    this._images.set(id, { width: bmp.width, height: bmp.height, data: imgData, format: 7 })
  }

  _imgOp(imagePtr, fn) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    fn(img.data.data, img.width, img.height, img)
  }

  _imgDraw(imagePtr, fn) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const oc = new OffscreenCanvas(img.width, img.height)
    const ctx2 = oc.getContext('2d')
    ctx2.putImageData(img.data, 0, 0)
    fn(ctx2, img.width, img.height)
    img.data = ctx2.getImageData(0, 0, img.width, img.height)
  }

  LoadImageFromMemory(resultPtr, fileTypePtr, dataPtr, dataSize) {
    const id = this._newId()
    this._writeImage(resultPtr, id, 0, 0)
    const bytes = new Uint8Array(this._memory.buffer, dataPtr, dataSize)
    const copy = new Uint8Array(bytes)
    createImageBitmap(new Blob([copy])).then(bmp => this._bitmapToImage(id, bmp)).catch(() => {})
    this._images.set(id, null)
  }

  LoadImageFromTexture(resultPtr, texturePtr) {
    const tex = this._readTexture(texturePtr)
    const bmp = this._textures.get(tex.id)
    const id = this._newId()
    if (bmp) {
      const oc = new OffscreenCanvas(tex.width || bmp.width, tex.height || bmp.height)
      oc.getContext('2d').drawImage(bmp, 0, 0)
      const imgData = oc.getContext('2d').getImageData(0, 0, oc.width, oc.height)
      this._images.set(id, { width: oc.width, height: oc.height, data: imgData, format: 7 })
      this._writeImage(resultPtr, id, oc.width, oc.height)
    } else {
      this._images.set(id, null)
      this._writeImage(resultPtr, id, 0, 0)
    }
  }

  LoadImageFromScreen(resultPtr) {
    const id = this._newId()
    const imgData = this._ctx.getImageData(0, 0, this._canvas.width, this._canvas.height)
    this._images.set(id, { width: this._canvas.width, height: this._canvas.height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, this._canvas.width, this._canvas.height)
  }

  LoadImageRaw(resultPtr, fileNamePtr, w, h, format, headerSize) {
    const id = this._newId()
    this._images.set(id, null); this._writeImage(resultPtr, id, 0, 0)
  }

  IsImageValid(imagePtr) {
    const id = this._view.getUint32(imagePtr, true)
    return (this._images.has(id) && this._images.get(id) !== null) ? 1 : 0
  }

  UnloadImage(imagePtr) {
    const id = this._view.getUint32(imagePtr, true)
    this._images.delete(id)
  }

  ExportImageToMemory(imagePtr, fileTypePtr, fileSizePtr) {
    // Async-only in browsers; return 0
    if (fileSizePtr) this._view.setInt32(fileSizePtr, 0, true)
    return 0
  }

  // GenImage functions — create procedural images
  GenImageColor(resultPtr, width, height, colorPtr) {
    const c = this._readColor(colorPtr)
    const id = this._newId()
    const imgData = new ImageData(width, height)
    const data = imgData.data
    for (let i = 0; i < width * height * 4; i += 4) {
      data[i] = c.r; data[i+1] = c.g; data[i+2] = c.b; data[i+3] = c.a
    }
    this._images.set(id, { width, height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, width, height)
  }

  GenImageChecked(resultPtr, width, height, checksX, checksY, col1Ptr, col2Ptr) {
    const c1 = this._readColor(col1Ptr), c2 = this._readColor(col2Ptr)
    const id = this._newId()
    const imgData = new ImageData(width, height)
    const data = imgData.data
    for (let y = 0; y < height; y++) for (let x = 0; x < width; x++) {
      const c = (Math.floor(x / checksX) + Math.floor(y / checksY)) % 2 === 0 ? c1 : c2
      const i = (y * width + x) * 4
      data[i] = c.r; data[i+1] = c.g; data[i+2] = c.b; data[i+3] = c.a
    }
    this._images.set(id, { width, height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, width, height)
  }

  GenImageGradientLinear(resultPtr, width, height, direction, startPtr, endPtr) {
    const cs = this._readColor(startPtr), ce = this._readColor(endPtr)
    const id = this._newId()
    const imgData = new ImageData(width, height)
    const data = imgData.data
    for (let y = 0; y < height; y++) for (let x = 0; x < width; x++) {
      const t = direction === 0 ? x/width : direction === 1 ? y/height :
                direction === 2 ? 1-x/width : 1-y/height
      const i = (y * width + x) * 4
      data[i] = (cs.r + (ce.r - cs.r) * t) | 0
      data[i+1] = (cs.g + (ce.g - cs.g) * t) | 0
      data[i+2] = (cs.b + (ce.b - cs.b) * t) | 0
      data[i+3] = (cs.a + (ce.a - cs.a) * t) | 0
    }
    this._images.set(id, { width, height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, width, height)
  }

  GenImageGradientRadial(resultPtr, width, height, density, innerPtr, outerPtr) {
    const ci = this._readColor(innerPtr), co = this._readColor(outerPtr)
    const id = this._newId()
    const imgData = new ImageData(width, height)
    const data = imgData.data
    const cx = width / 2, cy = height / 2, maxR = Math.sqrt(cx*cx + cy*cy)
    for (let y = 0; y < height; y++) for (let x = 0; x < width; x++) {
      const t = Math.min(1, Math.sqrt((x-cx)**2 + (y-cy)**2) / maxR / (1 - density + 0.001))
      const i = (y * width + x) * 4
      data[i] = (ci.r + (co.r - ci.r) * t) | 0
      data[i+1] = (ci.g + (co.g - ci.g) * t) | 0
      data[i+2] = (ci.b + (co.b - ci.b) * t) | 0
      data[i+3] = (ci.a + (co.a - ci.a) * t) | 0
    }
    this._images.set(id, { width, height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, width, height)
  }

  GenImageGradientSquare(resultPtr, width, height, density, innerPtr, outerPtr) {
    const inner = this._readColor(innerPtr), outer = this._readColor(outerPtr)
    const id = this._newId(), imgData = new ImageData(width, height), data = imgData.data
    const cx = width / 2, cy = height / 2
    for (let y = 0; y < height; y++) for (let x = 0; x < width; x++) {
      const dx = Math.abs(x - cx) / cx, dy = Math.abs(y - cy) / cy
      const dist = Math.max(dx, dy) // Chebyshev distance for square
      const t = Math.max(0, Math.min(1, (dist - density) / (1 - density + 0.0001)))
      const i = (y * width + x) * 4
      data[i] = (inner.r + (outer.r - inner.r) * t) | 0
      data[i+1] = (inner.g + (outer.g - inner.g) * t) | 0
      data[i+2] = (inner.b + (outer.b - inner.b) * t) | 0
      data[i+3] = (inner.a + (outer.a - inner.a) * t) | 0
    }
    this._images.set(id, { width, height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, width, height)
  }

  GenImageWhiteNoise(resultPtr, width, height, factor) {
    const id = this._newId()
    const imgData = new ImageData(width, height)
    const data = imgData.data
    for (let i = 0; i < data.length; i += 4) {
      const v = Math.random() < factor ? 255 : 0
      data[i] = data[i+1] = data[i+2] = v; data[i+3] = 255
    }
    this._images.set(id, { width, height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, width, height)
  }

  GenImagePerlinNoise(resultPtr, width, height, offsetX, offsetY, scale) {
    // Simple value-noise approximation (no true Perlin without a library)
    const id = this._newId()
    const imgData = new ImageData(width, height)
    const data = imgData.data
    for (let y = 0; y < height; y++) for (let x = 0; x < width; x++) {
      const nx = (x + offsetX) / scale, ny = (y + offsetY) / scale
      const v = (Math.sin(nx * 127.1 + ny * 311.7) * 43758.5453) % 1
      const c = (Math.abs(v) * 255) | 0
      const i = (y * width + x) * 4
      data[i] = data[i+1] = data[i+2] = c; data[i+3] = 255
    }
    this._images.set(id, { width, height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, width, height)
  }

  GenImageCellular(resultPtr, width, height, tileSize) {
    // Worley noise
    const id = this._newId(), imgData = new ImageData(width, height), data = imgData.data
    const cols = Math.ceil(width / tileSize), rows = Math.ceil(height / tileSize)
    const points = []
    for (let r = 0; r < rows; r++) for (let c = 0; c < cols; c++) {
      points.push({ x: (c + Math.random()) * tileSize, y: (r + Math.random()) * tileSize })
    }
    for (let y = 0; y < height; y++) for (let x = 0; x < width; x++) {
      let minDist = Infinity
      for (const p of points) { const d = (x - p.x) ** 2 + (y - p.y) ** 2; if (d < minDist) minDist = d }
      const v = Math.min(255, (Math.sqrt(minDist) / tileSize * 255) | 0)
      const i = (y * width + x) * 4; data[i] = data[i+1] = data[i+2] = v; data[i+3] = 255
    }
    this._images.set(id, { width, height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, width, height)
  }

  GenImageText(resultPtr, width, height, textPtr) {
    const text = this._readStr(textPtr)
    const id = this._newId()
    const oc = new OffscreenCanvas(width, height)
    const ctx2 = oc.getContext('2d')
    ctx2.fillStyle = 'black'; ctx2.fillRect(0, 0, width, height)
    ctx2.fillStyle = 'white'; ctx2.font = `${Math.max(8, height)}px monospace`; ctx2.textBaseline = 'top'
    ctx2.fillText(text, 0, 0)
    const imgData = ctx2.getImageData(0, 0, width, height)
    this._images.set(id, { width, height, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, width, height)
  }

  LoadImageColors(resultPtr, imagePtr) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img) { this._view.setUint32(resultPtr, 0, true); return }
    const ptr = this._exports.malloc(img.width * img.height * 4)
    new Uint8Array(this._memory.buffer, ptr, img.data.data.length).set(img.data.data)
    this._view.setUint32(resultPtr, ptr, true)
  }

  UnloadImageColors(colorsPtr) {
    const ptr = this._view.getUint32(colorsPtr, true)
    if (ptr && this._exports?.free) this._exports.free(ptr)
  }

  LoadImagePalette(resultPtr, imagePtr, maxPaletteSize, colorCountPtr) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) { this._view.setUint32(resultPtr, 0, true); if (colorCountPtr) this._view.setInt32(colorCountPtr, 0, true); return }
    const d = img.data.data, seen = new Map()
    for (let i = 0; i < d.length; i += 4) {
      const key = (d[i] << 24) | (d[i+1] << 16) | (d[i+2] << 8) | d[i+3]
      if (!seen.has(key)) { seen.set(key, [d[i], d[i+1], d[i+2], d[i+3]]); if (seen.size >= maxPaletteSize) break }
    }
    const count = seen.size, ptr = this._exports.malloc(count * 4)
    const out = new Uint8Array(this._memory.buffer, ptr, count * 4)
    let offset = 0
    for (const [, c] of seen) { out[offset++] = c[0]; out[offset++] = c[1]; out[offset++] = c[2]; out[offset++] = c[3] }
    this._view.setUint32(resultPtr, ptr, true)
    if (colorCountPtr) this._view.setInt32(colorCountPtr, count, true)
  }

  UnloadImagePalette(palettePtr) {
    const ptr = this._view.getUint32(palettePtr, true)
    if (ptr && this._exports?.free) this._exports.free(ptr)
  }

  GetImageColor(resultPtr, imagePtr, x, y) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img) { this._writeColor(resultPtr, 0, 0, 0, 0); return }
    const i = (y * img.width + x) * 4, d = img.data.data
    this._writeColor(resultPtr, d[i], d[i+1], d[i+2], d[i+3])
  }

  GetImageAlphaBorder(resultPtr, imagePtr, threshold) {
    const id = this._view.getUint32(imagePtr, true), img = this._images.get(id)
    if (!img?.data) { this._writeRect(resultPtr, 0, 0, 0, 0); return }
    const d = img.data.data, w = img.width, h = img.height, threshI = (threshold * 255) | 0
    let minX = w, minY = h, maxX = 0, maxY = 0
    for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
      if (d[(y * w + x) * 4 + 3] > threshI) {
        if (x < minX) minX = x; if (x > maxX) maxX = x
        if (y < minY) minY = y; if (y > maxY) maxY = y
      }
    }
    if (minX > maxX) { this._writeRect(resultPtr, 0, 0, 0, 0); return }
    this._writeRect(resultPtr, minX, minY, maxX - minX + 1, maxY - minY + 1)
  }
  GetPixelColor(resultPtr, pixelPtr, format) {
    const v = this._view
    // format 7 = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    const r = v.getUint8(pixelPtr), g = v.getUint8(pixelPtr+1), b = v.getUint8(pixelPtr+2), a = v.getUint8(pixelPtr+3)
    this._writeColor(resultPtr, r, g, b, a)
  }
  GetPixelDataSize(width, height, format) { return width * height * 4 }

  SetPixelColor(pixelPtr, colorPtr, format) {
    const c = this._readColor(colorPtr)
    this._writeColor(pixelPtr, c.r, c.g, c.b, c.a)
  }

  // Image manipulation (basic implementations)
  ImageCopy(resultPtr, imagePtr) {
    const id = this._view.getUint32(imagePtr, true)
    const src = this._images.get(id)
    const newId = this._newId()
    if (src) {
      const newData = new ImageData(new Uint8ClampedArray(src.data.data), src.width, src.height)
      this._images.set(newId, { width: src.width, height: src.height, data: newData, format: src.format })
      this._writeImage(resultPtr, newId, src.width, src.height)
    } else {
      this._images.set(newId, null); this._writeImage(resultPtr, newId, 0, 0)
    }
  }

  ImageCrop(imagePtr, cropPtr) {
    const id = this._view.getUint32(imagePtr, true)
    const src = this._images.get(id)
    if (!src) return
    const crop = this._readRect(cropPtr)
    const x = crop.x | 0, y = crop.y | 0, w = crop.width | 0, h = crop.height | 0
    const newData = new ImageData(w, h)
    for (let row = 0; row < h; row++) {
      const si = ((y + row) * src.width + x) * 4, di = row * w * 4
      newData.data.set(src.data.data.subarray(si, si + w * 4), di)
    }
    const v = this._view
    v.setInt32(imagePtr + 4, w, true); v.setInt32(imagePtr + 8, h, true)
    this._images.set(id, { width: w, height: h, data: newData, format: src.format })
  }

  ImageFlipVertical(imagePtr) {
    const id = this._view.getUint32(imagePtr, true)
    const src = this._images.get(id)
    if (!src) return
    const newData = new ImageData(src.width, src.height)
    for (let y = 0; y < src.height; y++) {
      const si = (src.height - 1 - y) * src.width * 4, di = y * src.width * 4
      newData.data.set(src.data.data.subarray(si, si + src.width * 4), di)
    }
    src.data = newData
  }

  ImageFlipHorizontal(imagePtr) {
    const id = this._view.getUint32(imagePtr, true)
    const src = this._images.get(id)
    if (!src) return
    const newData = new ImageData(src.width, src.height)
    for (let y = 0; y < src.height; y++)
      for (let x = 0; x < src.width; x++) {
        const si = (y * src.width + (src.width - 1 - x)) * 4, di = (y * src.width + x) * 4
        newData.data[di] = src.data.data[si]; newData.data[di+1] = src.data.data[si+1]
        newData.data[di+2] = src.data.data[si+2]; newData.data[di+3] = src.data.data[si+3]
      }
    src.data = newData
  }

  ImageClearBackground(imagePtr, colorPtr) {
    const c = this._readColor(colorPtr)
    this._imgOp(imagePtr, (d, w, h) => {
      for (let i = 0; i < w * h * 4; i += 4) { d[i] = c.r; d[i+1] = c.g; d[i+2] = c.b; d[i+3] = c.a }
    })
  }

  ImageColorGrayscale(imagePtr) {
    this._imgOp(imagePtr, (d, w, h) => {
      for (let i = 0; i < w * h * 4; i += 4) {
        const g = (d[i] * 0.299 + d[i+1] * 0.587 + d[i+2] * 0.114) | 0
        d[i] = d[i+1] = d[i+2] = g
      }
    })
  }

  ImageColorInvert(imagePtr) {
    this._imgOp(imagePtr, (d, w, h) => {
      for (let i = 0; i < w * h * 4; i += 4) { d[i] = 255 - d[i]; d[i+1] = 255 - d[i+1]; d[i+2] = 255 - d[i+2] }
    })
  }

  ImageColorTint(imagePtr, colorPtr) {
    const c = this._readColor(colorPtr)
    this._imgOp(imagePtr, (d, w, h) => {
      for (let i = 0; i < w * h * 4; i += 4) {
        d[i] = (d[i] * c.r / 255) | 0; d[i+1] = (d[i+1] * c.g / 255) | 0
        d[i+2] = (d[i+2] * c.b / 255) | 0; d[i+3] = (d[i+3] * c.a / 255) | 0
      }
    })
  }

  ImageColorBrightness(imagePtr, brightness) {
    this._imgOp(imagePtr, (d, w, h) => {
      for (let i = 0; i < w * h * 4; i += 4) {
        d[i] = Math.max(0, Math.min(255, d[i] + brightness))
        d[i+1] = Math.max(0, Math.min(255, d[i+1] + brightness))
        d[i+2] = Math.max(0, Math.min(255, d[i+2] + brightness))
      }
    })
  }

  ImageColorContrast(imagePtr, contrast) {
    this._imgOp(imagePtr, (d, w, h) => {
      const f = (259 * (contrast + 255)) / (255 * (259 - contrast))
      for (let i = 0; i < w * h * 4; i += 4) {
        d[i] = Math.max(0, Math.min(255, (f * (d[i] - 128) + 128) | 0))
        d[i+1] = Math.max(0, Math.min(255, (f * (d[i+1] - 128) + 128) | 0))
        d[i+2] = Math.max(0, Math.min(255, (f * (d[i+2] - 128) + 128) | 0))
      }
    })
  }

  ImageColorReplace(imagePtr, colorPtr, replacePtr) {
    const c = this._readColor(colorPtr), rep = this._readColor(replacePtr)
    this._imgOp(imagePtr, (d, w, h) => {
      for (let i = 0; i < w * h * 4; i += 4) {
        if (d[i] === c.r && d[i+1] === c.g && d[i+2] === c.b && d[i+3] === c.a) {
          d[i] = rep.r; d[i+1] = rep.g; d[i+2] = rep.b; d[i+3] = rep.a
        }
      }
    })
  }

  ImageAlphaClear(imagePtr, colorPtr, threshold) {
    const threshI = (threshold * 255) | 0
    this._imgOp(imagePtr, (d, w, h) => {
      for (let i = 0; i < w * h * 4; i += 4) { if (d[i+3] <= threshI) d[i+3] = 0 }
    })
  }

  ImageAlphaMask(imagePtr, alphaMaskPtr) {
    const maskId = this._view.getUint32(alphaMaskPtr, true)
    const mask = this._images.get(maskId)
    if (!mask?.data) return
    const md = mask.data.data
    this._imgOp(imagePtr, (d, w, h) => {
      for (let i = 0; i < w * h * 4; i += 4) d[i+3] = md[i] // mask red channel → dst alpha
    })
  }

  ImageAlphaPremultiply(imagePtr) {
    this._imgOp(imagePtr, (d, w, h) => {
      for (let i = 0; i < w * h * 4; i += 4) {
        const a = d[i+3] / 255
        d[i] = (d[i] * a) | 0; d[i+1] = (d[i+1] * a) | 0; d[i+2] = (d[i+2] * a) | 0
      }
    })
  }

  ImageAlphaCrop(imagePtr, threshold) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const d = img.data.data, w = img.width, h = img.height, threshI = (threshold * 255) | 0
    let minX = w, minY = h, maxX = 0, maxY = 0
    for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
      if (d[(y * w + x) * 4 + 3] > threshI) {
        if (x < minX) minX = x; if (x > maxX) maxX = x
        if (y < minY) minY = y; if (y > maxY) maxY = y
      }
    }
    if (minX > maxX || minY > maxY) { img.width = 1; img.height = 1; img.data = new ImageData(1, 1); return }
    const nw = maxX - minX + 1, nh = maxY - minY + 1
    const newData = new ImageData(nw, nh)
    for (let y = 0; y < nh; y++) {
      const si = ((y + minY) * w + minX) * 4
      newData.data.set(d.subarray(si, si + nw * 4), y * nw * 4)
    }
    img.width = nw; img.height = nh; img.data = newData
    this._view.setInt32(imagePtr + 4, nw, true); this._view.setInt32(imagePtr + 8, nh, true)
  }

  ImageBlurGaussian(imagePtr, blurSize) {
    if (blurSize < 1) return
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const oc = new OffscreenCanvas(img.width, img.height)
    const ctx2 = oc.getContext('2d')
    ctx2.filter = `blur(${blurSize}px)`
    ctx2.putImageData(img.data, 0, 0)
    ctx2.drawImage(oc, 0, 0)
    img.data = ctx2.getImageData(0, 0, img.width, img.height)
  }

  ImageKernelConvolution(imagePtr, kernelPtr, kernelSize) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const v = this._view, w = img.width, h = img.height, d = img.data.data
    const kernel = []
    for (let i = 0; i < kernelSize; i++) kernel.push(v.getFloat32(kernelPtr + i * 4, true))
    const kSide = Math.round(Math.sqrt(kernelSize)), kHalf = kSide >> 1
    const out = new Uint8ClampedArray(d.length)
    for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
      let r = 0, g = 0, b = 0, a = 0
      for (let ky = 0; ky < kSide; ky++) for (let kx = 0; kx < kSide; kx++) {
        const sx = Math.min(w - 1, Math.max(0, x + kx - kHalf))
        const sy = Math.min(h - 1, Math.max(0, y + ky - kHalf))
        const ki = (ky * kSide + kx), pi = (sy * w + sx) * 4
        r += d[pi] * kernel[ki]; g += d[pi+1] * kernel[ki]
        b += d[pi+2] * kernel[ki]; a += d[pi+3] * kernel[ki]
      }
      const oi = (y * w + x) * 4
      out[oi] = r; out[oi+1] = g; out[oi+2] = b; out[oi+3] = a
    }
    img.data = new ImageData(out, w, h)
  }

  ImageDither(imagePtr, rBpp, gBpp, bBpp, aBpp) {
    // Floyd-Steinberg dithering by reducing bit depth
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const d = new Uint8ClampedArray(img.data.data), w = img.width, h = img.height
    const rLevels = (1 << rBpp) - 1 || 1, gLevels = (1 << gBpp) - 1 || 1
    const bLevels = (1 << bBpp) - 1 || 1, aLevels = (1 << aBpp) - 1 || 1
    const quantize = (v, levels) => Math.round(v / 255 * levels) / levels * 255
    const spread = (arr, x, y, ch, err, f) => {
      if (x >= 0 && x < w && y >= 0 && y < h) arr[(y * w + x) * 4 + ch] = Math.max(0, Math.min(255, arr[(y * w + x) * 4 + ch] + err * f))
    }
    for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
      const i = (y * w + x) * 4
      for (const [ch, levels] of [[0, rLevels], [1, gLevels], [2, bLevels], [3, aLevels]]) {
        const old = d[i + ch], nw2 = quantize(old, levels), err = old - nw2
        d[i + ch] = nw2
        spread(d, x + 1, y, ch, err, 7/16); spread(d, x - 1, y + 1, ch, err, 3/16)
        spread(d, x, y + 1, ch, err, 5/16); spread(d, x + 1, y + 1, ch, err, 1/16)
      }
    }
    img.data = new ImageData(d, w, h)
  }

  ImageFormat(imagePtr, newFormat) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (img) { img.format = newFormat; this._view.setInt32(imagePtr + 16, newFormat, true) }
  }

  ImageMipmaps(imagePtr) {} // No-op: canvas doesn't use mipmaps

  ImageToPOT(imagePtr, colorPtr) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const nextPOT = n => { let p = 1; while (p < n) p <<= 1; return p }
    const nw = nextPOT(img.width), nh = nextPOT(img.height)
    if (nw === img.width && nh === img.height) return
    const c = this._readColor(colorPtr)
    const newData = new ImageData(nw, nh)
    const d = newData.data
    for (let i = 0; i < nw * nh * 4; i += 4) { d[i] = c.r; d[i+1] = c.g; d[i+2] = c.b; d[i+3] = c.a }
    const src = img.data.data
    for (let y = 0; y < img.height; y++)
      for (let x = 0; x < img.width; x++) {
        const si = (y * img.width + x) * 4, di = (y * nw + x) * 4
        d[di] = src[si]; d[di+1] = src[si+1]; d[di+2] = src[si+2]; d[di+3] = src[si+3]
      }
    img.width = nw; img.height = nh; img.data = newData
    this._view.setInt32(imagePtr + 4, nw, true); this._view.setInt32(imagePtr + 8, nh, true)
  }

  ImageResize(imagePtr, newW, newH) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const src = new OffscreenCanvas(img.width, img.height)
    src.getContext('2d').putImageData(img.data, 0, 0)
    const dst = new OffscreenCanvas(newW, newH)
    const ctx2 = dst.getContext('2d')
    ctx2.imageSmoothingEnabled = true
    ctx2.drawImage(src, 0, 0, newW, newH)
    img.width = newW; img.height = newH; img.data = ctx2.getImageData(0, 0, newW, newH)
    this._view.setInt32(imagePtr + 4, newW, true); this._view.setInt32(imagePtr + 8, newH, true)
  }

  ImageResizeNN(imagePtr, newW, newH) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const src = new OffscreenCanvas(img.width, img.height)
    src.getContext('2d').putImageData(img.data, 0, 0)
    const dst = new OffscreenCanvas(newW, newH)
    const ctx2 = dst.getContext('2d')
    ctx2.imageSmoothingEnabled = false
    ctx2.drawImage(src, 0, 0, newW, newH)
    img.width = newW; img.height = newH; img.data = ctx2.getImageData(0, 0, newW, newH)
    this._view.setInt32(imagePtr + 4, newW, true); this._view.setInt32(imagePtr + 8, newH, true)
  }

  ImageResizeCanvas(imagePtr, newW, newH, offsetX, offsetY, colorPtr) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const c = this._readColor(colorPtr)
    const newData = new ImageData(newW, newH)
    const d = newData.data
    for (let i = 0; i < newW * newH * 4; i += 4) { d[i] = c.r; d[i+1] = c.g; d[i+2] = c.b; d[i+3] = c.a }
    const src = img.data.data, sw = img.width, sh = img.height
    for (let y = 0; y < sh; y++) for (let x = 0; x < sw; x++) {
      const dx = x + offsetX, dy = y + offsetY
      if (dx >= 0 && dx < newW && dy >= 0 && dy < newH) {
        const si = (y * sw + x) * 4, di = (dy * newW + dx) * 4
        d[di] = src[si]; d[di+1] = src[si+1]; d[di+2] = src[si+2]; d[di+3] = src[si+3]
      }
    }
    img.width = newW; img.height = newH; img.data = newData
    this._view.setInt32(imagePtr + 4, newW, true); this._view.setInt32(imagePtr + 8, newH, true)
  }

  ImageRotate(imagePtr, degrees) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const rad = degrees * Math.PI / 180
    const cos = Math.abs(Math.cos(rad)), sin = Math.abs(Math.sin(rad))
    const nw = Math.round(img.width * cos + img.height * sin)
    const nh = Math.round(img.width * sin + img.height * cos)
    const src = new OffscreenCanvas(img.width, img.height)
    src.getContext('2d').putImageData(img.data, 0, 0)
    const dst = new OffscreenCanvas(nw, nh)
    const ctx2 = dst.getContext('2d')
    ctx2.translate(nw / 2, nh / 2); ctx2.rotate(rad)
    ctx2.drawImage(src, -img.width / 2, -img.height / 2)
    img.width = nw; img.height = nh; img.data = ctx2.getImageData(0, 0, nw, nh)
    this._view.setInt32(imagePtr + 4, nw, true); this._view.setInt32(imagePtr + 8, nh, true)
  }

  ImageRotateCW(imagePtr) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const src = new OffscreenCanvas(img.width, img.height)
    src.getContext('2d').putImageData(img.data, 0, 0)
    const dst = new OffscreenCanvas(img.height, img.width)
    const ctx2 = dst.getContext('2d')
    ctx2.translate(img.height, 0); ctx2.rotate(Math.PI / 2)
    ctx2.drawImage(src, 0, 0)
    const nw = img.height, nh = img.width
    img.width = nw; img.height = nh; img.data = ctx2.getImageData(0, 0, nw, nh)
    this._view.setInt32(imagePtr + 4, nw, true); this._view.setInt32(imagePtr + 8, nh, true)
  }

  ImageRotateCCW(imagePtr) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) return
    const src = new OffscreenCanvas(img.width, img.height)
    src.getContext('2d').putImageData(img.data, 0, 0)
    const dst = new OffscreenCanvas(img.height, img.width)
    const ctx2 = dst.getContext('2d')
    ctx2.translate(0, img.width); ctx2.rotate(-Math.PI / 2)
    ctx2.drawImage(src, 0, 0)
    const nw = img.height, nh = img.width
    img.width = nw; img.height = nh; img.data = ctx2.getImageData(0, 0, nw, nh)
    this._view.setInt32(imagePtr + 4, nw, true); this._view.setInt32(imagePtr + 8, nh, true)
  }

  ImageFromChannel(resultPtr, imagePtr, selectedChannel) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    if (!img?.data) { this._writeImage(resultPtr, 0, 0, 0); return }
    const newId = this._newId(), w = img.width, h = img.height
    const newData = new ImageData(w, h), src = img.data.data, dst = newData.data
    for (let i = 0; i < w * h * 4; i += 4) {
      const v = src[i + selectedChannel]
      dst[i] = dst[i+1] = dst[i+2] = v; dst[i+3] = 255
    }
    this._images.set(newId, { width: w, height: h, data: newData, format: 7 })
    this._writeImage(resultPtr, newId, w, h)
  }

  ImageFromImage(resultPtr, imagePtr, recPtr) {
    const id = this._view.getUint32(imagePtr, true)
    const img = this._images.get(id)
    const rec = this._readRect(recPtr)
    if (!img?.data) { this._writeImage(resultPtr, 0, 0, 0); return }
    const nw = rec.width | 0, nh = rec.height | 0
    const newId = this._newId(), newData = new ImageData(nw, nh)
    const src = img.data.data, dst = newData.data
    for (let y = 0; y < nh; y++) for (let x = 0; x < nw; x++) {
      const sx = (rec.x | 0) + x, sy = (rec.y | 0) + y
      if (sx >= 0 && sx < img.width && sy >= 0 && sy < img.height) {
        const si = (sy * img.width + sx) * 4, di = (y * nw + x) * 4
        dst[di] = src[si]; dst[di+1] = src[si+1]; dst[di+2] = src[si+2]; dst[di+3] = src[si+3]
      }
    }
    this._images.set(newId, { width: nw, height: nh, data: newData, format: 7 })
    this._writeImage(resultPtr, newId, nw, nh)
  }

  // Image drawing (draw onto image data via OffscreenCanvas)
  _imgColor(colorPtr) {
    const c = this._readColor(colorPtr)
    return `rgba(${c.r},${c.g},${c.b},${c.a/255})`
  }

  ImageDrawPixel(imagePtr, posX, posY, colorPtr) {
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.fillStyle = this._imgColor(colorPtr)
      ctx2.fillRect(posX, posY, 1, 1)
    })
  }

  ImageDrawPixelV(imagePtr, posPtr, colorPtr) {
    const p = this._readVec2(posPtr)
    this.ImageDrawPixel(imagePtr, p.x | 0, p.y | 0, colorPtr)
  }

  ImageDrawLine(imagePtr, x1, y1, x2, y2, colorPtr) {
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.strokeStyle = this._imgColor(colorPtr)
      ctx2.lineWidth = 1
      ctx2.beginPath(); ctx2.moveTo(x1, y1); ctx2.lineTo(x2, y2); ctx2.stroke()
    })
  }

  ImageDrawLineV(imagePtr, startPtr, endPtr, colorPtr) {
    const s = this._readVec2(startPtr), e = this._readVec2(endPtr)
    this.ImageDrawLine(imagePtr, s.x, s.y, e.x, e.y, colorPtr)
  }

  ImageDrawLineEx(imagePtr, startPtr, endPtr, thick, colorPtr) {
    const s = this._readVec2(startPtr), e = this._readVec2(endPtr)
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.strokeStyle = this._imgColor(colorPtr)
      ctx2.lineWidth = thick
      ctx2.beginPath(); ctx2.moveTo(s.x, s.y); ctx2.lineTo(e.x, e.y); ctx2.stroke()
    })
  }

  ImageDrawCircle(imagePtr, cx, cy, radius, colorPtr) {
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.fillStyle = this._imgColor(colorPtr)
      ctx2.beginPath(); ctx2.arc(cx, cy, radius, 0, Math.PI * 2); ctx2.fill()
    })
  }

  ImageDrawCircleV(imagePtr, centerPtr, radius, colorPtr) {
    const c = this._readVec2(centerPtr)
    this.ImageDrawCircle(imagePtr, c.x, c.y, radius, colorPtr)
  }

  ImageDrawCircleLines(imagePtr, cx, cy, radius, colorPtr) {
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.strokeStyle = this._imgColor(colorPtr)
      ctx2.lineWidth = 1
      ctx2.beginPath(); ctx2.arc(cx, cy, radius, 0, Math.PI * 2); ctx2.stroke()
    })
  }

  ImageDrawCircleLinesV(imagePtr, centerPtr, radius, colorPtr) {
    const c = this._readVec2(centerPtr)
    this.ImageDrawCircleLines(imagePtr, c.x, c.y, radius, colorPtr)
  }

  ImageDrawRectangle(imagePtr, posX, posY, w, h, colorPtr) {
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.fillStyle = this._imgColor(colorPtr)
      ctx2.fillRect(posX, posY, w, h)
    })
  }

  ImageDrawRectangleV(imagePtr, posPtr, sizePtr, colorPtr) {
    const p = this._readVec2(posPtr), s = this._readVec2(sizePtr)
    this.ImageDrawRectangle(imagePtr, p.x, p.y, s.x, s.y, colorPtr)
  }

  ImageDrawRectangleRec(imagePtr, recPtr, colorPtr) {
    const r = this._readRect(recPtr)
    this.ImageDrawRectangle(imagePtr, r.x, r.y, r.width, r.height, colorPtr)
  }

  ImageDrawRectangleLines(imagePtr, recPtr, thick, colorPtr) {
    const r = this._readRect(recPtr)
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.strokeStyle = this._imgColor(colorPtr)
      ctx2.lineWidth = thick
      ctx2.strokeRect(r.x + thick/2, r.y + thick/2, r.width - thick, r.height - thick)
    })
  }

  ImageDrawRectangleLinesEx(imagePtr, recPtr, thick, colorPtr) {
    this.ImageDrawRectangleLines(imagePtr, recPtr, thick, colorPtr)
  }

  ImageDrawTriangle(imagePtr, v1Ptr, v2Ptr, v3Ptr, colorPtr) {
    const v1 = this._readVec2(v1Ptr), v2 = this._readVec2(v2Ptr), v3 = this._readVec2(v3Ptr)
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.fillStyle = this._imgColor(colorPtr)
      ctx2.beginPath(); ctx2.moveTo(v1.x, v1.y); ctx2.lineTo(v2.x, v2.y); ctx2.lineTo(v3.x, v3.y)
      ctx2.closePath(); ctx2.fill()
    })
  }

  ImageDrawTriangleEx(imagePtr, v1Ptr, v2Ptr, v3Ptr, c1Ptr, c2Ptr, c3Ptr) {
    this.ImageDrawTriangle(imagePtr, v1Ptr, v2Ptr, v3Ptr, c1Ptr) // approximate: ignore per-vertex colors
  }

  ImageDrawTriangleGradient(imagePtr, v1Ptr, v2Ptr, v3Ptr, c1Ptr, c2Ptr, c3Ptr) {
    this.ImageDrawTriangle(imagePtr, v1Ptr, v2Ptr, v3Ptr, c1Ptr)
  }

  ImageDrawTriangleLines(imagePtr, v1Ptr, v2Ptr, v3Ptr, colorPtr) {
    const v1 = this._readVec2(v1Ptr), v2 = this._readVec2(v2Ptr), v3 = this._readVec2(v3Ptr)
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.strokeStyle = this._imgColor(colorPtr)
      ctx2.lineWidth = 1
      ctx2.beginPath(); ctx2.moveTo(v1.x, v1.y); ctx2.lineTo(v2.x, v2.y); ctx2.lineTo(v3.x, v3.y)
      ctx2.closePath(); ctx2.stroke()
    })
  }

  ImageDrawTriangleFan(imagePtr, pointsPtr, pointCount, colorPtr) {
    if (pointCount < 3) return
    const v = this._view
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.fillStyle = this._imgColor(colorPtr)
      ctx2.beginPath()
      const x0 = v.getFloat32(pointsPtr, true), y0 = v.getFloat32(pointsPtr + 4, true)
      ctx2.moveTo(x0, y0)
      for (let i = 1; i < pointCount; i++) ctx2.lineTo(v.getFloat32(pointsPtr + i*8, true), v.getFloat32(pointsPtr + i*8 + 4, true))
      ctx2.closePath(); ctx2.fill()
    })
  }

  ImageDrawTriangleStrip(imagePtr, pointsPtr, pointCount, colorPtr) {
    if (pointCount < 3) return
    const v = this._view
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.fillStyle = this._imgColor(colorPtr)
      for (let i = 0; i < pointCount - 2; i++) {
        const p = i * 8
        const x1 = v.getFloat32(pointsPtr + p, true), y1 = v.getFloat32(pointsPtr + p + 4, true)
        const x2 = v.getFloat32(pointsPtr + p + 8, true), y2 = v.getFloat32(pointsPtr + p + 12, true)
        const x3 = v.getFloat32(pointsPtr + p + 16, true), y3 = v.getFloat32(pointsPtr + p + 20, true)
        ctx2.beginPath(); ctx2.moveTo(x1, y1); ctx2.lineTo(x2, y2); ctx2.lineTo(x3, y3)
        ctx2.closePath(); ctx2.fill()
      }
    })
  }

  ImageDraw(dstPtr, srcPtr, srcRecPtr, dstRecPtr, tintPtr) {
    const dstId = this._view.getUint32(dstPtr, true)
    const srcId = this._view.getUint32(srcPtr, true)
    const dst = this._images.get(dstId), src = this._images.get(srcId)
    if (!dst?.data || !src?.data) return
    const sr = this._readRect(srcRecPtr), dr = this._readRect(dstRecPtr)
    const tint = tintPtr ? this._readColor(tintPtr) : { r: 255, g: 255, b: 255, a: 255 }
    const srcOc = new OffscreenCanvas(src.width, src.height)
    srcOc.getContext('2d').putImageData(src.data, 0, 0)
    const oc = new OffscreenCanvas(dst.width, dst.height)
    const ctx2 = oc.getContext('2d')
    ctx2.putImageData(dst.data, 0, 0)
    ctx2.globalAlpha = tint.a / 255
    ctx2.drawImage(srcOc, sr.x, sr.y, sr.width, sr.height, dr.x, dr.y, dr.width, dr.height)
    ctx2.globalAlpha = 1
    dst.data = ctx2.getImageData(0, 0, dst.width, dst.height)
  }

  ImageDrawText(imagePtr, textPtr, posX, posY, fontSize, colorPtr) {
    const text = this._readStr(textPtr)
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.fillStyle = this._imgColor(colorPtr)
      ctx2.font = `${fontSize}px monospace`
      ctx2.textBaseline = 'top'
      ctx2.fillText(text, posX, posY)
    })
  }

  ImageDrawTextEx(imagePtr, fontPtr, textPtr, posPtr, fontSize, spacing, tintPtr) {
    const text = this._readStr(textPtr), pos = this._readVec2(posPtr)
    const family = this._fontFamily(fontPtr)
    this._imgDraw(imagePtr, (ctx2) => {
      ctx2.fillStyle = this._imgColor(tintPtr)
      ctx2.font = `${fontSize}px ${family}`
      ctx2.textBaseline = 'top'
      ctx2.letterSpacing = `${spacing}px`
      ctx2.fillText(text, pos.x, pos.y)
    })
  }

  GenImageFontAtlas(resultPtr) { this._writeImage(resultPtr, 0, 0, 0) }
  ExportMesh() { return 0 }
  ExportMeshAsCode() { return 0 }

  // ── Audio system ──────────────────────────────────────────────────────────

  InitAudioDevice() { this._audioReady = true }
  CloseAudioDevice() { this._audioCtx?.close(); this._audioCtx = null; this._audioReady = false }
  IsAudioDeviceReady() { return this._audioReady ? 1 : 0 }

  SetMasterVolume(volume) {
    this._masterVolume = Math.max(0, Math.min(1, volume))
    if (this._masterGain) this._masterGain.gain.value = this._masterVolume
  }
  GetMasterVolume() { return this._masterVolume }

  // Load sound from file
  LoadSound(resultPtr, fileNamePtr) {
    const fileName = this._readStr(fileNamePtr)
    const id = this._newId()
    this._sounds.set(id, { buffer: null, sources: new Set() })
    this._writeSound(resultPtr, id)
    this._loadSoundFetch(id, fileName)
  }

  async _loadSoundFetch(id, url) {
    try {
      const data = this._fileData.get(url)
      const ab = data ? data.buffer.slice() : await (await fetch(url)).arrayBuffer()
      const buf = await this._audio.decodeAudioData(ab)
      const snd = this._sounds.get(id)
      if (snd) { snd.buffer = buf; snd.frameCount = buf.length; snd.sampleRate = buf.sampleRate }
    } catch (e) { console.warn(`LoadSound: ${url}`, e) }
  }

  LoadSoundFromWave(resultPtr, wavePtr) {
    const waveId = this._view.getUint32(wavePtr + 16, true) // data field
    const id = this._newId()
    const waveSnd = this._sounds.get(waveId)
    this._sounds.set(id, waveSnd ? { buffer: waveSnd.buffer, sources: new Set() } : { buffer: null, sources: new Set() })
    this._writeSound(resultPtr, id)
  }

  LoadSoundAlias(resultPtr, sourcePtr) {
    const srcId = this._getSoundId(sourcePtr)
    const id = this._newId()
    const src = this._sounds.get(srcId)
    this._sounds.set(id, src ? { buffer: src.buffer, sources: new Set() } : { buffer: null, sources: new Set() })
    this._writeSound(resultPtr, id)
  }

  IsSoundValid(soundPtr) {
    const id = this._getSoundId(soundPtr)
    return (this._sounds.has(id) && this._sounds.get(id).buffer !== null) ? 1 : 0
  }

  PlaySound(soundPtr) {
    const id = this._getSoundId(soundPtr)
    const snd = this._sounds.get(id)
    if (!snd?.buffer) return
    const src = this._audio.createBufferSource()
    src.buffer = snd.buffer
    if (snd.pitch) src.playbackRate.value = snd.pitch
    const gain = this._audio.createGain(); gain.gain.value = snd.volume ?? 1
    if (snd.pan != null) {
      const panner = this._audio.createStereoPanner(); panner.pan.value = snd.pan
      src.connect(panner); panner.connect(gain)
    } else { src.connect(gain) }
    gain.connect(this._masterGain)
    src.start(); snd.startTime = this._audio.currentTime
    snd.sources.add(src); src.onended = () => snd.sources.delete(src)
  }

  StopSound(soundPtr) {
    const id = this._getSoundId(soundPtr)
    const snd = this._sounds.get(id)
    if (!snd) return
    for (const s of snd.sources) try { s.stop() } catch {}
    snd.sources.clear()
  }

  PauseSound(soundPtr) {
    const id = this._getSoundId(soundPtr), snd = this._sounds.get(id)
    if (!snd || snd.paused) return
    snd.paused = true
    snd.pauseOffset = snd.startTime ? (this._audio.currentTime - snd.startTime) : 0
    for (const s of snd.sources) try { s.stop() } catch {}
    snd.sources.clear()
  }

  ResumeSound(soundPtr) {
    const id = this._getSoundId(soundPtr), snd = this._sounds.get(id)
    if (!snd?.paused || !snd.buffer) return
    snd.paused = false
    const src = this._audio.createBufferSource()
    src.buffer = snd.buffer
    if (snd.pitch) src.playbackRate.value = snd.pitch
    const gain = this._audio.createGain(); gain.gain.value = snd.volume ?? 1
    if (snd.pan != null) {
      const panner = this._audio.createStereoPanner(); panner.pan.value = snd.pan
      src.connect(panner); panner.connect(gain)
    } else { src.connect(gain) }
    gain.connect(this._masterGain)
    src.start(0, snd.pauseOffset || 0)
    snd.startTime = this._audio.currentTime - (snd.pauseOffset || 0)
    snd.sources.add(src); src.onended = () => snd.sources.delete(src)
  }

  IsSoundPlaying(soundPtr) {
    const id = this._getSoundId(soundPtr)
    const snd = this._sounds.get(id)
    return snd && snd.sources.size > 0 ? 1 : 0
  }

  SetSoundVolume(soundPtr, volume) {
    const id = this._getSoundId(soundPtr); const snd = this._sounds.get(id)
    if (snd) snd.volume = Math.max(0, Math.min(1, volume))
  }
  SetSoundPitch(soundPtr, pitch) {
    const id = this._getSoundId(soundPtr), snd = this._sounds.get(id)
    if (!snd) return; snd.pitch = pitch
    for (const s of snd.sources) s.playbackRate.value = pitch
  }

  SetSoundPan(soundPtr, pan) {
    const id = this._getSoundId(soundPtr), snd = this._sounds.get(id)
    if (snd) snd.pan = Math.max(-1, Math.min(1, pan * 2 - 1)) // 0..1 → -1..1
  }

  UpdateSound(soundPtr, dataPtr, samplesCount) {
    const id = this._getSoundId(soundPtr), snd = this._sounds.get(id)
    if (!snd) return
    const sampleRate = snd.sampleRate || 44100, channels = snd.channels || 1
    const frames = samplesCount / channels
    const buf = this._audio.createBuffer(channels, frames, sampleRate)
    const raw = new Float32Array(this._memory.buffer, dataPtr, samplesCount)
    for (let ch = 0; ch < channels; ch++) {
      const chan = buf.getChannelData(ch)
      for (let i = 0; i < frames; i++) chan[i] = raw[i * channels + ch]
    }
    snd.buffer = buf
  }

  UnloadSound(soundPtr) {
    this._sounds.delete(this._getSoundId(soundPtr))
  }
  UnloadSoundAlias(soundPtr) { this.UnloadSound(soundPtr) }

  // Wave
  LoadWave(resultPtr, fileNamePtr) {
    const fileName = this._readStr(fileNamePtr)
    const id = this._newId()
    this._sounds.set(id, { buffer: null, sources: new Set() })
    this._writeWave(resultPtr, id)
    this._loadSoundFetch(id, fileName)
  }

  LoadWaveFromMemory(resultPtr, fileTypePtr, dataPtr, dataSize) {
    const id = this._newId()
    this._sounds.set(id, { buffer: null, sources: new Set() })
    this._writeWave(resultPtr, id)
    const bytes = new Uint8Array(this._memory.buffer, dataPtr, dataSize)
    const copy = bytes.buffer.slice(dataPtr, dataPtr + dataSize)
    this._audio.decodeAudioData(copy).then(buf => {
      const snd = this._sounds.get(id); if (snd) snd.buffer = buf
    }).catch(() => {})
  }

  IsWaveValid(wavePtr) {
    const id = this._view.getUint32(wavePtr + 16, true)
    return (this._sounds.has(id) && this._sounds.get(id).buffer !== null) ? 1 : 0
  }

  WaveCopy(resultPtr, wavePtr) {
    const waveId = this._view.getUint32(wavePtr + 16, true)
    const snd = this._sounds.get(waveId)
    const id = this._newId()
    this._sounds.set(id, snd ? { buffer: snd.buffer, sources: new Set() } : { buffer: null, sources: new Set() })
    const fc = snd?.buffer?.length || 0, sr = snd?.buffer?.sampleRate || 44100
    this._writeWave(resultPtr, id, fc, sr)
  }

  WaveCrop(wavePtr, initSample, finalSample) {
    const waveId = this._view.getUint32(wavePtr + 16, true)
    const snd = this._sounds.get(waveId)
    if (!snd?.buffer) return
    const buf = snd.buffer, channels = buf.numberOfChannels
    const frames = finalSample - initSample
    if (frames <= 0) return
    const newBuf = this._audio.createBuffer(channels, frames, buf.sampleRate)
    for (let ch = 0; ch < channels; ch++) {
      const src = buf.getChannelData(ch), dst = newBuf.getChannelData(ch)
      for (let i = 0; i < frames; i++) dst[i] = src[initSample + i]
    }
    snd.buffer = newBuf
    this._view.setUint32(wavePtr, frames, true)
  }

  WaveFormat(wavePtr, sampleRate, sampleSize, channels) {
    const waveId = this._view.getUint32(wavePtr + 16, true)
    const snd = this._sounds.get(waveId)
    if (!snd?.buffer) return
    const buf = snd.buffer, frames = buf.length
    const newBuf = this._audio.createBuffer(channels, frames, sampleRate)
    for (let ch = 0; ch < channels; ch++) {
      const src = buf.getChannelData(ch % buf.numberOfChannels)
      newBuf.getChannelData(ch).set(src.subarray(0, frames))
    }
    snd.buffer = newBuf
    this._view.setUint32(wavePtr + 4, sampleRate, true)
    this._view.setUint32(wavePtr + 8, sampleSize, true)
    this._view.setUint32(wavePtr + 12, channels, true)
  }

  LoadWaveSamples(resultPtr, wavePtr) {
    const waveId = this._view.getUint32(wavePtr + 16, true)
    const snd = this._sounds.get(waveId)
    if (!snd?.buffer) { this._view.setUint32(resultPtr, 0, true); return }
    const buf = snd.buffer, ch = buf.numberOfChannels, frames = buf.length
    const total = frames * ch
    const ptr = this._exports.malloc(total * 4)
    const out = new Float32Array(this._memory.buffer, ptr, total)
    for (let c = 0; c < ch; c++) {
      const data = buf.getChannelData(c)
      for (let i = 0; i < frames; i++) out[i * ch + c] = data[i]
    }
    this._view.setUint32(resultPtr, ptr, true)
  }

  UnloadWaveSamples(samplesPtr) {
    const ptr = this._view.getUint32(samplesPtr, true)
    if (ptr && this._exports?.free) this._exports.free(ptr)
  }
  UnloadWave(wavePtr) { this._sounds.delete(this._view.getUint32(wavePtr + 16, true)) }

  // Music
  LoadMusicStream(resultPtr, fileNamePtr) {
    const fileName = this._readStr(fileNamePtr)
    const id = this._newId()
    this._music.set(id, { buffer: null, source: null, gainNode: null, looping: true, paused: false })
    this._writeSound(resultPtr, id)
    this._loadMusicFetch(id, fileName)
  }

  LoadMusicStreamFromMemory(resultPtr, fileTypePtr, dataPtr, dataSize) {
    const id = this._newId()
    this._music.set(id, { buffer: null, source: null, gainNode: null, looping: true, paused: false })
    this._writeSound(resultPtr, id)
    const bytes = new Uint8Array(this._memory.buffer, dataPtr, dataSize)
    const copy = bytes.buffer.slice(dataPtr, dataPtr + dataSize)
    this._audio.decodeAudioData(copy).then(buf => {
      const m = this._music.get(id); if (m) m.buffer = buf
    }).catch(() => {})
  }

  async _loadMusicFetch(id, url) {
    try {
      const data = this._fileData.get(url)
      const ab = data ? data.buffer.slice() : await (await fetch(url)).arrayBuffer()
      const buf = await this._audio.decodeAudioData(ab)
      const m = this._music.get(id); if (m) m.buffer = buf
    } catch (e) { console.warn(`LoadMusicStream: ${url}`, e) }
  }

  IsMusicValid(musicPtr) {
    const id = this._getSoundId(musicPtr)
    return (this._music.has(id) && this._music.get(id).buffer !== null) ? 1 : 0
  }

  PlayMusicStream(musicPtr) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    if (!m?.buffer) return
    this._startMusicSource(id, 0)
    m.startTime = this._audio.currentTime
    m.offset = 0
  }

  _startMusicSource(id, offset) {
    const m = this._music.get(id); if (!m?.buffer) return
    const src = this._audio.createBufferSource()
    src.buffer = m.buffer; src.loop = m.looping
    const gain = this._audio.createGain(); gain.gain.value = m.volume ?? 1
    src.connect(gain); gain.connect(this._masterGain)
    src.start(0, offset)
    if (m.source) try { m.source.stop() } catch {}
    m.source = src; m.gainNode = gain; m.startTime = this._audio.currentTime; m.offset = offset
    src.onended = () => { if (m.source === src) { m.source = null; m.gainNode = null } }
  }

  StopMusicStream(musicPtr) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    if (!m) return
    if (m.source) try { m.source.stop() } catch {}
    m.source = null; m.offset = 0
  }

  PauseMusicStream(musicPtr) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    if (!m?.source) return
    m.offset = (this._audio.currentTime - m.startTime) + m.offset
    try { m.source.stop() } catch {}
    m.source = null; m.paused = true
  }

  ResumeMusicStream(musicPtr) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    if (!m?.paused) return
    m.paused = false; this._startMusicSource(id, m.offset % (m.buffer?.duration || 1))
  }

  UpdateMusicStream() {} // Web Audio handles buffering automatically

  SeekMusicStream(musicPtr, position) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    if (!m) return
    const wasPlaying = !!m.source; this.StopMusicStream(musicPtr)
    m.offset = position; if (wasPlaying) this._startMusicSource(id, position)
  }

  IsMusicStreamPlaying(musicPtr) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    return (m?.source) ? 1 : 0
  }

  GetMusicTimeLength(musicPtr) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    return m?.buffer?.duration ?? 0
  }

  GetMusicTimePlayed(musicPtr) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    if (!m?.source) return m?.offset ?? 0
    return ((this._audio.currentTime - m.startTime) + m.offset) % (m.buffer?.duration || 1)
  }

  SetMusicVolume(musicPtr, volume) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    if (!m) return; m.volume = Math.max(0, Math.min(1, volume))
    if (m.gainNode) m.gainNode.gain.value = m.volume
  }
  SetMusicPitch(musicPtr, pitch) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    if (m?.source) m.source.playbackRate.value = pitch
  }
  SetMusicPan(musicPtr, pan) {
    const id = this._getSoundId(musicPtr), m = this._music.get(id)
    if (!m) return
    m.pan = Math.max(-1, Math.min(1, pan * 2 - 1))
    // Rebuild source chain if playing
    if (m.source) {
      const panner = this._audio.createStereoPanner(); panner.pan.value = m.pan
      const gain = m.gainNode || this._audio.createGain()
      try { m.source.disconnect() } catch {}
      m.source.connect(panner); panner.connect(gain); gain.connect(this._masterGain)
      m.gainNode = gain; m.pannerNode = panner
    }
  }
  UnloadMusicStream(musicPtr) {
    const id = this._getSoundId(musicPtr)
    const m = this._music.get(id); if (m?.source) try { m.source.stop() } catch {}
    this._music.delete(id)
  }

  // AudioStream
  LoadAudioStream(resultPtr, sampleRate, sampleSize, channels) {
    const id = this._newId()
    this._audioStreams.set(id, { gainNode: null, sampleRate, sampleSize, channels, playing: false, processed: true })
    this._writeSound(resultPtr, id, sampleRate, sampleSize, channels)
  }
  UnloadAudioStream(streamPtr) { this._audioStreams.delete(this._getSoundId(streamPtr)) }
  PlayAudioStream(streamPtr) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    if (!st) return
    st.playing = true
  }

  PauseAudioStream(streamPtr) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    if (st) { st.playing = false; if (st.source) { try { st.source.stop() } catch {}; st.source = null } }
  }

  ResumeAudioStream(streamPtr) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    if (st) st.playing = true
  }

  StopAudioStream(streamPtr) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    if (!st) return
    st.playing = false; st.processed = true
    if (st.source) { try { st.source.stop() } catch {}; st.source = null }
  }

  IsAudioStreamPlaying(streamPtr) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    return st?.playing ? 1 : 0
  }

  IsAudioStreamProcessed(streamPtr) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    return (st?.processed ?? true) ? 1 : 0
  }

  IsAudioStreamValid(streamPtr) {
    return this._audioStreams.has(this._getSoundId(streamPtr)) ? 1 : 0
  }

  SetAudioStreamVolume(streamPtr, volume) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    if (st) { st.volume = Math.max(0, Math.min(1, volume)); if (st.gainNode) st.gainNode.gain.value = st.volume }
  }

  SetAudioStreamPitch(streamPtr, pitch) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    if (st) st.pitch = pitch
  }

  SetAudioStreamPan(streamPtr, pan) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    if (st) st.pan = Math.max(-1, Math.min(1, pan * 2 - 1))
  }

  SetAudioStreamBufferSizeDefault(size) {}

  UpdateAudioStream(streamPtr, dataPtr, frameCount) {
    const id = this._getSoundId(streamPtr), st = this._audioStreams.get(id)
    if (!st?.playing) return
    const sampleRate = st.sampleRate || 44100, channels = st.channels || 1
    const buf = this._audio.createBuffer(channels, frameCount, sampleRate)
    const raw = new Float32Array(this._memory.buffer, dataPtr, frameCount * channels)
    for (let ch = 0; ch < channels; ch++) {
      const chan = buf.getChannelData(ch)
      for (let i = 0; i < frameCount; i++) chan[i] = raw[i * channels + ch]
    }
    const src = this._audio.createBufferSource(); src.buffer = buf
    const gain = this._audio.createGain(); gain.gain.value = st.volume ?? 1
    src.connect(gain); gain.connect(this._masterGain)
    if (st.source) try { st.source.stop() } catch {}
    st.source = src; st.gainNode = gain
    src.start(); src.onended = () => { if (st.source === src) st.processed = true }
    st.processed = false
  }

  // ── Input extras ──────────────────────────────────────────────────────────

  PollInputEvents() {
    // Advance button states (pressed→held, released→up)
    for (const [k, s] of this._mouse.buttons) {
      if (s === 1) this._mouse.buttons.set(k, 2)
      else if (s === 3) this._mouse.buttons.delete(k)
    }
    this._mouse.dx = 0; this._mouse.dy = 0
    this._mouse.scrollV = { x: 0, y: 0 }
  }

  GetKeyName(key) {
    const name = Object.entries(KEY_MAP).find(([, v]) => v === key)?.[0] || ''
    return this._mallocStr(name)
  }

  IsGestureDetected(gesture) { return 0 }
  GetGestureDetected() { return 0 }
  GetGestureHoldDuration() { return 0 }
  GetGestureDragVector(resultPtr) { this._writeVec2(resultPtr, 0, 0) }
  GetGestureDragAngle() { return 0 }
  GetGesturePinchVector(resultPtr) { this._writeVec2(resultPtr, 0, 0) }
  GetGesturePinchAngle() { return 0 }
  SetGesturesEnabled() {}
  EnableEventWaiting() {}
  DisableEventWaiting() {}

  // Gamepad — Gamepad API
  IsGamepadAvailable(gamepad) { return !!navigator.getGamepads?.()?.[gamepad] ? 1 : 0 }
  GetGamepadName(resultPtr, gamepad) {
    const name = navigator.getGamepads?.()?.[gamepad]?.id || ''
    return this._mallocStr(name)
  }
  IsGamepadButtonPressed(gamepad, btn) {
    const gp = navigator.getGamepads?.()?.[gamepad]; return gp?.buttons?.[btn]?.pressed ? 1 : 0
  }
  IsGamepadButtonDown(gamepad, btn) {
    const gp = navigator.getGamepads?.()?.[gamepad]; return gp?.buttons?.[btn]?.pressed ? 1 : 0
  }
  IsGamepadButtonReleased() { return 0 }
  IsGamepadButtonUp(gamepad, btn) {
    const gp = navigator.getGamepads?.()?.[gamepad]; return gp?.buttons?.[btn]?.pressed ? 0 : 1
  }
  GetGamepadButtonPressed() { return 0 }
  GetGamepadAxisCount(gamepad) { return navigator.getGamepads?.()?.[gamepad]?.axes?.length ?? 0 }
  GetGamepadAxisMovement(gamepad, axis) { return navigator.getGamepads?.()?.[gamepad]?.axes?.[axis] ?? 0 }
  SetGamepadMappings() { return 0 }
  SetGamepadVibration(gamepad, leftMotor, rightMotor) {
    navigator.getGamepads?.()?.[gamepad]?.vibrationActuator?.playEffect('dual-rumble', { strongMagnitude: leftMotor, weakMagnitude: rightMotor }).catch(() => {})
  }

  // Touch — touch events
  GetTouchX() { return this._touch[0]?.x | 0 || 0 }
  GetTouchY() { return this._touch[0]?.y | 0 || 0 }
  GetTouchPointCount() { return this._touch.length }
  GetTouchPointId(index) { return this._touch[index]?.id || 0 }
  GetTouchPosition(resultPtr, index) {
    const t = this._touch[index || 0]
    this._writeVec2(resultPtr, t?.x || 0, t?.y || 0)
  }

  // ── Clipboard ─────────────────────────────────────────────────────────────

  GetClipboardText() {
    // Async clipboard; return last cached value or 0
    navigator.clipboard?.readText().then(t => { this._clipboardText = t }).catch(() => {})
    return this._clipboardText ? this._mallocStr(this._clipboardText) : 0
  }
  SetClipboardText(textPtr) {
    navigator.clipboard?.writeText(this._readStr(textPtr)).catch(() => {})
  }
  GetClipboardImage(resultPtr) { this._writeImage(resultPtr, 0, 0, 0) }

  // ── File / directory (WASI handles actual I/O; these are stubs) ────────────

  FileExists(fileNamePtr) { return this._fileData.has(this._readStr(fileNamePtr)) ? 1 : 0 }
  DirectoryExists(dirPathPtr) {
    const dir = this._readStr(dirPathPtr).replace(/\/$/, '') + '/'
    for (const k of this._fileData.keys()) { if (k.startsWith(dir)) return 1 }
    return 0
  }
  IsFileExtension(fileNamePtr, extPtr) {
    const name = this._readStr(fileNamePtr), ext = this._readStr(extPtr)
    return name.endsWith(ext) ? 1 : 0
  }
  IsFileNameValid(fileNamePtr) { return 1 }
  IsPathFile(pathPtr) {
    const p = this._readStr(pathPtr); return p.includes('.') ? 1 : 0
  }
  GetFileLength(fileNamePtr) {
    return this._fileData.get(this._readStr(fileNamePtr))?.length || 0
  }
  GetFileExtension(fileNamePtr) {
    const name = this._readStr(fileNamePtr), dot = name.lastIndexOf('.')
    return this._mallocStr(dot >= 0 ? name.slice(dot) : '')
  }
  GetFileName(filePathPtr) {
    const p = this._readStr(filePathPtr)
    return this._mallocStr(p.split('/').pop() || p)
  }
  GetFileNameWithoutExt(filePathPtr) {
    const name = (this._readStr(filePathPtr).split('/').pop() || '')
    const dot = name.lastIndexOf('.')
    return this._mallocStr(dot >= 0 ? name.slice(0, dot) : name)
  }
  GetDirectoryPath(filePathPtr) {
    const p = this._readStr(filePathPtr), slash = p.lastIndexOf('/')
    return this._mallocStr(slash >= 0 ? p.slice(0, slash) : '.')
  }
  GetPrevDirectoryPath(dirPathPtr) { return this._mallocStr('.') }
  GetWorkingDirectory() { return this._mallocStr('/') }
  GetApplicationDirectory() { return this._mallocStr('/') }
  GetFileModTime(fileNamePtr) { return 0 }
  ChangeDirectory(dirPtr) {
    const dir = this._readStr(dirPtr)
    try { this._fs?.chdirSync?.(dir); this._cwd = dir; return 1 } catch { return 0 }
  }
  MakeDirectory(dirPathPtr) { return 0 }
  FileCopy(srcPtr, dstPtr) {
    try {
      const src = this._readStr(srcPtr), dst = this._readStr(dstPtr)
      const data = this._fs?.readFileSync(src)
      if (!data) return 0
      this._fs?.writeFileSync(dst, data)
      this._fileData.set(dst, new Uint8Array(data))
      return 1
    } catch { return 0 }
  }

  FileMove(srcPtr, dstPtr) {
    try {
      const src = this._readStr(srcPtr), dst = this._readStr(dstPtr)
      this._fs?.renameSync(src, dst)
      const d = this._fileData.get(src)
      if (d) { this._fileData.set(dst, d); this._fileData.delete(src) }
      return 1
    } catch { return 0 }
  }

  FileRemove(fileNamePtr) {
    try {
      const name = this._readStr(fileNamePtr)
      this._fs?.unlinkSync(name)
      this._fileData.delete(name)
      return 1
    } catch { return 0 }
  }

  FileRename(oldPtr, newPtr) {
    try {
      const oldName = this._readStr(oldPtr), newName = this._readStr(newPtr)
      this._fs?.renameSync(oldName, newName)
      const d = this._fileData.get(oldName)
      if (d) { this._fileData.set(newName, d); this._fileData.delete(oldName) }
      return 1
    } catch { return 0 }
  }
  IsFileDropped() { return 0 }
  LoadDroppedFiles(resultPtr) {
    // FilePathList struct: capacity(4), count(4), paths(4)
    const v = this._view
    v.setUint32(resultPtr, 0, true); v.setUint32(resultPtr+4, 0, true); v.setUint32(resultPtr+8, 0, true)
  }
  UnloadDroppedFiles() {}

  GetDirectoryFileCount(dirPathPtr) { return 0 }
  GetDirectoryFileCountEx(dirPathPtr) { return 0 }
  LoadDirectoryFiles(resultPtr) {
    const v = this._view
    v.setUint32(resultPtr, 0, true); v.setUint32(resultPtr+4, 0, true); v.setUint32(resultPtr+8, 0, true)
  }
  LoadDirectoryFilesEx(resultPtr) { this.LoadDirectoryFiles(resultPtr) }
  UnloadDirectoryFiles() {}
  LoadTextLines(resultPtr) { this._view.setUint32(resultPtr, 0, true) }
  UnloadTextLines() {}

  LoadFileData(resultPtr, fileNamePtr, dataSizePtr) {
    const fileName = this._readStr(fileNamePtr)
    const data = this._fileData.get(fileName)
    if (!data) { this._view.setUint32(resultPtr, 0, true); if (dataSizePtr) this._view.setUint32(dataSizePtr, 0, true); return }
    const ptr = this._exports.malloc(data.length)
    new Uint8Array(this._memory.buffer, ptr, data.length).set(data)
    this._view.setUint32(resultPtr, ptr, true)
    if (dataSizePtr) this._view.setUint32(dataSizePtr, data.length, true)
  }
  UnloadFileData(dataPtr) { if (this._exports?.free && dataPtr) this._exports.free(dataPtr) }

  LoadFileText(fileNamePtr) {
    const fileName = this._readStr(fileNamePtr)
    const data = this._fileData.get(fileName)
    if (!data) return 0
    return this._mallocStr(new TextDecoder().decode(data))
  }
  UnloadFileText(textPtr) { if (this._exports?.free && textPtr) this._exports.free(textPtr) }

  SaveFileData(fileNamePtr, dataPtr, dataSizePtr) {
    try {
      const name = this._readStr(fileNamePtr)
      const size = this._view.getUint32(dataSizePtr, true)
      const data = new Uint8Array(this._memory.buffer, dataPtr, size).slice()
      this._fs?.writeFileSync(name, data)
      this._fileData.set(name, data)
      return 1
    } catch { return 0 }
  }

  SaveFileText(fileNamePtr, textPtr) {
    try {
      const name = this._readStr(fileNamePtr), text = this._readStr(textPtr)
      const data = enc.encode(text)
      this._fs?.writeFileSync(name, data)
      this._fileData.set(name, data)
      return 1
    } catch { return 0 }
  }

  FileTextFindIndex(textPtr, findPtr) {
    const text = this._readStr(textPtr), find = this._readStr(findPtr)
    return text.indexOf(find)
  }
  FileTextReplace(textPtr, findPtr, replacePtr) {
    const text = this._readStr(textPtr), find = this._readStr(findPtr), rep = this._readStr(replacePtr)
    return this._mallocStr(text.replaceAll(find, rep))
  }

  // ── Text string utilities ─────────────────────────────────────────────────

  TextLength(textPtr) { return this._readStr(textPtr).length }
  TextIsEqual(text1Ptr, text2Ptr) { return this._readStr(text1Ptr) === this._readStr(text2Ptr) ? 1 : 0 }
  TextCopy(dstPtr, srcPtr) {
    const src = this._readStr(srcPtr), bytes = enc.encode(src + '\0')
    const capped = bytes.subarray(0, Math.min(bytes.length, 1024))
    new Uint8Array(this._memory.buffer, dstPtr, capped.length).set(capped)
    return src.length
  }
  TextFindIndex(textPtr, findPtr) { return this._readStr(textPtr).indexOf(this._readStr(findPtr)) }
  TextSubtext(textPtr, position, length) {
    const s = this._readStr(textPtr)
    return this._mallocStr(s.substr(position, length))
  }
  TextReplace(textPtr, replacePtr, byPtr) {
    return this._mallocStr(this._readStr(textPtr).replaceAll(this._readStr(replacePtr), this._readStr(byPtr)))
  }
  TextReplaceAlloc(resultPtr, textPtr, replacePtr, byPtr) {
    const s = this._readStr(textPtr).replaceAll(this._readStr(replacePtr), this._readStr(byPtr))
    this._view.setUint32(resultPtr, this._mallocStr(s), true)
  }
  TextReplaceBetween(textPtr, startPtr, endPtr, byPtr) {
    const text = this._readStr(textPtr), start = this._readStr(startPtr)
    const end = this._readStr(endPtr), by = this._readStr(byPtr)
    const si = text.indexOf(start), ei = text.indexOf(end, si + start.length)
    if (si < 0 || ei < 0) return this._mallocStr(text)
    return this._mallocStr(text.slice(0, si + start.length) + by + text.slice(ei))
  }
  TextReplaceBetweenAlloc(resultPtr, textPtr, startPtr, endPtr, byPtr) {
    this._view.setUint32(resultPtr, this.TextReplaceBetween(textPtr, startPtr, endPtr, byPtr), true)
  }
  TextInsert(textPtr, insertPtr, position) {
    const t = this._readStr(textPtr), ins = this._readStr(insertPtr)
    return this._mallocStr(t.slice(0, position) + ins + t.slice(position))
  }
  TextInsertAlloc(resultPtr, textPtr, insertPtr, position) {
    this._view.setUint32(resultPtr, this.TextInsert(textPtr, insertPtr, position), true)
  }
  TextAppend(textPtr, appendPtr, positionPtr) {
    const app = this._readStr(appendPtr), bytes = enc.encode(app + '\0')
    const pos = this._view.getInt32(positionPtr, true)
    new Uint8Array(this._memory.buffer, textPtr + pos, bytes.length).set(bytes)
    this._view.setInt32(positionPtr, pos + app.length, true)
  }
  TextJoin(textListPtr, count, delimiterPtr) {
    const v = this._view, delim = this._readStr(delimiterPtr), parts = []
    for (let i = 0; i < count; i++) parts.push(this._readStr(v.getUint32(textListPtr + i*4, true)))
    return this._mallocStr(parts.join(delim))
  }
  TextSplit(textPtr, delimiter, countPtr) {
    const parts = this._readStr(textPtr).split(String.fromCharCode(delimiter))
    const count = parts.length
    if (countPtr) this._view.setInt32(countPtr, count, true)
    const ptrArray = this._exports.malloc(count * 4)
    const v = this._view
    for (let i = 0; i < count; i++) v.setUint32(ptrArray + i*4, this._mallocStr(parts[i]), true)
    return ptrArray
  }
  TextToUpper(textPtr) { return this._mallocStr(this._readStr(textPtr).toUpperCase()) }
  TextToLower(textPtr) { return this._mallocStr(this._readStr(textPtr).toLowerCase()) }
  TextToPascal(textPtr) {
    return this._mallocStr(this._readStr(textPtr).replace(/(?:^|[-_ ])(\w)/g, (_, c) => c.toUpperCase()))
  }
  TextToSnake(textPtr) {
    return this._mallocStr(this._readStr(textPtr).replace(/([a-z])([A-Z])/g, '$1_$2').toLowerCase())
  }
  TextToCamel(textPtr) {
    const s = this._readStr(textPtr).replace(/[-_ ](\w)/g, (_, c) => c.toUpperCase())
    return this._mallocStr(s.charAt(0).toLowerCase() + s.slice(1))
  }
  TextToInteger(textPtr) { return parseInt(this._readStr(textPtr), 10) || 0 }
  TextToFloat(textPtr) { return parseFloat(this._readStr(textPtr)) || 0 }
  TextRemoveSpaces(textPtr) { return this._mallocStr(this._readStr(textPtr).replace(/\s+/g, '')) }
  GetTextBetween(textPtr, startPtr, endPtr) {
    const text = this._readStr(textPtr), start = this._readStr(startPtr), end = this._readStr(endPtr)
    const si = text.indexOf(start), ei = text.indexOf(end, si + start.length)
    return this._mallocStr(si >= 0 && ei >= 0 ? text.slice(si + start.length, ei) : '')
  }

  // Codepoint utilities
  GetCodepoint(textPtr, codepointSizePtr) {
    const text = this._readStr(textPtr)
    if (!text.length) { if (codepointSizePtr) this._view.setInt32(codepointSizePtr, 0, true); return 0 }
    const cp = text.codePointAt(0)
    if (codepointSizePtr) this._view.setInt32(codepointSizePtr, cp > 0xFFFF ? 2 : 1, true)
    return cp
  }
  GetCodepointCount(textPtr) { return [...this._readStr(textPtr)].length }
  GetCodepointNext(textPtr, codepointSizePtr) { return this.GetCodepoint(textPtr, codepointSizePtr) }
  GetCodepointPrevious(textPtr, codepointSizePtr) { return this.GetCodepoint(textPtr, codepointSizePtr) }
  CodepointToUTF8(resultPtr, codepoint, utf8SizePtr) {
    const s = String.fromCodePoint(codepoint), bytes = enc.encode(s)
    new Uint8Array(this._memory.buffer, resultPtr, bytes.length).set(bytes)
    if (utf8SizePtr) this._view.setInt32(utf8SizePtr, bytes.length, true)
    return resultPtr
  }
  LoadCodepoints(textPtr, countPtr) {
    const cps = [...this._readStr(textPtr)].map(c => c.codePointAt(0))
    const ptr = this._exports.malloc(cps.length * 4)
    const v = this._view
    cps.forEach((cp, i) => v.setInt32(ptr + i*4, cp, true))
    if (countPtr) v.setInt32(countPtr, cps.length, true)
    return ptr
  }
  UnloadCodepoints(codepointsPtr) { if (this._exports?.free && codepointsPtr) this._exports.free(codepointsPtr) }
  LoadUTF8(resultPtr, codepointsPtr, length) {
    const v = this._view, chars = []
    for (let i = 0; i < length; i++) chars.push(String.fromCodePoint(v.getInt32(codepointsPtr + i*4, true)))
    return this._mallocStr(chars.join(''))
  }
  UnloadUTF8(textPtr) { if (this._exports?.free && textPtr) this._exports.free(textPtr) }

  // ── Misc stubs ─────────────────────────────────────────────────────────────

  SetTraceLogLevel(level) { this._traceLogLevel = level }
  SetTraceLogCallback() {}
  TraceLog(logLevel, textPtr) {
    if (logLevel < (this._traceLogLevel ?? 3)) return
    const labels = ['ALL', 'TRACE', 'DEBUG', 'INFO', 'WARNING', 'ERROR', 'FATAL', 'NONE']
    const label = labels[logLevel] ?? 'LOG'
    const text = this._readStr(textPtr)
    if (logLevel >= 5) console.error(`[${label}] ${text}`)
    else if (logLevel >= 4) console.warn(`[${label}] ${text}`)
    else console.log(`[${label}] ${text}`)
  }

  OpenURL(urlPtr) { window.open(this._readStr(urlPtr), '_blank') }

  WaitTime(seconds) {
    // Synchronous sleep in browser is not feasible; use a spin-wait for very small values only
    if (seconds > 0 && seconds < 0.001) {
      const end = performance.now() + seconds * 1000
      while (performance.now() < end) {}
    }
  }
  LoadRandomSequence(resultPtr, count, min, max) {
    const ptr = this._exports.malloc(count * 4)
    const v = this._view
    for (let i = 0; i < count; i++) v.setInt32(ptr + i*4, this.GetRandomValue(min, max), true)
    this._view.setUint32(resultPtr, ptr, true)
  }
  UnloadRandomSequence(sequencePtr) {
    const ptr = this._view.getUint32(sequencePtr, true)
    if (ptr && this._exports?.free) this._exports.free(ptr)
  }

  // Hash / compress
  ComputeCRC32(resultPtr, dataPtr, dataSize) {
    const data = new Uint8Array(this._memory.buffer, dataPtr, dataSize)
    let crc = 0xFFFFFFFF
    for (let i = 0; i < dataSize; i++) {
      crc ^= data[i]
      for (let j = 0; j < 8; j++) crc = (crc >>> 1) ^ (crc & 1 ? 0xEDB88320 : 0)
    }
    this._view.setUint32(resultPtr, (crc ^ 0xFFFFFFFF) >>> 0, true)
  }

  ComputeMD5(resultPtr, dataPtr, dataSize) {
    // Async SubtleCrypto; synchronous fallback returns zeros
    const data = new Uint8Array(this._memory.buffer, dataPtr, dataSize).slice()
    crypto.subtle?.digest('MD5', data).then(ab => {
      new Uint8Array(this._memory.buffer, resultPtr, 16).set(new Uint8Array(ab))
    }).catch(() => {})
    for (let i = 0; i < 16; i++) this._view.setUint8(resultPtr + i, 0)
  }

  ComputeSHA1(resultPtr, dataPtr, dataSize) {
    const data = new Uint8Array(this._memory.buffer, dataPtr, dataSize).slice()
    crypto.subtle?.digest('SHA-1', data).then(ab => {
      new Uint8Array(this._memory.buffer, resultPtr, 20).set(new Uint8Array(ab))
    }).catch(() => {})
    for (let i = 0; i < 20; i++) this._view.setUint8(resultPtr + i, 0)
  }

  ComputeSHA256(resultPtr, dataPtr, dataSize) {
    const data = new Uint8Array(this._memory.buffer, dataPtr, dataSize).slice()
    crypto.subtle?.digest('SHA-256', data).then(ab => {
      new Uint8Array(this._memory.buffer, resultPtr, 32).set(new Uint8Array(ab))
    }).catch(() => {})
    for (let i = 0; i < 32; i++) this._view.setUint8(resultPtr + i, 0)
  }

  CompressData(resultPtr, dataPtr, dataSize, compDataSizePtr) {
    // CompressionStream (deflate) is async; return original data as fallback
    const src = new Uint8Array(this._memory.buffer, dataPtr, dataSize).slice()
    const ptr = this._exports.malloc(src.length)
    new Uint8Array(this._memory.buffer, ptr, src.length).set(src)
    this._view.setUint32(resultPtr, ptr, true)
    if (compDataSizePtr) this._view.setInt32(compDataSizePtr, src.length, true)
  }

  DecompressData(resultPtr, compDataPtr, compDataSize, dataSizePtr) {
    // DecompressionStream is async; return data as-is
    const src = new Uint8Array(this._memory.buffer, compDataPtr, compDataSize).slice()
    const ptr = this._exports.malloc(src.length)
    new Uint8Array(this._memory.buffer, ptr, src.length).set(src)
    this._view.setUint32(resultPtr, ptr, true)
    if (dataSizePtr) this._view.setInt32(dataSizePtr, src.length, true)
  }
  EncodeDataBase64(resultPtr, dataPtr, dataSize, outputSizePtr) {
    const bytes = new Uint8Array(this._memory.buffer, dataPtr, dataSize)
    let b64 = btoa(String.fromCharCode(...bytes))
    const ptr = this._mallocStr(b64)
    this._view.setUint32(resultPtr, ptr, true)
    if (outputSizePtr) this._view.setInt32(outputSizePtr, b64.length, true)
  }
  DecodeDataBase64(resultPtr, dataPtr, outputSizePtr) {
    const b64 = this._readStr(dataPtr)
    const raw = atob(b64)
    const ptr = this._exports.malloc(raw.length)
    new Uint8Array(this._memory.buffer, ptr, raw.length).set([...raw].map(c => c.charCodeAt(0)))
    this._view.setUint32(resultPtr, ptr, true)
    if (outputSizePtr) this._view.setInt32(outputSizePtr, raw.length, true)
  }

  // Screenshot / export
  TakeScreenshot(fileNamePtr) {
    const name = this._readStr(fileNamePtr) || 'screenshot.png'
    this._canvas.toBlob(blob => {
      const a = document.createElement('a'); a.href = URL.createObjectURL(blob); a.download = name; a.click()
    })
  }

  ExportImage(imagePtr, fileNamePtr) {
    const id = this._view.getUint32(imagePtr, true), img = this._images.get(id)
    if (!img?.data) return 0
    const name = this._readStr(fileNamePtr) || 'image.png'
    const oc = new OffscreenCanvas(img.width, img.height)
    oc.getContext('2d').putImageData(img.data, 0, 0)
    oc.convertToBlob({ type: 'image/png' }).then(blob => {
      const a = document.createElement('a'); a.href = URL.createObjectURL(blob); a.download = name; a.click()
    })
    return 1
  }

  ExportImageAsCode() { return 0 }
  ExportFontAsCode() { return 0 }
  ExportDataAsCode() { return 0 }

  ExportWave(wavePtr, fileNamePtr) {
    // Encode as WAV and download
    const waveId = this._view.getUint32(wavePtr + 16, true)
    const snd = this._sounds.get(waveId)
    if (!snd?.buffer) return 0
    const buf = snd.buffer, channels = buf.numberOfChannels, frames = buf.length, sr = buf.sampleRate
    const dataSize = frames * channels * 2 // 16-bit
    const ab = new ArrayBuffer(44 + dataSize)
    const dv = new DataView(ab)
    const writeStr = (offset, s) => { for (let i = 0; i < s.length; i++) dv.setUint8(offset + i, s.charCodeAt(i)) }
    writeStr(0, 'RIFF'); dv.setUint32(4, 36 + dataSize, true); writeStr(8, 'WAVE')
    writeStr(12, 'fmt '); dv.setUint32(16, 16, true); dv.setUint16(20, 1, true)
    dv.setUint16(22, channels, true); dv.setUint32(24, sr, true)
    dv.setUint32(28, sr * channels * 2, true); dv.setUint16(32, channels * 2, true); dv.setUint16(34, 16, true)
    writeStr(36, 'data'); dv.setUint32(40, dataSize, true)
    let offset = 44
    for (let i = 0; i < frames; i++) for (let ch = 0; ch < channels; ch++) {
      const v = Math.max(-1, Math.min(1, buf.getChannelData(ch)[i]))
      dv.setInt16(offset, v < 0 ? v * 32768 : v * 32767, true); offset += 2
    }
    const name = this._readStr(fileNamePtr) || 'audio.wav'
    const a = document.createElement('a'); a.href = URL.createObjectURL(new Blob([ab], { type: 'audio/wav' }))
    a.download = name; a.click()
    return 1
  }

  ExportWaveAsCode() { return 0 }

  // Automation
  LoadAutomationEventList(resultPtr) { this._view.setUint32(resultPtr, 0, true) }
  UnloadAutomationEventList() {}
  SetAutomationEventList() {}
  SetAutomationEventBaseFrame() {}
  StartAutomationEventRecording() {}
  StopAutomationEventRecording() {}
  PlayAutomationEvent() {}
  ExportAutomationEventList() { return 0 }

  // VR
  LoadVrStereoConfig(resultPtr) { for(let i=0;i<8*16;i++) this._view.setFloat32(resultPtr+i*4, i%5===0?1:0, true) }
  UnloadVrStereoConfig() {}

  // Shader stubs
  LoadShader(resultPtr) { this._view.setUint32(resultPtr, 0, true); this._view.setUint32(resultPtr+4, 0, true) }
  LoadShaderFromMemory(resultPtr) { this.LoadShader(resultPtr) }
  IsShaderValid() { return 0 }
  GetShaderLocation() { return -1 }
  GetShaderLocationAttrib() { return -1 }
  SetShaderValue() {}
  SetShaderValueV() {}
  SetShaderValueMatrix() {}
  SetShaderValueTexture() {}
  UnloadShader() {}

  // 3D model stubs
  LoadModel(resultPtr) { for(let i=0;i<4;i++) this._view.setUint32(resultPtr+i*4, 0, true) }
  LoadModelFromMesh(resultPtr) { this.LoadModel(resultPtr) }
  IsModelValid() { return 0 }
  UnloadModel() {}
  LoadModelAnimations(resultPtr) { this._view.setUint32(resultPtr, 0, true); return 0 }
  IsModelAnimationValid() { return 0 }
  UnloadModelAnimations() {}
  UpdateModelAnimation() {}
  UpdateModelAnimationEx() {}
  LoadMaterials(resultPtr) { this._view.setUint32(resultPtr, 0, true); return 0 }
  LoadMaterialDefault(resultPtr) { for(let i=0;i<4;i++) this._view.setUint32(resultPtr+i*4, 0, true) }
  IsMaterialValid() { return 0 }
  UnloadMaterial() {}
  SetMaterialTexture() {}
  SetModelMeshMaterial() {}

  // Mesh stubs
  GenMeshPlane(resultPtr) { for(let i=0;i<12;i++) this._view.setUint32(resultPtr+i*4, 0, true) }
  GenMeshCube(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshSphere(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshHemiSphere(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshCylinder(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshCone(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshTorus(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshKnot(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshHeightmap(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshCubicmap(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshPoly(resultPtr) { this.GenMeshPlane(resultPtr) }
  GenMeshTangents() {}
  UploadMesh() {}
  UnloadMesh() {}
  GetMeshBoundingBox(resultPtr) { for(let i=0;i<6;i++) this._view.setFloat32(resultPtr+i*4, 0, true) }
  GetModelBoundingBox(resultPtr) { this.GetMeshBoundingBox(resultPtr) }
  UpdateMeshBuffer() {}
  DrawMesh() {}
  DrawMeshInstanced() {}
  DrawModel() {}
  DrawModelEx() {}
  DrawModelWires() {}
  DrawModelWiresEx() {}
  DrawBoundingBox() {}
  DrawBillboard() {}
  DrawBillboardRec() {}
  DrawBillboardPro() {}

  // 3D shapes stubs
  DrawLine3D() {} DrawPoint3D() {} DrawCircle3D() {}
  DrawTriangle3D() {} DrawTriangleStrip3D() {}
  DrawCube() {} DrawCubeV() {} DrawCubeWires() {} DrawCubeWiresV() {}
  DrawSphere() {} DrawSphereEx() {} DrawSphereWires() {}
  DrawCylinder() {} DrawCylinderEx() {} DrawCylinderWires() {} DrawCylinderWiresEx() {}
  DrawCapsule() {} DrawCapsuleWires() {}
  DrawPlane() {} DrawRay() {} DrawGrid() {}

  GetRandomValue(min, max) {
    // LCG seeded RNG if seed is set, else Math.random
    if (this._rngSeed != null) {
      this._rngSeed = (this._rngSeed * 1664525 + 1013904223) >>> 0
      return min + (this._rngSeed % (max - min + 1))
    }
    return (Math.random() * (max - min + 1) + min) | 0
  }
  SetRandomSeed(seed) { this._rngSeed = seed >>> 0 }
  TextFormat(fmtPtr) { return fmtPtr } // variadic; game uses cart-side impl; stub returns fmt

  ImageText(resultPtr, textPtr, fontSize, colorPtr) {
    const text = this._readStr(textPtr), c = this._readColor(colorPtr)
    const oc = new OffscreenCanvas(1, 1)
    const ctx2 = oc.getContext('2d')
    ctx2.font = `${fontSize}px monospace`
    const w = Math.ceil(ctx2.measureText(text).width) || 1, h = fontSize | 0
    const oc2 = new OffscreenCanvas(w, h)
    const ctx3 = oc2.getContext('2d')
    ctx3.font = `${fontSize}px monospace`; ctx3.fillStyle = `rgba(${c.r},${c.g},${c.b},${c.a/255})`
    ctx3.textBaseline = 'top'; ctx3.fillText(text, 0, 0)
    const id = this._newId(), imgData = ctx3.getImageData(0, 0, w, h)
    this._images.set(id, { width: w, height: h, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, w, h)
  }

  ImageTextEx(resultPtr, fontPtr, textPtr, fontSize, spacing, tintPtr) {
    const text = this._readStr(textPtr), c = this._readColor(tintPtr)
    const family = this._fontFamily(fontPtr)
    const oc = new OffscreenCanvas(1, 1)
    const ctx2 = oc.getContext('2d')
    ctx2.font = `${fontSize}px ${family}`
    ctx2.letterSpacing = `${spacing}px`
    const w = Math.ceil(ctx2.measureText(text).width) || 1, h = fontSize | 0
    const oc2 = new OffscreenCanvas(w, h)
    const ctx3 = oc2.getContext('2d')
    ctx3.font = `${fontSize}px ${family}`; ctx3.letterSpacing = `${spacing}px`
    ctx3.fillStyle = `rgba(${c.r},${c.g},${c.b},${c.a/255})`
    ctx3.textBaseline = 'top'; ctx3.fillText(text, 0, 0)
    const id = this._newId(), imgData = ctx3.getImageData(0, 0, w, h)
    this._images.set(id, { width: w, height: h, data: imgData, format: 7 })
    this._writeImage(resultPtr, id, w, h)
  }

  LoadImageAnim(resultPtr, fileNamePtr, framesPtr) {
    this.LoadImage(resultPtr, fileNamePtr)
    if (framesPtr) this._view.setInt32(framesPtr, 1, true)
  }
  LoadImageAnimFromMemory(resultPtr, fileTypePtr, dataPtr, dataSize, framesPtr) {
    this.LoadImageFromMemory(resultPtr, fileTypePtr, dataPtr, dataSize)
    if (framesPtr) this._view.setInt32(framesPtr, 1, true)
  }

  // Excluded callbacks (CLAUDE.md) — stubs required by wasm ABI
  SetLoadFileDataCallback() {}
  SetSaveFileDataCallback() {}
  SetLoadFileTextCallback() {}
  SetSaveFileTextCallback() {}

  // Ray collision stubs
  GetRayCollisionSphere(resultPtr) { for(let i=0;i<5;i++) this._view.setUint32(resultPtr+i*4, 0, true) }
  GetRayCollisionBox(resultPtr) { this.GetRayCollisionSphere(resultPtr) }
  GetRayCollisionMesh(resultPtr) { this.GetRayCollisionSphere(resultPtr) }
  GetRayCollisionTriangle(resultPtr) { this.GetRayCollisionSphere(resultPtr) }
  GetRayCollisionQuad(resultPtr) { this.GetRayCollisionSphere(resultPtr) }

  // World/screen 3D stubs
  GetScreenToWorldRay(resultPtr) { for(let i=0;i<6;i++) this._view.setFloat32(resultPtr+i*4, 0, true) }
  GetScreenToWorldRayEx(resultPtr) { this.GetScreenToWorldRay(resultPtr) }
  GetWorldToScreen(resultPtr) { this._writeVec2(resultPtr, 0, 0) }
  GetWorldToScreenEx(resultPtr) { this._writeVec2(resultPtr, 0, 0) }

  // Memory (excluded per CLAUDE.md — wasm has its own malloc)
  MemAlloc(size) { return 0 }
  MemRealloc(ptr, size) { return 0 }
  MemFree(ptr) {}

  // ── Private: DOM event wiring ─────────────────────────────────────────────

  _attachEvents() {
    const c = this._canvas
    c.setAttribute('tabindex', '0')

    c.addEventListener('keydown', (e) => {
      const key = KEY_MAP[e.code] ?? -1
      if (key < 0) return
      if (!this._keys.has(key) || this._keys.get(key) === 0 || this._keys.get(key) === 3) {
        this._keys.set(key, 1) // newly pressed
        this._keysNew.push(key)
      } else if (this._keys.get(key) === 1) {
        this._keys.set(key, 2) // held
      }
      if (!['F5', 'F12', 'Tab'].includes(e.key)) e.preventDefault()
    })

    c.addEventListener('keyup', (e) => {
      const key = KEY_MAP[e.code] ?? -1
      if (key >= 0) this._keys.set(key, 3) // released
    })

    c.addEventListener('mousemove', (e) => {
      const r = c.getBoundingClientRect()
      const nx = e.clientX - r.left
      const ny = e.clientY - r.top
      this._mouse.dx = nx - this._mouse.x
      this._mouse.dy = ny - this._mouse.y
      this._mouse.x = nx
      this._mouse.y = ny
    })

    c.addEventListener('mousedown', (e) => {
      c.focus()
      this._mouse.buttons.set(e.button, 1)
      e.preventDefault()
    })

    c.addEventListener('mouseup', (e) => {
      this._mouse.buttons.set(e.button, 3)
    })

    c.addEventListener(
      'wheel',
      (e) => {
        this._mouse.scroll -= e.deltaY / 100
        e.preventDefault()
      },
      { passive: false }
    )

    c.addEventListener('contextmenu', (e) => e.preventDefault())

    c.addEventListener('mouseenter', () => { this._cursorOnScreen = true })
    c.addEventListener('mouseleave', () => { this._cursorOnScreen = false })

    window.addEventListener('resize', () => { this._windowResized = true })

    // Touch support
    c.addEventListener('touchstart', (e) => {
      const r = c.getBoundingClientRect()
      this._touch = Array.from(e.touches).map(t => ({ id: t.identifier, x: t.clientX - r.left, y: t.clientY - r.top }))
      e.preventDefault()
    }, { passive: false })
    c.addEventListener('touchmove', (e) => {
      const r = c.getBoundingClientRect()
      this._touch = Array.from(e.touches).map(t => ({ id: t.identifier, x: t.clientX - r.left, y: t.clientY - r.top }))
      e.preventDefault()
    }, { passive: false })
    c.addEventListener('touchend', (e) => {
      const r = c.getBoundingClientRect()
      this._touch = Array.from(e.touches).map(t => ({ id: t.identifier, x: t.clientX - r.left, y: t.clientY - r.top }))
    })
  }
}
