#include "raycart.h"

Camera camera = { 0 };
Vector3 position = { 0.0f, 0.0f, 0.0f };
Model model;

void CartInit() {
    InitWindow(800, 450, "Model Loading Test");

    camera.position = (Vector3){ 6.0f, 6.0f, 6.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    TraceLog(LOG_INFO, "About to load model");
    model = LoadModel("robot.glb");
    TraceLog(LOG_INFO, "Model loaded successfully");
}

void CartUpdate() {
    UpdateCamera(&camera, CAMERA_ORBITAL);

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
        DrawModel(model, position, 1.0f, WHITE);
        DrawGrid(10, 1.0f);
    EndMode3D();

    DrawText("Model loading test - NO animations", 10, 10, 20, GRAY);
}

void CartClose() {
    TraceLog(LOG_INFO, "Closing");
    UnloadModel(model);
}
