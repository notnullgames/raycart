#ifdef EMSCRIPTEN

#include "physfs.h"
#include "raylib.h"
#include "emscripten.h"

EM_ASYNC_JS(bool, raycart_cart_init, (char *wasmBuffer, PHYSFS_sint64 bytesRead), {
    if (!wasmBuffer || !bytesRead) {
        return false;
    }

    const wasmBytes = Module.HEAPU8.slice(wasmBuffer, wasmBuffer+Number(bytesRead));
    if (!wasmBytes.length) {
        return false;
    }

    // TODO: Generate this from host wasm exports
    const raycart = {};

    // this could have WASI access to cart too,
    // but I left it out for simplicity (so you must use raylib functions ot access files.)
    const wasi_snapshot_preview1 = new  Module.WasiPreview1();

    const { instance } = await WebAssembly.instantiate(wasmBytes, {raycart, wasi_snapshot_preview1});
    Module.cart = instance.exports;
    wasi_snapshot_preview1.start(instance.exports);

    return true;
});


bool CartInit() {
    // InitWindow(800, 450, "raylib [core] example - basic window");
    // SetTargetFPS(60);

    // Read main.wasm
    PHYSFS_File *file = PHYSFS_openRead("main.wasm");
    if (!file) {
        TraceLog(LOG_FATAL, "Could not open main.wasm: %s\n", PHYSFS_getLastError());
        return false;
    }
    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    if (size < 0) {
        TraceLog(LOG_FATAL, "Could not get size of main.wasm: %s\n", PHYSFS_getLastError());
        PHYSFS_close(file);
        return false;
    }
    char *wasmBuffer = malloc(size);
    PHYSFS_sint64 bytes_read = PHYSFS_readBytes(file, wasmBuffer, size);
    if (bytes_read != size) {
        TraceLog(LOG_FATAL, "Failed to read main.wasm: %s\n", PHYSFS_getLastError());
        free(wasmBuffer);
        PHYSFS_close(file);
        return false;
    }

    bool r = raycart_cart_init(wasmBuffer, bytes_read);
    free(wasmBuffer);

    return r;
}

bool CartUpdate() {
    // BeginDrawing();
    // ClearBackground(RAYWHITE);
    // DrawText("Congrats! You created your first web window!", 190, 200, 20, LIGHTGRAY);
    // EndDrawing();

    return true;
}

#endif