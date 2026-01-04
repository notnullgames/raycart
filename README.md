The idea here is a webassembly-host implementation of [raylib](https://www.raylib.com/).

The entire API is exposed to the webassembly, so it can run on web & native, without recompile.

## differences from raylib

- I have not implmented every raylib function, but I am building them up 1-by-1
- export `CartInit`/`CartUpdate` for your code, instead of `main` using window/draw-management stuff (`SetTargetFPS`, `WindowShouldClose`, `BeginDrawing`, `EndDrawing`, `CloseWindow`, etc)

## carts

You make your game as a "cart" which is just a zip file with `main.wasm` and any assets you want access to.

Technically, you can write your cart in any language that compiles to (or can be interpretred in) webassembly, but practically, it helps to have a header to make it more ergonomic.

Here are the cart-languages we directly support, now:

- C - it's very similar to regular raylib

I am also working on these, but they are not as complete:

- Javascript - Use quickjs, and put your game in main.js (in cart) not as efficient as walt/assemblyscript, so choose those, if you can
- [Nelua](https://nelua.io/) - Use a lua-like language to make a compiled cart
- [Assemblyscript](https://www.assemblyscript.org/) - Use a language very similar to typescript/javascript to make a compiled cart
- [Walt](https://github.com/ballercat/walt) - Use a language very similar to javascript to make a light compiled cart

## building

```sh
# build native and carts
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# build web
emcmake cmake -G Ninja -B wbuild -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
cmake --build wbuild

# test web version
npx -y live-server demo
```

## usage

Here is how to run it on native:

```sh
raycart <ZIP_OR_DIR>
```

It will mount multiple cart/dirs in order, so you can mix & match them.

You can also use it on the web:

```html
<canvas id="canvas" />
<script type="module">
  import raycart from "./raycart.js";
  const cart = [
    "c_basic_window.zip",
    await fetch("c_basic_window.zip").then((r) => r.arrayBuffer()),
  ];
  const host = await raycart([cart], document.getElementById("canvas"));
</script>
```

## todo

- host that works on ESP32 and other very low-end devices (2D-only, etc)

## thanks

- obviously, raylib is awesome, and the community is really helpful
- [zozlib.js](https://github.com/tsoding/zozlib.js) has some great ideas about non-emscripten raylib stuff
