#ifndef EMSCRIPTEN

#include "raylib.h"
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

static void raycart_InitWindow(wasm_exec_env_t exec_env, int width, int height, char* title) {
    InitWindow(width, height, title);
}

static void raycart_ClearBackground(wasm_exec_env_t exec_env, Color* color) {
    ClearBackground(*color);
}

static void raycart_DrawText(wasm_exec_env_t exec_env, char * text, int posX, int posY, int fontSize, Color* color) {
    DrawText(text, posX, posY, fontSize, *color);
}

#define raycart_native_symbols_count 3
static NativeSymbol raycart_native_symbols[raycart_native_symbols_count] = {
    {"InitWindow", raycart_InitWindow, "(ii$)"},
    {"ClearBackground", raycart_ClearBackground, "(*)"},
    {"DrawText", raycart_DrawText, "($iii*)"},
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