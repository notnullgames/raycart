# raycart

Wasm host that exposes the full raylib 6.1.0 API to WebAssembly modules. Games compile with wasi-sdk (no emscripten). Same `.wasm` binary runs on native wasm runtimes (WAMR, wasm3) and in the browser.

## Goal

Users write a standard C raylib game, compile with wasi-sdk targeting wasi32, and the host supplies all raylib functions as wasm imports. No recompile to switch between native and web. Other wasm-targeting languages (Rust, AssemblyScript, Zig, etc.) get full raylib access for free.

## Architecture

```
main.c  →  wasi-sdk  →  game.wasm
                              │
              ┌───────────────┼───────────────┐
              │               │               │
        native host     web host (JS)    other hosts
        (C + WAMR)     (canvas/WebGL)   (wasm3, etc.)
```

### Hosts

**Native** (`host/native/`): C program using WAMR. Links against raylib natively. Registers all raylib functions as wasm imports, loads `.wasm`, drives the game loop.

**Web** (`host/web/`): JavaScript class (like `glfw.js` in `ideas/wasi-sdk-glfw/web/glfw.js`). Uses `WebAssembly.instantiateStreaming`. Drives game loop via `requestAnimationFrame`. Renders to a `<canvas>` using raylib-web (or a JS reimplementation of the raylib draw API over WebGL/Canvas2D).

### Cart (wasm module)

The game wasm exports:

- `memory` — linear memory (required)
- `malloc` / `free` — for host to allocate strings/data in wasm memory
- `_start` (WASI entry) — optional init; game loop runs separately
- lifecycle: `CartPreload`, `CartInit`, `CartUpdate`

The game imports everything from module `"raylib"`:

```c
__attribute__((import_module("raylib"), import_name("InitWindow")))
void InitWindow(int width, int height, const char* title);
```

wasi-sdk by default puts undefined symbols in `"env"` namespace. To use `"raylib"` namespace, the header uses `__attribute__((import_module(...), import_name(...)))`.

### Codegen

All host bindings and the wasm header are generated from `raylib_api.json` (raylib 6.1.0 official API JSON). Scripts live in `codegen/`:

- `scripts/gen_cart_c_header.js` → `include/raylib.h` (wasm-side import declarations)
- `scripts/gen_host_native.js` → `host/native/bindings.c` (WAMR NativeSymbol table)
- `scripts/gen_host_web.js` → `host/web/raylib.js` (JS host class)

Run all: `npm run codegen`

## Memory Passing — No-Copy Strategy

This is the core design constraint. Prefer zero-copy over any serialization/deserialization.

### How wasm linear memory works

Both hosts have a direct byte view into wasm memory:

- **WAMR**: `wasm_runtime_addr_app_to_native(inst, app_ptr, size)` → native `void*`
- **Web**: `new Uint8Array(memory.buffer, ptr, size)` or `new DataView(memory.buffer)`

### Rules by type

**Strings (wasm → host)**
Pass a `uint32_t` pointer into wasm memory. Host reads null-terminated bytes directly from wasm memory. No copy, no allocation.

```c
// wasm side (generated header)
__attribute__((import_module("raylib"), import_name("InitWindow")))
void InitWindow(int width, int height, const char* title);

// native host binding — title is already a native ptr via WAMR
static void host_InitWindow(wasm_exec_env_t env, int w, int h, const char* title) {
    InitWindow(w, h, title);  // WAMR validates and gives us native ptr
}
```

```js
// web host binding — title is a ptr into wasm memory
InitWindow(w, h, titlePtr) {
    const title = this._readStr(titlePtr);  // zero-copy read, one TextDecoder call
    // set up canvas, store dimensions
}
```

**Structs as parameters (wasm → host)**
Pass a pointer to the struct in wasm memory. Host reads fields directly from wasm memory. No copy.

```c
// wasm side: Color passed as pointer
__attribute__((import_module("raylib"), import_name("ClearBackground")))
void ClearBackground(Color* color);  // pointer, not by value
```

