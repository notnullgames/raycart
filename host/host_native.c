#ifndef EMSCRIPTEN

#include "raylib.h"
#include "raylib-physfs.h"
#include "wamr_wasi_physfs.h"
#include "wasm_export.h"
#include <stddef.h>

// TODO: make sure these mem-sizes match web-host, so you have a known amount of RAM
static uint32_t stack_size = 1024 * 1024 * 10; // 10 MB
static uint32_t heap_size = 1024 * 1024 * 10;  // 10 MB

static wasm_module_t module = NULL;
static wasm_module_inst_t module_inst = NULL;
static wasm_exec_env_t exec_env = NULL;

static wasm_function_inst_t cart_callback_init = NULL;
static wasm_function_inst_t cart_callback_update = NULL;
static wasm_function_inst_t cart_callback_close = NULL;

// Generated raylib wrapper functions
static void raycart_InitWindow(wasm_exec_env_t exec_env, int width, int height, const char * title) {
    InitWindow(width, height, title);
}

static void raycart_CloseWindow(wasm_exec_env_t exec_env) {
    CloseWindow();
}

static bool raycart_WindowShouldClose(wasm_exec_env_t exec_env) {
    return WindowShouldClose();
}

static bool raycart_IsWindowReady(wasm_exec_env_t exec_env) {
    return IsWindowReady();
}

static bool raycart_IsWindowFullscreen(wasm_exec_env_t exec_env) {
    return IsWindowFullscreen();
}

static bool raycart_IsWindowHidden(wasm_exec_env_t exec_env) {
    return IsWindowHidden();
}

static bool raycart_IsWindowMinimized(wasm_exec_env_t exec_env) {
    return IsWindowMinimized();
}

static bool raycart_IsWindowMaximized(wasm_exec_env_t exec_env) {
    return IsWindowMaximized();
}

static bool raycart_IsWindowFocused(wasm_exec_env_t exec_env) {
    return IsWindowFocused();
}

static bool raycart_IsWindowResized(wasm_exec_env_t exec_env) {
    return IsWindowResized();
}

static bool raycart_IsWindowState(wasm_exec_env_t exec_env, unsigned int flag) {
    return IsWindowState(flag);
}

static void raycart_SetWindowState(wasm_exec_env_t exec_env, unsigned int flags) {
    SetWindowState(flags);
}

static void raycart_ClearWindowState(wasm_exec_env_t exec_env, unsigned int flags) {
    ClearWindowState(flags);
}

static void raycart_ToggleFullscreen(wasm_exec_env_t exec_env) {
    ToggleFullscreen();
}

static void raycart_ToggleBorderlessWindowed(wasm_exec_env_t exec_env) {
    ToggleBorderlessWindowed();
}

static void raycart_MaximizeWindow(wasm_exec_env_t exec_env) {
    MaximizeWindow();
}

static void raycart_MinimizeWindow(wasm_exec_env_t exec_env) {
    MinimizeWindow();
}

static void raycart_RestoreWindow(wasm_exec_env_t exec_env) {
    RestoreWindow();
}

static void raycart_SetWindowIcon(wasm_exec_env_t exec_env, Image* image) {
    SetWindowIcon(*image);
}

static void raycart_SetWindowIcons(wasm_exec_env_t exec_env, Image * images, int count) {
    SetWindowIcons(images, count);
}

static void raycart_SetWindowTitle(wasm_exec_env_t exec_env, const char * title) {
    SetWindowTitle(title);
}

static void raycart_SetWindowPosition(wasm_exec_env_t exec_env, int x, int y) {
    SetWindowPosition(x, y);
}

static void raycart_SetWindowMonitor(wasm_exec_env_t exec_env, int monitor) {
    SetWindowMonitor(monitor);
}

static void raycart_SetWindowMinSize(wasm_exec_env_t exec_env, int width, int height) {
    SetWindowMinSize(width, height);
}

static void raycart_SetWindowMaxSize(wasm_exec_env_t exec_env, int width, int height) {
    SetWindowMaxSize(width, height);
}

static void raycart_SetWindowSize(wasm_exec_env_t exec_env, int width, int height) {
    SetWindowSize(width, height);
}

static void raycart_SetWindowOpacity(wasm_exec_env_t exec_env, float opacity) {
    SetWindowOpacity(opacity);
}

static void raycart_SetWindowFocused(wasm_exec_env_t exec_env) {
    SetWindowFocused();
}

static void * raycart_GetWindowHandle(wasm_exec_env_t exec_env) {
    return GetWindowHandle();
}

static int raycart_GetScreenWidth(wasm_exec_env_t exec_env) {
    return GetScreenWidth();
}

static int raycart_GetScreenHeight(wasm_exec_env_t exec_env) {
    return GetScreenHeight();
}

static int raycart_GetRenderWidth(wasm_exec_env_t exec_env) {
    return GetRenderWidth();
}

static int raycart_GetRenderHeight(wasm_exec_env_t exec_env) {
    return GetRenderHeight();
}

static int raycart_GetMonitorCount(wasm_exec_env_t exec_env) {
    return GetMonitorCount();
}

static int raycart_GetCurrentMonitor(wasm_exec_env_t exec_env) {
    return GetCurrentMonitor();
}

static void raycart_GetMonitorPosition(wasm_exec_env_t exec_env, Vector2* __result, int monitor) {
    *__result = GetMonitorPosition(monitor);
}

static int raycart_GetMonitorWidth(wasm_exec_env_t exec_env, int monitor) {
    return GetMonitorWidth(monitor);
}

static int raycart_GetMonitorHeight(wasm_exec_env_t exec_env, int monitor) {
    return GetMonitorHeight(monitor);
}

static int raycart_GetMonitorPhysicalWidth(wasm_exec_env_t exec_env, int monitor) {
    return GetMonitorPhysicalWidth(monitor);
}

static int raycart_GetMonitorPhysicalHeight(wasm_exec_env_t exec_env, int monitor) {
    return GetMonitorPhysicalHeight(monitor);
}

static int raycart_GetMonitorRefreshRate(wasm_exec_env_t exec_env, int monitor) {
    return GetMonitorRefreshRate(monitor);
}

static void raycart_GetWindowPosition(wasm_exec_env_t exec_env, Vector2* __result) {
    *__result = GetWindowPosition();
}

static void raycart_GetWindowScaleDPI(wasm_exec_env_t exec_env, Vector2* __result) {
    *__result = GetWindowScaleDPI();
}

static const char * raycart_GetMonitorName(wasm_exec_env_t exec_env, int monitor) {
    return GetMonitorName(monitor);
}

static void raycart_SetClipboardText(wasm_exec_env_t exec_env, const char * text) {
    SetClipboardText(text);
}

static const char * raycart_GetClipboardText(wasm_exec_env_t exec_env) {
    return GetClipboardText();
}

static void raycart_EnableEventWaiting(wasm_exec_env_t exec_env) {
    EnableEventWaiting();
}

static void raycart_DisableEventWaiting(wasm_exec_env_t exec_env) {
    DisableEventWaiting();
}

static void raycart_ShowCursor(wasm_exec_env_t exec_env) {
    ShowCursor();
}

static void raycart_HideCursor(wasm_exec_env_t exec_env) {
    HideCursor();
}

static bool raycart_IsCursorHidden(wasm_exec_env_t exec_env) {
    return IsCursorHidden();
}

static void raycart_EnableCursor(wasm_exec_env_t exec_env) {
    EnableCursor();
}

static void raycart_DisableCursor(wasm_exec_env_t exec_env) {
    DisableCursor();
}

static bool raycart_IsCursorOnScreen(wasm_exec_env_t exec_env) {
    return IsCursorOnScreen();
}

static void raycart_ClearBackground(wasm_exec_env_t exec_env, Color* color) {
    ClearBackground(*color);
}

static void raycart_BeginDrawing(wasm_exec_env_t exec_env) {
    BeginDrawing();
}

static void raycart_EndDrawing(wasm_exec_env_t exec_env) {
    EndDrawing();
}

static void raycart_BeginMode2D(wasm_exec_env_t exec_env, Camera2D* camera) {
    BeginMode2D(*camera);
}

static void raycart_EndMode2D(wasm_exec_env_t exec_env) {
    EndMode2D();
}

static void raycart_BeginMode3D(wasm_exec_env_t exec_env, Camera3D* camera) {
    BeginMode3D(*camera);
}

static void raycart_EndMode3D(wasm_exec_env_t exec_env) {
    EndMode3D();
}

static void raycart_BeginTextureMode(wasm_exec_env_t exec_env, RenderTexture* target) {
    BeginTextureMode(*target);
}

static void raycart_EndTextureMode(wasm_exec_env_t exec_env) {
    EndTextureMode();
}

static void raycart_BeginShaderMode(wasm_exec_env_t exec_env, Shader* shader) {
    BeginShaderMode(*shader);
}

static void raycart_EndShaderMode(wasm_exec_env_t exec_env) {
    EndShaderMode();
}

static void raycart_BeginBlendMode(wasm_exec_env_t exec_env, int mode) {
    BeginBlendMode(mode);
}

static void raycart_EndBlendMode(wasm_exec_env_t exec_env) {
    EndBlendMode();
}

static void raycart_BeginScissorMode(wasm_exec_env_t exec_env, int x, int y, int width, int height) {
    BeginScissorMode(x, y, width, height);
}

static void raycart_EndScissorMode(wasm_exec_env_t exec_env) {
    EndScissorMode();
}

static void raycart_BeginVrStereoMode(wasm_exec_env_t exec_env, VrStereoConfig* config) {
    BeginVrStereoMode(*config);
}

static void raycart_EndVrStereoMode(wasm_exec_env_t exec_env) {
    EndVrStereoMode();
}

static void raycart_LoadVrStereoConfig(wasm_exec_env_t exec_env, VrStereoConfig* __result, VrDeviceInfo* device) {
    *__result = LoadVrStereoConfig(*device);
}

static void raycart_UnloadVrStereoConfig(wasm_exec_env_t exec_env, VrStereoConfig* config) {
    UnloadVrStereoConfig(*config);
}

static void raycart_LoadShader(wasm_exec_env_t exec_env, Shader* __result, const char * vsFileName, const char * fsFileName) {
    *__result = LoadShaderFromPhysFS(vsFileName, fsFileName);
}

static void raycart_LoadShaderFromMemory(wasm_exec_env_t exec_env, Shader* __result, const char * vsCode, const char * fsCode) {
    *__result = LoadShaderFromMemory(vsCode, fsCode);
}

static bool raycart_IsShaderValid(wasm_exec_env_t exec_env, Shader* shader) {
    return IsShaderValid(*shader);
}

static int raycart_GetShaderLocation(wasm_exec_env_t exec_env, Shader* shader, const char * uniformName) {
    return GetShaderLocation(*shader, uniformName);
}

static int raycart_GetShaderLocationAttrib(wasm_exec_env_t exec_env, Shader* shader, const char * attribName) {
    return GetShaderLocationAttrib(*shader, attribName);
}

static void raycart_SetShaderValue(wasm_exec_env_t exec_env, Shader* shader, int locIndex, const void * value, int uniformType) {
    SetShaderValue(*shader, locIndex, value, uniformType);
}

static void raycart_SetShaderValueV(wasm_exec_env_t exec_env, Shader* shader, int locIndex, const void * value, int uniformType, int count) {
    SetShaderValueV(*shader, locIndex, value, uniformType, count);
}

static void raycart_SetShaderValueMatrix(wasm_exec_env_t exec_env, Shader* shader, int locIndex, Matrix* mat) {
    SetShaderValueMatrix(*shader, locIndex, *mat);
}

static void raycart_SetShaderValueTexture(wasm_exec_env_t exec_env, Shader* shader, int locIndex, Texture* texture) {
    SetShaderValueTexture(*shader, locIndex, *texture);
}

static void raycart_UnloadShader(wasm_exec_env_t exec_env, Shader* shader) {
    UnloadShader(*shader);
}

static void raycart_GetScreenToWorldRay(wasm_exec_env_t exec_env, Ray* __result, Vector2* position, Camera3D* camera) {
    *__result = GetScreenToWorldRay(*position, *camera);
}

static void raycart_GetScreenToWorldRayEx(wasm_exec_env_t exec_env, Ray* __result, Vector2* position, Camera3D* camera, int width, int height) {
    *__result = GetScreenToWorldRayEx(*position, *camera, width, height);
}

static void raycart_GetWorldToScreen(wasm_exec_env_t exec_env, Vector2* __result, Vector3* position, Camera3D* camera) {
    *__result = GetWorldToScreen(*position, *camera);
}

static void raycart_GetWorldToScreenEx(wasm_exec_env_t exec_env, Vector2* __result, Vector3* position, Camera3D* camera, int width, int height) {
    *__result = GetWorldToScreenEx(*position, *camera, width, height);
}

static void raycart_GetWorldToScreen2D(wasm_exec_env_t exec_env, Vector2* __result, Vector2* position, Camera2D* camera) {
    *__result = GetWorldToScreen2D(*position, *camera);
}

static void raycart_GetScreenToWorld2D(wasm_exec_env_t exec_env, Vector2* __result, Vector2* position, Camera2D* camera) {
    *__result = GetScreenToWorld2D(*position, *camera);
}

static void raycart_GetCameraMatrix(wasm_exec_env_t exec_env, Matrix* __result, Camera3D* camera) {
    *__result = GetCameraMatrix(*camera);
}

static void raycart_GetCameraMatrix2D(wasm_exec_env_t exec_env, Matrix* __result, Camera2D* camera) {
    *__result = GetCameraMatrix2D(*camera);
}

static void raycart_SetTargetFPS(wasm_exec_env_t exec_env, int fps) {
    SetTargetFPS(fps);
}

static float raycart_GetFrameTime(wasm_exec_env_t exec_env) {
    return GetFrameTime();
}

static double raycart_GetTime(wasm_exec_env_t exec_env) {
    return GetTime();
}

static int raycart_GetFPS(wasm_exec_env_t exec_env) {
    return GetFPS();
}

static void raycart_SwapScreenBuffer(wasm_exec_env_t exec_env) {
    SwapScreenBuffer();
}

static void raycart_PollInputEvents(wasm_exec_env_t exec_env) {
    PollInputEvents();
}

static void raycart_WaitTime(wasm_exec_env_t exec_env, double seconds) {
    WaitTime(seconds);
}

static void raycart_SetRandomSeed(wasm_exec_env_t exec_env, unsigned int seed) {
    SetRandomSeed(seed);
}

static int raycart_GetRandomValue(wasm_exec_env_t exec_env, int min, int max) {
    return GetRandomValue(min, max);
}

static int * raycart_LoadRandomSequence(wasm_exec_env_t exec_env, unsigned int count, int min, int max) {
    return LoadRandomSequence(count, min, max);
}

static void raycart_UnloadRandomSequence(wasm_exec_env_t exec_env, int * sequence) {
    UnloadRandomSequence(sequence);
}

static void raycart_TakeScreenshot(wasm_exec_env_t exec_env, const char * fileName) {
    TakeScreenshot(fileName);
}

static void raycart_SetConfigFlags(wasm_exec_env_t exec_env, unsigned int flags) {
    SetConfigFlags(flags);
}

static void raycart_OpenURL(wasm_exec_env_t exec_env, const char * url) {
    OpenURL(url);
}

static void raycart_SetTraceLogLevel(wasm_exec_env_t exec_env, int logLevel) {
    SetTraceLogLevel(logLevel);
}

static unsigned char * raycart_LoadFileData(wasm_exec_env_t exec_env, const char * fileName, int * dataSize) {
    return LoadFileDataFromPhysFS(fileName, dataSize);
}

static void raycart_UnloadFileData(wasm_exec_env_t exec_env, unsigned char * data) {
    UnloadFileData(data);
}

static bool raycart_SaveFileData(wasm_exec_env_t exec_env, const char * fileName, void * data, int dataSize) {
    return SaveFileDataToPhysFS(fileName, data, dataSize);
}

static bool raycart_ExportDataAsCode(wasm_exec_env_t exec_env, const unsigned char * data, int dataSize, const char * fileName) {
    return ExportDataAsCode(data, dataSize, fileName);
}

static char * raycart_LoadFileText(wasm_exec_env_t exec_env, const char * fileName) {
    return LoadFileTextFromPhysFS(fileName);
}

static void raycart_UnloadFileText(wasm_exec_env_t exec_env, char * text) {
    UnloadFileText(text);
}

static bool raycart_SaveFileText(wasm_exec_env_t exec_env, const char * fileName, const char * text) {
    return SaveFileTextToPhysFS(fileName, text);
}

static bool raycart_FileExists(wasm_exec_env_t exec_env, const char * fileName) {
    return FileExistsInPhysFS(fileName);
}

static bool raycart_DirectoryExists(wasm_exec_env_t exec_env, const char * dirPath) {
    return DirectoryExistsInPhysFS(dirPath);
}

static bool raycart_IsFileExtension(wasm_exec_env_t exec_env, const char * fileName, const char * ext) {
    return IsFileExtension(fileName, ext);
}

static int raycart_GetFileLength(wasm_exec_env_t exec_env, const char * fileName) {
    return GetFileLength(fileName);
}

static long raycart_GetFileModTime(wasm_exec_env_t exec_env, const char * fileName) {
    return GetFileModTimeFromPhysFS(fileName);
}

static const char * raycart_GetFileExtension(wasm_exec_env_t exec_env, const char * fileName) {
    return GetFileExtension(fileName);
}

static const char * raycart_GetFileName(wasm_exec_env_t exec_env, const char * filePath) {
    return GetFileName(filePath);
}

static const char * raycart_GetFileNameWithoutExt(wasm_exec_env_t exec_env, const char * filePath) {
    return GetFileNameWithoutExt(filePath);
}

static const char * raycart_GetDirectoryPath(wasm_exec_env_t exec_env, const char * filePath) {
    return GetDirectoryPath(filePath);
}

static const char * raycart_GetPrevDirectoryPath(wasm_exec_env_t exec_env, const char * dirPath) {
    return GetPrevDirectoryPath(dirPath);
}

static const char * raycart_GetWorkingDirectory(wasm_exec_env_t exec_env) {
    return GetWorkingDirectory();
}

static const char * raycart_GetApplicationDirectory(wasm_exec_env_t exec_env) {
    return GetApplicationDirectory();
}

static int raycart_MakeDirectory(wasm_exec_env_t exec_env, const char * dirPath) {
    return MakeDirectory(dirPath);
}

static bool raycart_ChangeDirectory(wasm_exec_env_t exec_env, const char * dirPath) {
    return ChangeDirectory(dirPath);
}

static bool raycart_IsPathFile(wasm_exec_env_t exec_env, const char * path) {
    return IsPathFile(path);
}

