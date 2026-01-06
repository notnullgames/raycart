#!/usr/bin/env node
import { readFileSync, writeFileSync } from 'fs'
import { fileURLToPath } from 'url'
import { dirname, join } from 'path'

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

const api = JSON.parse(readFileSync(join(__dirname, 'raylib_api.json'), 'utf8'))

// Map C types to QuickJS converter function names
const typeConverters = {
  bool: { to: 'bool_to_js', from: 'bool_from_js' },
  int: { to: 'i32_to_js', from: 'i32_from_js' },
  'unsigned int': { to: 'u32_to_js', from: 'u32_from_js' },
  float: { to: 'f32_to_js', from: 'f32_from_js' },
  double: { to: 'f32_to_js', from: 'f32_from_js' }, // JS has only float64
  'unsigned char': { to: 'u32_to_js', from: 'u32_from_js' },
  char: { to: 'i32_to_js', from: 'i32_from_js' },
  short: { to: 'i32_to_js', from: 'i32_from_js' },
  'unsigned short': { to: 'u32_to_js', from: 'u32_from_js' },
  long: { to: 'i32_to_js', from: 'i32_from_js' },
  'unsigned long': { to: 'u32_to_js', from: 'u32_from_js' },
  'long long': { to: 'u64_to_js', from: 'u64_from_js' },
  'unsigned long long': { to: 'u64_to_js', from: 'u64_from_js' }
}

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

// Get converter function names for a type
function getConverter(type) {
  const cleanType = type
    .replace(/\s+/g, ' ')
    .replace(/const\s+/, '')
    .replace(/\s*\*/, '')
    .trim()

  if (isString(type)) {
    return { to: 'string_to_js', from: 'string_from_js' }
  }

  if (isStruct(type) && !isPointer(type)) {
    const structName = getTypeName(type)
    return {
      to: `${structName.toLowerCase()}_to_js`,
      from: `${structName.toLowerCase()}_from_js`
    }
  }

  // For pointers to structs or other types, we'll need special handling
  if (isPointer(type)) {
    return null // Skip for now
  }

  return typeConverters[cleanType] || null
}

// Generate struct to_js converter
function generateStructToJs(struct) {
  const lines = []
  lines.push(`static JSValue ${struct.name.toLowerCase()}_to_js(${struct.name} value) {`)
  lines.push(`  JSValue obj = JS_NewObject(ctx);`)

  for (const field of struct.fields) {
    const converter = getConverter(field.type)
    if (converter && converter.to) {
      lines.push(`  JS_SetPropertyStr(ctx, obj, "${field.name}", ${converter.to}(value.${field.name}));`)
    } else if (isPointer(field.type)) {
      // Skip pointers for now
      lines.push(`  // TODO: handle pointer field ${field.name}`)
    }
  }

  lines.push(`  return obj;`)
  lines.push(`}`)
  return lines.join('\n')
}

// Generate struct from_js converter
function generateStructFromJs(struct) {
  const lines = []
  lines.push(`static ${struct.name} ${struct.name.toLowerCase()}_from_js(JSValue obj) {`)
  lines.push(`  ${struct.name} result = {0};`)

  for (const field of struct.fields) {
    const converter = getConverter(field.type)
    if (converter && converter.from) {
      lines.push(`  JSValue ${field.name}_val = JS_GetPropertyStr(ctx, obj, "${field.name}");`)
      lines.push(`  if (!JS_IsUndefined(${field.name}_val)) {`)
      lines.push(`    result.${field.name} = ${converter.from}(${field.name}_val);`)
      lines.push(`  }`)
      lines.push(`  JS_FreeValue(ctx, ${field.name}_val);`)
    } else if (isPointer(field.type)) {
      // Skip pointers for now
      lines.push(`  // TODO: handle pointer field ${field.name}`)
    }
  }

  lines.push(`  return result;`)
  lines.push(`}`)
  return lines.join('\n')
}

