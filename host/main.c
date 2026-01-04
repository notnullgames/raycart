#include <stddef.h>

#ifdef EMSCRIPTEN
#include "emscripten.h"
#define RLAPI EMSCRIPTEN_KEEPALIVE
#endif

#include "raylib.h"

#define PHYSFS_IMPL
#define PHYSFS_PLATFORM_IMPL
#include "physfs.h"

// TODO
void CartInit() {
    InitWindow(800, 450, "raylib [core] example - basic window");
    SetTargetFPS(60);
}

void CartUpdate() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
}

RLAPI int main(int argc, char *argv[]) {
    // load all zip/dir args into PhysFS

    if (argc < 2) {
        TraceLog(LOG_FATAL, "No carts/dirs provided.");
        return 1;
    }

    if (PHYSFS_init(0) == 0) {
        TraceLog(LOG_FATAL, "PhysFS could not be initialized.");
        return 1;
    }

    const char* newDir = GetWorkingDirectory();
    if (PHYSFS_setWriteDir(newDir) == 0) {
        TraceLog(LOG_FATAL, "Could not set write-dir: %s", newDir);
        return 1;
    }

    TraceLog(LOG_DEBUG, "PHYSFS: Initialized PhysFS");

    for (int i=1;i<argc;i++) {
        TraceLog(LOG_INFO, "Cart added: %s", argv[i]);
        if (PHYSFS_mount(argv[i], NULL, 1) == 0) {
            TraceLog(LOG_FATAL, "Cart could not be loaded: %s", argv[i]);
            return 1;
        }
    }

    // Use PhysFS in raylib
    // It's missing some callbacks, see https://github.com/raysan5/raylib/issues/4551
    // SetLoadFileDataCallback()
    // SetSaveFileDataCallback()
    // SetLoadFileTextCallback()
    // SetSaveFileTextCallback()

    // TODO: on native setup wamr and live-reloading

    CartInit();
    while (!WindowShouldClose()) {
        CartUpdate();
    }
    CloseWindow();
    
    if (PHYSFS_deinit() == 0) {
        TraceLog(LOG_FATAL, "PhysFS could not be de-initialized.");
        return 1;
    }
    TraceLog(LOG_DEBUG, "PHYSFS: Closed successfully");

    return 0;
}