static bool raycart_IsFileNameValid(wasm_exec_env_t exec_env, const char * fileName) {
    return IsFileNameValid(fileName);
}

static void raycart_LoadDirectoryFiles(wasm_exec_env_t exec_env, FilePathList* __result, const char * dirPath) {
    *__result = LoadDirectoryFilesFromPhysFS(dirPath);
}

static void raycart_LoadDirectoryFilesEx(wasm_exec_env_t exec_env, FilePathList* __result, const char * basePath, const char * filter, bool scanSubdirs) {
    *__result = LoadDirectoryFilesEx(basePath, filter, scanSubdirs);
}

static void raycart_UnloadDirectoryFiles(wasm_exec_env_t exec_env, FilePathList* files) {
    UnloadDirectoryFiles(*files);
}

static bool raycart_IsFileDropped(wasm_exec_env_t exec_env) {
    return IsFileDropped();
}

static void raycart_LoadDroppedFiles(wasm_exec_env_t exec_env, FilePathList* __result) {
    *__result = LoadDroppedFiles();
}

static void raycart_UnloadDroppedFiles(wasm_exec_env_t exec_env, FilePathList* files) {
    UnloadDroppedFiles(*files);
}

static unsigned char * raycart_CompressData(wasm_exec_env_t exec_env, const unsigned char * data, int dataSize, int * compDataSize) {
    return CompressData(data, dataSize, compDataSize);
}

static unsigned char * raycart_DecompressData(wasm_exec_env_t exec_env, const unsigned char * compData, int compDataSize, int * dataSize) {
    return DecompressData(compData, compDataSize, dataSize);
}

static char * raycart_EncodeDataBase64(wasm_exec_env_t exec_env, const unsigned char * data, int dataSize, int * outputSize) {
    return EncodeDataBase64(data, dataSize, outputSize);
}

static unsigned char * raycart_DecodeDataBase64(wasm_exec_env_t exec_env, const char * text, int * outputSize) {
    return DecodeDataBase64(text, outputSize);
}

static unsigned int raycart_ComputeCRC32(wasm_exec_env_t exec_env, unsigned char * data, int dataSize) {
    return ComputeCRC32(data, dataSize);
}

static unsigned int * raycart_ComputeMD5(wasm_exec_env_t exec_env, unsigned char * data, int dataSize) {
    return ComputeMD5(data, dataSize);
}

static unsigned int * raycart_ComputeSHA1(wasm_exec_env_t exec_env, unsigned char * data, int dataSize) {
    return ComputeSHA1(data, dataSize);
}

static void raycart_LoadAutomationEventList(wasm_exec_env_t exec_env, AutomationEventList* __result, const char * fileName) {
    *__result = LoadAutomationEventList(fileName);
}

static void raycart_UnloadAutomationEventList(wasm_exec_env_t exec_env, AutomationEventList* list) {
    UnloadAutomationEventList(*list);
}

static bool raycart_ExportAutomationEventList(wasm_exec_env_t exec_env, AutomationEventList* list, const char * fileName) {
    return ExportAutomationEventList(*list, fileName);
}

static void raycart_SetAutomationEventList(wasm_exec_env_t exec_env, AutomationEventList * list) {
    SetAutomationEventList(list);
}

static void raycart_SetAutomationEventBaseFrame(wasm_exec_env_t exec_env, int frame) {
    SetAutomationEventBaseFrame(frame);
}

static void raycart_StartAutomationEventRecording(wasm_exec_env_t exec_env) {
    StartAutomationEventRecording();
}

static void raycart_StopAutomationEventRecording(wasm_exec_env_t exec_env) {
    StopAutomationEventRecording();
}

static void raycart_PlayAutomationEvent(wasm_exec_env_t exec_env, AutomationEvent* event) {
    PlayAutomationEvent(*event);
}

static bool raycart_IsKeyPressed(wasm_exec_env_t exec_env, int key) {
    return IsKeyPressed(key);
}

static bool raycart_IsKeyPressedRepeat(wasm_exec_env_t exec_env, int key) {
    return IsKeyPressedRepeat(key);
}

static bool raycart_IsKeyDown(wasm_exec_env_t exec_env, int key) {
    return IsKeyDown(key);
}

static bool raycart_IsKeyReleased(wasm_exec_env_t exec_env, int key) {
    return IsKeyReleased(key);
}

static bool raycart_IsKeyUp(wasm_exec_env_t exec_env, int key) {
    return IsKeyUp(key);
}

static int raycart_GetKeyPressed(wasm_exec_env_t exec_env) {
    return GetKeyPressed();
}

static int raycart_GetCharPressed(wasm_exec_env_t exec_env) {
    return GetCharPressed();
}

static void raycart_SetExitKey(wasm_exec_env_t exec_env, int key) {
    SetExitKey(key);
}

static bool raycart_IsGamepadAvailable(wasm_exec_env_t exec_env, int gamepad) {
    return IsGamepadAvailable(gamepad);
}

static const char * raycart_GetGamepadName(wasm_exec_env_t exec_env, int gamepad) {
    return GetGamepadName(gamepad);
}

static bool raycart_IsGamepadButtonPressed(wasm_exec_env_t exec_env, int gamepad, int button) {
    return IsGamepadButtonPressed(gamepad, button);
}

static bool raycart_IsGamepadButtonDown(wasm_exec_env_t exec_env, int gamepad, int button) {
    return IsGamepadButtonDown(gamepad, button);
}

static bool raycart_IsGamepadButtonReleased(wasm_exec_env_t exec_env, int gamepad, int button) {
    return IsGamepadButtonReleased(gamepad, button);
}

static bool raycart_IsGamepadButtonUp(wasm_exec_env_t exec_env, int gamepad, int button) {
    return IsGamepadButtonUp(gamepad, button);
}

static int raycart_GetGamepadButtonPressed(wasm_exec_env_t exec_env) {
    return GetGamepadButtonPressed();
}

static int raycart_GetGamepadAxisCount(wasm_exec_env_t exec_env, int gamepad) {
    return GetGamepadAxisCount(gamepad);
}

static float raycart_GetGamepadAxisMovement(wasm_exec_env_t exec_env, int gamepad, int axis) {
    return GetGamepadAxisMovement(gamepad, axis);
}

static int raycart_SetGamepadMappings(wasm_exec_env_t exec_env, const char * mappings) {
    return SetGamepadMappings(mappings);
}

static void raycart_SetGamepadVibration(wasm_exec_env_t exec_env, int gamepad, float leftMotor, float rightMotor, float duration) {
    SetGamepadVibration(gamepad, leftMotor, rightMotor, duration);
}

static bool raycart_IsMouseButtonPressed(wasm_exec_env_t exec_env, int button) {
    return IsMouseButtonPressed(button);
}

static bool raycart_IsMouseButtonDown(wasm_exec_env_t exec_env, int button) {
    return IsMouseButtonDown(button);
}

static bool raycart_IsMouseButtonReleased(wasm_exec_env_t exec_env, int button) {
    return IsMouseButtonReleased(button);
}

static bool raycart_IsMouseButtonUp(wasm_exec_env_t exec_env, int button) {
    return IsMouseButtonUp(button);
}

static int raycart_GetMouseX(wasm_exec_env_t exec_env) {
    return GetMouseX();
}

static int raycart_GetMouseY(wasm_exec_env_t exec_env) {
    return GetMouseY();
}

static void raycart_GetMousePosition(wasm_exec_env_t exec_env, Vector2* __result) {
    *__result = GetMousePosition();
}

static void raycart_GetMouseDelta(wasm_exec_env_t exec_env, Vector2* __result) {
    *__result = GetMouseDelta();
}

static void raycart_SetMousePosition(wasm_exec_env_t exec_env, int x, int y) {
    SetMousePosition(x, y);
}

static void raycart_SetMouseOffset(wasm_exec_env_t exec_env, int offsetX, int offsetY) {
    SetMouseOffset(offsetX, offsetY);
}

static void raycart_SetMouseScale(wasm_exec_env_t exec_env, float scaleX, float scaleY) {
    SetMouseScale(scaleX, scaleY);
}

static float raycart_GetMouseWheelMove(wasm_exec_env_t exec_env) {
    return GetMouseWheelMove();
}

static void raycart_GetMouseWheelMoveV(wasm_exec_env_t exec_env, Vector2* __result) {
    *__result = GetMouseWheelMoveV();
}

static void raycart_SetMouseCursor(wasm_exec_env_t exec_env, int cursor) {
    SetMouseCursor(cursor);
}

static int raycart_GetTouchX(wasm_exec_env_t exec_env) {
    return GetTouchX();
}

static int raycart_GetTouchY(wasm_exec_env_t exec_env) {
    return GetTouchY();
}

static void raycart_GetTouchPosition(wasm_exec_env_t exec_env, Vector2* __result, int index) {
    *__result = GetTouchPosition(index);
}

static int raycart_GetTouchPointId(wasm_exec_env_t exec_env, int index) {
    return GetTouchPointId(index);
}

static int raycart_GetTouchPointCount(wasm_exec_env_t exec_env) {
    return GetTouchPointCount();
}

static void raycart_SetGesturesEnabled(wasm_exec_env_t exec_env, unsigned int flags) {
    SetGesturesEnabled(flags);
}

static bool raycart_IsGestureDetected(wasm_exec_env_t exec_env, unsigned int gesture) {
    return IsGestureDetected(gesture);
}

static int raycart_GetGestureDetected(wasm_exec_env_t exec_env) {
    return GetGestureDetected();
}

static float raycart_GetGestureHoldDuration(wasm_exec_env_t exec_env) {
    return GetGestureHoldDuration();
}

static void raycart_GetGestureDragVector(wasm_exec_env_t exec_env, Vector2* __result) {
    *__result = GetGestureDragVector();
}

static float raycart_GetGestureDragAngle(wasm_exec_env_t exec_env) {
    return GetGestureDragAngle();
}

static void raycart_GetGesturePinchVector(wasm_exec_env_t exec_env, Vector2* __result) {
    *__result = GetGesturePinchVector();
}

static float raycart_GetGesturePinchAngle(wasm_exec_env_t exec_env) {
    return GetGesturePinchAngle();
}

static void raycart_UpdateCamera(wasm_exec_env_t exec_env, Camera * camera, int mode) {
    UpdateCamera(camera, mode);
}

static void raycart_UpdateCameraPro(wasm_exec_env_t exec_env, Camera * camera, Vector3* movement, Vector3* rotation, float zoom) {
    UpdateCameraPro(camera, *movement, *rotation, zoom);
}

static void raycart_SetShapesTexture(wasm_exec_env_t exec_env, Texture* texture, Rectangle* source) {
    SetShapesTexture(*texture, *source);
}

static void raycart_GetShapesTexture(wasm_exec_env_t exec_env, Texture2D* __result) {
    *__result = GetShapesTexture();
}

static void raycart_GetShapesTextureRectangle(wasm_exec_env_t exec_env, Rectangle* __result) {
    *__result = GetShapesTextureRectangle();
}

static void raycart_DrawPixel(wasm_exec_env_t exec_env, int posX, int posY, Color* color) {
    DrawPixel(posX, posY, *color);
}

static void raycart_DrawPixelV(wasm_exec_env_t exec_env, Vector2* position, Color* color) {
    DrawPixelV(*position, *color);
}

static void raycart_DrawLine(wasm_exec_env_t exec_env, int startPosX, int startPosY, int endPosX, int endPosY, Color* color) {
    DrawLine(startPosX, startPosY, endPosX, endPosY, *color);
}

static void raycart_DrawLineV(wasm_exec_env_t exec_env, Vector2* startPos, Vector2* endPos, Color* color) {
    DrawLineV(*startPos, *endPos, *color);
}

static void raycart_DrawLineEx(wasm_exec_env_t exec_env, Vector2* startPos, Vector2* endPos, float thick, Color* color) {
    DrawLineEx(*startPos, *endPos, thick, *color);
}

static void raycart_DrawLineStrip(wasm_exec_env_t exec_env, const Vector2 * points, int pointCount, Color* color) {
    DrawLineStrip(points, pointCount, *color);
}

static void raycart_DrawLineBezier(wasm_exec_env_t exec_env, Vector2* startPos, Vector2* endPos, float thick, Color* color) {
    DrawLineBezier(*startPos, *endPos, thick, *color);
}

static void raycart_DrawCircle(wasm_exec_env_t exec_env, int centerX, int centerY, float radius, Color* color) {
    DrawCircle(centerX, centerY, radius, *color);
}

static void raycart_DrawCircleSector(wasm_exec_env_t exec_env, Vector2* center, float radius, float startAngle, float endAngle, int segments, Color* color) {
    DrawCircleSector(*center, radius, startAngle, endAngle, segments, *color);
}

static void raycart_DrawCircleSectorLines(wasm_exec_env_t exec_env, Vector2* center, float radius, float startAngle, float endAngle, int segments, Color* color) {
    DrawCircleSectorLines(*center, radius, startAngle, endAngle, segments, *color);
}

static void raycart_DrawCircleGradient(wasm_exec_env_t exec_env, int centerX, int centerY, float radius, Color* inner, Color* outer) {
    DrawCircleGradient(centerX, centerY, radius, *inner, *outer);
}

static void raycart_DrawCircleV(wasm_exec_env_t exec_env, Vector2* center, float radius, Color* color) {
    DrawCircleV(*center, radius, *color);
}

static void raycart_DrawCircleLines(wasm_exec_env_t exec_env, int centerX, int centerY, float radius, Color* color) {
    DrawCircleLines(centerX, centerY, radius, *color);
}

static void raycart_DrawCircleLinesV(wasm_exec_env_t exec_env, Vector2* center, float radius, Color* color) {
    DrawCircleLinesV(*center, radius, *color);
}

static void raycart_DrawEllipse(wasm_exec_env_t exec_env, int centerX, int centerY, float radiusH, float radiusV, Color* color) {
    DrawEllipse(centerX, centerY, radiusH, radiusV, *color);
}

static void raycart_DrawEllipseLines(wasm_exec_env_t exec_env, int centerX, int centerY, float radiusH, float radiusV, Color* color) {
    DrawEllipseLines(centerX, centerY, radiusH, radiusV, *color);
}

static void raycart_DrawRing(wasm_exec_env_t exec_env, Vector2* center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color* color) {
    DrawRing(*center, innerRadius, outerRadius, startAngle, endAngle, segments, *color);
}

static void raycart_DrawRingLines(wasm_exec_env_t exec_env, Vector2* center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color* color) {
    DrawRingLines(*center, innerRadius, outerRadius, startAngle, endAngle, segments, *color);
}

static void raycart_DrawRectangle(wasm_exec_env_t exec_env, int posX, int posY, int width, int height, Color* color) {
    DrawRectangle(posX, posY, width, height, *color);
}

static void raycart_DrawRectangleV(wasm_exec_env_t exec_env, Vector2* position, Vector2* size, Color* color) {
    DrawRectangleV(*position, *size, *color);
}

static void raycart_DrawRectangleRec(wasm_exec_env_t exec_env, Rectangle* rec, Color* color) {
    DrawRectangleRec(*rec, *color);
}

static void raycart_DrawRectanglePro(wasm_exec_env_t exec_env, Rectangle* rec, Vector2* origin, float rotation, Color* color) {
    DrawRectanglePro(*rec, *origin, rotation, *color);
}

static void raycart_DrawRectangleGradientV(wasm_exec_env_t exec_env, int posX, int posY, int width, int height, Color* top, Color* bottom) {
    DrawRectangleGradientV(posX, posY, width, height, *top, *bottom);
}

static void raycart_DrawRectangleGradientH(wasm_exec_env_t exec_env, int posX, int posY, int width, int height, Color* left, Color* right) {
    DrawRectangleGradientH(posX, posY, width, height, *left, *right);
}

static void raycart_DrawRectangleGradientEx(wasm_exec_env_t exec_env, Rectangle* rec, Color* topLeft, Color* bottomLeft, Color* bottomRight, Color* topRight) {
    DrawRectangleGradientEx(*rec, *topLeft, *bottomLeft, *bottomRight, *topRight);
}

static void raycart_DrawRectangleLines(wasm_exec_env_t exec_env, int posX, int posY, int width, int height, Color* color) {
    DrawRectangleLines(posX, posY, width, height, *color);
}

static void raycart_DrawRectangleLinesEx(wasm_exec_env_t exec_env, Rectangle* rec, float lineThick, Color* color) {
    DrawRectangleLinesEx(*rec, lineThick, *color);
}

static void raycart_DrawRectangleRounded(wasm_exec_env_t exec_env, Rectangle* rec, float roundness, int segments, Color* color) {
    DrawRectangleRounded(*rec, roundness, segments, *color);
}

static void raycart_DrawRectangleRoundedLines(wasm_exec_env_t exec_env, Rectangle* rec, float roundness, int segments, Color* color) {
    DrawRectangleRoundedLines(*rec, roundness, segments, *color);
}

static void raycart_DrawRectangleRoundedLinesEx(wasm_exec_env_t exec_env, Rectangle* rec, float roundness, int segments, float lineThick, Color* color) {
    DrawRectangleRoundedLinesEx(*rec, roundness, segments, lineThick, *color);
}

static void raycart_DrawTriangle(wasm_exec_env_t exec_env, Vector2* v1, Vector2* v2, Vector2* v3, Color* color) {
    DrawTriangle(*v1, *v2, *v3, *color);
}

static void raycart_DrawTriangleLines(wasm_exec_env_t exec_env, Vector2* v1, Vector2* v2, Vector2* v3, Color* color) {
    DrawTriangleLines(*v1, *v2, *v3, *color);
}

static void raycart_DrawTriangleFan(wasm_exec_env_t exec_env, const Vector2 * points, int pointCount, Color* color) {
    DrawTriangleFan(points, pointCount, *color);
}

static void raycart_DrawTriangleStrip(wasm_exec_env_t exec_env, const Vector2 * points, int pointCount, Color* color) {
    DrawTriangleStrip(points, pointCount, *color);
}

static void raycart_DrawPoly(wasm_exec_env_t exec_env, Vector2* center, int sides, float radius, float rotation, Color* color) {
    DrawPoly(*center, sides, radius, rotation, *color);
}

static void raycart_DrawPolyLines(wasm_exec_env_t exec_env, Vector2* center, int sides, float radius, float rotation, Color* color) {
    DrawPolyLines(*center, sides, radius, rotation, *color);
}

static void raycart_DrawPolyLinesEx(wasm_exec_env_t exec_env, Vector2* center, int sides, float radius, float rotation, float lineThick, Color* color) {
    DrawPolyLinesEx(*center, sides, radius, rotation, lineThick, *color);
}

