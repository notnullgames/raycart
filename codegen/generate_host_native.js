#!/usr/bin/env node
import { readFileSync, writeFileSync } from 'fs'
import { fileURLToPath } from 'url'
import { dirname, join } from 'path'

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

const api = JSON.parse(readFileSync(join(__dirname, 'raylib_api.json'), 'utf8'))

// Resolve type aliases
function resolveAlias(type) {
  const cleanType = type
    .replace(/\s+/g, ' ')
    .replace(/const\s+/, '')
    .replace(/\s*\*/, '')
    .trim()
  const alias = api.aliases?.find((a) => a.name === cleanType)
  return alias ? alias.type : cleanType
}

// Check if type is a struct
function isStruct(type) {
  const cleanType = type
    .replace(/\s+/g, ' ')
    .replace(/const\s+/, '')
    .replace(/\s*\*/, '')
    .trim()
  const resolved = resolveAlias(cleanType)
  return api.structs.some((s) => s.name === resolved)
}

// Check if type is a string
function isString(type) {
  return type.includes('char *') || type.includes('char*')
}

// Check if type is a pointer
function isPointer(type) {
  return type.includes('*')
}

// Get clean type name
function getTypeName(type) {
  const cleanType = type
    .replace(/\s+/g, ' ')
    .replace(/const\s+/, '')
    .replace(/\s*\*/, '')
    .trim()
  return resolveAlias(cleanType)
}

// Check if type is a primitive pointer (int*, float*, etc.) - these need manual conversion in WAMR
function isPrimitivePointer(type) {
  if (!isPointer(type)) return false
  const cleanType = type.replace(/const\s+/, '').replace(/\*/g, '').trim()
  const primitives = ['int', 'unsigned int', 'float', 'double', 'bool', 'char', 'unsigned char',
                      'short', 'unsigned short', 'long', 'unsigned long',
                      'int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t',
                      'int64_t', 'uint64_t']
  return primitives.includes(cleanType) || cleanType.match(/int\d+/) || cleanType.match(/uint\d+/)
}

// Functions to exclude from the API
const functionsToExclude = [
  'MemAlloc',
  'MemRealloc',
  'MemFree',
  'TraceLog', // Variadic
  'TextFormat', // Variadic
  'SetTraceLogCallback',
  'SetLoadFileDataCallback',
  'SetSaveFileDataCallback',
  'SetLoadFileTextCallback',
  'SetSaveFileTextCallback',
  'SetAudioStreamCallback',
  'AttachAudioStreamProcessor',
  'DetachAudioStreamProcessor',
  'AttachAudioMixedProcessor',
  'DetachAudioMixedProcessor',

  // Functions not available in this raylib version
  'GetClipboardImage',
  'FileRename',
  'FileRemove',
  'FileCopy',
  'FileMove',
  'FileTextReplace',
  'FileTextFindIndex',
  'ComputeSHA256',
  'DrawLineDashed',
  'DrawEllipseV',
  'DrawEllipseLinesV',
  'LoadTextLines',
  'UnloadTextLines',
  'TextRemoveSpaces',
  'GetTextBetween',
  'TextReplaceBetween',
  'GetKeyName', // Not in this raylib version
  'LoadFontData', // Parameter mismatch between JSON and actual raylib

  // Functions with complex pointer returns + pointer params that WAMR can't handle
  'LoadModelAnimations',
  'LoadMaterials',
  'LoadImageColors',
  'LoadImagePalette'
]

// Map C types to WAMR signature characters
function getWAMRTypeChar(type) {
  const cleanType = type.replace(/const\s+/, '').trim()

  if (isString(cleanType)) return '$' // string
  // Primitive pointers (int*, float*, etc.) are passed as int (WASM address) and converted manually
  if (isPrimitivePointer(cleanType)) return 'i'
  if (isPointer(cleanType)) return '*' // pointer (struct or other)

  // Primitive types
  if (cleanType === 'void') return ''
  if (cleanType === 'bool') return 'i' // bool is int in WAMR
  if (cleanType === 'int' || cleanType.includes('int32')) return 'i'
  if (cleanType === 'unsigned int' || cleanType.includes('uint32')) return 'i'
  if (cleanType === 'short' || cleanType === 'unsigned short') return 'i'
  if (cleanType === 'char' || cleanType === 'unsigned char') return 'i'
  if (cleanType.includes('int64')) return 'I'
  if (cleanType.includes('uint64')) return 'I'
  if (cleanType === 'float') return 'f'
  if (cleanType === 'double') return 'F'

  // If it's a struct by value, it's passed as pointer in WAMR
  if (isStruct(cleanType)) return '*'

  return 'i' // default to int
}

