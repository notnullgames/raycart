// TODO: this needs some host-support

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

#include "raycart.h"

Camera camera = { 0 };
Vector3 position = { 0.0f, 0.0f, 0.0f }; // Set model position

Model model;
int animsCount = 0;
unsigned int animIndex = 0;
unsigned int animCurrentFrame = 0;
ModelAnimation *modelAnimations;

void CartInit() {
    camera.position = (Vector3){ 6.0f, 6.0f, 6.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    model = LoadModel("robot.glb");

    modelAnimations = LoadModelAnimations("robot.glb", &animsCount);
}

void CartUpdate () {
    // Update
    //----------------------------------------------------------------------------------
    UpdateCamera(&camera, CAMERA_ORBITAL);

    if (modelAnimations != NULL && animsCount > 0) {
        // Select current animation
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) animIndex = (animIndex + 1)%animsCount;
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) animIndex = (animIndex + animsCount - 1)%animsCount;

        // Update model animation
        ModelAnimation anim = modelAnimations[animIndex];
        animCurrentFrame = (animCurrentFrame + 1)%anim.frameCount;
        UpdateModelAnimation(model, anim, animCurrentFrame);
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
        DrawModel(model, position, 1.0f, WHITE);    // Draw animated model
        DrawGrid(10, 1.0f);
    EndMode3D();

    if (modelAnimations != NULL && animsCount > 0) {
        ModelAnimation anim = modelAnimations[animIndex];
        DrawText("Use the LEFT/RIGHT mouse buttons to switch animation", 10, 10, 20, GRAY);
        DrawText(TextFormat("Animation: %s", anim.name), 10, GetScreenHeight() - 20, 10, DARKGRAY);
    } else {
        DrawText("Model animations not supported yet", 10, 10, 20, RED);
        DrawText("(Complex nested pointer structures need deep copying)", 10, 30, 10, GRAY);
    }
}

void CartClose() {
    UnloadModel(model);
}
