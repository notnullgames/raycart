#ifndef EMSCRIPTEN

#include "raylib.h"

// TODO: implement WAMR host

bool CartInit(char *wasmBuffer, int bytesRead) {
    InitWindow(800, 450, "raylib [core] example - basic window");
    SetTargetFPS(60);
    return true;
}

bool CartUpdate() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first native window!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
    return true;
}

#endif