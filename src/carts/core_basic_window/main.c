#include "raylib.h"

void CartUpdate(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first raycart C window!", 140, 200, 20, LIGHTGRAY);
    EndDrawing();
}

int main(void) {
    InitWindow(800, 450, "raycart basic");
    return 0;
}