// Generate WAMR function signature
function generateWAMRSignature(func) {
  const params = func.params || []
  const returnsStruct = isStruct(func.returnType) && !isPointer(func.returnType)

  let paramSig = ''

  // If returning struct by value, first param is result pointer
  if (returnsStruct) {
    paramSig += '*'
  }

  // Add parameter types
  for (const param of params) {
    paramSig += getWAMRTypeChar(param.type)
  }

  // Return type
  const returnSig = returnsStruct ? '' : getWAMRTypeChar(func.returnType)

  return `(${paramSig})${returnSig}`
}

// Generate C parameter list for wrapper function
function generateParamList(func) {
  const params = func.params || []
  const returnsStruct = isStruct(func.returnType) && !isPointer(func.returnType)

  let paramList = ['wasm_exec_env_t exec_env']

  // If returning struct by value, first param is result pointer
  if (returnsStruct) {
    paramList.push(`${func.returnType}* __result`)
  }

  // Add function parameters
  for (const param of params) {
    let paramType = param.type.trim()

    // Strings are handled specially by WAMR
    if (isString(paramType)) {
      paramList.push(`${paramType} ${param.name}`)
    }
    // Primitive pointers are passed as WASM addresses (uint32_t)
    else if (isPrimitivePointer(paramType)) {
      paramList.push(`uint32_t ${param.name}`)
    }
    // Structs are passed by pointer in WAMR
    else if (isStruct(paramType) && !isPointer(paramType)) {
      const typeName = getTypeName(paramType)
      paramList.push(`${typeName}* ${param.name}`)
    } else {
      paramList.push(`${paramType} ${param.name}`)
    }
  }

  return paramList.join(', ')
}

// Generate function call with proper parameter handling
function generateFunctionCall(func) {
  const params = func.params || []
  const returnsStruct = isStruct(func.returnType) && !isPointer(func.returnType)
  const hasPrimitivePointers = params.some(p => isPrimitivePointer(p.type))

  let lines = []
  let callParams = []

  // If we have primitive pointers, we need module_inst for conversion
  if (hasPrimitivePointers) {
    lines.push('wasm_module_inst_t module_inst = wasm_runtime_get_module_inst(exec_env);')
  }

  // Add parameters, converting primitive pointers and dereferencing struct pointers
  for (const param of params) {
    // Strings are handled automatically by WAMR, don't convert
    if (isString(param.type)) {
      callParams.push(param.name)
    }
    else if (isPrimitivePointer(param.type)) {
      // Convert WASM address to native pointer
      const nativeName = `${param.name}_native`
      const cleanType = param.type.replace(/const\s+/, '').trim()
      lines.push(`    ${cleanType} ${nativeName} = (${cleanType})wasm_runtime_addr_app_to_native(module_inst, ${param.name});`)
      callParams.push(nativeName)
    } else if (isStruct(param.type) && !isPointer(param.type)) {
      // Dereference struct pointer
      callParams.push(`*${param.name}`)
    } else {
      callParams.push(param.name)
    }
  }

  const callParamsStr = callParams.join(', ')

  if (returnsStruct) {
    // Assign struct return value to result pointer
    lines.push(`*__result = ${func.name}(${callParamsStr});`)
    return lines.join('\n    ')
  } else if (func.returnType === 'void') {
    lines.push(`${func.name}(${callParamsStr});`)
    return lines.join('\n    ')
  } else {
    lines.push(`return ${func.name}(${callParamsStr});`)
    return lines.join('\n    ')
  }
}

// Generate wrapper function
function generateWrapperFunction(func) {
  const returnsStruct = isStruct(func.returnType) && !isPointer(func.returnType)
  const returnType = returnsStruct ? 'void' : func.returnType

  const paramList = generateParamList(func)
  const functionCall = generateFunctionCall(func)

  return `static ${returnType} raycart_${func.name}(${paramList}) {
    ${functionCall}
}`
}