static void raycart_DrawSplineLinear(wasm_exec_env_t exec_env, const Vector2 * points, int pointCount, float thick, Color* color) {
    DrawSplineLinear(points, pointCount, thick, *color);
}

static void raycart_DrawSplineBasis(wasm_exec_env_t exec_env, const Vector2 * points, int pointCount, float thick, Color* color) {
    DrawSplineBasis(points, pointCount, thick, *color);
}

static void raycart_DrawSplineCatmullRom(wasm_exec_env_t exec_env, const Vector2 * points, int pointCount, float thick, Color* color) {
    DrawSplineCatmullRom(points, pointCount, thick, *color);
}

static void raycart_DrawSplineBezierQuadratic(wasm_exec_env_t exec_env, const Vector2 * points, int pointCount, float thick, Color* color) {
    DrawSplineBezierQuadratic(points, pointCount, thick, *color);
}

static void raycart_DrawSplineBezierCubic(wasm_exec_env_t exec_env, const Vector2 * points, int pointCount, float thick, Color* color) {
    DrawSplineBezierCubic(points, pointCount, thick, *color);
}

static void raycart_DrawSplineSegmentLinear(wasm_exec_env_t exec_env, Vector2* p1, Vector2* p2, float thick, Color* color) {
    DrawSplineSegmentLinear(*p1, *p2, thick, *color);
}

static void raycart_DrawSplineSegmentBasis(wasm_exec_env_t exec_env, Vector2* p1, Vector2* p2, Vector2* p3, Vector2* p4, float thick, Color* color) {
    DrawSplineSegmentBasis(*p1, *p2, *p3, *p4, thick, *color);
}

static void raycart_DrawSplineSegmentCatmullRom(wasm_exec_env_t exec_env, Vector2* p1, Vector2* p2, Vector2* p3, Vector2* p4, float thick, Color* color) {
    DrawSplineSegmentCatmullRom(*p1, *p2, *p3, *p4, thick, *color);
}

static void raycart_DrawSplineSegmentBezierQuadratic(wasm_exec_env_t exec_env, Vector2* p1, Vector2* c2, Vector2* p3, float thick, Color* color) {
    DrawSplineSegmentBezierQuadratic(*p1, *c2, *p3, thick, *color);
}

static void raycart_DrawSplineSegmentBezierCubic(wasm_exec_env_t exec_env, Vector2* p1, Vector2* c2, Vector2* c3, Vector2* p4, float thick, Color* color) {
    DrawSplineSegmentBezierCubic(*p1, *c2, *c3, *p4, thick, *color);
}

static void raycart_GetSplinePointLinear(wasm_exec_env_t exec_env, Vector2* __result, Vector2* startPos, Vector2* endPos, float t) {
    *__result = GetSplinePointLinear(*startPos, *endPos, t);
}

static void raycart_GetSplinePointBasis(wasm_exec_env_t exec_env, Vector2* __result, Vector2* p1, Vector2* p2, Vector2* p3, Vector2* p4, float t) {
    *__result = GetSplinePointBasis(*p1, *p2, *p3, *p4, t);
}

static void raycart_GetSplinePointCatmullRom(wasm_exec_env_t exec_env, Vector2* __result, Vector2* p1, Vector2* p2, Vector2* p3, Vector2* p4, float t) {
    *__result = GetSplinePointCatmullRom(*p1, *p2, *p3, *p4, t);
}

static void raycart_GetSplinePointBezierQuad(wasm_exec_env_t exec_env, Vector2* __result, Vector2* p1, Vector2* c2, Vector2* p3, float t) {
    *__result = GetSplinePointBezierQuad(*p1, *c2, *p3, t);
}

static void raycart_GetSplinePointBezierCubic(wasm_exec_env_t exec_env, Vector2* __result, Vector2* p1, Vector2* c2, Vector2* c3, Vector2* p4, float t) {
    *__result = GetSplinePointBezierCubic(*p1, *c2, *c3, *p4, t);
}

static bool raycart_CheckCollisionRecs(wasm_exec_env_t exec_env, Rectangle* rec1, Rectangle* rec2) {
    return CheckCollisionRecs(*rec1, *rec2);
}

static bool raycart_CheckCollisionCircles(wasm_exec_env_t exec_env, Vector2* center1, float radius1, Vector2* center2, float radius2) {
    return CheckCollisionCircles(*center1, radius1, *center2, radius2);
}

static bool raycart_CheckCollisionCircleRec(wasm_exec_env_t exec_env, Vector2* center, float radius, Rectangle* rec) {
    return CheckCollisionCircleRec(*center, radius, *rec);
}

static bool raycart_CheckCollisionCircleLine(wasm_exec_env_t exec_env, Vector2* center, float radius, Vector2* p1, Vector2* p2) {
    return CheckCollisionCircleLine(*center, radius, *p1, *p2);
}

static bool raycart_CheckCollisionPointRec(wasm_exec_env_t exec_env, Vector2* point, Rectangle* rec) {
    return CheckCollisionPointRec(*point, *rec);
}

static bool raycart_CheckCollisionPointCircle(wasm_exec_env_t exec_env, Vector2* point, Vector2* center, float radius) {
    return CheckCollisionPointCircle(*point, *center, radius);
}

static bool raycart_CheckCollisionPointTriangle(wasm_exec_env_t exec_env, Vector2* point, Vector2* p1, Vector2* p2, Vector2* p3) {
    return CheckCollisionPointTriangle(*point, *p1, *p2, *p3);
}

static bool raycart_CheckCollisionPointLine(wasm_exec_env_t exec_env, Vector2* point, Vector2* p1, Vector2* p2, int threshold) {
    return CheckCollisionPointLine(*point, *p1, *p2, threshold);
}

static bool raycart_CheckCollisionPointPoly(wasm_exec_env_t exec_env, Vector2* point, const Vector2 * points, int pointCount) {
    return CheckCollisionPointPoly(*point, points, pointCount);
}

static bool raycart_CheckCollisionLines(wasm_exec_env_t exec_env, Vector2* startPos1, Vector2* endPos1, Vector2* startPos2, Vector2* endPos2, Vector2 * collisionPoint) {
    return CheckCollisionLines(*startPos1, *endPos1, *startPos2, *endPos2, collisionPoint);
}

static void raycart_GetCollisionRec(wasm_exec_env_t exec_env, Rectangle* __result, Rectangle* rec1, Rectangle* rec2) {
    *__result = GetCollisionRec(*rec1, *rec2);
}

static void raycart_LoadImage(wasm_exec_env_t exec_env, Image* __result, const char * fileName) {
    *__result = LoadImageFromPhysFS(fileName);
}

static void raycart_LoadImageRaw(wasm_exec_env_t exec_env, Image* __result, const char * fileName, int width, int height, int format, int headerSize) {
    *__result = LoadImageRaw(fileName, width, height, format, headerSize);
}

static void raycart_LoadImageAnim(wasm_exec_env_t exec_env, Image* __result, const char * fileName, int * frames) {
    *__result = LoadImageAnim(fileName, frames);
}

static void raycart_LoadImageAnimFromMemory(wasm_exec_env_t exec_env, Image* __result, const char * fileType, const unsigned char * fileData, int dataSize, int * frames) {
    *__result = LoadImageAnimFromMemory(fileType, fileData, dataSize, frames);
}

static void raycart_LoadImageFromMemory(wasm_exec_env_t exec_env, Image* __result, const char * fileType, const unsigned char * fileData, int dataSize) {
    *__result = LoadImageFromMemory(fileType, fileData, dataSize);
}

static void raycart_LoadImageFromTexture(wasm_exec_env_t exec_env, Image* __result, Texture* texture) {
    *__result = LoadImageFromTexture(*texture);
}

static void raycart_LoadImageFromScreen(wasm_exec_env_t exec_env, Image* __result) {
    *__result = LoadImageFromScreen();
}

static bool raycart_IsImageValid(wasm_exec_env_t exec_env, Image* image) {
    return IsImageValid(*image);
}

static void raycart_UnloadImage(wasm_exec_env_t exec_env, Image* image) {
    UnloadImage(*image);
}

static bool raycart_ExportImage(wasm_exec_env_t exec_env, Image* image, const char * fileName) {
    return ExportImage(*image, fileName);
}

static unsigned char * raycart_ExportImageToMemory(wasm_exec_env_t exec_env, Image* image, const char * fileType, int * fileSize) {
    return ExportImageToMemory(*image, fileType, fileSize);
}

static bool raycart_ExportImageAsCode(wasm_exec_env_t exec_env, Image* image, const char * fileName) {
    return ExportImageAsCode(*image, fileName);
}

static void raycart_GenImageColor(wasm_exec_env_t exec_env, Image* __result, int width, int height, Color* color) {
    *__result = GenImageColor(width, height, *color);
}

static void raycart_GenImageGradientLinear(wasm_exec_env_t exec_env, Image* __result, int width, int height, int direction, Color* start, Color* end) {
    *__result = GenImageGradientLinear(width, height, direction, *start, *end);
}

static void raycart_GenImageGradientRadial(wasm_exec_env_t exec_env, Image* __result, int width, int height, float density, Color* inner, Color* outer) {
    *__result = GenImageGradientRadial(width, height, density, *inner, *outer);
}

static void raycart_GenImageGradientSquare(wasm_exec_env_t exec_env, Image* __result, int width, int height, float density, Color* inner, Color* outer) {
    *__result = GenImageGradientSquare(width, height, density, *inner, *outer);
}

static void raycart_GenImageChecked(wasm_exec_env_t exec_env, Image* __result, int width, int height, int checksX, int checksY, Color* col1, Color* col2) {
    *__result = GenImageChecked(width, height, checksX, checksY, *col1, *col2);
}

static void raycart_GenImageWhiteNoise(wasm_exec_env_t exec_env, Image* __result, int width, int height, float factor) {
    *__result = GenImageWhiteNoise(width, height, factor);
}

static void raycart_GenImagePerlinNoise(wasm_exec_env_t exec_env, Image* __result, int width, int height, int offsetX, int offsetY, float scale) {
    *__result = GenImagePerlinNoise(width, height, offsetX, offsetY, scale);
}

static void raycart_GenImageCellular(wasm_exec_env_t exec_env, Image* __result, int width, int height, int tileSize) {
    *__result = GenImageCellular(width, height, tileSize);
}

static void raycart_GenImageText(wasm_exec_env_t exec_env, Image* __result, int width, int height, const char * text) {
    *__result = GenImageText(width, height, text);
}

static void raycart_ImageCopy(wasm_exec_env_t exec_env, Image* __result, Image* image) {
    *__result = ImageCopy(*image);
}

static void raycart_ImageFromImage(wasm_exec_env_t exec_env, Image* __result, Image* image, Rectangle* rec) {
    *__result = ImageFromImage(*image, *rec);
}

static void raycart_ImageFromChannel(wasm_exec_env_t exec_env, Image* __result, Image* image, int selectedChannel) {
    *__result = ImageFromChannel(*image, selectedChannel);
}

static void raycart_ImageText(wasm_exec_env_t exec_env, Image* __result, const char * text, int fontSize, Color* color) {
    *__result = ImageText(text, fontSize, *color);
}

static void raycart_ImageTextEx(wasm_exec_env_t exec_env, Image* __result, Font* font, const char * text, float fontSize, float spacing, Color* tint) {
    *__result = ImageTextEx(*font, text, fontSize, spacing, *tint);
}

static void raycart_ImageFormat(wasm_exec_env_t exec_env, Image * image, int newFormat) {
    ImageFormat(image, newFormat);
}

static void raycart_ImageToPOT(wasm_exec_env_t exec_env, Image * image, Color* fill) {
    ImageToPOT(image, *fill);
}

static void raycart_ImageCrop(wasm_exec_env_t exec_env, Image * image, Rectangle* crop) {
    ImageCrop(image, *crop);
}

static void raycart_ImageAlphaCrop(wasm_exec_env_t exec_env, Image * image, float threshold) {
    ImageAlphaCrop(image, threshold);
}

static void raycart_ImageAlphaClear(wasm_exec_env_t exec_env, Image * image, Color* color, float threshold) {
    ImageAlphaClear(image, *color, threshold);
}

static void raycart_ImageAlphaMask(wasm_exec_env_t exec_env, Image * image, Image* alphaMask) {
    ImageAlphaMask(image, *alphaMask);
}

static void raycart_ImageAlphaPremultiply(wasm_exec_env_t exec_env, Image * image) {
    ImageAlphaPremultiply(image);
}

static void raycart_ImageBlurGaussian(wasm_exec_env_t exec_env, Image * image, int blurSize) {
    ImageBlurGaussian(image, blurSize);
}

static void raycart_ImageKernelConvolution(wasm_exec_env_t exec_env, Image * image, const float * kernel, int kernelSize) {
    ImageKernelConvolution(image, kernel, kernelSize);
}

static void raycart_ImageResize(wasm_exec_env_t exec_env, Image * image, int newWidth, int newHeight) {
    ImageResize(image, newWidth, newHeight);
}

static void raycart_ImageResizeNN(wasm_exec_env_t exec_env, Image * image, int newWidth, int newHeight) {
    ImageResizeNN(image, newWidth, newHeight);
}

static void raycart_ImageResizeCanvas(wasm_exec_env_t exec_env, Image * image, int newWidth, int newHeight, int offsetX, int offsetY, Color* fill) {
    ImageResizeCanvas(image, newWidth, newHeight, offsetX, offsetY, *fill);
}

static void raycart_ImageMipmaps(wasm_exec_env_t exec_env, Image * image) {
    ImageMipmaps(image);
}

static void raycart_ImageDither(wasm_exec_env_t exec_env, Image * image, int rBpp, int gBpp, int bBpp, int aBpp) {
    ImageDither(image, rBpp, gBpp, bBpp, aBpp);
}

static void raycart_ImageFlipVertical(wasm_exec_env_t exec_env, Image * image) {
    ImageFlipVertical(image);
}

static void raycart_ImageFlipHorizontal(wasm_exec_env_t exec_env, Image * image) {
    ImageFlipHorizontal(image);
}

static void raycart_ImageRotate(wasm_exec_env_t exec_env, Image * image, int degrees) {
    ImageRotate(image, degrees);
}

static void raycart_ImageRotateCW(wasm_exec_env_t exec_env, Image * image) {
    ImageRotateCW(image);
}

static void raycart_ImageRotateCCW(wasm_exec_env_t exec_env, Image * image) {
    ImageRotateCCW(image);
}

static void raycart_ImageColorTint(wasm_exec_env_t exec_env, Image * image, Color* color) {
    ImageColorTint(image, *color);
}

static void raycart_ImageColorInvert(wasm_exec_env_t exec_env, Image * image) {
    ImageColorInvert(image);
}

static void raycart_ImageColorGrayscale(wasm_exec_env_t exec_env, Image * image) {
    ImageColorGrayscale(image);
}

static void raycart_ImageColorContrast(wasm_exec_env_t exec_env, Image * image, float contrast) {
    ImageColorContrast(image, contrast);
}

static void raycart_ImageColorBrightness(wasm_exec_env_t exec_env, Image * image, int brightness) {
    ImageColorBrightness(image, brightness);
}

static void raycart_ImageColorReplace(wasm_exec_env_t exec_env, Image * image, Color* color, Color* replace) {
    ImageColorReplace(image, *color, *replace);
}

static void raycart_LoadImageColors(wasm_exec_env_t exec_env, Color ** __result, Image* image) {
    *__result = LoadImageColors(*image);
}

static void raycart_LoadImagePalette(wasm_exec_env_t exec_env, Color ** __result, Image* image, int maxPaletteSize, int * colorCount) {
    *__result = LoadImagePalette(*image, maxPaletteSize, colorCount);
}

static void raycart_UnloadImageColors(wasm_exec_env_t exec_env, Color * colors) {
    UnloadImageColors(colors);
}

static void raycart_UnloadImagePalette(wasm_exec_env_t exec_env, Color * colors) {
    UnloadImagePalette(colors);
}

static void raycart_GetImageAlphaBorder(wasm_exec_env_t exec_env, Rectangle* __result, Image* image, float threshold) {
    *__result = GetImageAlphaBorder(*image, threshold);
}

static void raycart_GetImageColor(wasm_exec_env_t exec_env, Color* __result, Image* image, int x, int y) {
    *__result = GetImageColor(*image, x, y);
}

static void raycart_ImageClearBackground(wasm_exec_env_t exec_env, Image * dst, Color* color) {
    ImageClearBackground(dst, *color);
}

static void raycart_ImageDrawPixel(wasm_exec_env_t exec_env, Image * dst, int posX, int posY, Color* color) {
    ImageDrawPixel(dst, posX, posY, *color);
}

static void raycart_ImageDrawPixelV(wasm_exec_env_t exec_env, Image * dst, Vector2* position, Color* color) {
    ImageDrawPixelV(dst, *position, *color);
}

static void raycart_ImageDrawLine(wasm_exec_env_t exec_env, Image * dst, int startPosX, int startPosY, int endPosX, int endPosY, Color* color) {
    ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, *color);
}

static void raycart_ImageDrawLineV(wasm_exec_env_t exec_env, Image * dst, Vector2* start, Vector2* end, Color* color) {
    ImageDrawLineV(dst, *start, *end, *color);
}

static void raycart_ImageDrawLineEx(wasm_exec_env_t exec_env, Image * dst, Vector2* start, Vector2* end, int thick, Color* color) {
    ImageDrawLineEx(dst, *start, *end, thick, *color);
}

static void raycart_ImageDrawCircle(wasm_exec_env_t exec_env, Image * dst, int centerX, int centerY, int radius, Color* color) {
    ImageDrawCircle(dst, centerX, centerY, radius, *color);
}

static void raycart_ImageDrawCircleV(wasm_exec_env_t exec_env, Image * dst, Vector2* center, int radius, Color* color) {
    ImageDrawCircleV(dst, *center, radius, *color);
}

static void raycart_ImageDrawCircleLines(wasm_exec_env_t exec_env, Image * dst, int centerX, int centerY, int radius, Color* color) {
    ImageDrawCircleLines(dst, centerX, centerY, radius, *color);
}

static void raycart_ImageDrawCircleLinesV(wasm_exec_env_t exec_env, Image * dst, Vector2* center, int radius, Color* color) {
    ImageDrawCircleLinesV(dst, *center, radius, *color);
}

static void raycart_ImageDrawRectangle(wasm_exec_env_t exec_env, Image * dst, int posX, int posY, int width, int height, Color* color) {
    ImageDrawRectangle(dst, posX, posY, width, height, *color);
}

static void raycart_ImageDrawRectangleV(wasm_exec_env_t exec_env, Image * dst, Vector2* position, Vector2* size, Color* color) {
    ImageDrawRectangleV(dst, *position, *size, *color);
}