// Same blacklist as other generators
const functionsToExclude = ['MemAlloc', 'MemRealloc', 'MemFree', 'SetTraceLogCallback', 'SetLoadFileDataCallback', 'SetSaveFileDataCallback', 'SetLoadFileTextCallback', 'SetSaveFileTextCallback', 'AttachAudioStreamProcessor', 'DetachAudioStreamProcessor', 'AttachAudioMixedProcessor', 'DetachAudioMixedProcessor', 'SetAudioStreamCallback']

// Check if function should be excluded
function shouldExcludeFunction(func) {
  if (functionsToExclude.includes(func.name)) {
    return true
  }

  // Exclude functions with callback parameters
  const callbackTypes = ['AudioCallback', 'TraceLogCallback', 'LoadFileDataCallback', 'SaveFileDataCallback', 'LoadFileTextCallback', 'SaveFileTextCallback']
  const params = func.params || []

  for (const param of params) {
    if (callbackTypes.includes(param.type)) {
      return true
    }
    // Exclude variadic functions
    if (param.type === '...') {
      return true
    }
    // Exclude functions with any pointer parameters (except strings)
    // This includes struct pointers like Image*, void*, etc.
    if (isPointer(param.type) && !isString(param.type)) {
      return true
    }
  }

  // Exclude functions returning pointers (except strings)
  if (isPointer(func.returnType) && !isString(func.returnType)) {
    return true
  }

  return false
}

// Generate JS wrapper function
function generateJsFunction(func) {
  const params = func.params || []
  const lines = []

  lines.push(`static JSValue js_${func.name}(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {`)

  // Convert parameters
  const callParams = []
  for (let i = 0; i < params.length; i++) {
    const param = params[i]
    const converter = getConverter(param.type)

    if (converter && converter.from) {
      callParams.push(`${converter.from}(argv[${i}])`)
    } else {
      callParams.push(`/* TODO: argv[${i}] */`)
    }
  }

  // Make the call and return
  if (func.returnType === 'void') {
    lines.push(`  ${func.name}(${callParams.join(', ')});`)
    lines.push(`  return JS_UNDEFINED;`)
  } else {
    const returnConverter = getConverter(func.returnType)
    if (returnConverter && returnConverter.to) {
      lines.push(`  return ${returnConverter.to}(${func.name}(${callParams.join(', ')}));`)
    } else {
      lines.push(`  // TODO: handle return type ${func.returnType}`)
      lines.push(`  ${func.name}(${callParams.join(', ')});`)
      lines.push(`  return JS_UNDEFINED;`)
    }
  }

  lines.push(`}`)
  return lines.join('\n')
}

// Generate expose_things_to_js function
function generateExposeFunction(filteredFunctions) {
  const lines = []
  lines.push(`void expose_things_to_js() {`)

  // Expose color constants
  const colorDefines = api.defines.filter((d) => d.type === 'COLOR')
  lines.push(`  // Color constants`)
  for (const define of colorDefines) {
    lines.push(`  JS_SetPropertyStr(ctx, global, "${define.name}", color_to_js(${define.name}));`)
  }

  // Expose enum values
  lines.push(``)
  lines.push(`  // Enum values`)
  for (const enumDef of api.enums) {
    for (const value of enumDef.values) {
      lines.push(`  JS_SetPropertyStr(ctx, global, "${value.name}", i32_to_js(${value.name}));`)
    }
  }

  // Expose functions
  lines.push(``)
  lines.push(`  // Functions`)
  for (const func of filteredFunctions) {
    const paramCount = func.params?.length || 0
    lines.push(`  JS_SetPropertyStr(ctx, global, "${func.name}", JS_NewCFunction(ctx, js_${func.name}, "${func.name}", ${paramCount}));`)
  }

  lines.push(`}`)
  return lines.join('\n')
}

// Collect structs that need converters
function getUsedStructs() {
  const used = new Set()

  for (const func of api.functions) {
    if (shouldExcludeFunction(func)) continue

    // Check return type
    if (isStruct(func.returnType) && !isPointer(func.returnType)) {
      used.add(getTypeName(func.returnType))
    }

    // Check parameters
    for (const param of func.params || []) {
      if (isStruct(param.type) && !isPointer(param.type)) {
        used.add(getTypeName(param.type))
      }
    }
  }

  return Array.from(used)
}

