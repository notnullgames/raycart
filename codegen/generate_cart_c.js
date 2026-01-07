#!/usr/bin/env node
import { readFileSync, writeFileSync } from 'fs'
import { fileURLToPath } from 'url'
import { dirname, join } from 'path'

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

const api = JSON.parse(readFileSync(join(__dirname, 'raylib_api.json'), 'utf8'))

// Generate struct typedef
function generateStruct(struct) {
  const fields = struct.fields
    .map((f) => {
      // Handle array types like "int[4]" -> "int name[4]"
      let type = f.type
      let name = f.name
      const arrayMatch = type.match(/^(.+?)(\[.+\])$/)
      if (arrayMatch) {
        type = arrayMatch[1]
        name = f.name + arrayMatch[2]
      }

      const comment = f.description ? ' // ' + f.description : ''
      return `    ${type} ${name};${comment}`
    })
    .join('\n')

  return `// ${struct.description}
typedef struct ${struct.name} {
${fields}
} ${struct.name};`
}

// Generate enum typedef
function generateEnum(enumDef) {
  const values = enumDef.values
    .map((v) => {
      const comment = v.description ? ` // ${v.description}` : ''
      return `    ${v.name} = ${v.value},${comment}`
    })
    .join('\n')

  return `// ${enumDef.description}
typedef enum {
${values}
} ${enumDef.name};`
}

// Generate define
function generateDefine(define) {
  // Skip macro-style defines and internal defines
  if (define.type === 'MACRO' || define.type === 'GUARD' || define.type === 'UNKNOWN') {
    return null
  }

  // Handle color defines specially
  if (define.type === 'COLOR') {
    // Convert CLITERAL(Color){ ... } to (Color){ ... }
    const value = define.value.replace('CLITERAL(Color)', '(Color)')
    const comment = define.description ? `   // ${define.description}` : ''
    return `#define ${define.name.padEnd(20)} ${value}${comment}`
  }

  // Handle other defines
  if (define.value !== undefined && define.value !== '') {
    const comment = define.description ? `   // ${define.description}` : ''
    return `#define ${define.name.padEnd(20)} ${define.value}${comment}`
  }

  return null
}

// Generate type alias
function generateAlias(alias) {
  return `typedef ${alias.type} ${alias.name};`
}