static void raycart_ImageDrawRectangleRec(wasm_exec_env_t exec_env, Image * dst, Rectangle* rec, Color* color) {
    ImageDrawRectangleRec(dst, *rec, *color);
}

static void raycart_ImageDrawRectangleLines(wasm_exec_env_t exec_env, Image * dst, Rectangle* rec, int thick, Color* color) {
    ImageDrawRectangleLines(dst, *rec, thick, *color);
}

static void raycart_ImageDrawTriangle(wasm_exec_env_t exec_env, Image * dst, Vector2* v1, Vector2* v2, Vector2* v3, Color* color) {
    ImageDrawTriangle(dst, *v1, *v2, *v3, *color);
}

static void raycart_ImageDrawTriangleEx(wasm_exec_env_t exec_env, Image * dst, Vector2* v1, Vector2* v2, Vector2* v3, Color* c1, Color* c2, Color* c3) {
    ImageDrawTriangleEx(dst, *v1, *v2, *v3, *c1, *c2, *c3);
}

static void raycart_ImageDrawTriangleLines(wasm_exec_env_t exec_env, Image * dst, Vector2* v1, Vector2* v2, Vector2* v3, Color* color) {
    ImageDrawTriangleLines(dst, *v1, *v2, *v3, *color);
}

static void raycart_ImageDrawTriangleFan(wasm_exec_env_t exec_env, Image * dst, const Vector2 * points, int pointCount, Color* color) {
    ImageDrawTriangleFan(dst, points, pointCount, *color);
}

static void raycart_ImageDrawTriangleStrip(wasm_exec_env_t exec_env, Image * dst, const Vector2 * points, int pointCount, Color* color) {
    ImageDrawTriangleStrip(dst, points, pointCount, *color);
}

static void raycart_ImageDraw(wasm_exec_env_t exec_env, Image * dst, Image* src, Rectangle* srcRec, Rectangle* dstRec, Color* tint) {
    ImageDraw(dst, *src, *srcRec, *dstRec, *tint);
}

static void raycart_ImageDrawText(wasm_exec_env_t exec_env, Image * dst, const char * text, int posX, int posY, int fontSize, Color* color) {
    ImageDrawText(dst, text, posX, posY, fontSize, *color);
}

static void raycart_ImageDrawTextEx(wasm_exec_env_t exec_env, Image * dst, Font* font, const char * text, Vector2* position, float fontSize, float spacing, Color* tint) {
    ImageDrawTextEx(dst, *font, text, *position, fontSize, spacing, *tint);
}

static void raycart_LoadTexture(wasm_exec_env_t exec_env, Texture2D* __result, const char * fileName) {
    *__result = LoadTextureFromPhysFS(fileName);
}

static void raycart_LoadTextureFromImage(wasm_exec_env_t exec_env, Texture2D* __result, Image* image) {
    *__result = LoadTextureFromImage(*image);
}

static void raycart_LoadTextureCubemap(wasm_exec_env_t exec_env, TextureCubemap* __result, Image* image, int layout) {
    *__result = LoadTextureCubemap(*image, layout);
}

static void raycart_LoadRenderTexture(wasm_exec_env_t exec_env, RenderTexture2D* __result, int width, int height) {
    *__result = LoadRenderTexture(width, height);
}

static bool raycart_IsTextureValid(wasm_exec_env_t exec_env, Texture* texture) {
    return IsTextureValid(*texture);
}

static void raycart_UnloadTexture(wasm_exec_env_t exec_env, Texture* texture) {
    UnloadTexture(*texture);
}

static bool raycart_IsRenderTextureValid(wasm_exec_env_t exec_env, RenderTexture* target) {
    return IsRenderTextureValid(*target);
}

static void raycart_UnloadRenderTexture(wasm_exec_env_t exec_env, RenderTexture* target) {
    UnloadRenderTexture(*target);
}

static void raycart_UpdateTexture(wasm_exec_env_t exec_env, Texture* texture, const void * pixels) {
    UpdateTexture(*texture, pixels);
}

static void raycart_UpdateTextureRec(wasm_exec_env_t exec_env, Texture* texture, Rectangle* rec, const void * pixels) {
    UpdateTextureRec(*texture, *rec, pixels);
}

static void raycart_GenTextureMipmaps(wasm_exec_env_t exec_env, Texture2D * texture) {
    GenTextureMipmaps(texture);
}

static void raycart_SetTextureFilter(wasm_exec_env_t exec_env, Texture* texture, int filter) {
    SetTextureFilter(*texture, filter);
}

static void raycart_SetTextureWrap(wasm_exec_env_t exec_env, Texture* texture, int wrap) {
    SetTextureWrap(*texture, wrap);
}

static void raycart_DrawTexture(wasm_exec_env_t exec_env, Texture* texture, int posX, int posY, Color* tint) {
    DrawTexture(*texture, posX, posY, *tint);
}

static void raycart_DrawTextureV(wasm_exec_env_t exec_env, Texture* texture, Vector2* position, Color* tint) {
    DrawTextureV(*texture, *position, *tint);
}

static void raycart_DrawTextureEx(wasm_exec_env_t exec_env, Texture* texture, Vector2* position, float rotation, float scale, Color* tint) {
    DrawTextureEx(*texture, *position, rotation, scale, *tint);
}

static void raycart_DrawTextureRec(wasm_exec_env_t exec_env, Texture* texture, Rectangle* source, Vector2* position, Color* tint) {
    DrawTextureRec(*texture, *source, *position, *tint);
}

static void raycart_DrawTexturePro(wasm_exec_env_t exec_env, Texture* texture, Rectangle* source, Rectangle* dest, Vector2* origin, float rotation, Color* tint) {
    DrawTexturePro(*texture, *source, *dest, *origin, rotation, *tint);
}

static void raycart_DrawTextureNPatch(wasm_exec_env_t exec_env, Texture* texture, NPatchInfo* nPatchInfo, Rectangle* dest, Vector2* origin, float rotation, Color* tint) {
    DrawTextureNPatch(*texture, *nPatchInfo, *dest, *origin, rotation, *tint);
}

static bool raycart_ColorIsEqual(wasm_exec_env_t exec_env, Color* col1, Color* col2) {
    return ColorIsEqual(*col1, *col2);
}

static void raycart_Fade(wasm_exec_env_t exec_env, Color* __result, Color* color, float alpha) {
    *__result = Fade(*color, alpha);
}

static int raycart_ColorToInt(wasm_exec_env_t exec_env, Color* color) {
    return ColorToInt(*color);
}

static void raycart_ColorNormalize(wasm_exec_env_t exec_env, Vector4* __result, Color* color) {
    *__result = ColorNormalize(*color);
}

static void raycart_ColorFromNormalized(wasm_exec_env_t exec_env, Color* __result, Vector4* normalized) {
    *__result = ColorFromNormalized(*normalized);
}

static void raycart_ColorToHSV(wasm_exec_env_t exec_env, Vector3* __result, Color* color) {
    *__result = ColorToHSV(*color);
}

static void raycart_ColorFromHSV(wasm_exec_env_t exec_env, Color* __result, float hue, float saturation, float value) {
    *__result = ColorFromHSV(hue, saturation, value);
}

static void raycart_ColorTint(wasm_exec_env_t exec_env, Color* __result, Color* color, Color* tint) {
    *__result = ColorTint(*color, *tint);
}

static void raycart_ColorBrightness(wasm_exec_env_t exec_env, Color* __result, Color* color, float factor) {
    *__result = ColorBrightness(*color, factor);
}

static void raycart_ColorContrast(wasm_exec_env_t exec_env, Color* __result, Color* color, float contrast) {
    *__result = ColorContrast(*color, contrast);
}

static void raycart_ColorAlpha(wasm_exec_env_t exec_env, Color* __result, Color* color, float alpha) {
    *__result = ColorAlpha(*color, alpha);
}

static void raycart_ColorAlphaBlend(wasm_exec_env_t exec_env, Color* __result, Color* dst, Color* src, Color* tint) {
    *__result = ColorAlphaBlend(*dst, *src, *tint);
}

static void raycart_ColorLerp(wasm_exec_env_t exec_env, Color* __result, Color* color1, Color* color2, float factor) {
    *__result = ColorLerp(*color1, *color2, factor);
}

static void raycart_GetColor(wasm_exec_env_t exec_env, Color* __result, unsigned int hexValue) {
    *__result = GetColor(hexValue);
}

static void raycart_GetPixelColor(wasm_exec_env_t exec_env, Color* __result, void * srcPtr, int format) {
    *__result = GetPixelColor(srcPtr, format);
}

static void raycart_SetPixelColor(wasm_exec_env_t exec_env, void * dstPtr, Color* color, int format) {
    SetPixelColor(dstPtr, *color, format);
}

static int raycart_GetPixelDataSize(wasm_exec_env_t exec_env, int width, int height, int format) {
    return GetPixelDataSize(width, height, format);
}

static void raycart_GetFontDefault(wasm_exec_env_t exec_env, Font* __result) {
    *__result = GetFontDefault();
}

static void raycart_LoadFont(wasm_exec_env_t exec_env, Font* __result, const char * fileName) {
    *__result = LoadFont(fileName);
}

static void raycart_LoadFontEx(wasm_exec_env_t exec_env, Font* __result, const char * fileName, int fontSize, const int * codepoints, int codepointCount) {
    *__result = LoadFontEx(fileName, fontSize, codepoints, codepointCount);
}

static void raycart_LoadFontFromImage(wasm_exec_env_t exec_env, Font* __result, Image* image, Color* key, int firstChar) {
    *__result = LoadFontFromImage(*image, *key, firstChar);
}

static void raycart_LoadFontFromMemory(wasm_exec_env_t exec_env, Font* __result, const char * fileType, const unsigned char * fileData, int dataSize, int fontSize, const int * codepoints, int codepointCount) {
    *__result = LoadFontFromMemory(fileType, fileData, dataSize, fontSize, codepoints, codepointCount);
}

static bool raycart_IsFontValid(wasm_exec_env_t exec_env, Font* font) {
    return IsFontValid(*font);
}

static void raycart_GenImageFontAtlas(wasm_exec_env_t exec_env, Image* __result, const GlyphInfo * glyphs, Rectangle ** glyphRecs, int glyphCount, int fontSize, int padding, int packMethod) {
    *__result = GenImageFontAtlas(glyphs, glyphRecs, glyphCount, fontSize, padding, packMethod);
}

static void raycart_UnloadFontData(wasm_exec_env_t exec_env, GlyphInfo * glyphs, int glyphCount) {
    UnloadFontData(glyphs, glyphCount);
}

static void raycart_UnloadFont(wasm_exec_env_t exec_env, Font* font) {
    UnloadFont(*font);
}

static bool raycart_ExportFontAsCode(wasm_exec_env_t exec_env, Font* font, const char * fileName) {
    return ExportFontAsCode(*font, fileName);
}

static void raycart_DrawFPS(wasm_exec_env_t exec_env, int posX, int posY) {
    DrawFPS(posX, posY);
}

static void raycart_DrawText(wasm_exec_env_t exec_env, const char * text, int posX, int posY, int fontSize, Color* color) {
    DrawText(text, posX, posY, fontSize, *color);
}

static void raycart_DrawTextEx(wasm_exec_env_t exec_env, Font* font, const char * text, Vector2* position, float fontSize, float spacing, Color* tint) {
    DrawTextEx(*font, text, *position, fontSize, spacing, *tint);
}

static void raycart_DrawTextPro(wasm_exec_env_t exec_env, Font* font, const char * text, Vector2* position, Vector2* origin, float rotation, float fontSize, float spacing, Color* tint) {
    DrawTextPro(*font, text, *position, *origin, rotation, fontSize, spacing, *tint);
}

static void raycart_DrawTextCodepoint(wasm_exec_env_t exec_env, Font* font, int codepoint, Vector2* position, float fontSize, Color* tint) {
    DrawTextCodepoint(*font, codepoint, *position, fontSize, *tint);
}

static void raycart_DrawTextCodepoints(wasm_exec_env_t exec_env, Font* font, const int * codepoints, int codepointCount, Vector2* position, float fontSize, float spacing, Color* tint) {
    DrawTextCodepoints(*font, codepoints, codepointCount, *position, fontSize, spacing, *tint);
}

static void raycart_SetTextLineSpacing(wasm_exec_env_t exec_env, int spacing) {
    SetTextLineSpacing(spacing);
}

static int raycart_MeasureText(wasm_exec_env_t exec_env, const char * text, int fontSize) {
    return MeasureText(text, fontSize);
}

static void raycart_MeasureTextEx(wasm_exec_env_t exec_env, Vector2* __result, Font* font, const char * text, float fontSize, float spacing) {
    *__result = MeasureTextEx(*font, text, fontSize, spacing);
}

static int raycart_GetGlyphIndex(wasm_exec_env_t exec_env, Font* font, int codepoint) {
    return GetGlyphIndex(*font, codepoint);
}

static void raycart_GetGlyphInfo(wasm_exec_env_t exec_env, GlyphInfo* __result, Font* font, int codepoint) {
    *__result = GetGlyphInfo(*font, codepoint);
}

static void raycart_GetGlyphAtlasRec(wasm_exec_env_t exec_env, Rectangle* __result, Font* font, int codepoint) {
    *__result = GetGlyphAtlasRec(*font, codepoint);
}

static char * raycart_LoadUTF8(wasm_exec_env_t exec_env, const int * codepoints, int length) {
    return LoadUTF8(codepoints, length);
}

static void raycart_UnloadUTF8(wasm_exec_env_t exec_env, char * text) {
    UnloadUTF8(text);
}

static int * raycart_LoadCodepoints(wasm_exec_env_t exec_env, const char * text, int * count) {
    return LoadCodepoints(text, count);
}

static void raycart_UnloadCodepoints(wasm_exec_env_t exec_env, int * codepoints) {
    UnloadCodepoints(codepoints);
}

static int raycart_GetCodepointCount(wasm_exec_env_t exec_env, const char * text) {
    return GetCodepointCount(text);
}

static int raycart_GetCodepoint(wasm_exec_env_t exec_env, const char * text, int * codepointSize) {
    return GetCodepoint(text, codepointSize);
}

static int raycart_GetCodepointNext(wasm_exec_env_t exec_env, const char * text, int * codepointSize) {
    return GetCodepointNext(text, codepointSize);
}

static int raycart_GetCodepointPrevious(wasm_exec_env_t exec_env, const char * text, int * codepointSize) {
    return GetCodepointPrevious(text, codepointSize);
}

static const char * raycart_CodepointToUTF8(wasm_exec_env_t exec_env, int codepoint, int * utf8Size) {
    return CodepointToUTF8(codepoint, utf8Size);
}

static int raycart_TextCopy(wasm_exec_env_t exec_env, char * dst, const char * src) {
    return TextCopy(dst, src);
}

static bool raycart_TextIsEqual(wasm_exec_env_t exec_env, const char * text1, const char * text2) {
    return TextIsEqual(text1, text2);
}

static unsigned int raycart_TextLength(wasm_exec_env_t exec_env, const char * text) {
    return TextLength(text);
}

static const char * raycart_TextSubtext(wasm_exec_env_t exec_env, const char * text, int position, int length) {
    return TextSubtext(text, position, length);
}

static char * raycart_TextReplace(wasm_exec_env_t exec_env, const char * text, const char * search, const char * replacement) {
    return TextReplace(text, search, replacement);
}

static char * raycart_TextInsert(wasm_exec_env_t exec_env, const char * text, const char * insert, int position) {
    return TextInsert(text, insert, position);
}

static char * raycart_TextJoin(wasm_exec_env_t exec_env, char ** textList, int count, const char * delimiter) {
    return TextJoin(textList, count, delimiter);
}

static char ** raycart_TextSplit(wasm_exec_env_t exec_env, const char * text, char delimiter, int * count) {
    return TextSplit(text, delimiter, count);
}

static void raycart_TextAppend(wasm_exec_env_t exec_env, char * text, const char * append, int * position) {
    TextAppend(text, append, position);
}

static int raycart_TextFindIndex(wasm_exec_env_t exec_env, const char * text, const char * search) {
    return TextFindIndex(text, search);
}

static char * raycart_TextToUpper(wasm_exec_env_t exec_env, const char * text) {
    return TextToUpper(text);
}

static char * raycart_TextToLower(wasm_exec_env_t exec_env, const char * text) {
    return TextToLower(text);
}

static char * raycart_TextToPascal(wasm_exec_env_t exec_env, const char * text) {
    return TextToPascal(text);
}

static char * raycart_TextToSnake(wasm_exec_env_t exec_env, const char * text) {
    return TextToSnake(text);
}

static char * raycart_TextToCamel(wasm_exec_env_t exec_env, const char * text) {
    return TextToCamel(text);
}

static int raycart_TextToInteger(wasm_exec_env_t exec_env, const char * text) {
    return TextToInteger(text);
}

static float raycart_TextToFloat(wasm_exec_env_t exec_env, const char * text) {
    return TextToFloat(text);
}

static void raycart_DrawLine3D(wasm_exec_env_t exec_env, Vector3* startPos, Vector3* endPos, Color* color) {
    DrawLine3D(*startPos, *endPos, *color);
}

static void raycart_DrawPoint3D(wasm_exec_env_t exec_env, Vector3* position, Color* color) {
    DrawPoint3D(*position, *color);
}

static void raycart_DrawCircle3D(wasm_exec_env_t exec_env, Vector3* center, float radius, Vector3* rotationAxis, float rotationAngle, Color* color) {
    DrawCircle3D(*center, radius, *rotationAxis, rotationAngle, *color);
}

static void raycart_DrawTriangle3D(wasm_exec_env_t exec_env, Vector3* v1, Vector3* v2, Vector3* v3, Color* color) {
    DrawTriangle3D(*v1, *v2, *v3, *color);
}

static void raycart_DrawTriangleStrip3D(wasm_exec_env_t exec_env, const Vector3 * points, int pointCount, Color* color) {
    DrawTriangleStrip3D(points, pointCount, *color);
}

static void raycart_DrawCube(wasm_exec_env_t exec_env, Vector3* position, float width, float height, float length, Color* color) {
    DrawCube(*position, width, height, length, *color);
}

static void raycart_DrawCubeV(wasm_exec_env_t exec_env, Vector3* position, Vector3* size, Color* color) {
    DrawCubeV(*position, *size, *color);
}

static void raycart_DrawCubeWires(wasm_exec_env_t exec_env, Vector3* position, float width, float height, float length, Color* color) {
    DrawCubeWires(*position, width, height, length, *color);
}

static void raycart_DrawCubeWiresV(wasm_exec_env_t exec_env, Vector3* position, Vector3* size, Color* color) {
    DrawCubeWiresV(*position, *size, *color);
}

