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

// Approximate raylib's default font: spacing factor per font size unit
// raylib default font is ~10px wide at fontSize=10
const CHAR_WIDTH_RATIO = 0.6

export class Raylib {
  constructor({ canvas } = {}) {
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
    this._mouse = { x: 0, y: 0, buttons: new Map(), scroll: 0 }

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
  IsWindowMinimized() {
    return 0
  }
  IsWindowMaximized() {
    return 0
  }
  IsWindowFocused() {
    return document.hasFocus() ? 1 : 0
  }
  IsWindowResized() {
    return 0
  }
  IsWindowFullscreen() {
    return 0
  }

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
  GetMonitorWidth() {
    return screen.width
  }
  GetMonitorHeight() {
    return screen.height
  }
  GetMonitorCount() {
    return 1
  }
  GetCurrentMonitor() {
    return 0
  }

  SetWindowTitle(titlePtr) {
    document.title = this._readStr(titlePtr)
  }
  SetWindowSize(w, h) {
    this._canvas.width = w
    this._canvas.height = h
    this._width = w
    this._height = h
  }
  SetWindowMinSize() {}
  SetWindowMaxSize() {}
  SetWindowPosition() {}
  SetWindowState() {}
  ClearWindowState() {}
  MaximizeWindow() {}
  MinimizeWindow() {}
  RestoreWindow() {}
  ToggleFullscreen() {}
  ToggleBorderlessWindowed() {}
  SetExitKey() {}

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
  IsCursorOnScreen() {
    return 1
  }
  EnableCursor() {}
  DisableCursor() {
    this._canvas.requestPointerLock?.()
  }
  SetMouseCursor() {}

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
    this._keysNew = []
  }

  // ── Drawing ───────────────────────────────────────────────────────────────

  BeginDrawing() {}
  EndDrawing() {}

  BeginMode2D() {}
  EndMode2D() {}
  BeginMode3D() {}
  EndMode3D() {}
  BeginTextureMode() {}
  EndTextureMode() {}
  BeginShaderMode() {}
  EndShaderMode() {}
  BeginBlendMode() {}
  EndBlendMode() {}
  BeginScissorMode() {}
  EndScissorMode() {}
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

  // ── Text ──────────────────────────────────────────────────────────────────

  _setFont(fontSize) {
    this._ctx.font = `${fontSize}px monospace`
  }

  DrawText(textPtr, posX, posY, fontSize, colorPtr) {
    this._setFont(fontSize)
    this._ctx.fillStyle = this._css(colorPtr)
    // raylib baseline is top; canvas baseline is alphabetic — offset by ~0.75*fontSize
    this._ctx.fillText(this._readStr(textPtr), posX, posY + fontSize * 0.75)
  }

  DrawTextEx(fontPtr, textPtr, posPtr, fontSize, spacing, colorPtr) {
    const pos = this._readVec2(posPtr)
    this.DrawText(textPtr, pos.x, pos.y, fontSize, colorPtr)
  }

  MeasureText(textPtr, fontSize) {
    this._setFont(fontSize)
    return this._ctx.measureText(this._readStr(textPtr)).width | 0
  }

  // MeasureTextEx writes a Vector2 result (sret)
  MeasureTextEx(resultPtr, fontPtr, textPtr, fontSize, spacing) {
    this._setFont(fontSize)
    const w = this._ctx.measureText(this._readStr(textPtr)).width
    this._writeVec2(resultPtr, w, fontSize)
  }

  DrawFPS(posX, posY) {
    const fps = this.GetFPS()
    this._setFont(20)
    this._ctx.fillStyle = fps < 30 ? 'red' : 'lime'
    this._ctx.fillText(`${fps} FPS`, posX, posY + 15)
  }

  GetGlyphIndex() {
    return 0
  }
  LoadFont() {
    return 0
  }
  LoadFontEx() {
    return 0
  }
  UnloadFont() {}
  IsFontReady() {
    return 0
  }

  // ── Mouse ─────────────────────────────────────────────────────────────────

  // GetMousePosition uses sret: first param is result Vector2 ptr
  GetMousePosition(resultPtr) {
    this._writeVec2(resultPtr, this._mouse.x, this._mouse.y)
  }

  GetMouseX() {
    return this._mouse.x | 0
  }
  GetMouseY() {
    return this._mouse.y | 0
  }

  GetMouseDelta(resultPtr) {
    this._writeVec2(resultPtr, this._mouse.dx || 0, this._mouse.dy || 0)
  }

  SetMousePosition(x, y) {
    this._mouse.x = x
    this._mouse.y = y
  }
  SetMouseOffset() {}
  SetMouseScale() {}

  GetMouseWheelMove() {
    const s = this._mouse.scroll
    this._mouse.scroll = 0
    return s
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
  SetExitKey() {}

  // ── Gamepad (stub) ────────────────────────────────────────────────────────

  IsGamepadAvailable() {
    return 0
  }
  IsGamepadButtonPressed() {
    return 0
  }
  IsGamepadButtonDown() {
    return 0
  }
  IsGamepadButtonReleased() {
    return 0
  }
  IsGamepadButtonUp() {
    return 0
  }
  GetGamepadButtonPressed() {
    return 0
  }
  GetGamepadAxisCount() {
    return 0
  }
  GetGamepadAxisMovement() {
    return 0
  }
  GetGamepadName() {
    return 0
  }

  // ── Touch (stub) ──────────────────────────────────────────────────────────

  GetTouchX() {
    return 0
  }
  GetTouchY() {
    return 0
  }
  GetTouchPointId() {
    return 0
  }
  GetTouchPointCount() {
    return 0
  }
  GetTouchPosition(resultPtr) {
    this._writeVec2(resultPtr, 0, 0)
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

  // ── Misc ──────────────────────────────────────────────────────────────────

  SetTraceLogLevel() {}
  TraceLog(logLevel, textPtr) {
    const labels = ['ALL', 'TRACE', 'DEBUG', 'INFO', 'WARNING', 'ERROR', 'FATAL', 'NONE']
    const label = labels[logLevel] ?? 'LOG'
    const text = this._readStr(textPtr)
    if (logLevel >= 5) console.error(`[${label}] ${text}`)
    else if (logLevel >= 4) console.warn(`[${label}] ${text}`)
    else console.log(`[${label}] ${text}`)
  }

  OpenURL(urlPtr) {
    window.open(this._readStr(urlPtr), '_blank')
  }

  GetRandomValue(min, max) {
    return (Math.random() * (max - min + 1) + min) | 0
  }
  SetRandomSeed() {}

  TakeScreenshot() {}
  ExportDataAsCode() {
    return 0
  }

  IsFileDropped() {
    return 0
  }
  LoadDroppedFiles() {
    return 0
  }
  UnloadDroppedFiles() {}

  // Clipboard
  GetClipboardText() {
    return 0
  }
  SetClipboardText() {}

  // Storage (stub — no persistent storage in basic web host)
  SaveStorageValue() {
    return 0
  }
  LoadStorageValue() {
    return 0
  }

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
  }
}