// Functions to exclude from the API (cart has its own or doesn't make sense to expose)
const functionsToExclude = [
  'MemAlloc',
  'MemRealloc',
  'MemFree',
  'SetTraceLogCallback',
  'SetLoadFileDataCallback',  // Set in host/main.c to use PhysFS
  'SetSaveFileDataCallback',  // Set in host/main.c to use PhysFS
  'SetLoadFileTextCallback',  // Set in host/main.c to use PhysFS
  'SetSaveFileTextCallback',  // Set in host/main.c to use PhysFS
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

// Variadic functions that will be implemented cart-side
const variadicFunctions = ['TraceLog', 'TextFormat']

// Check if function uses callback types or variadic args
function shouldExcludeFunction(func) {
  if (functionsToExclude.includes(func.name)) {
    return true
  }

  // Exclude variadic functions (will be implemented cart-side)
  if (variadicFunctions.includes(func.name)) {
    return true
  }

  // Note: We don't exclude callback functions for C carts since C supports function pointers natively
  // Only exclude specific callbacks that are in the functionsToExclude list

  // Exclude any remaining variadic functions
  const params = func.params || []
  for (const param of params) {
    if (param.type === '...') {
      return true
    }
  }

  return false
}

// Generate function declaration
function generateFunctionDeclaration(func) {
  const params = func.params || []

  // Format parameters
  const paramList =
    params.length > 0
      ? params
          .map((p) => {
            // Handle variadic parameters
            if (p.type === '...') {
              return '...'
            }
            return `${p.type} ${p.name}`
          })
          .join(', ')
      : 'void'

  return `RC_IMPORT("${func.name}")
${func.returnType} ${func.name}(${paramList});`
}

// Generate structs with aliases inserted at the right places
const aliasMap = {}
for (const alias of api.aliases || []) {
  if (!aliasMap[alias.type]) {
    aliasMap[alias.type] = []
  }
  aliasMap[alias.type].push(alias)
}

const structsWithAliases = []
for (const struct of api.structs) {
  structsWithAliases.push(generateStruct(struct))

  // Add aliases right after their base type
  if (aliasMap[struct.name]) {
    for (const alias of aliasMap[struct.name]) {
      structsWithAliases.push(`\n// ${alias.description}\n${generateAlias(alias)}`)
    }
  }
}

const structs = structsWithAliases.join('\n\n')
const enums = api.enums.map(generateEnum).join('\n\n')
const defines = api.defines
  .map(generateDefine)
  .filter((d) => d !== null)
  .join('\n')
const filteredFunctions = api.functions.filter((f) => !shouldExcludeFunction(f))
const functionImports = filteredFunctions.map(generateFunctionDeclaration).join('\n\n')

// Generate complete header
const header = `#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RC_EXPORT(n) __attribute__((export_name(n)))
#define RC_IMPORT(n) __attribute__((import_module("raycart"), import_name(n)))

//----------------------------------------------------------------------------------
// Opaque Structs (forward declarations for internal types)
//----------------------------------------------------------------------------------
typedef struct rAudioBuffer rAudioBuffer;
typedef struct rAudioProcessor rAudioProcessor;

//----------------------------------------------------------------------------------
// Structures Definition (with type aliases)
//----------------------------------------------------------------------------------
${structs}

//----------------------------------------------------------------------------------
// Enumerators Definition
//----------------------------------------------------------------------------------
${enums}

//----------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------
${defines}

//----------------------------------------------------------------------------------
// Cart Memory Management (cart has its own malloc/free)
//----------------------------------------------------------------------------------
RC_EXPORT("malloc")
void* MemAlloc(size_t size) {
  return malloc(size);
}

RC_EXPORT("free")
void MemFree(void *ptr) {
  free(ptr);
}

//----------------------------------------------------------------------------------
// Cart Callbacks (exported to host)
//----------------------------------------------------------------------------------
RC_EXPORT("CartInit")
void CartInit();

RC_EXPORT("CartUpdate")
void CartUpdate();

RC_EXPORT("CartClose")
void CartClose();

// Currently needed to defer to CartInit
int main() {
    return 0;
}

//----------------------------------------------------------------------------------
// Variadic Function Implementations (cart-side)
//----------------------------------------------------------------------------------

// TextFormat - Text formatting with variables (sprintf style)
static char textFormatBuffer[1024];
static inline const char* TextFormat(const char* text, ...) {
  va_list args;
  va_start(args, text);
  vsnprintf(textFormatBuffer, sizeof(textFormatBuffer), text, args);
  va_end(args);
  return textFormatBuffer;
}

// TraceLog - Show trace log messages (LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR...)
static inline void TraceLog(int logLevel, const char* text, ...) {
  const char* logLevelStr = "UNKNOWN";
  switch (logLevel) {
    case 0: logLevelStr = "ALL"; break;
    case 1: logLevelStr = "TRACE"; break;
    case 2: logLevelStr = "DEBUG"; break;
    case 3: logLevelStr = "INFO"; break;
    case 4: logLevelStr = "WARNING"; break;
    case 5: logLevelStr = "ERROR"; break;
    case 6: logLevelStr = "FATAL"; break;
    case 7: logLevelStr = "NONE"; break;
  }

  fprintf(stderr, "[%s] ", logLevelStr);
  va_list args;
  va_start(args, text);
  vfprintf(stderr, text, args);
  va_end(args);
  fprintf(stderr, "\\n");
}

//----------------------------------------------------------------------------------
// Raylib Function Imports (from host)
//----------------------------------------------------------------------------------
${functionImports}

//----------------------------------------------------------------------------------
// Manual function wrappers for complex pointer patterns
//----------------------------------------------------------------------------------

// LoadModelAnimations returns pointer to array, manually wrapped
RC_IMPORT("LoadModelAnimations")
void __raycart_LoadModelAnimations(uint32_t *result, const char *fileName, int *animCount);

static inline ModelAnimation *LoadModelAnimations(const char *fileName, int *animCount) {
    uint32_t result;
    __raycart_LoadModelAnimations(&result, fileName, animCount);
    return (ModelAnimation *)(uintptr_t)result;
}

// LoadMaterials returns pointer to array, manually wrapped
RC_IMPORT("LoadMaterials")
void __raycart_LoadMaterials(uint32_t *result, const char *fileName, int *materialCount);

static inline Material *LoadMaterials(const char *fileName, int *materialCount) {
    uint32_t result;
    __raycart_LoadMaterials(&result, fileName, materialCount);
    return (Material *)(uintptr_t)result;
}

// LoadImageColors returns pointer to array, manually wrapped
RC_IMPORT("LoadImageColors")
void __raycart_LoadImageColors(uint32_t *result, Image image);

static inline Color *LoadImageColors(Image image) {
    uint32_t result;
    __raycart_LoadImageColors(&result, image);
    return (Color *)(uintptr_t)result;
}

// LoadImagePalette returns pointer to array, manually wrapped
RC_IMPORT("LoadImagePalette")
void __raycart_LoadImagePalette(uint32_t *result, Image image, int maxPaletteSize, int *colorCount);

static inline Color *LoadImagePalette(Image image, int maxPaletteSize, int *colorCount) {
    uint32_t result;
    __raycart_LoadImagePalette(&result, image, maxPaletteSize, colorCount);
    return (Color *)(uintptr_t)result;
}
`

// Calculate excluded functions with reasons
const totalFunctions = api.functions.length
const excludedFunctions = api.functions.filter((f) => shouldExcludeFunction(f))

// Write the file
writeFileSync(join(__dirname, '../carts/c/raycart.h'), header)
console.log(`Generated carts/c/raycart.h`)
console.log(`  Structs:           ${api.structs.length}`)
console.log(`  Type aliases:      ${api.aliases?.length || 0}`)
console.log(`  Enums:             ${api.enums.length}`)
console.log(`  Defines:           ${api.defines.filter((d) => generateDefine(d) !== null).length}`)
console.log(`  Functions:         ${filteredFunctions.length}/${totalFunctions}`)
if (excludedFunctions.length > 0) {
  console.log(`  Excluded:          ${excludedFunctions.map((f) => f.name).join(', ')}`)
}

console.log('')
