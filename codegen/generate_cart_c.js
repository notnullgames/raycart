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

// Same blacklist as host_web
const functionsToExclude = [
  'MemAlloc',
  'MemRealloc',
  'MemFree',
  'SetTraceLogCallback',
  'SetLoadFileDataCallback',
  'SetSaveFileDataCallback',
  'SetLoadFileTextCallback',
  'SetSaveFileTextCallback',
  'AttachAudioStreamProcessor',
  'DetachAudioStreamProcessor',
  'AttachAudioMixedProcessor',
  'DetachAudioMixedProcessor',
  'SetAudioStreamCallback' // Uses AudioCallback type
]

// Check if function uses callback types or variadic args
function shouldExcludeFunction(func) {
  if (functionsToExclude.includes(func.name)) {
    return true
  }

  // Exclude functions with callback parameters (not in C types)
  const callbackTypes = ['AudioCallback', 'TraceLogCallback', 'LoadFileDataCallback', 'SaveFileDataCallback', 'LoadFileTextCallback', 'SaveFileTextCallback']
  const params = func.params || []

  for (const param of params) {
    if (callbackTypes.includes(param.type)) {
      return true
    }
    // Also exclude variadic functions for now (complex to handle in WASM)
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
// Raylib Function Imports (from host)
//----------------------------------------------------------------------------------
${functionImports}
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
