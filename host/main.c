#include "raylib.h"

#define RAYLIB_PHYSFS_IMPLEMENTATION
#include "raylib-physfs.h"

// these are implemented for each host-type
extern bool CartInit(char *wasmBuffer, int bytesRead);
extern bool CartUpdate();
extern void CartClose();

#ifdef EMSCRIPTEN
#include "emscripten.h"
void CartUpdateWrapper() {
    if (!CartUpdate()) {
        emscripten_cancel_main_loop();
    }
}
#endif

int main(int argc, char *argv[]) {
    if (argc < 2) {
        TraceLog(LOG_FATAL, "Usage: %s <ZIP_FILE>", argv[0]);
        return 1;
    }
    
    if (!PHYSFS_init(argv[0])) {
        TraceLog(LOG_FATAL, "PHYSFS_init failed: %s", PHYSFS_getLastError());
        return 1;
    }
    
    if (!PHYSFS_mount(argv[1], "/", 1)) {
        TraceLog(LOG_FATAL, "Failed to mount %s: %s", argv[1], PHYSFS_getLastError());
        PHYSFS_deinit();
        return 1;
    }

    TraceLog(LOG_INFO, "Mounted %s successfully", argv[1]);

    // Read main.wasm
    PHYSFS_File *file = PHYSFS_openRead("main.wasm");
    if (!file) {
        TraceLog(LOG_FATAL, "Could not open main.wasm: %s", PHYSFS_getLastError());
        return 1;
    }
    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    if (size < 0) {
        TraceLog(LOG_FATAL, "Could not get size of main.wasm: %s", PHYSFS_getLastError());
        PHYSFS_close(file);
        return 1;
    }
    char *wasmBuffer = MemAlloc(size);
    PHYSFS_sint64 bytes_read = PHYSFS_readBytes(file, wasmBuffer, size);
    if (bytes_read != size) {
        TraceLog(LOG_FATAL, "Failed to read main.wasm: %s", PHYSFS_getLastError());
        MemFree(wasmBuffer);
        PHYSFS_close(file);
        return 1;
    }

    bool r = CartInit(wasmBuffer, bytes_read);
    MemFree(wasmBuffer);
    
    if (r) {
        // WindowShouldClose works on emscripten too (via asyncify) but this is a bit lighter 
        #ifndef EMSCRIPTEN
        while (!WindowShouldClose() && CartUpdate()) {}
        #else
        emscripten_set_main_loop(CartUpdateWrapper, 60, true);
        #endif
    }

    CartClose();
    CloseWindow();
    PHYSFS_deinit();

    return 0;
}
