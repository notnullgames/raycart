#ifdef EMSCRIPTEN

#include "raylib.h"
#include "emscripten.h"

// TODO: move this out to external file for better ergonomics (and codegen)
EM_ASYNC_JS(bool, CartInit, (char *wasmBuffer, int bytesRead), {
    if (!wasmBuffer || !bytesRead) {
        return false;
    }

    const wasmBytes = Module.HEAPU8.slice(wasmBuffer, wasmBuffer+Number(bytesRead));
    if (!wasmBytes.length) {
        return false;
    }

    // utils for copying memory
    // this allocates, so make sure to MemFree it
    // TODO "scratch space" mem would be more efficient & might be easier to work with
    const copyFromCart = (ptr, size) => {
        const cartMemory = new Uint8Array(cart.memory.buffer);
        const hostPtr = Module._MemAlloc(size);
        Module.HEAPU8.set(hostPtr, cartMemory.slice(ptr, ptr+size));
        return hostPtr;
    };

    const cartStringLen = ptr => {
        const cartMemory = new Uint8Array(cart.memory.buffer, ptr, 1024);
        let strlen = 0;
        while (cartMemory[strlen] !== 0 && strlen < cartMemory.length) {
            strlen++;
        }
        return strlen;
    };

    const copyStringFromCart = (ptr) => copyFromCart(ptr, ptr + cartStringLen(ptr) + 1);

    const decoder = new TextDecoder();
    const debugCartString = ptr => decoder.decode(new Uint8Array(cart.memory.buffer, ptr, cartStringLen(ptr)));

    const debugCartColor = ptr => {
    const [r, g, b, a] = new Uint8Array(cart.memory.buffer, ptr, 4);
        return `{ r: ${r}, g: ${g}, b: ${b}, a: ${a} }`;
    };

    // TODO: Generate this from host wasm exports
    const raycart = {
        InitWindow(width, height, title) {
            console.log(`CART called InitWindow(${width}, ${height}, "${debugCartString(title)}")`);
            const cartTitle = copyStringFromCart(title);
            Module._InitWindow(width, height, cartTitle);
            Module._MemFree(cartTitle);
        },
        
        ClearBackground(color) {
            console.log(`CART called ClearBackground(${debugCartColor(color)})`);
            const cartColor = copyFromCart(color, 4);
            Module._ClearBackground(cartColor);
            Module._MemFree(cartColor);
        },
        
        DrawText(text, posX, posY, fontSize, color) {
            console.log(`CART called DrawText("${debugCartString(text)}", ${posX}, ${posY}, ${fontSize}, ${debugCartColor(color)})`);
            const cartText = copyStringFromCart(text);
            const cartColor = copyFromCart(color, 4);
            Module._DrawText(cartText, posX, posY, fontSize, cartColor);
            Module._MemFree(cartText);
            Module._MemFree(cartColor);
        }
    };

    // this could have WASI access to cart too,
    // but I left it out for simplicity (so you must use raylib functions ot access files.)
    const wasi_snapshot_preview1 = new  Module.WasiPreview1();

    const { instance } = await WebAssembly.instantiate(wasmBytes, {raycart, wasi_snapshot_preview1});
    const cart = Module.cart = instance.exports;
    wasi_snapshot_preview1.start(cart);
    
    console.log('CART', cart);

    cart?.CartInit && cart.CartInit();

    return true;
});


EM_JS(bool, CartUpdate, (), {
    Module._BeginDrawing();
    Module?.cart?.CartUpdate && Module.cart.CartUpdate();
    Module._EndDrawing();
    return true;
});

#endif