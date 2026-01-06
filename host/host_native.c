#ifndef EMSCRIPTEN

#include <stddef.h>
#include "raylib.h"
#include "wasm_export.h"
#include "wamr_wasi_physfs.h"

static uint32_t stack_size = 1024 * 1024 * 10; // 10 MB
static uint32_t heap_size = 1024 * 1024 * 10;  // 10 MB
static wasm_module_t module = NULL;
static wasm_module_inst_t module_inst = NULL;
static wasm_exec_env_t exec_env = NULL;

static wasm_function_inst_t cart_callback_init = NULL;
static wasm_function_inst_t cart_callback_update = NULL;
static wasm_function_inst_t cart_callback_close = NULL;

bool CartInit(char *wasmBuffer, int bytesRead) {
    // TODO: add stuff from https://github.com/notnullgames/null0/blob/main/host/src/cart_wamr.c
    InitWindow(800, 450, "raylib [core] example - basic window");
    SetTargetFPS(60);
    return true;
}

bool CartUpdate() {
    // TODO: call cart_callback_update()
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first native window!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
    return true;
}

void CartClose() {
    // TODO: call cart_callback_close()
}

#endif