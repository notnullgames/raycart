# Code Generation

This directory contains code generators for automatically creating bindings between the host (raylib) and cart (WASM) environments.

## Scripts

- **generate_host_native.js** - Generates `host/host_native.c` (WAMR bindings for native wasm-host)
- **generate_host_web.js** - Generates `host/host_web.c` (JavaScript bindings for Emscripten)
- **generate_cart_c.js** - Generates import declarations for `carts/c/raycart.h` (C header for carts)
- **generate_cart_js.js** - Generates `carts/c/js/raycart_bindings.c` (QuickJS bindings for JS carts)

You can run all the code-generators in parallell:

```bash
npm run codegen
```

### Customization

By default, all raylib functions are exposed except those in the `functionsToExclude` list, for each generator. To exclude additional functions, edit the list:

```javascript
const functionsToExclude = [
  'MemAlloc', // Cart has its own memory management
  'MemFree'
  // Add more function names to exclude here
]
```

### Special cases

- **PhysFS**: Uses, for example, `LoadTextureFromPhysFS` is called on host, instead of `LoadTexture` to load from the PhysFS virtual filesystem, but appear to just be plain files, from the user-perspective.
- **Struct returns**: WebAssembly functions that return structs receive a result pointer as the first parameter

## generate_host_native.js

Generates `host/host_native.c` from `raylib_api.json`. This creates C bindings that bridge between the cart WASM module and the host's native [WAMR](https://github.com/bytecodealliance/wasm-micro-runtime) raylib.

### How it works

1. **Reads raylib_api.json** - Contains struct definitions, function signatures, and type information
2. **Resolves type aliases** - Handles types like `Texture2D` (alias of `Texture`)
3. **Generates function bindings** - For each raylib function:
   - Detects if it returns a struct (adds `resultPtr` as first parameter)
   - Handles string parameters (directly, as WAMR copies them)
   - Handles struct parameters (directly as a pointer, as WAMR copies them)
   - Handles primitives (passes directly)

### Key patterns

**Functions returning void:**

```c
static void raycart_InitWindow(wasm_exec_env_t exec_env, int width, int height, const char * title) {
    InitWindow(width, height, title);
}

// later
{"InitWindow", raycart_InitWindow, "(ii$)"}, // params: int, int, string
```

**Functions returning scalar-types:**

```c
static bool raycart_IsWindowReady(wasm_exec_env_t exec_env) {
    return IsWindowReady();
}

// later
{"IsWindowReady", raycart_IsWindowReady, "()i"}, // no params, returns int
```

**Functions returning structs:**

```c
static void raycart_LoadTexture(wasm_exec_env_t exec_env, Texture2D* __result, const char * fileName) {
    *__result = LoadTextureFromPhysFS(fileName);
}

// later
{"LoadTexture", raycart_LoadTexture, "(*$)"}, // params: pointer, string
```

**Functions with struct parameters:**

```c
static void raycart_ClearBackground(wasm_exec_env_t exec_env, Color* color) {
    ClearBackground(*color);
}

// later
{"ClearBackground", raycart_ClearBackground, "(*)"}, // pointer
```

### Usage

```bash
npm run codegen:hostnative
```

## generate_host_web.js

Generates `host/host_web.c` from `raylib_api.json`. This creates JavaScript bindings that bridge between the cart WASM module and the host's Emscripten-compiled raylib.

### How it works

1. **Reads raylib_api.json** - Contains struct definitions, function signatures, and type information
2. **Calculates struct sizes** - Determines memory layout for each struct type
3. **Resolves type aliases** - Handles types like `Texture2D` (alias of `Texture`)
4. **Generates memory helpers** - Creates `cart*` and `host*` functions for each struct type
5. **Generates function bindings** - For each raylib function:
   - Detects if it returns a struct (adds `resultPtr` as first parameter)
   - Handles string parameters (copies using `cartString`)
   - Handles struct parameters (copies using `cart*` helpers)
   - Handles primitives (passes directly)

### Key patterns

**Functions returning void:**

```javascript
InitWindow(width, height, title) {
    const title_h = cartString(title);
    Module._InitWindow(width, height, title_h);
    Module._MemFree(title_h);
}
```

**Functions returning structs:**

```javascript
LoadTexture(resultPtr, fileName) {
    const fileName_h = cartString(fileName);
    const result_h = Module._MemAlloc(20);
    Module._LoadTextureFromPhysFS(result_h, fileName_h);
    Module._MemFree(fileName_h);
    copyHostToCart(result_h, resultPtr, 20);
    Module._MemFree(result_h);
}
```

**Functions with struct parameters:**

```javascript
ClearBackground(color) {
    const color_h = cartColor(color);
    Module._ClearBackground(color_h);
    Module._MemFree(color_h);
}
```

### Usage

```bash
npm run codegen:hostweb
```

## generate_cart_c.js

Generates the complete `carts/c/raycart.h` header file from `raylib_api.json`. This creates all type definitions, enums, constants, and function prototypes that carts need to use raylib.

### How it works

1. **Reads raylib_api.json** - Gets all API information
2. **Generates structs** - Creates typedef struct definitions with proper field types and array handling
3. **Inserts type aliases** - Places aliases (Texture2D, Quaternion, etc.) right after their base types
4. **Generates enums** - Creates all enumeration types
5. **Generates defines** - Creates color constants and other defines
6. **Filters functions** - Excludes memory management, callbacks, and variadic functions
7. **Generates imports** - Creates `RC_IMPORT` and function prototypes

### Excluded functions

