#ifdef EMSCRIPTEN

#include "raylib.h"
#include "emscripten.h"

// TODO: move this out to external JS file for better ergonomics (and codegen)
EM_ASYNC_JS(bool, CartInit, (char *wasmBuffer, int bytesRead), {
    if (!wasmBuffer || !bytesRead) {
        console.error('wasm byte-length is 0.');
        return false;
    }

    const wasmBytes = Module.HEAPU8.slice(wasmBuffer, wasmBuffer+Number(bytesRead));
    if (!wasmBytes.length) {
        console.error('wasm byte-length is not what was expted.');
        return false;
    }

    if (!Module.wasi_snapshot_preview1) {
        console.error('wasi_snapshot_preview1 has not been provided.');
        return false;
    }

    // utils for copying memory
    // this allocates, so make sure to MemFree it
    // TODO "scratch space" mem would be more efficient & might be easier to work with
    const copyFromCart = (ptr, size) => {
        const cartMemory = new Uint8Array(cart.memory.buffer);
        const hostPtr = Module._MemAlloc(size);
        Module.HEAPU8.set(cartMemory.subarray(ptr, ptr + size), hostPtr);
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

    const copyStringFromCart = (ptr) => copyFromCart(ptr, cartStringLen(ptr) + 1);

    const decoder = new TextDecoder();
    const debugCartString = ptr => decoder.decode(new Uint8Array(cart.memory.buffer, ptr, cartStringLen(ptr)));

    const debugCartColor = ptr => {
        const [r, g, b, a] = new Uint8Array(cart.memory.buffer, ptr, 4);
        return `{ r: ${r}, g: ${g}, b: ${b}, a: ${a} }`;
    };

    const cartColor = ptr => copyFromCart(ptr, 4);

    // TODO: Generate this from host wasm exports
    const raycart = {
        InitWindow(width, height, title) {
            // console.log(`CART called InitWindow(${width}, ${height}, "${debugCartString(title)}")`);
            const title_h = copyStringFromCart(title);
            Module._InitWindow(width, height, title_h);
            Module._MemFree(title_h);
        },
        
        ClearBackground(color) {
            // console.log(`CART called ClearBackground(${debugCartColor(color)})`);
            const color_h = cartColor(color);
            Module._ClearBackground(color_h);
            Module._MemFree(color_h);
        },
        
        DrawText(text, posX, posY, fontSize, color) {
            // console.log(`CART called DrawText("${debugCartString(text)}", ${posX}, ${posY}, ${fontSize}, ${debugCartColor(color)})`);
            const text_h = copyStringFromCart(text);
            const color_h = cartColor(color);
            Module._DrawText(text_h, posX, posY, fontSize, color_h);
            Module._MemFree(text_h);
            Module._MemFree(color_h);
        }
    };

    const { instance } = await WebAssembly.instantiate(wasmBytes, {raycart, wasi_snapshot_preview1: Module.wasi_snapshot_preview1});
    const cart = Module.cart = instance.exports;
    Module.wasi_snapshot_preview1.start(cart);
    
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