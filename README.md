The idea here is a webassembly-host implementation of [raylib](https://www.raylib.com/) 5.5 for native & web.

The entire API is exposed to the webassembly, so anyone can run your game on web or without a web-browser (native wasm-host) without recompile.

## differences from raylib

- export `CartInit`/`CartUpdate` for your code, instead of `main` using window/draw-management stuff (`SetTargetFPS`, `WindowShouldClose`, `BeginDrawing`, `EndDrawing`, `CloseWindow`, etc.) I might drop `CartInit` (and use main) but it's kinda nice to seperate them.
- I have not implmented every raylib function (just enough to run a hello-world) but I will setup a code-generator at some point that wraps everything

## carts

You make your game as a "cart" which is just a zip file with `main.wasm` and any assets you want access to.

Technically, you can write your cart in any language that compiles to (or can be interpretred in) webassembly, but practically, it helps to have a header to make it more ergonomic.

Here are the cart-languages we directly support, now:

- C - it's very similar to regular raylib

I am also working on these, but they are not as complete:

- Javascript - Uses quickjs main.wasm, and put your game in main.js (in cart) not as efficient as walt/assemblyscript, so choose those, if you can
- Javascript (web) - this is similar to [raylib-wasm](https://github.com/konsumer/raylib-wasm), but you can also mount carts for filesystem, and dynamically code your game in JS.
- [Nelua](https://nelua.io/) - Use a lua-like language to make a compiled cart
- [Assemblyscript](https://www.assemblyscript.org/) - Use a language very similar to typescript/javascript to make a compiled cart
- [Walt](https://github.com/ballercat/walt) - Use a language very similar to javascript to make a light compiled cart
- Rust

## usage

You can see available commands with `npm run`, but the quickest way to get started is `npm start`.

## todo

- host that works on ESP32 and other very low-end devices (2D-only, etc)
- live-reloading web-build
- live-reloading native build

## thanks

- obviously, raylib is awesome, and the community is really helpful
- [zozlib.js](https://github.com/tsoding/zozlib.js) has some great ideas about non-emscripten raylib stuff