- **Memory management** - `MemAlloc`, `MemFree` (carts have their own)
- **Callbacks** - Functions with callback parameters (not supported across WASM boundary)
- **Variadic functions** - `TraceLog`, `TextFormat` (complex to handle in WASM)

### Generated content

The complete header includes:

**Types:**

```c
typedef struct Vector2 {
    float x;
    float y;
} Vector2;

// Type alias right after base type
typedef Vector4 Quaternion;
```

**Enums:**

```c
typedef enum {
    FLAG_VSYNC_HINT = 64,
    FLAG_FULLSCREEN_MODE = 2,
    // ...
} ConfigFlags;
```

**Defines:**

```c
#define LIGHTGRAY    (Color){ 200, 200, 200, 255 }
#define RAYWHITE     (Color){ 245, 245, 245, 255 }
```

**Function imports:**

```c
RC_IMPORT("InitWindow")
void InitWindow(int width, int height, const char * title);

RC_IMPORT("LoadTexture")
Texture2D LoadTexture(const char * fileName);
```

### Usage

```bash
npm run codegen:cart
```

## generate_cart_js.js

Generates `carts/c/js/raycart_bindings.c` from `raylib_api.json`. This creates QuickJS C bindings that expose the raylib API to JavaScript code running inside a QuickJS cart.

This was my first interpretor cart, and it uses a C cart, but then exposes everything to a QuickJS interpretor. You can think of this as an example for other interpretors (python, etc.)

### How it works

1. **Reads raylib_api.json** - Gets all API information
2. **Filters functions** - Excludes functions with callbacks, variadic args, and pointer parameters (except strings)
3. **Generates type converters** - Creates `to_js` and `from_js` functions for all primitive and struct types
4. **Generates function wrappers** - For each raylib function, creates a `js_FunctionName` wrapper that:
   - Converts JSValue parameters to C types
   - Calls the actual raylib function
   - Converts return value back to JSValue
5. **Generates expose function** - Creates `expose_things_to_js()` that registers:
   - All color constants
   - All enum values
   - All function wrappers

### Key patterns

**Type converters for primitives:**

```c
static JSValue i32_to_js(int32_t value) {
  return JS_NewInt32(ctx, value);
}

static int32_t i32_from_js(JSValue val) {
  int32_t result = 0;
  JS_ToInt32(ctx, &result, val);
  return result;
}
```

**Type converters for structs:**

```c
static JSValue color_to_js(Color value) {
  JSValue obj = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, obj, "r", u32_to_js(value.r));
  JS_SetPropertyStr(ctx, obj, "g", u32_to_js(value.g));
  JS_SetPropertyStr(ctx, obj, "b", u32_to_js(value.b));
  JS_SetPropertyStr(ctx, obj, "a", u32_to_js(value.a));
  return obj;
}

static Color color_from_js(JSValue obj) {
  Color result = {0};
  JSValue r_val = JS_GetPropertyStr(ctx, obj, "r");
  if (!JS_IsUndefined(r_val)) {
    result.r = u32_from_js(r_val);
  }
  JS_FreeValue(ctx, r_val);
  // ... repeat for g, b, a
  return result;
}
```

**Function wrappers:**

```c
// Void return
static JSValue js_ClearBackground(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  ClearBackground(color_from_js(argv[0]));
  return JS_UNDEFINED;
}

// Primitive return
static JSValue js_WindowShouldClose(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  return bool_to_js(WindowShouldClose());
}

// Struct return
static JSValue js_GetColor(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  return color_to_js(GetColor(u32_from_js(argv[0])));
}
```

**Exposing to JavaScript:**

```c
void expose_things_to_js() {
  // Color constants
  JS_SetPropertyStr(ctx, global, "RAYWHITE", color_to_js(RAYWHITE));

  // Enum values
  JS_SetPropertyStr(ctx, global, "FLAG_VSYNC_HINT", i32_to_js(FLAG_VSYNC_HINT));

  // Functions
  JS_SetPropertyStr(ctx, global, "InitWindow", JS_NewCFunction(ctx, js_InitWindow, "InitWindow", 3));
}
```

### Usage

```bash
npm run codegen:cartjs
```

### Excluded functions

74 functions are excluded (463 of 537 total functions are included):

- **Memory management** - `MemAlloc`, `MemFree`, `MemRealloc`
- **Callback functions** - Cannot pass functions across WASM boundary
  - `SetTraceLogCallback`, `SetLoadFileDataCallback`, `SetSaveFileDataCallback`
  - `SetLoadFileTextCallback`, `SetSaveFileTextCallback`
  - `AttachAudioStreamProcessor`, `DetachAudioStreamProcessor`
  - `AttachAudioMixedProcessor`, `DetachAudioMixedProcessor`
  - `SetAudioStreamCallback`
- **Variadic functions** - `TraceLog`, `TextFormat` (cannot handle `...` parameters)
- **Pointer parameters** (except strings) - Too complex for safe JS binding
  - Array parameters like `Image *images`
  - Mutable output parameters like `unsigned char *fileData`
  - Generic pointers like `void *data`
- **Pointer returns** (except strings) - Memory management complexity

### Integration

The generated file is included in `carts/c/js/main.c`:

```c
#include "raycart_bindings.c"

void CartInit() {
  // ... setup QuickJS ...
  expose_things_to_js();
  // ... load cart JavaScript ...
}
```

This allows the cart's JavaScript code to call raylib functions directly:

```javascript
// main.js
export function CartInit() {
  InitWindow(800, 450, 'QuickJS Cart')
}

export function CartUpdate() {
  ClearBackground(RAYWHITE)
  DrawText('Hello from JavaScript!', 190, 200, 20, LIGHTGRAY)
}
```