static void raycart_DrawSphere(wasm_exec_env_t exec_env, Vector3* centerPos, float radius, Color* color) {
    DrawSphere(*centerPos, radius, *color);
}

static void raycart_DrawSphereEx(wasm_exec_env_t exec_env, Vector3* centerPos, float radius, int rings, int slices, Color* color) {
    DrawSphereEx(*centerPos, radius, rings, slices, *color);
}

static void raycart_DrawSphereWires(wasm_exec_env_t exec_env, Vector3* centerPos, float radius, int rings, int slices, Color* color) {
    DrawSphereWires(*centerPos, radius, rings, slices, *color);
}

static void raycart_DrawCylinder(wasm_exec_env_t exec_env, Vector3* position, float radiusTop, float radiusBottom, float height, int slices, Color* color) {
    DrawCylinder(*position, radiusTop, radiusBottom, height, slices, *color);
}

static void raycart_DrawCylinderEx(wasm_exec_env_t exec_env, Vector3* startPos, Vector3* endPos, float startRadius, float endRadius, int sides, Color* color) {
    DrawCylinderEx(*startPos, *endPos, startRadius, endRadius, sides, *color);
}

static void raycart_DrawCylinderWires(wasm_exec_env_t exec_env, Vector3* position, float radiusTop, float radiusBottom, float height, int slices, Color* color) {
    DrawCylinderWires(*position, radiusTop, radiusBottom, height, slices, *color);
}

static void raycart_DrawCylinderWiresEx(wasm_exec_env_t exec_env, Vector3* startPos, Vector3* endPos, float startRadius, float endRadius, int sides, Color* color) {
    DrawCylinderWiresEx(*startPos, *endPos, startRadius, endRadius, sides, *color);
}

static void raycart_DrawCapsule(wasm_exec_env_t exec_env, Vector3* startPos, Vector3* endPos, float radius, int slices, int rings, Color* color) {
    DrawCapsule(*startPos, *endPos, radius, slices, rings, *color);
}

static void raycart_DrawCapsuleWires(wasm_exec_env_t exec_env, Vector3* startPos, Vector3* endPos, float radius, int slices, int rings, Color* color) {
    DrawCapsuleWires(*startPos, *endPos, radius, slices, rings, *color);
}

static void raycart_DrawPlane(wasm_exec_env_t exec_env, Vector3* centerPos, Vector2* size, Color* color) {
    DrawPlane(*centerPos, *size, *color);
}

static void raycart_DrawRay(wasm_exec_env_t exec_env, Ray* ray, Color* color) {
    DrawRay(*ray, *color);
}

static void raycart_DrawGrid(wasm_exec_env_t exec_env, int slices, float spacing) {
    DrawGrid(slices, spacing);
}

static void raycart_LoadModel(wasm_exec_env_t exec_env, Model* __result, const char * fileName) {
    *__result = LoadModel(fileName);
}

static void raycart_LoadModelFromMesh(wasm_exec_env_t exec_env, Model* __result, Mesh* mesh) {
    *__result = LoadModelFromMesh(*mesh);
}

static bool raycart_IsModelValid(wasm_exec_env_t exec_env, Model* model) {
    return IsModelValid(*model);
}

static void raycart_UnloadModel(wasm_exec_env_t exec_env, Model* model) {
    UnloadModel(*model);
}

static void raycart_GetModelBoundingBox(wasm_exec_env_t exec_env, BoundingBox* __result, Model* model) {
    *__result = GetModelBoundingBox(*model);
}

static void raycart_DrawModel(wasm_exec_env_t exec_env, Model* model, Vector3* position, float scale, Color* tint) {
    DrawModel(*model, *position, scale, *tint);
}

static void raycart_DrawModelEx(wasm_exec_env_t exec_env, Model* model, Vector3* position, Vector3* rotationAxis, float rotationAngle, Vector3* scale, Color* tint) {
    DrawModelEx(*model, *position, *rotationAxis, rotationAngle, *scale, *tint);
}

static void raycart_DrawModelWires(wasm_exec_env_t exec_env, Model* model, Vector3* position, float scale, Color* tint) {
    DrawModelWires(*model, *position, scale, *tint);
}

static void raycart_DrawModelWiresEx(wasm_exec_env_t exec_env, Model* model, Vector3* position, Vector3* rotationAxis, float rotationAngle, Vector3* scale, Color* tint) {
    DrawModelWiresEx(*model, *position, *rotationAxis, rotationAngle, *scale, *tint);
}

static void raycart_DrawModelPoints(wasm_exec_env_t exec_env, Model* model, Vector3* position, float scale, Color* tint) {
    DrawModelPoints(*model, *position, scale, *tint);
}

static void raycart_DrawModelPointsEx(wasm_exec_env_t exec_env, Model* model, Vector3* position, Vector3* rotationAxis, float rotationAngle, Vector3* scale, Color* tint) {
    DrawModelPointsEx(*model, *position, *rotationAxis, rotationAngle, *scale, *tint);
}

static void raycart_DrawBoundingBox(wasm_exec_env_t exec_env, BoundingBox* box, Color* color) {
    DrawBoundingBox(*box, *color);
}

static void raycart_DrawBillboard(wasm_exec_env_t exec_env, Camera3D* camera, Texture* texture, Vector3* position, float scale, Color* tint) {
    DrawBillboard(*camera, *texture, *position, scale, *tint);
}

static void raycart_DrawBillboardRec(wasm_exec_env_t exec_env, Camera3D* camera, Texture* texture, Rectangle* source, Vector3* position, Vector2* size, Color* tint) {
    DrawBillboardRec(*camera, *texture, *source, *position, *size, *tint);
}

static void raycart_DrawBillboardPro(wasm_exec_env_t exec_env, Camera3D* camera, Texture* texture, Rectangle* source, Vector3* position, Vector3* up, Vector2* size, Vector2* origin, float rotation, Color* tint) {
    DrawBillboardPro(*camera, *texture, *source, *position, *up, *size, *origin, rotation, *tint);
}

static void raycart_UploadMesh(wasm_exec_env_t exec_env, Mesh * mesh, bool dynamic) {
    UploadMesh(mesh, dynamic);
}

static void raycart_UpdateMeshBuffer(wasm_exec_env_t exec_env, Mesh* mesh, int index, const void * data, int dataSize, int offset) {
    UpdateMeshBuffer(*mesh, index, data, dataSize, offset);
}

static void raycart_UnloadMesh(wasm_exec_env_t exec_env, Mesh* mesh) {
    UnloadMesh(*mesh);
}

static void raycart_DrawMesh(wasm_exec_env_t exec_env, Mesh* mesh, Material* material, Matrix* transform) {
    DrawMesh(*mesh, *material, *transform);
}

static void raycart_DrawMeshInstanced(wasm_exec_env_t exec_env, Mesh* mesh, Material* material, const Matrix * transforms, int instances) {
    DrawMeshInstanced(*mesh, *material, transforms, instances);
}

static void raycart_GetMeshBoundingBox(wasm_exec_env_t exec_env, BoundingBox* __result, Mesh* mesh) {
    *__result = GetMeshBoundingBox(*mesh);
}

static void raycart_GenMeshTangents(wasm_exec_env_t exec_env, Mesh * mesh) {
    GenMeshTangents(mesh);
}

static bool raycart_ExportMesh(wasm_exec_env_t exec_env, Mesh* mesh, const char * fileName) {
    return ExportMesh(*mesh, fileName);
}

static bool raycart_ExportMeshAsCode(wasm_exec_env_t exec_env, Mesh* mesh, const char * fileName) {
    return ExportMeshAsCode(*mesh, fileName);
}

static void raycart_GenMeshPoly(wasm_exec_env_t exec_env, Mesh* __result, int sides, float radius) {
    *__result = GenMeshPoly(sides, radius);
}

static void raycart_GenMeshPlane(wasm_exec_env_t exec_env, Mesh* __result, float width, float length, int resX, int resZ) {
    *__result = GenMeshPlane(width, length, resX, resZ);
}

static void raycart_GenMeshCube(wasm_exec_env_t exec_env, Mesh* __result, float width, float height, float length) {
    *__result = GenMeshCube(width, height, length);
}

static void raycart_GenMeshSphere(wasm_exec_env_t exec_env, Mesh* __result, float radius, int rings, int slices) {
    *__result = GenMeshSphere(radius, rings, slices);
}

static void raycart_GenMeshHemiSphere(wasm_exec_env_t exec_env, Mesh* __result, float radius, int rings, int slices) {
    *__result = GenMeshHemiSphere(radius, rings, slices);
}

static void raycart_GenMeshCylinder(wasm_exec_env_t exec_env, Mesh* __result, float radius, float height, int slices) {
    *__result = GenMeshCylinder(radius, height, slices);
}

static void raycart_GenMeshCone(wasm_exec_env_t exec_env, Mesh* __result, float radius, float height, int slices) {
    *__result = GenMeshCone(radius, height, slices);
}

static void raycart_GenMeshTorus(wasm_exec_env_t exec_env, Mesh* __result, float radius, float size, int radSeg, int sides) {
    *__result = GenMeshTorus(radius, size, radSeg, sides);
}

static void raycart_GenMeshKnot(wasm_exec_env_t exec_env, Mesh* __result, float radius, float size, int radSeg, int sides) {
    *__result = GenMeshKnot(radius, size, radSeg, sides);
}

static void raycart_GenMeshHeightmap(wasm_exec_env_t exec_env, Mesh* __result, Image* heightmap, Vector3* size) {
    *__result = GenMeshHeightmap(*heightmap, *size);
}

static void raycart_GenMeshCubicmap(wasm_exec_env_t exec_env, Mesh* __result, Image* cubicmap, Vector3* cubeSize) {
    *__result = GenMeshCubicmap(*cubicmap, *cubeSize);
}

static void raycart_LoadMaterials(wasm_exec_env_t exec_env, Material ** __result, const char * fileName, int * materialCount) {
    *__result = LoadMaterials(fileName, materialCount);
}

static void raycart_LoadMaterialDefault(wasm_exec_env_t exec_env, Material* __result) {
    *__result = LoadMaterialDefault();
}

static bool raycart_IsMaterialValid(wasm_exec_env_t exec_env, Material* material) {
    return IsMaterialValid(*material);
}

static void raycart_UnloadMaterial(wasm_exec_env_t exec_env, Material* material) {
    UnloadMaterial(*material);
}

static void raycart_SetMaterialTexture(wasm_exec_env_t exec_env, Material * material, int mapType, Texture* texture) {
    SetMaterialTexture(material, mapType, *texture);
}

static void raycart_SetModelMeshMaterial(wasm_exec_env_t exec_env, Model * model, int meshId, int materialId) {
    SetModelMeshMaterial(model, meshId, materialId);
}

static void raycart_LoadModelAnimations(wasm_exec_env_t exec_env, ModelAnimation ** __result, const char * fileName, int * animCount) {
    *__result = LoadModelAnimations(fileName, animCount);
}

static void raycart_UpdateModelAnimation(wasm_exec_env_t exec_env, Model* model, ModelAnimation* anim, int frame) {
    UpdateModelAnimation(*model, *anim, frame);
}

static void raycart_UpdateModelAnimationBones(wasm_exec_env_t exec_env, Model* model, ModelAnimation* anim, int frame) {
    UpdateModelAnimationBones(*model, *anim, frame);
}

static void raycart_UnloadModelAnimation(wasm_exec_env_t exec_env, ModelAnimation* anim) {
    UnloadModelAnimation(*anim);
}

static void raycart_UnloadModelAnimations(wasm_exec_env_t exec_env, ModelAnimation * animations, int animCount) {
    UnloadModelAnimations(animations, animCount);
}

static bool raycart_IsModelAnimationValid(wasm_exec_env_t exec_env, Model* model, ModelAnimation* anim) {
    return IsModelAnimationValid(*model, *anim);
}

static bool raycart_CheckCollisionSpheres(wasm_exec_env_t exec_env, Vector3* center1, float radius1, Vector3* center2, float radius2) {
    return CheckCollisionSpheres(*center1, radius1, *center2, radius2);
}

static bool raycart_CheckCollisionBoxes(wasm_exec_env_t exec_env, BoundingBox* box1, BoundingBox* box2) {
    return CheckCollisionBoxes(*box1, *box2);
}

static bool raycart_CheckCollisionBoxSphere(wasm_exec_env_t exec_env, BoundingBox* box, Vector3* center, float radius) {
    return CheckCollisionBoxSphere(*box, *center, radius);
}

static void raycart_GetRayCollisionSphere(wasm_exec_env_t exec_env, RayCollision* __result, Ray* ray, Vector3* center, float radius) {
    *__result = GetRayCollisionSphere(*ray, *center, radius);
}

static void raycart_GetRayCollisionBox(wasm_exec_env_t exec_env, RayCollision* __result, Ray* ray, BoundingBox* box) {
    *__result = GetRayCollisionBox(*ray, *box);
}

static void raycart_GetRayCollisionMesh(wasm_exec_env_t exec_env, RayCollision* __result, Ray* ray, Mesh* mesh, Matrix* transform) {
    *__result = GetRayCollisionMesh(*ray, *mesh, *transform);
}

static void raycart_GetRayCollisionTriangle(wasm_exec_env_t exec_env, RayCollision* __result, Ray* ray, Vector3* p1, Vector3* p2, Vector3* p3) {
    *__result = GetRayCollisionTriangle(*ray, *p1, *p2, *p3);
}

static void raycart_GetRayCollisionQuad(wasm_exec_env_t exec_env, RayCollision* __result, Ray* ray, Vector3* p1, Vector3* p2, Vector3* p3, Vector3* p4) {
    *__result = GetRayCollisionQuad(*ray, *p1, *p2, *p3, *p4);
}

static void raycart_InitAudioDevice(wasm_exec_env_t exec_env) {
    InitAudioDevice();
}

static void raycart_CloseAudioDevice(wasm_exec_env_t exec_env) {
    CloseAudioDevice();
}

static bool raycart_IsAudioDeviceReady(wasm_exec_env_t exec_env) {
    return IsAudioDeviceReady();
}

static void raycart_SetMasterVolume(wasm_exec_env_t exec_env, float volume) {
    SetMasterVolume(volume);
}

static float raycart_GetMasterVolume(wasm_exec_env_t exec_env) {
    return GetMasterVolume();
}

static void raycart_LoadWave(wasm_exec_env_t exec_env, Wave* __result, const char * fileName) {
    *__result = LoadWaveFromPhysFS(fileName);
}

static void raycart_LoadWaveFromMemory(wasm_exec_env_t exec_env, Wave* __result, const char * fileType, const unsigned char * fileData, int dataSize) {
    *__result = LoadWaveFromMemory(fileType, fileData, dataSize);
}

static bool raycart_IsWaveValid(wasm_exec_env_t exec_env, Wave* wave) {
    return IsWaveValid(*wave);
}

static void raycart_LoadSound(wasm_exec_env_t exec_env, Sound* __result, const char * fileName) {
    *__result = LoadSound(fileName);
}

static void raycart_LoadSoundFromWave(wasm_exec_env_t exec_env, Sound* __result, Wave* wave) {
    *__result = LoadSoundFromWave(*wave);
}

static void raycart_LoadSoundAlias(wasm_exec_env_t exec_env, Sound* __result, Sound* source) {
    *__result = LoadSoundAlias(*source);
}

static bool raycart_IsSoundValid(wasm_exec_env_t exec_env, Sound* sound) {
    return IsSoundValid(*sound);
}

static void raycart_UpdateSound(wasm_exec_env_t exec_env, Sound* sound, const void * data, int sampleCount) {
    UpdateSound(*sound, data, sampleCount);
}

static void raycart_UnloadWave(wasm_exec_env_t exec_env, Wave* wave) {
    UnloadWave(*wave);
}

static void raycart_UnloadSound(wasm_exec_env_t exec_env, Sound* sound) {
    UnloadSound(*sound);
}

static void raycart_UnloadSoundAlias(wasm_exec_env_t exec_env, Sound* alias) {
    UnloadSoundAlias(*alias);
}

static bool raycart_ExportWave(wasm_exec_env_t exec_env, Wave* wave, const char * fileName) {
    return ExportWave(*wave, fileName);
}

static bool raycart_ExportWaveAsCode(wasm_exec_env_t exec_env, Wave* wave, const char * fileName) {
    return ExportWaveAsCode(*wave, fileName);
}

static void raycart_PlaySound(wasm_exec_env_t exec_env, Sound* sound) {
    PlaySound(*sound);
}

static void raycart_StopSound(wasm_exec_env_t exec_env, Sound* sound) {
    StopSound(*sound);
}

static void raycart_PauseSound(wasm_exec_env_t exec_env, Sound* sound) {
    PauseSound(*sound);
}

static void raycart_ResumeSound(wasm_exec_env_t exec_env, Sound* sound) {
    ResumeSound(*sound);
}

static bool raycart_IsSoundPlaying(wasm_exec_env_t exec_env, Sound* sound) {
    return IsSoundPlaying(*sound);
}

static void raycart_SetSoundVolume(wasm_exec_env_t exec_env, Sound* sound, float volume) {
    SetSoundVolume(*sound, volume);
}

static void raycart_SetSoundPitch(wasm_exec_env_t exec_env, Sound* sound, float pitch) {
    SetSoundPitch(*sound, pitch);
}

static void raycart_SetSoundPan(wasm_exec_env_t exec_env, Sound* sound, float pan) {
    SetSoundPan(*sound, pan);
}

static void raycart_WaveCopy(wasm_exec_env_t exec_env, Wave* __result, Wave* wave) {
    *__result = WaveCopy(*wave);
}

static void raycart_WaveCrop(wasm_exec_env_t exec_env, Wave * wave, int initFrame, int finalFrame) {
    WaveCrop(wave, initFrame, finalFrame);
}

static void raycart_WaveFormat(wasm_exec_env_t exec_env, Wave * wave, int sampleRate, int sampleSize, int channels) {
    WaveFormat(wave, sampleRate, sampleSize, channels);
}

static float * raycart_LoadWaveSamples(wasm_exec_env_t exec_env, Wave* wave) {
    return LoadWaveSamples(*wave);
}

static void raycart_UnloadWaveSamples(wasm_exec_env_t exec_env, float * samples) {
    UnloadWaveSamples(samples);
}

static void raycart_LoadMusicStream(wasm_exec_env_t exec_env, Music* __result, const char * fileName) {
    *__result = LoadMusicStreamFromPhysFS(fileName);
}

static void raycart_LoadMusicStreamFromMemory(wasm_exec_env_t exec_env, Music* __result, const char * fileType, const unsigned char * data, int dataSize) {
    *__result = LoadMusicStreamFromMemory(fileType, data, dataSize);
}

