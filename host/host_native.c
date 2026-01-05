#ifndef EMSCRIPTEN

#include "physfs.h"
#include "raylib.h"

bool CartInit() {
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