```c
// native host: WAMR gives validated native ptr
static void host_ClearBackground(wasm_exec_env_t env, Color* color) {
    ClearBackground(*color);
}
```

```js
// web host: read fields directly from memory
ClearBackground(colorPtr) {
    const v = this._view;
    const r = v.getUint8(colorPtr), g = v.getUint8(colorPtr+1),
          b = v.getUint8(colorPtr+2), a = v.getUint8(colorPtr+3);
    // use r,g,b,a
}
```

**Structs as return values (host → wasm)**
wasm ABI can't return structs by value across the boundary. Use sret convention: caller allocates space in wasm memory, passes a result pointer as the first parameter, host writes into it.

```c
// wasm side
__attribute__((import_module("raylib"), import_name("GetMousePosition")))
void GetMousePosition(Vector2* result);  // result written into caller's wasm memory

// user-facing wrapper in header (inline, no import)
static inline Vector2 GetMousePosition(void) {
    Vector2 r;
    __raylib_GetMousePosition(&r);
    return r;
}
```

```c
// native host: result is a valid native ptr into wasm memory
static void host_GetMousePosition(wasm_exec_env_t env, Vector2* result) {
    *result = GetMousePosition();  // write directly into wasm memory
}
```

```js
// web host: write fields directly into wasm memory
GetMousePosition(resultPtr) {
    // ... get mouse position ...
    this._view.setFloat32(resultPtr, x, true);
    this._view.setFloat32(resultPtr + 4, y, true);
}
```

**Arrays (host → wasm)**
When host needs to return a variable-length array (e.g. `LoadImageColors`), allocate in wasm memory via the exported `malloc`, write directly, return the pointer. Wasm module owns the memory and calls `free`.

```c
// native host
static void host_LoadImageColors(wasm_exec_env_t env, uint32_t* resultPtr, Image* image) {
    Color* colors = LoadImageColors(*image);
    int count = image->width * image->height;
    uint32_t wasm_ptr = wasm_runtime_module_malloc(inst, count * sizeof(Color), NULL);
    memcpy(wasm_runtime_addr_app_to_native(inst, wasm_ptr, count * sizeof(Color)),
           colors, count * sizeof(Color));
    UnloadImageColors(colors);
    *resultPtr = wasm_ptr;
}
```

**Strings (host → wasm)**
Allocate in wasm memory via malloc export, write bytes, return pointer. Caller responsible for freeing (or use static buffer for short-lived strings).

### What not to do

- Do NOT copy struct fields one-by-one through JS objects (like the emscripten raycart approach)
- Do NOT alloc a host-side buffer just to copy it into wasm memory immediately after
- Do NOT use JSON or any serialization format

## Variadic Functions

Skip from host imports entirely. Implement cart-side in the header:

```c
// In generated raylib.h — no import, pure cart-side
static char _textFormatBuf[1024];
static inline const char* TextFormat(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(_textFormatBuf, sizeof(_textFormatBuf), fmt, ap);
    va_end(ap);
    return _textFormatBuf;
}

static inline void TraceLog(int level, const char* fmt, ...) {
    // write to stderr via wasi fd_write
}
```

Variadic list: `TraceLog`, `TextFormat` (and any others with `...` params in the JSON).

## Game Loop

No `while(!WindowShouldClose())` in wasm. The host drives the loop.

**wasm exports:**

```c
// game.c
#include "raylib.h"

void update(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Hello", 10, 10, 20, BLACK);
    EndDrawing();
}

int main(void) {
    InitWindow(800, 450, "My Game");
    SetTargetFPS(60);
    return 0;
}
```

- `main` / `_start`: runs once at load time. Call `InitWindow`, set up resources.
- `update`: called each frame by host. Contains `BeginDrawing` / `EndDrawing` and all draw calls.

Host calls `_start` (via WASI), then calls `update` in a loop (native) or via `requestAnimationFrame` (web).

