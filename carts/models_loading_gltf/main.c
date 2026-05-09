// TODO: this needs some host-support for animation

/*******************************************************************************************
*
*   raylib [models] example - loading gltf
*
*   Example complexity rating: [★☆☆☆] 1/4
*
*   LIMITATIONS:
*     - Only supports 1 armature per file, and skips loading it if there are multiple armatures
*     - Only supports linear interpolation (default method in Blender when checked
*       "Always Sample Animations" when exporting a GLTF file)
*     - Only supports translation/rotation/scale animation channel.path,
*       weights not considered (i.e. morph targets)
*
*   Example originally created with raylib 3.7, last time updated with raylib 4.2
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2020-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

Camera camera = { 0 };
Vector3 position = { 0.0f, 0.0f, 0.0f }; // Set model position

Model model;
int animsCount = 0;
unsigned int animIndex = 0;
unsigned int animCurrentFrame = 0;

void CartUpdate () {
    UpdateCamera(&camera, CAMERA_ORBITAL);
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    DrawModel(model, position, 1.0f, WHITE);    // Draw animated model
    DrawGrid(10, 1.0f);
    EndMode3D();

    DrawText("3D Model Loading Demo", 10, 10, 20, GRAY);
    DrawText("(Animations not supported yet)", 10, 30, 10, DARKGRAY);
}

void CartInit(void) {
    TraceLog(LOG_INFO, "Window initialized");

    camera.position = (Vector3){ 6.0f, 6.0f, 6.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    TraceLog(LOG_INFO, "Loading model...");
    model = LoadModel("robot.glb");
    TraceLog(LOG_INFO, "Model loaded successfully");
}

int main(void) {
    InitWindow(800, 450, "raylib [models] example - loading gltf");
    return 0;
}