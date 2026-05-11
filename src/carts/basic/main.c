#include "raylib.h"
#include <math.h>

void CartUpdate(void) {
    float t = (float)GetTime();
    int w = GetScreenWidth(), h = GetScreenHeight();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    int cx = w / 2 + (int)(cosf(t) * 120);
    int cy = h / 2 + (int)(sinf(t * 0.7f) * 80);
    DrawCircle(cx, cy, 40, MAROON);

    int rw = 200 + (int)(sinf(t * 2) * 30);
    DrawRectangle(w / 2 - rw / 2, 20, rw, 14, DARKBLUE);

    DrawText("raycart — raylib on wasm via wasi-sdk", 10, h - 30, 16, GRAY);
    DrawFPS(10, 10);

    EndDrawing();
}

int main(void) {
    InitWindow(800, 450, "raycart basic");
    return 0;
}
