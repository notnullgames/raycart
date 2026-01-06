# QuickJS Cart

This directory contains a C cart that embeds QuickJS to run JavaScript carts. It provides the full raylib API to JavaScript through auto-generated bindings.

## Architecture

```
┌─────────────────────────────────────┐
│  JavaScript Cart (main.js)          │
│  - CartInit()                        │
│  - CartUpdate()                      │
│  - Uses raylib functions directly   │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│  raycart_bindings.c (Generated)     │
│  - Type converters (to_js/from_js)  │
│  - 537 function wrappers            │
│  - expose_things_to_js()            │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│  main.c (QuickJS Runtime)           │
│  - Initializes QuickJS              │
│  - Loads main.js from cart          │
│  - Calls CartInit/CartUpdate        │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│  raycart.h (C API)                  │
│  - raylib function imports          │
│  - Communicates with host           │
└─────────────────────────────────────┘
```

## Files

- **main.c** - QuickJS runtime initialization and cart callback handling
- **raycart_bindings.c** - Auto-generated raylib bindings (do not edit manually)
- **quickjs.c**, **quickjs-libc.c**, etc. - QuickJS source code

## Building

The QuickJS cart is built automatically when you run:

```bash
npm run build:carts
```

This compiles all the QuickJS source and `main.c` into a single `cart-js` WASM module.

**Note:** You may see compiler warnings about pointer type conversions (`char *` vs `unsigned char *`) and QuickJS FILE\* handling in WASI. These are expected and don't affect functionality - the build will complete successfully.

## Creating a JS Cart

1. Create a directory in `carts/js/your_cart_name/`
2. Add a `main.js` file with exported functions:

```javascript
export function CartInit() {
  InitWindow(800, 450, 'My Cart')
}

export function CartUpdate() {
  ClearBackground(RAYWHITE)
  DrawText('Hello from JS!', 190, 200, 20, LIGHTGRAY)
}

export function CartClose() {
  // Optional cleanup
}
```

3. Add the cart to `carts/CMakeLists.txt`:

```cmake
BUILD_CART_JS("your_cart_name")
```

4. Build: `npm run build:carts`

## Available APIs

JavaScript carts have access to:

- **Most raylib functions** (463 functions)
  - Window management: `InitWindow`, `CloseWindow`, `WindowShouldClose`
  - Drawing: `ClearBackground`, `DrawText`, `DrawRectangle`, `DrawTexture`
  - Input: `IsKeyPressed`, `GetMousePosition`, `IsMouseButtonPressed`
  - Textures: `LoadTexture`, `UnloadTexture`, `DrawTexture`
  - And many more!

- **All raylib constants**
  - Colors: `RAYWHITE`, `LIGHTGRAY`, `RED`, `BLUE`, etc.
  - Config flags: `FLAG_VSYNC_HINT`, `FLAG_FULLSCREEN_MODE`
  - Key codes: `KEY_SPACE`, `KEY_ENTER`, etc.

- **QuickJS standard library**
  - `std` module - File I/O, timers, etc.
  - `os` module - Operating system functions
  - ES6 modules with `import`/`export`

## Type Conversions

JavaScript types are automatically converted to/from C types:

| C Type         | JavaScript Type                        | Notes                     |
| -------------- | -------------------------------------- | ------------------------- |
| `int`, `float` | `number`                               | Direct conversion         |
| `bool`         | `boolean`                              | Direct conversion         |
| `const char*`  | `string`                               | Null-terminated strings   |
| `Color`        | `{r, g, b, a}`                         | Object with number fields |
| `Vector2`      | `{x, y}`                               | Object with number fields |
| `Texture2D`    | `{id, width, height, mipmaps, format}` | Object with number fields |

## Example: Drawing

```javascript
export function CartUpdate() {
  // Clear screen
  ClearBackground(RAYWHITE)

  // Draw shapes
  DrawRectangle(100, 100, 200, 100, RED)
  DrawCircle(200, 300, 50, BLUE)

  // Draw text
  DrawText('Score: 42', 10, 10, 20, DARKGRAY)
}
```

## Example: Input

```javascript
let playerX = 400
let playerY = 225

export function CartUpdate() {
  // Handle input
  if (IsKeyDown(KEY_RIGHT)) playerX += 2
  if (IsKeyDown(KEY_LEFT)) playerX -= 2
  if (IsKeyDown(KEY_DOWN)) playerY += 2
  if (IsKeyDown(KEY_UP)) playerY -= 2

  ClearBackground(RAYWHITE)
  DrawCircle(playerX, playerY, 20, MAROON)
}
```

## Regenerating Bindings

If the raylib API changes, regenerate the bindings:

```bash
npm run codegen:quickjs
```

This updates `raycart_bindings.c` with the latest API from `codegen/raylib_api.json`.

## Limitations

The following raylib functions are **not** available in JS carts (74 functions excluded):

- **Memory management** (`MemAlloc`, `MemFree`) - Use JavaScript's own memory
- **Callback functions** - Cannot pass functions across WASM boundary
  - `SetTraceLogCallback`, `SetLoadFileDataCallback`, etc.
  - Audio stream processors
- **Variadic functions** (`TraceLog`, `TextFormat`) - Use JavaScript's `console.log` or template strings
- **Pointer parameters** (except strings) - Complex to handle safely
  - Array parameters like `SetWindowIcons(Image *images, int count)`
  - Output parameters like `LoadImageFromMemory(const char *fileType, unsigned char *fileData, int dataSize)`
  - Use alternative functions that work with single values instead

## Performance

QuickJS carts have some overhead compared to native C carts:

- Type conversions between JS and C
- QuickJS JIT compilation
- Additional memory usage

For performance-critical code, consider using C carts instead. QuickJS carts are ideal for:

- Rapid prototyping
- Game logic and scripting
- UI and menus
- Educational purposes
