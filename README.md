The idea here is a webassembly-host implementation of [raylib](https://www.raylib.com/) 5.5 for native & web.

The entire API is exposed to the webassembly, so anyone can run your standalone game on web or without a web-browser (native wasm-host) without recompiling it.

## differences from raylib

- export `CartInit`/`CartUpdate` for your code, instead of `main` using window/draw-management stuff (`SetTargetFPS`, `WindowShouldClose`, `BeginDrawing`, `EndDrawing`, `CloseWindow`, etc.) I might drop `CartInit` (and use main) but it's kinda nice to seperate them.
- I have not implmented every raylib function, but I am working on adding most things to the code-generators.
- I will probably skip file/audio callbacks
- memory-management is generally done cart-side.
- It's always 60 FPS, since I want it to be uniform. It can run much faster (especially on native) so I might provide an option to override this, but that is how it is for now.

## carts

You make your game as a "cart" which is just a zip-file with `main.wasm` and any assets you want access to.

Technically, you can write your cart in any language that compiles to (or can be interpretred in) webassembly, but practically, it helps to have a header to make it more ergonomic.

Here are the cart-languages we directly support, now:

- C - it's very similar to regular raylib
- Javascript - Uses quickjs main.wasm, and put your game in main.js (in cart.) This is not as efficient (slower, bigger file) as walt/assemblyscript, so choose those, if you can.

I am also working on these, but they are not as complete:

- Javascript (web) - this is similar to [raylib-wasm](https://github.com/konsumer/raylib-wasm), but you can also mount cart for filesystem, and dynamically code your game in JS, using the browser's engine, not QuickJS. This uses the main.js in the cart, and you can skip the QuickJS main.wasm, if you don't need your game to work on native. The syntax should be mostly identical to QuickJS carts.
- [Nelua](https://nelua.io/) - Use a lua-like language to make a compiled cart
- [Assemblyscript](https://www.assemblyscript.org/) - Use a language very similar to typescript/javascript to make a compiled cart
- [Walt](https://github.com/ballercat/walt) - Use a language very similar to javascript to make a light compiled cart
- Rust

## development

You will need a C compiler (clang, etc) cmake, emscripten (for web host) and carts have verious dependencies (C carts need wasi-sdk, nelua, rust, etc.) I will try to get this all building in CI, so you can find it in [releases](https://github.com/notnullgames/raycart/releases), and not need to build anything yourself.

```sh
# build carts & web runtime, and start a local reloading web-server
npm start

# delete any built files
npm run clean

# build carts and web/native runtime
npm run build

# build only native runtime
npm run build:native

# build only carts
npm run build:carts

# build both carts & native host
npm run build:nativefull

# build only web runtime
npm run build:web

# format your JS code
npm run format

# generate all the source-files from the raylib JSON
# be careful if you have modified anything
# this is not really needed for most people
npm run codegen
```

### web

Have a look at [index.html](demo/index.html) for how to use it in your own web-project. At some point I will publish it on CDNs and add a web-bundle to CI to make it a bit easier to integrate. If you have emscritpen installed, you can just run `npm start` to see it, locally.

### native

You can also use the native runtime (without a browser) like this:

```sh
./build/host/raycart demo/textures_logo_c.zip
```

## todo

- host that works on ESP32 and other very low-end devices (2D-only, etc)
- live-reloading web-build
- live-reloading native build
- Don't manage `BeginDrawing`/`EndDrawing` (require user to call these) so you can do other stuff in your loop (textures, etc)
- self-running (like love2d) and run a directory (instead of zip)

## thanks

- obviously, raylib is awesome, and the community is really helpful
- [zozlib.js](https://github.com/tsoding/zozlib.js) has some great ideas about non-emscripten raylib stuff. I didn't use this directly, but I like their style, and it provides a nice browser-native implementation of the same raylib functions.
