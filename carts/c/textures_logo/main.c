#include "raycart.h"

Texture2D texture;

static const int screenWidth = 800;
static const int screenHeight = 450;

void CartInit() {
    InitWindow(screenWidth, screenHeight, "raylib [textures] example - logo raylib");
    texture = LoadTexture("raylib_logo.png");
}

void CartUpdate() {
    ClearBackground(RAYWHITE);
    DrawTexture(texture, screenWidth/2 - texture.width/2, screenHeight/2 - texture.height/2, WHITE);
    DrawText("this IS a texture!", 360, 370, 10, GRAY);
}

void CartClose() {
    UnloadTexture(texture);
}