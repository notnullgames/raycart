This is a webassembly host-implementation for raylib. Essentially, all raylib functions are exposed to your wasm.

My main test is C code that looks like normal C raylib, but is compiled with wasi-sdk, but you should be able to use the same host with wasm made in nelua, assemblyscript, walt, go, or any other language that compiles to wasm. All assets are available in the WASI filesystem, and should work like normal.

The "web" in webassembly is a misnomer here. It's not just for the web (but will run there) it's a sandboxed runtime abstraction, for all kinds of places. A real strength is no-recompile is needed, and it will run on many things (browser, native on desktop computer, barebones pi, esp32, etc.)

I am still working on filling in all the raylib API. A lot is missing.

## cart

A "cart" is just a wasm file, or a zip file with `main.wasm` and any assets it needs.

## API differences

- Lifecycle exports are split up like this:
  - `_start` - this is a WASI thing, it calls `main()` in C. Setup the basic screen here.
  - `CartPreload` - This is called before assets are loaded, and you can use it to show a progress-bar or something
  - `CartInit` - assets are loaded in filesystem, so you can load images and stuff here
  - `CartUpdate` - called every frame (about 60FPS on mac web)
- All variable-argument functions (`Tracelog`, etc) should do their text-formatting cart-side. `Tracelog` just logs plain string with a type-tag, for example, but in practice it works the same because the C header (for was-sdk) does the formatting. Other wasm languages should do this too

## thanks

- obviously, raylib is awesome, and the community is really helpful
- [zozlib.js](https://github.com/tsoding/zozlib.js) has some great ideas about non-emscripten raylib stuff. I didn't use this directly, but I like their style, and it provides a nice browser-native implementation of the same raylib functions.
