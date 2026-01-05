#include "physfs.h"
#include "raylib.h"

extern bool CartInit();
extern bool CartUpdate();

int main(int argc, char *argv[]) {
    if (argc < 2) {
        TraceLog(LOG_FATAL, "Usage: %s <ZIP_FILE>\n", argv[0]);
        return 1;
    }
    
    if (!PHYSFS_init(argv[0])) {
        TraceLog(LOG_FATAL, "PHYSFS_init failed: %s\n", PHYSFS_getLastError());
        return 1;
    }
    
    if (!PHYSFS_mount(argv[1], "/", 1)) {
        TraceLog(LOG_FATAL, "Failed to mount %s: %s\n", argv[1], PHYSFS_getLastError());
        PHYSFS_deinit();
        return 1;
    }

    TraceLog(LOG_INFO, "Mounted %s successfully\n\n", argv[1]);
    
    if (CartInit()) {
        while (!WindowShouldClose() && CartUpdate()) {}
    }

    CloseWindow();
    PHYSFS_deinit();
    return 0;
}