Functions like `WindowShouldClose`, `SetTargetFPS`, `BeginDrawing`, `EndDrawing`, `CloseWindow` are all still exposed — user can call them normally; host just ignores the return value of `WindowShouldClose` and manages the loop timing itself.

## Compile Target

Games compile with wasi-sdk:

```sh
/opt/wasi-sdk/bin/clang \
  --sysroot=/opt/wasi-sdk/share/wasi-sysroot \
  -target wasm32-wasi \
  -Iinclude \
  game.c -o game.wasm \
  -Wl,--export=update \
  -Wl,--export=malloc \
  -Wl,--export=free \
  -Wl,--export=memory
```

No `-mexec-model=reactor` needed — `main` is the entry point, `update` is exported separately.

## Project Layout

```
raycart/
├── CLAUDE.md
├── package.json
├── raylib_api.json           # raylib 6.0 official API JSON (source of truth)
├── codegen/
│   ├── gen_header.js         # → include/raylib.h
│   ├── gen_host_native.js    # → host/native/bindings.c
│   └── gen_host_web.js       # → host/web/raylib.js
├── include/
│   └── raylib.h              # generated wasm-side header (do not edit by hand)
├── host/
│   ├── native/
│   │   ├── main.c            # WAMR runtime, loads wasm, drives loop
│   │   └── bindings.c        # generated: NativeSymbol table registering all funcs
│   └── web/
│       ├── index.html
│       ├── index.js          # bootstraps wasm + raylib host
│       └── raylib.js         # generated: JS class exposing raylib to wasm
├── examples/
│   └── basic/
│       └── game.c            # minimal example game
└── CMakeLists.txt            # builds native host
```

## Key Decisions

- **Import module name**: `"raylib"` (not `"env"`). Requires explicit `import_module` attribute in header; cleanly namespaced.
- **Struct params**: always by pointer across the wasm boundary, even when raylib takes by value natively. Inline wrapper in header restores by-value ergonomics.
- **Struct returns**: always sret (result pointer first param). Inline wrapper in header restores normal return-value ergonomics.
- **No physfs / zip**: games are plain `.wasm` files, not zipped carts. Keep it simple; filesystem via WASI.
- **No quickjs / interpreter carts**: out of scope. Focus on compiled languages.
- **Callbacks**: skip audio callbacks and stream callbacks. Window/input callbacks (key, mouse, etc.) are fine — pass function table index as i32, host calls via indirect call table.

## Excluded Functions

Functions excluded from host and header (generated by codegen):

- Memory: `MemAlloc`, `MemRealloc`, `MemFree` (wasm module has its own libc malloc)
- Variadic: `TraceLog`, `TextFormat` (implemented cart-side in header)
- File callbacks: `SetLoadFileDataCallback`, `SetSaveFileDataCallback`, `SetLoadFileTextCallback`, `SetSaveFileTextCallback` (WASI handles file I/O)
- Audio processor callbacks: `SetAudioStreamCallback`, `AttachAudioStreamProcessor`, `DetachAudioStreamProcessor`, `AttachAudioMixedProcessor`, `DetachAudioMixedProcessor`

Everything else in the raylib 6.0 API is exposed.

## Build

```sh
npm run codegen      # regenerate include/raylib.h, host/native/bindings.c, host/web/raylib.js
npm run build        # build native host + example wasm
npm run build:native # cmake build of native host only
npm run build:wasm   # compile example games with wasi-sdk
npm start            # serve web host locally
```

## Dependencies

- **raylib 6.0** (native, linked into native host)
- **WAMR** (native host wasm runtime)
- **wasi-sdk** (to compile game wasm modules)
- **Node.js** (codegen scripts, dev server)
- **cmake** (native host build)

## References

- `ideas/raycart/` — prior emscripten-based approach (do not use emscripten in this project)
- `ideas/wasi-sdk-glfw/` — pattern for wasi-sdk + JS host without emscripten
- `ideas/raycart/codegen/raylib_api.json` — copy this as `raylib_api.json`
- `ideas/wasi-sdk-glfw/web/glfw.js` — reference for JS host memory helper patterns