// Generate the complete bindings file
const usedStructs = getUsedStructs()
const filteredFunctions = api.functions.filter((f) => !shouldExcludeFunction(f))

// Generate struct converters
const structConverters = []
for (const structName of usedStructs) {
  const struct = api.structs.find((s) => s.name === structName)
  if (struct) {
    structConverters.push(generateStructToJs(struct))
    structConverters.push('')
    structConverters.push(generateStructFromJs(struct))
  }
}

// Generate function wrappers
const functionWrappers = filteredFunctions.map(generateJsFunction)

// Generate expose function
const exposeFunction = generateExposeFunction(filteredFunctions)

// Build complete file
const output = `// This file is auto-generated from raylib_api.json
// Do not edit manually - run: npm run codegen:cartjs
//
// NOTE: This file is meant to be included in main.c, not compiled separately.
// main.c should already have included raycart.h and quickjs.h before including this file.

// Type converters - primitives
static JSValue bool_to_js(bool value) {
  return JS_NewBool(ctx, value);
}

static JSValue i32_to_js(int32_t value) {
  return JS_NewInt32(ctx, value);
}

static JSValue u32_to_js(uint32_t value) {
  return JS_NewUint32(ctx, value);
}

static JSValue f32_to_js(float value) {
  return JS_NewFloat64(ctx, (double)value);
}

static JSValue u64_to_js(uint64_t value) {
  if (value <= 0x1FFFFFFFFFFFFF) {
    return JS_NewFloat64(ctx, (double)value);
  }
  return JS_NewBigInt64(ctx, (int64_t)value);
}

static JSValue string_to_js(const char* value) {
  return JS_NewString(ctx, value);
}

static bool bool_from_js(JSValue val) {
  return JS_ToBool(ctx, val);
}

static int32_t i32_from_js(JSValue val) {
  int32_t result = 0;
  JS_ToInt32(ctx, &result, val);
  return result;
}

static uint32_t u32_from_js(JSValue val) {
  uint32_t result = 0;
  JS_ToUint32(ctx, &result, val);
  return result;
}

static float f32_from_js(JSValue val) {
  double result = 0.0;
  JS_ToFloat64(ctx, &result, val);
  return (float)result;
}

static uint64_t u64_from_js(JSValue val) {
  uint64_t result = 0;

  if (JS_IsNumber(val)) {
    double d;
    if (JS_ToFloat64(ctx, &d, val) == 0) {
      result = (uint64_t)d;
    }
  } else if (JS_IsBigInt(ctx, val)) {
    int64_t signed_result = 0;
    if (JS_ToBigInt64(ctx, &signed_result, val) == 0) {
      result = (uint64_t)signed_result;
    }
  }

  return result;
}

static const char* string_from_js(JSValue val) {
  return JS_ToCString(ctx, val);
}

// Type converters - structs (forward declarations)
${usedStructs
  .map((s) => {
    const lower = s.toLowerCase()
    return `static JSValue ${lower}_to_js(${s} value);\nstatic ${s} ${lower}_from_js(JSValue obj);`
  })
  .join('\n')}

// Type converters - structs (implementations)
${structConverters.join('\n\n')}

// Function wrappers
${functionWrappers.join('\n\n')}

// Expose everything to JavaScript
${exposeFunction}
`

// Calculate excluded functions
const totalFunctions = api.functions.length
const excludedFunctions = api.functions.filter((f) => shouldExcludeFunction(f))

// Write the file
writeFileSync(join(__dirname, '../carts/c/js/raycart_bindings.c'), output)
console.log(`\nGenerated carts/c/js/raycart_bindings.c`)
console.log(`  Struct converters: ${usedStructs.length}`)
console.log(`  Color constants:   ${api.defines.filter((d) => d.type === 'COLOR').length}`)
console.log(`  Enum values:       ${api.enums.reduce((sum, e) => sum + e.values.length, 0)}`)
console.log(`  Functions:         ${filteredFunctions.length}/${totalFunctions}`)
if (excludedFunctions.length > 0) {
  console.log(`  Excluded:          ${excludedFunctions.map((f) => f.name).join(', ')}`)
}
