#include "raylib.h"

#define RAYLIB_PHYSFS_IMPLEMENTATION
#include "raylib-physfs.h"

// these are implemented for each host-type
extern bool CartInit(char *wasmBuffer, int bytesRead);
extern bool CartUpdate();
extern void CartClose();

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

    // Set up PhysFS callbacks so all raylib file operations use PhysFS
    SetLoadFileDataCallback(LoadFileDataFromPhysFS);
    SetSaveFileDataCallback(SaveFileDataToPhysFS);
    SetLoadFileTextCallback(LoadFileTextFromPhysFS);
    SetSaveFileTextCallback(SaveFileTextToPhysFS);

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

    SetTargetFPS(60);
    
    if (r) {
        while (!WindowShouldClose() && CartUpdate()) {}
    }

    CartClose();
    CloseWindow();
    PHYSFS_deinit();

    return 0;
}