static bool raycart_IsMusicValid(wasm_exec_env_t exec_env, Music* music) {
    return IsMusicValid(*music);
}

static void raycart_UnloadMusicStream(wasm_exec_env_t exec_env, Music* music) {
    UnloadMusicStream(*music);
}

static void raycart_PlayMusicStream(wasm_exec_env_t exec_env, Music* music) {
    PlayMusicStream(*music);
}

static bool raycart_IsMusicStreamPlaying(wasm_exec_env_t exec_env, Music* music) {
    return IsMusicStreamPlaying(*music);
}

static void raycart_UpdateMusicStream(wasm_exec_env_t exec_env, Music* music) {
    UpdateMusicStream(*music);
}

static void raycart_StopMusicStream(wasm_exec_env_t exec_env, Music* music) {
    StopMusicStream(*music);
}

static void raycart_PauseMusicStream(wasm_exec_env_t exec_env, Music* music) {
    PauseMusicStream(*music);
}

static void raycart_ResumeMusicStream(wasm_exec_env_t exec_env, Music* music) {
    ResumeMusicStream(*music);
}

static void raycart_SeekMusicStream(wasm_exec_env_t exec_env, Music* music, float position) {
    SeekMusicStream(*music, position);
}

static void raycart_SetMusicVolume(wasm_exec_env_t exec_env, Music* music, float volume) {
    SetMusicVolume(*music, volume);
}

static void raycart_SetMusicPitch(wasm_exec_env_t exec_env, Music* music, float pitch) {
    SetMusicPitch(*music, pitch);
}

static void raycart_SetMusicPan(wasm_exec_env_t exec_env, Music* music, float pan) {
    SetMusicPan(*music, pan);
}

static float raycart_GetMusicTimeLength(wasm_exec_env_t exec_env, Music* music) {
    return GetMusicTimeLength(*music);
}

static float raycart_GetMusicTimePlayed(wasm_exec_env_t exec_env, Music* music) {
    return GetMusicTimePlayed(*music);
}

static void raycart_LoadAudioStream(wasm_exec_env_t exec_env, AudioStream* __result, unsigned int sampleRate, unsigned int sampleSize, unsigned int channels) {
    *__result = LoadAudioStream(sampleRate, sampleSize, channels);
}

static bool raycart_IsAudioStreamValid(wasm_exec_env_t exec_env, AudioStream* stream) {
    return IsAudioStreamValid(*stream);
}

static void raycart_UnloadAudioStream(wasm_exec_env_t exec_env, AudioStream* stream) {
    UnloadAudioStream(*stream);
}

static void raycart_UpdateAudioStream(wasm_exec_env_t exec_env, AudioStream* stream, const void * data, int frameCount) {
    UpdateAudioStream(*stream, data, frameCount);
}

static bool raycart_IsAudioStreamProcessed(wasm_exec_env_t exec_env, AudioStream* stream) {
    return IsAudioStreamProcessed(*stream);
}

static void raycart_PlayAudioStream(wasm_exec_env_t exec_env, AudioStream* stream) {
    PlayAudioStream(*stream);
}

static void raycart_PauseAudioStream(wasm_exec_env_t exec_env, AudioStream* stream) {
    PauseAudioStream(*stream);
}

static void raycart_ResumeAudioStream(wasm_exec_env_t exec_env, AudioStream* stream) {
    ResumeAudioStream(*stream);
}

static bool raycart_IsAudioStreamPlaying(wasm_exec_env_t exec_env, AudioStream* stream) {
    return IsAudioStreamPlaying(*stream);
}

static void raycart_StopAudioStream(wasm_exec_env_t exec_env, AudioStream* stream) {
    StopAudioStream(*stream);
}

static void raycart_SetAudioStreamVolume(wasm_exec_env_t exec_env, AudioStream* stream, float volume) {
    SetAudioStreamVolume(*stream, volume);
}

static void raycart_SetAudioStreamPitch(wasm_exec_env_t exec_env, AudioStream* stream, float pitch) {
    SetAudioStreamPitch(*stream, pitch);
}

static void raycart_SetAudioStreamPan(wasm_exec_env_t exec_env, AudioStream* stream, float pan) {
    SetAudioStreamPan(*stream, pan);
}

static void raycart_SetAudioStreamBufferSizeDefault(wasm_exec_env_t exec_env, int size) {
    SetAudioStreamBufferSizeDefault(size);
}

