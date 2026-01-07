#!/usr/bin/env node
import { readFileSync, writeFileSync } from 'fs'
import { fileURLToPath } from 'url'
import { dirname, join } from 'path'

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

const api = JSON.parse(readFileSync(join(__dirname, 'raylib_api.json'), 'utf8'))

// Type size mapping
const typeSizes = {
  char: 1,
  'unsigned char': 1,
  short: 2,
  'unsigned short': 2,
  int: 4,
  'unsigned int': 4,
  long: 4,
  'unsigned long': 4,
  float: 4,
  double: 8,
  'void *': 4 // pointer size in wasm32
}

// Calculate struct size
function getStructSize(structName) {
  const resolved = resolveAlias(structName)
  const struct = api.structs.find((s) => s.name === resolved)
  if (!struct) return 0

  let size = 0
  for (const field of struct.fields) {
    const fieldType = field.type.replace(/\s+/g, ' ').trim()
    if (typeSizes[fieldType]) {
      size += typeSizes[fieldType]
    } else if (fieldType.includes('*')) {
      size += 4 // pointer
    } else {
      // Nested struct
      size += getStructSize(fieldType)
    }
  }
  return size
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

// Get clean type name for helper function
function getTypeName(type) {
  const cleanType = type
    .replace(/\s+/g, ' ')
    .replace(/const\s+/, '')
    .replace(/\s*\*/, '')
    .trim()
  return resolveAlias(cleanType)
}

// Generate helper name for struct
function getHelperName(type) {
  const name = getTypeName(type)
  return name.charAt(0).toLowerCase() + name.slice(1)
}

// Collect all struct types used in the API
function getUsedStructs() {
  const used = new Set()

  for (const func of api.functions) {
    // Check return type
    if (isStruct(func.returnType)) {
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

// Functions to exclude from the API (cart has its own or doesn't make sense to expose)
const functionsToExclude = [
  'MemAlloc',
  'MemRealloc',
  'MemFree',
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

  // Functions with complex pointer returns + pointer params that WAMR can't handle
  'LoadModelAnimations',
  'LoadMaterials',
  'LoadImageColors',
  'LoadImagePalette'
]

// Generate function binding
function generateFunction(func) {
  const params = func.params || []
  const returnsStruct = isStruct(func.returnType) && !isPointer(func.returnType)

  // Build parameter list
  const jsParams = returnsStruct ? ['resultPtr'] : []
  jsParams.push(...params.map((p) => p.name))

  // Build function body
  const lines = []

  // Convert parameters from cart to host
  const hostParams = []
  const cleanupLines = []

  for (const param of params) {
    const hostName = `${param.name}_h`

    if (isString(param.type)) {
      lines.push(`            const ${hostName} = cartString(${param.name});`)
      hostParams.push(hostName)
      cleanupLines.push(`            Module._MemFree(${hostName});`)
    } else if (isStruct(param.type) && !isPointer(param.type)) {
      // Struct passed by value - copy from cart to host
      const helperName = `cart${getTypeName(param.type)}`
      lines.push(`            const ${hostName} = ${helperName}(${param.name});`)
      hostParams.push(hostName)
      cleanupLines.push(`            Module._MemFree(${hostName});`)
    } else if (isStruct(param.type) && isPointer(param.type)) {
      // Struct pointer - just pass the pointer
      hostParams.push(param.name)
    } else {
      // Primitive - pass directly
      hostParams.push(param.name)
    }
  }

  // Handle function call based on return type
  if (returnsStruct) {
    const size = getStructSize(func.returnType)
    lines.push(`            const result_h = Module._MemAlloc(${size});`)
    const callParams = hostParams.length > 0 ? `result_h, ${hostParams.join(', ')}` : 'result_h'
    lines.push(`            Module._${func.name}(${callParams});`)
    lines.push(...cleanupLines)
    lines.push(`            copyHostToCart(result_h, resultPtr, ${size});`)
    lines.push(`            Module._MemFree(result_h);`)
  } else if (func.returnType !== 'void') {
    lines.push(`            const result = Module._${func.name}(${hostParams.join(', ')});`)
    lines.push(...cleanupLines)
    lines.push(`            return result;`)
  } else {
    lines.push(`            Module._${func.name}(${hostParams.join(', ')});`)
    lines.push(...cleanupLines)
  }

  return `        ${func.name}(${jsParams.join(', ')}) {\n${lines.join('\n')}\n        }`
}

// Generate the complete host_web.c file
function generateHostWeb() {
  const usedStructs = getUsedStructs()

  // Generate struct helpers
  const structHelpers = usedStructs
    .map((structName) => {
      const size = getStructSize(structName)
      const helperName = getHelperName(structName)
      return `    const cart${structName} = ptr => copyFromCart(ptr, ${size});`
    })
    .join('\n')

  const hostStructHelpers = usedStructs
    .map((structName) => {
      const size = getStructSize(structName)
      const helperName = getHelperName(structName)
      return `    const host${structName} = ptr => copyToCart(ptr, ${size});`
    })
    .join('\n')

  // Use the shared exclusion list defined at module level

  const functionBindings = api.functions
    .filter((f) => !functionsToExclude.includes(f.name))
    .map(generateFunction)
    .join(',\n\n')

  return `#ifdef EMSCRIPTEN

#include "raylib.h"
#include "emscripten.h"

EM_ASYNC_JS(bool, CartInit, (char *wasmBuffer, int bytesRead), {
    if (!wasmBuffer || !bytesRead) {
        console.error('wasm byte-length is 0.');
        return false;
    }

    const wasmBytes = Module.HEAPU8.slice(wasmBuffer, wasmBuffer+Number(bytesRead));
    if (!wasmBytes.length) {
        console.error('wasm byte-length is not what was expted.');
        return false;
    }

    if (!Module.wasi_snapshot_preview1) {
        console.error('wasi_snapshot_preview1 has not been provided.');
        return false;
    }

    // utils for copying memory
    // this allocates, so make sure to MemFree it
    // TODO "scratch space" mem would be more efficient & might be easier to work with
    const copyFromCart = (ptr, size) => {
        const cartMemory = new Uint8Array(Module.cart.memory.buffer);
        const hostPtr = Module._MemAlloc(size);
        Module.HEAPU8.set(cartMemory.subarray(ptr, ptr + size), hostPtr);
        return hostPtr;
    };

    const copyToCart = (hostPtr, size) => {
        const hostMemory = Module.HEAPU8;
        const cartPtr = Module.cart.malloc(size);
        const cartMemory = new Uint8Array(Module.cart.memory.buffer);
        cartMemory.set(hostMemory.subarray(hostPtr, hostPtr + size), cartPtr);
        return cartPtr;
    };

    const copyHostToCart = (hostPtr, cartPtr, size) => {
        const cartMemory = new Uint8Array(Module.cart.memory.buffer);
        const hostMemory = Module.HEAPU8;
        cartMemory.set(hostMemory.subarray(hostPtr, hostPtr + size), cartPtr);
    };

    const cartStringLen = ptr => {
        const cartMemory = new Uint8Array(Module.cart.memory.buffer, ptr, 1024);
        let strlen = 0;
        while (cartMemory[strlen] !== 0 && strlen < cartMemory.length) {
            strlen++;
        }
        return strlen;
    };

    const hostStringLen = ptr => {
        const hostMemory = Module.HEAPU8;
        let strlen = 0;
        while (hostMemory[ptr + strlen] !== 0 && strlen < 1024) {
            strlen++;
        }
        return strlen;
    };

    // copy from cart to host
    const cartString = (ptr) => copyFromCart(ptr, cartStringLen(ptr) + 1);
${structHelpers}

    // copy from host to cart
    const hostString = (ptr) => copyToCart(ptr, hostStringLen(ptr) + 1);
${hostStructHelpers}

    // Generated raylib bindings
    const raycart = {
${functionBindings}
    };

    const { instance } = await WebAssembly.instantiate(wasmBytes, {raycart, wasi_snapshot_preview1: Module.wasi_snapshot_preview1});
    const cart = Module.cart = instance.exports;
    Module.wasi_snapshot_preview1.start(cart);

    console.log('CART', cart);

    cart?.CartInit && cart.CartInit();

    return true;
});


EM_JS(bool, CartUpdate, (), {
    Module._BeginDrawing();
    Module?.cart?.CartUpdate && Module.cart.CartUpdate();
    Module._EndDrawing();
    return true;
});

EM_JS(void, CartClose, (), {
    Module?.cart?.CartClose && Module.cart.CartClose();
});

#endif
`
}

// Calculate excluded functions
const totalFunctions = api.functions.length
const excludedFunctions = api.functions.filter((f) => functionsToExclude.includes(f.name))

// Generate web exports list
const exports = new Set()

// Functions that are used internally, but not exposed to cart
exports.add('_main')
exports.add('_MemAlloc')
exports.add('_MemFree')
exports.add('_BeginDrawing')
exports.add('_EndDrawing')

// All non-excluded raylib functions
for (const func of api.functions) {
  if (!functionsToExclude.includes(func.name)) {
    exports.add(`_${func.name}`)
  }
}

// Sort and write exports
const sortedExports = Array.from(exports).sort()
const exportsContent = sortedExports.join('\n') + '\n'

// Write the generated files
const output = generateHostWeb()
writeFileSync(join(__dirname, '../host/host_web.c'), output)
writeFileSync(join(__dirname, '../host/web_exports.txt'), exportsContent)

console.log('Generated host/host_web.c')
console.log(`  Struct helpers:    ${getUsedStructs().length}`)
console.log(`  Functions:         ${totalFunctions - excludedFunctions.length}/${totalFunctions}`)
if (excludedFunctions.length > 0) {
  console.log(`  Excluded:          ${excludedFunctions.map((f) => f.name).join(', ')}`)
}

console.log('\nGenerated host/web_exports.txt')
console.log(`  Exported symbols:  ${sortedExports.length}`)
console.log('')