// Generate the complete host_native.c file
function generateHostNative() {
  const filteredFunctions = api.functions.filter((f) => !functionsToExclude.includes(f.name))

  // Generate wrapper functions
  const wrapperFunctions = filteredFunctions.map(generateWrapperFunction).join('\n\n')

  // Generate native symbol array
  const nativeSymbols = filteredFunctions
    .map((func) => {
      const signature = generateWAMRSignature(func)
      return `    {"${func.name}", raycart_${func.name}, "${signature}"}`
    })
    .join(',\n')

  return `#ifndef EMSCRIPTEN

#include "raylib.h"
#include "raylib-physfs.h"
#include "wamr_wasi_physfs.h"
#include "wasm_export.h"
#include <stddef.h>
#include <string.h>

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
${wrapperFunctions}

// Manual wrappers for functions with complex pointer patterns
// TODO: LoadModelAnimations requires deep copying of nested pointer structures
// (name, bones, framePoses). For now, return NULL to prevent crashes.
static void raycart_LoadModelAnimations(wasm_exec_env_t exec_env, uint32_t result_ptr, const char *fileName, uint32_t animCount) {
    wasm_module_inst_t module_inst = wasm_runtime_get_module_inst(exec_env);
    int *animCount_native = (int *)wasm_runtime_addr_app_to_native(module_inst, animCount);

    // Set count to 0 and return NULL
    *animCount_native = 0;
    uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
    *result_wasm = 0;

    TraceLog(LOG_WARNING, "LoadModelAnimations not fully implemented (requires deep copy of nested pointers)");
}

static void raycart_LoadMaterials(wasm_exec_env_t exec_env, uint32_t result_ptr, const char *fileName, uint32_t materialCount) {
    wasm_module_inst_t module_inst = wasm_runtime_get_module_inst(exec_env);
    int *materialCount_native = (int *)wasm_runtime_addr_app_to_native(module_inst, materialCount);

    Material *result = LoadMaterials(fileName, materialCount_native);

    if (result && *materialCount_native > 0) {
        uint32_t wasm_size = sizeof(Material) * (*materialCount_native);
        uint32_t wasm_ptr = wasm_runtime_module_malloc(module_inst, wasm_size, NULL);

        if (wasm_ptr) {
            Material *wasm_array = (Material *)wasm_runtime_addr_app_to_native(module_inst, wasm_ptr);
            memcpy(wasm_array, result, wasm_size);

            uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
            *result_wasm = wasm_ptr;
        } else {
            uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
            *result_wasm = 0;
        }
    } else {
        uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
        *result_wasm = 0;
    }
}

static void raycart_LoadImageColors(wasm_exec_env_t exec_env, uint32_t result_ptr, Image *image) {
    wasm_module_inst_t module_inst = wasm_runtime_get_module_inst(exec_env);

    Color *result = LoadImageColors(*image);

    if (result) {
        // Image width * height gives us the number of colors
        uint32_t wasm_size = sizeof(Color) * image->width * image->height;
        uint32_t wasm_ptr = wasm_runtime_module_malloc(module_inst, wasm_size, NULL);

        if (wasm_ptr) {
            Color *wasm_array = (Color *)wasm_runtime_addr_app_to_native(module_inst, wasm_ptr);
            memcpy(wasm_array, result, wasm_size);

            uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
            *result_wasm = wasm_ptr;
        } else {
            uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
            *result_wasm = 0;
        }
        UnloadImageColors(result);
    } else {
        uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
        *result_wasm = 0;
    }
}

static void raycart_LoadImagePalette(wasm_exec_env_t exec_env, uint32_t result_ptr, Image *image, int maxPaletteSize, uint32_t colorCount) {
    wasm_module_inst_t module_inst = wasm_runtime_get_module_inst(exec_env);
    int *colorCount_native = (int *)wasm_runtime_addr_app_to_native(module_inst, colorCount);

    Color *result = LoadImagePalette(*image, maxPaletteSize, colorCount_native);

    if (result && *colorCount_native > 0) {
        uint32_t wasm_size = sizeof(Color) * (*colorCount_native);
        uint32_t wasm_ptr = wasm_runtime_module_malloc(module_inst, wasm_size, NULL);

        if (wasm_ptr) {
            Color *wasm_array = (Color *)wasm_runtime_addr_app_to_native(module_inst, wasm_ptr);
            memcpy(wasm_array, result, wasm_size);

            uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
            *result_wasm = wasm_ptr;
        } else {
            uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
            *result_wasm = 0;
        }
        UnloadImagePalette(result);
    } else {
        uint32_t *result_wasm = (uint32_t *)wasm_runtime_addr_app_to_native(module_inst, result_ptr);
        *result_wasm = 0;
    }
}

#define raycart_native_symbols_count ${filteredFunctions.length + 4}
static NativeSymbol raycart_native_symbols[raycart_native_symbols_count] = {
${nativeSymbols},
    {"LoadModelAnimations", raycart_LoadModelAnimations, "(i$i)"},
    {"LoadMaterials", raycart_LoadMaterials, "(i$i)"},
    {"LoadImageColors", raycart_LoadImageColors, "(i*)"},
    {"LoadImagePalette", raycart_LoadImagePalette, "(i*ii)"}
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
    // printf("Added %d WASI functions\\n", wasi_count);
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

  // this makes it match web
  SetTargetFPS(60);

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
`
}

// Calculate excluded functions
const totalFunctions = api.functions.length
const excludedFunctions = api.functions.filter((f) => functionsToExclude.includes(f.name))

// Write the generated file
const output = generateHostNative()
writeFileSync(join(__dirname, '../host/host_native.c'), output)

console.log('Generated host/host_native.c')
console.log(`  Functions:         ${totalFunctions - excludedFunctions.length}/${totalFunctions}`)
if (excludedFunctions.length > 0) {
  console.log(`  Excluded:          ${excludedFunctions.map((f) => f.name).join(', ')}`)
}
console.log('')
