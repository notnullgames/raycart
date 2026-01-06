#include "raycart.h"

void CartInit() {
    InitWindow(800, 450, "raylib [core] example - basic window");
}

void CartUpdate() {
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first raycart C window!", 140, 200, 20, LIGHTGRAY);
}