#define raycart_native_symbols_count 564
static NativeSymbol raycart_native_symbols[raycart_native_symbols_count] = {
    {"InitWindow", raycart_InitWindow, "(ii$)"},
    {"CloseWindow", raycart_CloseWindow, "()"},
    {"WindowShouldClose", raycart_WindowShouldClose, "()i"},
    {"IsWindowReady", raycart_IsWindowReady, "()i"},
    {"IsWindowFullscreen", raycart_IsWindowFullscreen, "()i"},
    {"IsWindowHidden", raycart_IsWindowHidden, "()i"},
    {"IsWindowMinimized", raycart_IsWindowMinimized, "()i"},
    {"IsWindowMaximized", raycart_IsWindowMaximized, "()i"},
    {"IsWindowFocused", raycart_IsWindowFocused, "()i"},
    {"IsWindowResized", raycart_IsWindowResized, "()i"},
    {"IsWindowState", raycart_IsWindowState, "(i)i"},
    {"SetWindowState", raycart_SetWindowState, "(i)"},
    {"ClearWindowState", raycart_ClearWindowState, "(i)"},
    {"ToggleFullscreen", raycart_ToggleFullscreen, "()"},
    {"ToggleBorderlessWindowed", raycart_ToggleBorderlessWindowed, "()"},
    {"MaximizeWindow", raycart_MaximizeWindow, "()"},
    {"MinimizeWindow", raycart_MinimizeWindow, "()"},
    {"RestoreWindow", raycart_RestoreWindow, "()"},
    {"SetWindowIcon", raycart_SetWindowIcon, "(*)"},
    {"SetWindowIcons", raycart_SetWindowIcons, "(*i)"},
    {"SetWindowTitle", raycart_SetWindowTitle, "($)"},
    {"SetWindowPosition", raycart_SetWindowPosition, "(ii)"},
    {"SetWindowMonitor", raycart_SetWindowMonitor, "(i)"},
    {"SetWindowMinSize", raycart_SetWindowMinSize, "(ii)"},
    {"SetWindowMaxSize", raycart_SetWindowMaxSize, "(ii)"},
    {"SetWindowSize", raycart_SetWindowSize, "(ii)"},
    {"SetWindowOpacity", raycart_SetWindowOpacity, "(f)"},
    {"SetWindowFocused", raycart_SetWindowFocused, "()"},
    {"GetWindowHandle", raycart_GetWindowHandle, "()*"},
    {"GetScreenWidth", raycart_GetScreenWidth, "()i"},
    {"GetScreenHeight", raycart_GetScreenHeight, "()i"},
    {"GetRenderWidth", raycart_GetRenderWidth, "()i"},
    {"GetRenderHeight", raycart_GetRenderHeight, "()i"},
    {"GetMonitorCount", raycart_GetMonitorCount, "()i"},
    {"GetCurrentMonitor", raycart_GetCurrentMonitor, "()i"},
    {"GetMonitorPosition", raycart_GetMonitorPosition, "(*i)"},
    {"GetMonitorWidth", raycart_GetMonitorWidth, "(i)i"},
    {"GetMonitorHeight", raycart_GetMonitorHeight, "(i)i"},
    {"GetMonitorPhysicalWidth", raycart_GetMonitorPhysicalWidth, "(i)i"},
    {"GetMonitorPhysicalHeight", raycart_GetMonitorPhysicalHeight, "(i)i"},
    {"GetMonitorRefreshRate", raycart_GetMonitorRefreshRate, "(i)i"},
    {"GetWindowPosition", raycart_GetWindowPosition, "(*)"},
    {"GetWindowScaleDPI", raycart_GetWindowScaleDPI, "(*)"},
    {"GetMonitorName", raycart_GetMonitorName, "(i)$"},
    {"SetClipboardText", raycart_SetClipboardText, "($)"},
    {"GetClipboardText", raycart_GetClipboardText, "()$"},
    {"EnableEventWaiting", raycart_EnableEventWaiting, "()"},
    {"DisableEventWaiting", raycart_DisableEventWaiting, "()"},
    {"ShowCursor", raycart_ShowCursor, "()"},
    {"HideCursor", raycart_HideCursor, "()"},
    {"IsCursorHidden", raycart_IsCursorHidden, "()i"},
    {"EnableCursor", raycart_EnableCursor, "()"},
    {"DisableCursor", raycart_DisableCursor, "()"},
    {"IsCursorOnScreen", raycart_IsCursorOnScreen, "()i"},
    {"ClearBackground", raycart_ClearBackground, "(*)"},
    {"BeginDrawing", raycart_BeginDrawing, "()"},
    {"EndDrawing", raycart_EndDrawing, "()"},
    {"BeginMode2D", raycart_BeginMode2D, "(*)"},
    {"EndMode2D", raycart_EndMode2D, "()"},
    {"BeginMode3D", raycart_BeginMode3D, "(*)"},
    {"EndMode3D", raycart_EndMode3D, "()"},
    {"BeginTextureMode", raycart_BeginTextureMode, "(*)"},
    {"EndTextureMode", raycart_EndTextureMode, "()"},
    {"BeginShaderMode", raycart_BeginShaderMode, "(*)"},
    {"EndShaderMode", raycart_EndShaderMode, "()"},
    {"BeginBlendMode", raycart_BeginBlendMode, "(i)"},
    {"EndBlendMode", raycart_EndBlendMode, "()"},
    {"BeginScissorMode", raycart_BeginScissorMode, "(iiii)"},
    {"EndScissorMode", raycart_EndScissorMode, "()"},
    {"BeginVrStereoMode", raycart_BeginVrStereoMode, "(*)"},
    {"EndVrStereoMode", raycart_EndVrStereoMode, "()"},
    {"LoadVrStereoConfig", raycart_LoadVrStereoConfig, "(**)"},
    {"UnloadVrStereoConfig", raycart_UnloadVrStereoConfig, "(*)"},
    {"LoadShader", raycart_LoadShader, "(*$$)"},
    {"LoadShaderFromMemory", raycart_LoadShaderFromMemory, "(*$$)"},
    {"IsShaderValid", raycart_IsShaderValid, "(*)i"},
    {"GetShaderLocation", raycart_GetShaderLocation, "(*$)i"},
    {"GetShaderLocationAttrib", raycart_GetShaderLocationAttrib, "(*$)i"},
    {"SetShaderValue", raycart_SetShaderValue, "(*i*i)"},
    {"SetShaderValueV", raycart_SetShaderValueV, "(*i*ii)"},
    {"SetShaderValueMatrix", raycart_SetShaderValueMatrix, "(*i*)"},
    {"SetShaderValueTexture", raycart_SetShaderValueTexture, "(*i*)"},
    {"UnloadShader", raycart_UnloadShader, "(*)"},
    {"GetScreenToWorldRay", raycart_GetScreenToWorldRay, "(***)"},
    {"GetScreenToWorldRayEx", raycart_GetScreenToWorldRayEx, "(***ii)"},
    {"GetWorldToScreen", raycart_GetWorldToScreen, "(***)"},
    {"GetWorldToScreenEx", raycart_GetWorldToScreenEx, "(***ii)"},
    {"GetWorldToScreen2D", raycart_GetWorldToScreen2D, "(***)"},
    {"GetScreenToWorld2D", raycart_GetScreenToWorld2D, "(***)"},
    {"GetCameraMatrix", raycart_GetCameraMatrix, "(**)"},
    {"GetCameraMatrix2D", raycart_GetCameraMatrix2D, "(**)"},
    {"SetTargetFPS", raycart_SetTargetFPS, "(i)"},
    {"GetFrameTime", raycart_GetFrameTime, "()f"},
    {"GetTime", raycart_GetTime, "()F"},
    {"GetFPS", raycart_GetFPS, "()i"},
    {"SwapScreenBuffer", raycart_SwapScreenBuffer, "()"},
    {"PollInputEvents", raycart_PollInputEvents, "()"},
    {"WaitTime", raycart_WaitTime, "(F)"},
    {"SetRandomSeed", raycart_SetRandomSeed, "(i)"},
    {"GetRandomValue", raycart_GetRandomValue, "(ii)i"},
    {"LoadRandomSequence", raycart_LoadRandomSequence, "(iii)*"},
    {"UnloadRandomSequence", raycart_UnloadRandomSequence, "(*)"},
    {"TakeScreenshot", raycart_TakeScreenshot, "($)"},
    {"SetConfigFlags", raycart_SetConfigFlags, "(i)"},
    {"OpenURL", raycart_OpenURL, "($)"},
    {"SetTraceLogLevel", raycart_SetTraceLogLevel, "(i)"},
    {"LoadFileData", raycart_LoadFileData, "($*)$"},
    {"UnloadFileData", raycart_UnloadFileData, "($)"},
    {"SaveFileData", raycart_SaveFileData, "($*i)i"},
    {"ExportDataAsCode", raycart_ExportDataAsCode, "($i$)i"},
    {"LoadFileText", raycart_LoadFileText, "($)$"},
    {"UnloadFileText", raycart_UnloadFileText, "($)"},
    {"SaveFileText", raycart_SaveFileText, "($$)i"},
    {"FileExists", raycart_FileExists, "($)i"},
    {"DirectoryExists", raycart_DirectoryExists, "($)i"},
    {"IsFileExtension", raycart_IsFileExtension, "($$)i"},
    {"GetFileLength", raycart_GetFileLength, "($)i"},
    {"GetFileModTime", raycart_GetFileModTime, "($)i"},
    {"GetFileExtension", raycart_GetFileExtension, "($)$"},
    {"GetFileName", raycart_GetFileName, "($)$"},
    {"GetFileNameWithoutExt", raycart_GetFileNameWithoutExt, "($)$"},
    {"GetDirectoryPath", raycart_GetDirectoryPath, "($)$"},
    {"GetPrevDirectoryPath", raycart_GetPrevDirectoryPath, "($)$"},
    {"GetWorkingDirectory", raycart_GetWorkingDirectory, "()$"},
    {"GetApplicationDirectory", raycart_GetApplicationDirectory, "()$"},
    {"MakeDirectory", raycart_MakeDirectory, "($)i"},
    {"ChangeDirectory", raycart_ChangeDirectory, "($)i"},
    {"IsPathFile", raycart_IsPathFile, "($)i"},
    {"IsFileNameValid", raycart_IsFileNameValid, "($)i"},
    {"LoadDirectoryFiles", raycart_LoadDirectoryFiles, "(*$)"},
    {"LoadDirectoryFilesEx", raycart_LoadDirectoryFilesEx, "(*$$i)"},
    {"UnloadDirectoryFiles", raycart_UnloadDirectoryFiles, "(*)"},
    {"IsFileDropped", raycart_IsFileDropped, "()i"},
    {"LoadDroppedFiles", raycart_LoadDroppedFiles, "(*)"},
    {"UnloadDroppedFiles", raycart_UnloadDroppedFiles, "(*)"},
    {"CompressData", raycart_CompressData, "($i*)$"},
    {"DecompressData", raycart_DecompressData, "($i*)$"},
    {"EncodeDataBase64", raycart_EncodeDataBase64, "($i*)$"},
    {"DecodeDataBase64", raycart_DecodeDataBase64, "($*)$"},
    {"ComputeCRC32", raycart_ComputeCRC32, "($i)i"},
    {"ComputeMD5", raycart_ComputeMD5, "($i)*"},
    {"ComputeSHA1", raycart_ComputeSHA1, "($i)*"},
    {"LoadAutomationEventList", raycart_LoadAutomationEventList, "(*$)"},
    {"UnloadAutomationEventList", raycart_UnloadAutomationEventList, "(*)"},
    {"ExportAutomationEventList", raycart_ExportAutomationEventList, "(*$)i"},
    {"SetAutomationEventList", raycart_SetAutomationEventList, "(*)"},
    {"SetAutomationEventBaseFrame", raycart_SetAutomationEventBaseFrame, "(i)"},
    {"StartAutomationEventRecording", raycart_StartAutomationEventRecording, "()"},
    {"StopAutomationEventRecording", raycart_StopAutomationEventRecording, "()"},
    {"PlayAutomationEvent", raycart_PlayAutomationEvent, "(*)"},
    {"IsKeyPressed", raycart_IsKeyPressed, "(i)i"},
    {"IsKeyPressedRepeat", raycart_IsKeyPressedRepeat, "(i)i"},
    {"IsKeyDown", raycart_IsKeyDown, "(i)i"},
    {"IsKeyReleased", raycart_IsKeyReleased, "(i)i"},
    {"IsKeyUp", raycart_IsKeyUp, "(i)i"},
    {"GetKeyPressed", raycart_GetKeyPressed, "()i"},
    {"GetCharPressed", raycart_GetCharPressed, "()i"},
    {"SetExitKey", raycart_SetExitKey, "(i)"},
    {"IsGamepadAvailable", raycart_IsGamepadAvailable, "(i)i"},
    {"GetGamepadName", raycart_GetGamepadName, "(i)$"},
    {"IsGamepadButtonPressed", raycart_IsGamepadButtonPressed, "(ii)i"},
    {"IsGamepadButtonDown", raycart_IsGamepadButtonDown, "(ii)i"},
    {"IsGamepadButtonReleased", raycart_IsGamepadButtonReleased, "(ii)i"},
    {"IsGamepadButtonUp", raycart_IsGamepadButtonUp, "(ii)i"},
    {"GetGamepadButtonPressed", raycart_GetGamepadButtonPressed, "()i"},
    {"GetGamepadAxisCount", raycart_GetGamepadAxisCount, "(i)i"},
    {"GetGamepadAxisMovement", raycart_GetGamepadAxisMovement, "(ii)f"},
    {"SetGamepadMappings", raycart_SetGamepadMappings, "($)i"},
    {"SetGamepadVibration", raycart_SetGamepadVibration, "(ifff)"},
    {"IsMouseButtonPressed", raycart_IsMouseButtonPressed, "(i)i"},
    {"IsMouseButtonDown", raycart_IsMouseButtonDown, "(i)i"},
    {"IsMouseButtonReleased", raycart_IsMouseButtonReleased, "(i)i"},
    {"IsMouseButtonUp", raycart_IsMouseButtonUp, "(i)i"},
    {"GetMouseX", raycart_GetMouseX, "()i"},
    {"GetMouseY", raycart_GetMouseY, "()i"},
    {"GetMousePosition", raycart_GetMousePosition, "(*)"},
    {"GetMouseDelta", raycart_GetMouseDelta, "(*)"},
    {"SetMousePosition", raycart_SetMousePosition, "(ii)"},
    {"SetMouseOffset", raycart_SetMouseOffset, "(ii)"},
    {"SetMouseScale", raycart_SetMouseScale, "(ff)"},
    {"GetMouseWheelMove", raycart_GetMouseWheelMove, "()f"},
    {"GetMouseWheelMoveV", raycart_GetMouseWheelMoveV, "(*)"},
    {"SetMouseCursor", raycart_SetMouseCursor, "(i)"},
    {"GetTouchX", raycart_GetTouchX, "()i"},
    {"GetTouchY", raycart_GetTouchY, "()i"},
    {"GetTouchPosition", raycart_GetTouchPosition, "(*i)"},
    {"GetTouchPointId", raycart_GetTouchPointId, "(i)i"},
    {"GetTouchPointCount", raycart_GetTouchPointCount, "()i"},
    {"SetGesturesEnabled", raycart_SetGesturesEnabled, "(i)"},
    {"IsGestureDetected", raycart_IsGestureDetected, "(i)i"},
    {"GetGestureDetected", raycart_GetGestureDetected, "()i"},
    {"GetGestureHoldDuration", raycart_GetGestureHoldDuration, "()f"},
    {"GetGestureDragVector", raycart_GetGestureDragVector, "(*)"},
    {"GetGestureDragAngle", raycart_GetGestureDragAngle, "()f"},
    {"GetGesturePinchVector", raycart_GetGesturePinchVector, "(*)"},
    {"GetGesturePinchAngle", raycart_GetGesturePinchAngle, "()f"},
    {"UpdateCamera", raycart_UpdateCamera, "(*i)"},
    {"UpdateCameraPro", raycart_UpdateCameraPro, "(***f)"},
    {"SetShapesTexture", raycart_SetShapesTexture, "(**)"},
    {"GetShapesTexture", raycart_GetShapesTexture, "(*)"},
    {"GetShapesTextureRectangle", raycart_GetShapesTextureRectangle, "(*)"},
    {"DrawPixel", raycart_DrawPixel, "(ii*)"},
    {"DrawPixelV", raycart_DrawPixelV, "(**)"},
    {"DrawLine", raycart_DrawLine, "(iiii*)"},
    {"DrawLineV", raycart_DrawLineV, "(***)"},
    {"DrawLineEx", raycart_DrawLineEx, "(**f*)"},
    {"DrawLineStrip", raycart_DrawLineStrip, "(*i*)"},
    {"DrawLineBezier", raycart_DrawLineBezier, "(**f*)"},
    {"DrawCircle", raycart_DrawCircle, "(iif*)"},
    {"DrawCircleSector", raycart_DrawCircleSector, "(*fffi*)"},
    {"DrawCircleSectorLines", raycart_DrawCircleSectorLines, "(*fffi*)"},
    {"DrawCircleGradient", raycart_DrawCircleGradient, "(iif**)"},
    {"DrawCircleV", raycart_DrawCircleV, "(*f*)"},
    {"DrawCircleLines", raycart_DrawCircleLines, "(iif*)"},
    {"DrawCircleLinesV", raycart_DrawCircleLinesV, "(*f*)"},
    {"DrawEllipse", raycart_DrawEllipse, "(iiff*)"},
    {"DrawEllipseLines", raycart_DrawEllipseLines, "(iiff*)"},
    {"DrawRing", raycart_DrawRing, "(*ffffi*)"},
    {"DrawRingLines", raycart_DrawRingLines, "(*ffffi*)"},
    {"DrawRectangle", raycart_DrawRectangle, "(iiii*)"},
    {"DrawRectangleV", raycart_DrawRectangleV, "(***)"},
    {"DrawRectangleRec", raycart_DrawRectangleRec, "(**)"},
    {"DrawRectanglePro", raycart_DrawRectanglePro, "(**f*)"},
    {"DrawRectangleGradientV", raycart_DrawRectangleGradientV, "(iiii**)"},
    {"DrawRectangleGradientH", raycart_DrawRectangleGradientH, "(iiii**)"},
    {"DrawRectangleGradientEx", raycart_DrawRectangleGradientEx, "(*****)"},
    {"DrawRectangleLines", raycart_DrawRectangleLines, "(iiii*)"},
    {"DrawRectangleLinesEx", raycart_DrawRectangleLinesEx, "(*f*)"},
    {"DrawRectangleRounded", raycart_DrawRectangleRounded, "(*fi*)"},
    {"DrawRectangleRoundedLines", raycart_DrawRectangleRoundedLines, "(*fi*)"},
    {"DrawRectangleRoundedLinesEx", raycart_DrawRectangleRoundedLinesEx, "(*fif*)"},
    {"DrawTriangle", raycart_DrawTriangle, "(****)"},
    {"DrawTriangleLines", raycart_DrawTriangleLines, "(****)"},
    {"DrawTriangleFan", raycart_DrawTriangleFan, "(*i*)"},
    {"DrawTriangleStrip", raycart_DrawTriangleStrip, "(*i*)"},
    {"DrawPoly", raycart_DrawPoly, "(*iff*)"},
    {"DrawPolyLines", raycart_DrawPolyLines, "(*iff*)"},
    {"DrawPolyLinesEx", raycart_DrawPolyLinesEx, "(*ifff*)"},
    {"DrawSplineLinear", raycart_DrawSplineLinear, "(*if*)"},
    {"DrawSplineBasis", raycart_DrawSplineBasis, "(*if*)"},
    {"DrawSplineCatmullRom", raycart_DrawSplineCatmullRom, "(*if*)"},
    {"DrawSplineBezierQuadratic", raycart_DrawSplineBezierQuadratic, "(*if*)"},
    {"DrawSplineBezierCubic", raycart_DrawSplineBezierCubic, "(*if*)"},
    {"DrawSplineSegmentLinear", raycart_DrawSplineSegmentLinear, "(**f*)"},
    {"DrawSplineSegmentBasis", raycart_DrawSplineSegmentBasis, "(****f*)"},
    {"DrawSplineSegmentCatmullRom", raycart_DrawSplineSegmentCatmullRom, "(****f*)"},
    {"DrawSplineSegmentBezierQuadratic", raycart_DrawSplineSegmentBezierQuadratic, "(***f*)"},
    {"DrawSplineSegmentBezierCubic", raycart_DrawSplineSegmentBezierCubic, "(****f*)"},
    {"GetSplinePointLinear", raycart_GetSplinePointLinear, "(***f)"},
    {"GetSplinePointBasis", raycart_GetSplinePointBasis, "(*****f)"},
    {"GetSplinePointCatmullRom", raycart_GetSplinePointCatmullRom, "(*****f)"},
    {"GetSplinePointBezierQuad", raycart_GetSplinePointBezierQuad, "(****f)"},
    {"GetSplinePointBezierCubic", raycart_GetSplinePointBezierCubic, "(*****f)"},
    {"CheckCollisionRecs", raycart_CheckCollisionRecs, "(**)i"},
    {"CheckCollisionCircles", raycart_CheckCollisionCircles, "(*f*f)i"},
    {"CheckCollisionCircleRec", raycart_CheckCollisionCircleRec, "(*f*)i"},
    {"CheckCollisionCircleLine", raycart_CheckCollisionCircleLine, "(*f**)i"},
    {"CheckCollisionPointRec", raycart_CheckCollisionPointRec, "(**)i"},
    {"CheckCollisionPointCircle", raycart_CheckCollisionPointCircle, "(**f)i"},
    {"CheckCollisionPointTriangle", raycart_CheckCollisionPointTriangle, "(****)i"},
    {"CheckCollisionPointLine", raycart_CheckCollisionPointLine, "(***i)i"},
    {"CheckCollisionPointPoly", raycart_CheckCollisionPointPoly, "(**i)i"},
    {"CheckCollisionLines", raycart_CheckCollisionLines, "(*****)i"},
    {"GetCollisionRec", raycart_GetCollisionRec, "(***)"},
    {"LoadImage", raycart_LoadImage, "(*$)"},
    {"LoadImageRaw", raycart_LoadImageRaw, "(*$iiii)"},
    {"LoadImageAnim", raycart_LoadImageAnim, "(*$*)"},
    {"LoadImageAnimFromMemory", raycart_LoadImageAnimFromMemory, "(*$$i*)"},
    {"LoadImageFromMemory", raycart_LoadImageFromMemory, "(*$$i)"},
    {"LoadImageFromTexture", raycart_LoadImageFromTexture, "(**)"},
    {"LoadImageFromScreen", raycart_LoadImageFromScreen, "(*)"},
    {"IsImageValid", raycart_IsImageValid, "(*)i"},
    {"UnloadImage", raycart_UnloadImage, "(*)"},
    {"ExportImage", raycart_ExportImage, "(*$)i"},
    {"ExportImageToMemory", raycart_ExportImageToMemory, "(*$*)$"},
    {"ExportImageAsCode", raycart_ExportImageAsCode, "(*$)i"},
    {"GenImageColor", raycart_GenImageColor, "(*ii*)"},
    {"GenImageGradientLinear", raycart_GenImageGradientLinear, "(*iii**)"},
    {"GenImageGradientRadial", raycart_GenImageGradientRadial, "(*iif**)"},
    {"GenImageGradientSquare", raycart_GenImageGradientSquare, "(*iif**)"},
    {"GenImageChecked", raycart_GenImageChecked, "(*iiii**)"},
    {"GenImageWhiteNoise", raycart_GenImageWhiteNoise, "(*iif)"},
    {"GenImagePerlinNoise", raycart_GenImagePerlinNoise, "(*iiiif)"},
    {"GenImageCellular", raycart_GenImageCellular, "(*iii)"},
    {"GenImageText", raycart_GenImageText, "(*ii$)"},
    {"ImageCopy", raycart_ImageCopy, "(**)"},
    {"ImageFromImage", raycart_ImageFromImage, "(***)"},
    {"ImageFromChannel", raycart_ImageFromChannel, "(**i)"},
    {"ImageText", raycart_ImageText, "(*$i*)"},
    {"ImageTextEx", raycart_ImageTextEx, "(**$ff*)"},
    {"ImageFormat", raycart_ImageFormat, "(*i)"},
    {"ImageToPOT", raycart_ImageToPOT, "(**)"},
    {"ImageCrop", raycart_ImageCrop, "(**)"},
    {"ImageAlphaCrop", raycart_ImageAlphaCrop, "(*f)"},
    {"ImageAlphaClear", raycart_ImageAlphaClear, "(**f)"},
    {"ImageAlphaMask", raycart_ImageAlphaMask, "(**)"},
    {"ImageAlphaPremultiply", raycart_ImageAlphaPremultiply, "(*)"},
    {"ImageBlurGaussian", raycart_ImageBlurGaussian, "(*i)"},
    {"ImageKernelConvolution", raycart_ImageKernelConvolution, "(**i)"},
    {"ImageResize", raycart_ImageResize, "(*ii)"},
    {"ImageResizeNN", raycart_ImageResizeNN, "(*ii)"},
    {"ImageResizeCanvas", raycart_ImageResizeCanvas, "(*iiii*)"},
    {"ImageMipmaps", raycart_ImageMipmaps, "(*)"},
    {"ImageDither", raycart_ImageDither, "(*iiii)"},
    {"ImageFlipVertical", raycart_ImageFlipVertical, "(*)"},
    {"ImageFlipHorizontal", raycart_ImageFlipHorizontal, "(*)"},
    {"ImageRotate", raycart_ImageRotate, "(*i)"},
    {"ImageRotateCW", raycart_ImageRotateCW, "(*)"},
    {"ImageRotateCCW", raycart_ImageRotateCCW, "(*)"},
    {"ImageColorTint", raycart_ImageColorTint, "(**)"},
    {"ImageColorInvert", raycart_ImageColorInvert, "(*)"},
    {"ImageColorGrayscale", raycart_ImageColorGrayscale, "(*)"},
    {"ImageColorContrast", raycart_ImageColorContrast, "(*f)"},
    {"ImageColorBrightness", raycart_ImageColorBrightness, "(*i)"},
    {"ImageColorReplace", raycart_ImageColorReplace, "(***)"},
    {"LoadImageColors", raycart_LoadImageColors, "(**)"},
    {"LoadImagePalette", raycart_LoadImagePalette, "(**i*)"},
    {"UnloadImageColors", raycart_UnloadImageColors, "(*)"},
    {"UnloadImagePalette", raycart_UnloadImagePalette, "(*)"},
    {"GetImageAlphaBorder", raycart_GetImageAlphaBorder, "(**f)"},
    {"GetImageColor", raycart_GetImageColor, "(**ii)"},
    {"ImageClearBackground", raycart_ImageClearBackground, "(**)"},
    {"ImageDrawPixel", raycart_ImageDrawPixel, "(*ii*)"},
    {"ImageDrawPixelV", raycart_ImageDrawPixelV, "(***)"},
    {"ImageDrawLine", raycart_ImageDrawLine, "(*iiii*)"},
    {"ImageDrawLineV", raycart_ImageDrawLineV, "(****)"},
    {"ImageDrawLineEx", raycart_ImageDrawLineEx, "(***i*)"},
    {"ImageDrawCircle", raycart_ImageDrawCircle, "(*iii*)"},
    {"ImageDrawCircleV", raycart_ImageDrawCircleV, "(**i*)"},
    {"ImageDrawCircleLines", raycart_ImageDrawCircleLines, "(*iii*)"},
    {"ImageDrawCircleLinesV", raycart_ImageDrawCircleLinesV, "(**i*)"},
    {"ImageDrawRectangle", raycart_ImageDrawRectangle, "(*iiii*)"},
    {"ImageDrawRectangleV", raycart_ImageDrawRectangleV, "(****)"},
    {"ImageDrawRectangleRec", raycart_ImageDrawRectangleRec, "(***)"},
    {"ImageDrawRectangleLines", raycart_ImageDrawRectangleLines, "(**i*)"},
    {"ImageDrawTriangle", raycart_ImageDrawTriangle, "(*****)"},
    {"ImageDrawTriangleEx", raycart_ImageDrawTriangleEx, "(*******)"},
    {"ImageDrawTriangleLines", raycart_ImageDrawTriangleLines, "(*****)"},
    {"ImageDrawTriangleFan", raycart_ImageDrawTriangleFan, "(**i*)"},
    {"ImageDrawTriangleStrip", raycart_ImageDrawTriangleStrip, "(**i*)"},
    {"ImageDraw", raycart_ImageDraw, "(*****)"},
    {"ImageDrawText", raycart_ImageDrawText, "(*$iii*)"},
    {"ImageDrawTextEx", raycart_ImageDrawTextEx, "(**$*ff*)"},
    {"LoadTexture", raycart_LoadTexture, "(*$)"},
    {"LoadTextureFromImage", raycart_LoadTextureFromImage, "(**)"},
    {"LoadTextureCubemap", raycart_LoadTextureCubemap, "(**i)"},
    {"LoadRenderTexture", raycart_LoadRenderTexture, "(*ii)"},
    {"IsTextureValid", raycart_IsTextureValid, "(*)i"},
    {"UnloadTexture", raycart_UnloadTexture, "(*)"},
    {"IsRenderTextureValid", raycart_IsRenderTextureValid, "(*)i"},
    {"UnloadRenderTexture", raycart_UnloadRenderTexture, "(*)"},
    {"UpdateTexture", raycart_UpdateTexture, "(**)"},
    {"UpdateTextureRec", raycart_UpdateTextureRec, "(***)"},
    {"GenTextureMipmaps", raycart_GenTextureMipmaps, "(*)"},
    {"SetTextureFilter", raycart_SetTextureFilter, "(*i)"},
    {"SetTextureWrap", raycart_SetTextureWrap, "(*i)"},
    {"DrawTexture", raycart_DrawTexture, "(*ii*)"},
    {"DrawTextureV", raycart_DrawTextureV, "(***)"},
    {"DrawTextureEx", raycart_DrawTextureEx, "(**ff*)"},
    {"DrawTextureRec", raycart_DrawTextureRec, "(****)"},
    {"DrawTexturePro", raycart_DrawTexturePro, "(****f*)"},
    {"DrawTextureNPatch", raycart_DrawTextureNPatch, "(****f*)"},
    {"ColorIsEqual", raycart_ColorIsEqual, "(**)i"},
    {"Fade", raycart_Fade, "(**f)"},
    {"ColorToInt", raycart_ColorToInt, "(*)i"},
    {"ColorNormalize", raycart_ColorNormalize, "(**)"},
    {"ColorFromNormalized", raycart_ColorFromNormalized, "(**)"},
    {"ColorToHSV", raycart_ColorToHSV, "(**)"},
    {"ColorFromHSV", raycart_ColorFromHSV, "(*fff)"},
    {"ColorTint", raycart_ColorTint, "(***)"},
    {"ColorBrightness", raycart_ColorBrightness, "(**f)"},
    {"ColorContrast", raycart_ColorContrast, "(**f)"},
    {"ColorAlpha", raycart_ColorAlpha, "(**f)"},
    {"ColorAlphaBlend", raycart_ColorAlphaBlend, "(****)"},
    {"ColorLerp", raycart_ColorLerp, "(***f)"},
    {"GetColor", raycart_GetColor, "(*i)"},
    {"GetPixelColor", raycart_GetPixelColor, "(**i)"},
    {"SetPixelColor", raycart_SetPixelColor, "(**i)"},
    {"GetPixelDataSize", raycart_GetPixelDataSize, "(iii)i"},
    {"GetFontDefault", raycart_GetFontDefault, "(*)"},
    {"LoadFont", raycart_LoadFont, "(*$)"},
    {"LoadFontEx", raycart_LoadFontEx, "(*$i*i)"},
    {"LoadFontFromImage", raycart_LoadFontFromImage, "(***i)"},
    {"LoadFontFromMemory", raycart_LoadFontFromMemory, "(*$$ii*i)"},
    {"IsFontValid", raycart_IsFontValid, "(*)i"},
    {"GenImageFontAtlas", raycart_GenImageFontAtlas, "(***iiii)"},
    {"UnloadFontData", raycart_UnloadFontData, "(*i)"},
    {"UnloadFont", raycart_UnloadFont, "(*)"},
    {"ExportFontAsCode", raycart_ExportFontAsCode, "(*$)i"},
    {"DrawFPS", raycart_DrawFPS, "(ii)"},
    {"DrawText", raycart_DrawText, "($iii*)"},
    {"DrawTextEx", raycart_DrawTextEx, "(*$*ff*)"},
    {"DrawTextPro", raycart_DrawTextPro, "(*$**fff*)"},
    {"DrawTextCodepoint", raycart_DrawTextCodepoint, "(*i*f*)"},
    {"DrawTextCodepoints", raycart_DrawTextCodepoints, "(**i*ff*)"},
    {"SetTextLineSpacing", raycart_SetTextLineSpacing, "(i)"},
    {"MeasureText", raycart_MeasureText, "($i)i"},
    {"MeasureTextEx", raycart_MeasureTextEx, "(**$ff)"},
    {"GetGlyphIndex", raycart_GetGlyphIndex, "(*i)i"},
    {"GetGlyphInfo", raycart_GetGlyphInfo, "(**i)"},
    {"GetGlyphAtlasRec", raycart_GetGlyphAtlasRec, "(**i)"},
    {"LoadUTF8", raycart_LoadUTF8, "(*i)$"},
    {"UnloadUTF8", raycart_UnloadUTF8, "($)"},
    {"LoadCodepoints", raycart_LoadCodepoints, "($*)*"},
    {"UnloadCodepoints", raycart_UnloadCodepoints, "(*)"},
    {"GetCodepointCount", raycart_GetCodepointCount, "($)i"},
    {"GetCodepoint", raycart_GetCodepoint, "($*)i"},
    {"GetCodepointNext", raycart_GetCodepointNext, "($*)i"},
    {"GetCodepointPrevious", raycart_GetCodepointPrevious, "($*)i"},
    {"CodepointToUTF8", raycart_CodepointToUTF8, "(i*)$"},
    {"TextCopy", raycart_TextCopy, "($$)i"},
    {"TextIsEqual", raycart_TextIsEqual, "($$)i"},
    {"TextLength", raycart_TextLength, "($)i"},
    {"TextSubtext", raycart_TextSubtext, "($ii)$"},
    {"TextReplace", raycart_TextReplace, "($$$)$"},
    {"TextInsert", raycart_TextInsert, "($$i)$"},
    {"TextJoin", raycart_TextJoin, "($i$)$"},
    {"TextSplit", raycart_TextSplit, "($i*)$"},
    {"TextAppend", raycart_TextAppend, "($$*)"},
    {"TextFindIndex", raycart_TextFindIndex, "($$)i"},
    {"TextToUpper", raycart_TextToUpper, "($)$"},
    {"TextToLower", raycart_TextToLower, "($)$"},
    {"TextToPascal", raycart_TextToPascal, "($)$"},
    {"TextToSnake", raycart_TextToSnake, "($)$"},
    {"TextToCamel", raycart_TextToCamel, "($)$"},
    {"TextToInteger", raycart_TextToInteger, "($)i"},
    {"TextToFloat", raycart_TextToFloat, "($)f"},
    {"DrawLine3D", raycart_DrawLine3D, "(***)"},
    {"DrawPoint3D", raycart_DrawPoint3D, "(**)"},
    {"DrawCircle3D", raycart_DrawCircle3D, "(*f*f*)"},
    {"DrawTriangle3D", raycart_DrawTriangle3D, "(****)"},
    {"DrawTriangleStrip3D", raycart_DrawTriangleStrip3D, "(*i*)"},
    {"DrawCube", raycart_DrawCube, "(*fff*)"},
    {"DrawCubeV", raycart_DrawCubeV, "(***)"},
    {"DrawCubeWires", raycart_DrawCubeWires, "(*fff*)"},
    {"DrawCubeWiresV", raycart_DrawCubeWiresV, "(***)"},
    {"DrawSphere", raycart_DrawSphere, "(*f*)"},
    {"DrawSphereEx", raycart_DrawSphereEx, "(*fii*)"},
    {"DrawSphereWires", raycart_DrawSphereWires, "(*fii*)"},
    {"DrawCylinder", raycart_DrawCylinder, "(*fffi*)"},
    {"DrawCylinderEx", raycart_DrawCylinderEx, "(**ffi*)"},
    {"DrawCylinderWires", raycart_DrawCylinderWires, "(*fffi*)"},
    {"DrawCylinderWiresEx", raycart_DrawCylinderWiresEx, "(**ffi*)"},
    {"DrawCapsule", raycart_DrawCapsule, "(**fii*)"},
    {"DrawCapsuleWires", raycart_DrawCapsuleWires, "(**fii*)"},
    {"DrawPlane", raycart_DrawPlane, "(***)"},
    {"DrawRay", raycart_DrawRay, "(**)"},
    {"DrawGrid", raycart_DrawGrid, "(if)"},
    {"LoadModel", raycart_LoadModel, "(*$)"},
    {"LoadModelFromMesh", raycart_LoadModelFromMesh, "(**)"},
    {"IsModelValid", raycart_IsModelValid, "(*)i"},
    {"UnloadModel", raycart_UnloadModel, "(*)"},
    {"GetModelBoundingBox", raycart_GetModelBoundingBox, "(**)"},
    {"DrawModel", raycart_DrawModel, "(**f*)"},
    {"DrawModelEx", raycart_DrawModelEx, "(***f**)"},
    {"DrawModelWires", raycart_DrawModelWires, "(**f*)"},
    {"DrawModelWiresEx", raycart_DrawModelWiresEx, "(***f**)"},
    {"DrawModelPoints", raycart_DrawModelPoints, "(**f*)"},
    {"DrawModelPointsEx", raycart_DrawModelPointsEx, "(***f**)"},
    {"DrawBoundingBox", raycart_DrawBoundingBox, "(**)"},
    {"DrawBillboard", raycart_DrawBillboard, "(***f*)"},
    {"DrawBillboardRec", raycart_DrawBillboardRec, "(******)"},
    {"DrawBillboardPro", raycart_DrawBillboardPro, "(*******f*)"},
    {"UploadMesh", raycart_UploadMesh, "(*i)"},
    {"UpdateMeshBuffer", raycart_UpdateMeshBuffer, "(*i*ii)"},
    {"UnloadMesh", raycart_UnloadMesh, "(*)"},
    {"DrawMesh", raycart_DrawMesh, "(***)"},
    {"DrawMeshInstanced", raycart_DrawMeshInstanced, "(***i)"},
    {"GetMeshBoundingBox", raycart_GetMeshBoundingBox, "(**)"},
    {"GenMeshTangents", raycart_GenMeshTangents, "(*)"},
    {"ExportMesh", raycart_ExportMesh, "(*$)i"},
    {"ExportMeshAsCode", raycart_ExportMeshAsCode, "(*$)i"},
    {"GenMeshPoly", raycart_GenMeshPoly, "(*if)"},
    {"GenMeshPlane", raycart_GenMeshPlane, "(*ffii)"},
    {"GenMeshCube", raycart_GenMeshCube, "(*fff)"},
    {"GenMeshSphere", raycart_GenMeshSphere, "(*fii)"},
    {"GenMeshHemiSphere", raycart_GenMeshHemiSphere, "(*fii)"},
    {"GenMeshCylinder", raycart_GenMeshCylinder, "(*ffi)"},
    {"GenMeshCone", raycart_GenMeshCone, "(*ffi)"},
    {"GenMeshTorus", raycart_GenMeshTorus, "(*ffii)"},
    {"GenMeshKnot", raycart_GenMeshKnot, "(*ffii)"},
    {"GenMeshHeightmap", raycart_GenMeshHeightmap, "(***)"},
    {"GenMeshCubicmap", raycart_GenMeshCubicmap, "(***)"},
    {"LoadMaterials", raycart_LoadMaterials, "(*$*)"},
    {"LoadMaterialDefault", raycart_LoadMaterialDefault, "(*)"},
    {"IsMaterialValid", raycart_IsMaterialValid, "(*)i"},
    {"UnloadMaterial", raycart_UnloadMaterial, "(*)"},
    {"SetMaterialTexture", raycart_SetMaterialTexture, "(*i*)"},
    {"SetModelMeshMaterial", raycart_SetModelMeshMaterial, "(*ii)"},
    {"LoadModelAnimations", raycart_LoadModelAnimations, "(*$*)"},
    {"UpdateModelAnimation", raycart_UpdateModelAnimation, "(**i)"},
    {"UpdateModelAnimationBones", raycart_UpdateModelAnimationBones, "(**i)"},
    {"UnloadModelAnimation", raycart_UnloadModelAnimation, "(*)"},
    {"UnloadModelAnimations", raycart_UnloadModelAnimations, "(*i)"},
    {"IsModelAnimationValid", raycart_IsModelAnimationValid, "(**)i"},
    {"CheckCollisionSpheres", raycart_CheckCollisionSpheres, "(*f*f)i"},
    {"CheckCollisionBoxes", raycart_CheckCollisionBoxes, "(**)i"},
    {"CheckCollisionBoxSphere", raycart_CheckCollisionBoxSphere, "(**f)i"},
    {"GetRayCollisionSphere", raycart_GetRayCollisionSphere, "(***f)"},
    {"GetRayCollisionBox", raycart_GetRayCollisionBox, "(***)"},
    {"GetRayCollisionMesh", raycart_GetRayCollisionMesh, "(****)"},
    {"GetRayCollisionTriangle", raycart_GetRayCollisionTriangle, "(*****)"},
    {"GetRayCollisionQuad", raycart_GetRayCollisionQuad, "(******)"},
    {"InitAudioDevice", raycart_InitAudioDevice, "()"},
    {"CloseAudioDevice", raycart_CloseAudioDevice, "()"},
    {"IsAudioDeviceReady", raycart_IsAudioDeviceReady, "()i"},
    {"SetMasterVolume", raycart_SetMasterVolume, "(f)"},
    {"GetMasterVolume", raycart_GetMasterVolume, "()f"},
    {"LoadWave", raycart_LoadWave, "(*$)"},
    {"LoadWaveFromMemory", raycart_LoadWaveFromMemory, "(*$$i)"},
    {"IsWaveValid", raycart_IsWaveValid, "(*)i"},
    {"LoadSound", raycart_LoadSound, "(*$)"},
    {"LoadSoundFromWave", raycart_LoadSoundFromWave, "(**)"},
    {"LoadSoundAlias", raycart_LoadSoundAlias, "(**)"},
    {"IsSoundValid", raycart_IsSoundValid, "(*)i"},
    {"UpdateSound", raycart_UpdateSound, "(**i)"},
    {"UnloadWave", raycart_UnloadWave, "(*)"},
    {"UnloadSound", raycart_UnloadSound, "(*)"},
    {"UnloadSoundAlias", raycart_UnloadSoundAlias, "(*)"},
    {"ExportWave", raycart_ExportWave, "(*$)i"},
    {"ExportWaveAsCode", raycart_ExportWaveAsCode, "(*$)i"},
    {"PlaySound", raycart_PlaySound, "(*)"},
    {"StopSound", raycart_StopSound, "(*)"},
    {"PauseSound", raycart_PauseSound, "(*)"},
    {"ResumeSound", raycart_ResumeSound, "(*)"},
    {"IsSoundPlaying", raycart_IsSoundPlaying, "(*)i"},
    {"SetSoundVolume", raycart_SetSoundVolume, "(*f)"},
    {"SetSoundPitch", raycart_SetSoundPitch, "(*f)"},
    {"SetSoundPan", raycart_SetSoundPan, "(*f)"},
    {"WaveCopy", raycart_WaveCopy, "(**)"},
    {"WaveCrop", raycart_WaveCrop, "(*ii)"},
    {"WaveFormat", raycart_WaveFormat, "(*iii)"},
    {"LoadWaveSamples", raycart_LoadWaveSamples, "(*)*"},
    {"UnloadWaveSamples", raycart_UnloadWaveSamples, "(*)"},
    {"LoadMusicStream", raycart_LoadMusicStream, "(*$)"},
    {"LoadMusicStreamFromMemory", raycart_LoadMusicStreamFromMemory, "(*$$i)"},
    {"IsMusicValid", raycart_IsMusicValid, "(*)i"},
    {"UnloadMusicStream", raycart_UnloadMusicStream, "(*)"},
    {"PlayMusicStream", raycart_PlayMusicStream, "(*)"},
    {"IsMusicStreamPlaying", raycart_IsMusicStreamPlaying, "(*)i"},
    {"UpdateMusicStream", raycart_UpdateMusicStream, "(*)"},
    {"StopMusicStream", raycart_StopMusicStream, "(*)"},
    {"PauseMusicStream", raycart_PauseMusicStream, "(*)"},
    {"ResumeMusicStream", raycart_ResumeMusicStream, "(*)"},
    {"SeekMusicStream", raycart_SeekMusicStream, "(*f)"},
    {"SetMusicVolume", raycart_SetMusicVolume, "(*f)"},
    {"SetMusicPitch", raycart_SetMusicPitch, "(*f)"},
    {"SetMusicPan", raycart_SetMusicPan, "(*f)"},
    {"GetMusicTimeLength", raycart_GetMusicTimeLength, "(*)f"},
    {"GetMusicTimePlayed", raycart_GetMusicTimePlayed, "(*)f"},
    {"LoadAudioStream", raycart_LoadAudioStream, "(*iii)"},
    {"IsAudioStreamValid", raycart_IsAudioStreamValid, "(*)i"},
    {"UnloadAudioStream", raycart_UnloadAudioStream, "(*)"},
    {"UpdateAudioStream", raycart_UpdateAudioStream, "(**i)"},
    {"IsAudioStreamProcessed", raycart_IsAudioStreamProcessed, "(*)i"},
    {"PlayAudioStream", raycart_PlayAudioStream, "(*)"},
    {"PauseAudioStream", raycart_PauseAudioStream, "(*)"},
    {"ResumeAudioStream", raycart_ResumeAudioStream, "(*)"},
    {"IsAudioStreamPlaying", raycart_IsAudioStreamPlaying, "(*)i"},
    {"StopAudioStream", raycart_StopAudioStream, "(*)"},
    {"SetAudioStreamVolume", raycart_SetAudioStreamVolume, "(*f)"},
    {"SetAudioStreamPitch", raycart_SetAudioStreamPitch, "(*f)"},
    {"SetAudioStreamPan", raycart_SetAudioStreamPan, "(*f)"},
    {"SetAudioStreamBufferSizeDefault", raycart_SetAudioStreamBufferSizeDefault, "(i)"}
};

bool CartInit(char *wasmBytes, int wasmSize) {
  char error_buf[128];
  RuntimeInitArgs init_args = {0};

  // TODO: both wasm hosts should probably set this, but web does not, so I will just ignore it
  // char *argv[] = {"raycart", "CARTNAME"};
  // wasi_set_args(2, argv);

  void *heap_buf = malloc(16 * 1024 * 1024);
  if (!heap_buf) {
    TraceLog(LOG_FATAL, "Failed to allocate heap buffer");
    return false;
  }

  init_args.mem_alloc_type = Alloc_With_Pool;
  init_args.mem_alloc_option.pool.heap_buf = heap_buf;
  init_args.mem_alloc_option.pool.heap_size = 16 * 1024 * 1024;
  init_args.max_thread_num = 1;

  if (!wasm_runtime_full_init(&init_args)) {
    TraceLog(LOG_FATAL, "init: runtime");
    free(heap_buf);
    return false;
  }

  if (raycart_native_symbols_count) {
    if (!wasm_runtime_register_natives("raycart", raycart_native_symbols, raycart_native_symbols_count)) {
      TraceLog(LOG_FATAL, "raycart: register");
      return false;
    }
  } else {
    TraceLog(LOG_WARNING, "raycart: no symbols");
  }

  int wasi_count = sizeof(wasi_native_symbols) / sizeof(NativeSymbol);
  if (wasi_count) {
    if (!wasm_runtime_register_natives("wasi_snapshot_preview1", wasi_native_symbols, wasi_count)) {
      TraceLog(LOG_FATAL, "wasi: register");
      return false;
    }
    // printf("Added %d WASI functions\n", wasi_count);
  } else {
    TraceLog(LOG_WARNING, "wasi: no symbols");
  }

  // Load WASM module
  module = wasm_runtime_load(wasmBytes, wasmSize, error_buf, sizeof(error_buf));
  if (!module) {
    TraceLog(LOG_FATAL, error_buf);
    wasm_runtime_destroy();
    return false;
  }

  // Instantiate the module
  module_inst = wasm_runtime_instantiate(module, stack_size, heap_size, error_buf, sizeof(error_buf));
  if (!module_inst) {
    TraceLog(LOG_FATAL, error_buf);
    wasm_runtime_unload(module);
    wasm_runtime_destroy();
    return false;
  }

  // Create execution environment
  exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
  if (!exec_env) {
    TraceLog(LOG_FATAL, wasm_runtime_get_exception(module_inst));
    wasm_runtime_deinstantiate(module_inst);
    wasm_runtime_unload(module);
    wasm_runtime_destroy();
    return false;
  }

  cart_callback_update = wasm_runtime_lookup_function(module_inst, "CartUpdate");
  cart_callback_close = wasm_runtime_lookup_function(module_inst, "CartClose");

  // Look for main function
  wasm_function_inst_t start_func = wasm_runtime_lookup_function(module_inst, "_start");
  if (start_func) {
    if (!wasm_runtime_call_wasm(exec_env, start_func, 0, NULL)) {
      // not fatal, but warn about it
      TraceLog(LOG_WARNING, wasm_runtime_get_exception(module_inst));
    }
  }

  wasm_function_inst_t main_func = wasm_runtime_lookup_function(module_inst, "main");
  if (main_func) {
    if (!wasm_runtime_call_wasm(exec_env, main_func, 0, NULL)) {
      // not fatal, but warn about it
      TraceLog(LOG_WARNING, wasm_runtime_get_exception(module_inst));
    }
  }

  wasm_function_inst_t cart_callback_load = wasm_runtime_lookup_function(module_inst, "CartInit");
  if (cart_callback_load != NULL) {
    if (!wasm_runtime_call_wasm(exec_env, cart_callback_load, 0, NULL)) {
      // not fatal, but warn about it
      TraceLog(LOG_WARNING, wasm_runtime_get_exception(module_inst));
    }
  }

  TraceLog(LOG_INFO, "Native wasm-host loaded.");

  return true;
}

bool CartUpdate() {
  BeginDrawing();

  if (cart_callback_update != NULL) {
    if (!wasm_runtime_call_wasm(exec_env, cart_callback_update, 0, NULL)) {
      // not fatal, but warn about it
      TraceLog(LOG_WARNING, wasm_runtime_get_exception(module_inst));
    }
  }

  EndDrawing();
  return true;
}

void CartClose() {
 if (cart_callback_close != NULL) {
    if (!wasm_runtime_call_wasm(exec_env, cart_callback_close, 0, NULL)) {
      // not fatal, but warn about it
      TraceLog(LOG_WARNING, wasm_runtime_get_exception(module_inst));
    }
  }
  // TODO: do I need to cleanup any WAMR stuff?
}

#endif
