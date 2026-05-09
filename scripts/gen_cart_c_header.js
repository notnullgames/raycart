#!/usr/bin/env node
// Generates include/raylib.h — wasm-side import header for raycart carts.
//
// ABI conventions (all enforced here so hosts are consistent):
//   - Structs passed as params: become const StructType* in the import; inline
//     wrapper takes by value, passes &local to the private import.
//   - Structs returned by value: import uses sret (void return, result ptr
//     prepended as first param); inline wrapper allocates on stack and returns.
//   - Strings / other pointers: pass through unchanged (i32 wasm ptr).
//   - Variadic (TraceLog): private import takes plain string; inline varg wrapper
//     does vsnprintf cart-side, calls private import.
//   - Variadic (TextFormat): implemented entirely cart-side, no host import.

import { readFile, writeFile, mkdir } from 'node:fs/promises'
import { fileURLToPath } from 'node:url'
import { dirname, join } from 'node:path'

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)
const ROOT = join(__dirname, '..')

const api = JSON.parse(await readFile(join(ROOT, 'raylib_api.json'), 'utf8'))

// ── Type helpers ──────────────────────────────────────────────────────────────

const aliasMap = {}
for (const a of api.aliases || []) aliasMap[a.name] = a.type

const structNames = new Set(api.structs.map((s) => s.name))
// Include aliases that resolve to structs
for (const a of api.aliases || []) {
  if (structNames.has(a.type)) structNames.add(a.name)
}

function resolveAlias(type) {
  const clean = type
    .replace(/const\s+/g, '')
    .replace(/\s*\*.*/, '')
    .trim()
  return aliasMap[clean] || clean
}

function isStructByValue(type) {
  if (type.includes('*')) return false
  return structNames.has(resolveAlias(type))
}

function isVoid(type) {
  return type.trim() === 'void'
}
function isString(type) {
  return /char\s*\*/.test(type)
}
function isVariadic(func) {
  return func.params?.some((p) => p.type === '...')
}

// ── Exclusions ────────────────────────────────────────────────────────────────

// Not exposed to carts — cart has its own libc malloc, or wasm can't handle
const EXCLUDE = new Set([
  'MemAlloc',
  'MemRealloc',
  'MemFree',
  // Callbacks that can't cross the wasm boundary
  'SetTraceLogCallback',
  'SetLoadFileDataCallback',
  'SetSaveFileDataCallback',
  'SetLoadFileTextCallback',
  'SetSaveFileTextCallback',
  'SetAudioStreamCallback',
  'AttachAudioStreamProcessor',
  'DetachAudioStreamProcessor',
  'AttachAudioMixedProcessor',
  'DetachAudioMixedProcessor'
])

// ── Code generators ───────────────────────────────────────────────────────────

function generateStruct(s) {
  const fields = s.fields
    .map((f) => {
      let type = f.type,
        name = f.name
      const arr = type.match(/^(.+?)(\[.+\])$/)
      if (arr) {
        type = arr[1]
        name += arr[2]
      }
      return `    ${type} ${name};`
    })
    .join('\n')
  return `typedef struct ${s.name} {\n${fields}\n} ${s.name};`
}

function generateEnum(e) {
  const vals = e.values.map((v) => `    ${v.name} = ${v.value},`).join('\n')
  return `typedef enum {\n${vals}\n} ${e.name};`
}

function generateDefine(d) {
  if (['MACRO', 'GUARD', 'UNKNOWN'].includes(d.type)) return null
  const val = d.type === 'COLOR' ? d.value.replace('CLITERAL(Color)', '(Color)') : d.value
  if (val === undefined || val === '') return null
  return `#define ${d.name.padEnd(24)} ${val}`
}

function generateAlias(a) {
  return `typedef ${a.type} ${a.name};`
}

// Build alias insertion map: base type name → aliases to insert after it
const aliasInsertMap = {}
for (const a of api.aliases || []) {
  if (!aliasInsertMap[a.type]) aliasInsertMap[a.type] = []
  aliasInsertMap[a.type].push(a)
}

// ── Function generation ───────────────────────────────────────────────────────

function importAttr(name) {
  return `__attribute__((import_module("raylib"), import_name("${name}")))`
}

// Generate one function's worth of declarations (import + optional wrapper).
// Returns null to skip the function entirely.
function generateFunction(func) {
  if (EXCLUDE.has(func.name)) return null

  const isVarg = isVariadic(func)

  // TextFormat: fully cart-side, no host import needed
  if (func.name === 'TextFormat') {
    return `\
// TextFormat — cart-side only, no host import (raylib uses circular 64-slot buffer)
static char __textfmt_bufs[64][1024];
static int  __textfmt_idx = 0;
static inline const char* TextFormat(const char* text, ...) {
    char* buf = __textfmt_bufs[__textfmt_idx++ & 63];
    va_list ap;
    va_start(ap, text);
    vsnprintf(buf, 1024, text, ap);
    va_end(ap);
    return buf;
}`
  }

  // Filter out the '...' sentinel from param list
  const params = (func.params || []).filter((p) => p.type !== '...')

  const sret = isStructByValue(func.returnType)
  const needsParamWrap = params.some((p) => isStructByValue(p.type))
  const needsWrap = sret || needsParamWrap || isVarg

  if (!needsWrap) {
    // Direct import — no wrapper needed
    const paramStr = params.length ? params.map((p) => `${p.type} ${p.name}`).join(', ') : 'void'
    return `${importAttr(func.name)}\n${func.returnType} ${func.name}(${paramStr});`
  }

  const lines = []

  // ── Private import declaration ──────────────────────────────────────────────
  // sret: void return, result ptr prepended
  // struct params: become const StructType*
  // varg (TraceLog): strip '...' — host receives pre-formatted string

  const importParams = []
  if (sret) importParams.push(`${func.returnType}* __result`)
  for (const p of params) {
    if (isStructByValue(p.type)) {
      importParams.push(`const ${p.type}* ${p.name}`)
    } else {
      importParams.push(`${p.type} ${p.name}`)
    }
  }

  const importRet = sret ? 'void' : func.returnType
  const importParamStr = importParams.length ? importParams.join(', ') : 'void'

  lines.push(`${importAttr(func.name)}`)
  lines.push(`${importRet} __raylib_${func.name}(${importParamStr});`)
  lines.push('')

  // ── Public inline wrapper ───────────────────────────────────────────────────

  if (isVarg) {
    // TraceLog(int logLevel, const char* text, ...) pattern:
    // format text cart-side, call private import with plain string
    const fixedParamStr = params.map((p) => `${p.type} ${p.name}`).join(', ')
    lines.push(`static inline ${func.returnType} ${func.name}(${fixedParamStr}, ...) {`)
    lines.push(`    char __buf[1024];`)
    lines.push(`    va_list __ap;`)

    // find the format-string param (last non-varg fixed param before '...')
    const fmtParam = params[params.length - 1]
    lines.push(`    va_start(__ap, ${fmtParam.name});`)
    lines.push(`    vsnprintf(__buf, sizeof(__buf), ${fmtParam.name}, __ap);`)
    lines.push(`    va_end(__ap);`)

    // build call args: replace format-string param with __buf
    const callArgs = []
    if (sret) callArgs.push('&__r')
    for (const p of params) {
      if (p.name === fmtParam.name) callArgs.push('__buf')
      else if (isStructByValue(p.type)) callArgs.push(`&${p.name}`)
      else callArgs.push(p.name)
    }

    if (sret) {
      lines.unshift(`    ${func.returnType} __r;`) // prepend — rare case
      lines.push(`    __raylib_${func.name}(${callArgs.join(', ')});`)
      lines.push(`    return __r;`)
    } else {
      lines.push(`    __raylib_${func.name}(${callArgs.join(', ')});`)
    }
    lines.push(`}`)
    return lines.join('\n')
  }

  // Regular wrapper (struct return and/or struct params)
  const wrapperParamStr = params.length ? params.map((p) => `${p.type} ${p.name}`).join(', ') : 'void'

  lines.push(`static inline ${func.returnType} ${func.name}(${wrapperParamStr}) {`)

  const callArgs = []
  if (sret) {
    lines.push(`    ${func.returnType} __r;`)
    callArgs.push('&__r')
  }
  for (const p of params) {
    callArgs.push(isStructByValue(p.type) ? `&${p.name}` : p.name)
  }

  if (sret) {
    lines.push(`    __raylib_${func.name}(${callArgs.join(', ')});`)
    lines.push(`    return __r;`)
  } else if (isVoid(func.returnType)) {
    lines.push(`    __raylib_${func.name}(${callArgs.join(', ')});`)
  } else {
    lines.push(`    return __raylib_${func.name}(${callArgs.join(', ')});`)
  }

  lines.push(`}`)
  return lines.join('\n')
}

// ── Assemble header ───────────────────────────────────────────────────────────

const structs = []
for (const s of api.structs) {
  structs.push(generateStruct(s))
  for (const a of aliasInsertMap[s.name] || []) {
    structs.push(generateAlias(a))
  }
}

const enums = api.enums.map(generateEnum)

const defines = api.defines.map(generateDefine).filter(Boolean)

const functions = api.functions.map(generateFunction).filter(Boolean)

const excluded = api.functions.filter((f) => generateFunction(f) === null).map((f) => f.name)

const header = `\
/**
 * raylib.h — raycart wasm-side import header (auto-generated, do not edit)
 *
 * Generated from raylib_api.json by codegen/gen_header.js
 *
 * ABI notes:
 *   - All struct params are passed as pointers (const Foo*) to the host.
 *     Public inline wrappers restore by-value call syntax.
 *   - Struct-returning functions use sret (result ptr as first param).
 *     Public inline wrappers restore normal return-value syntax.
 *   - Strings pass as wasm linear-memory pointers (no copy).
 *   - Variadic functions (TraceLog) format cart-side; host receives plain string.
 *   - TextFormat is fully cart-side; no host import needed.
 */

#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//----------------------------------------------------------------------------------
// Opaque internal types
//----------------------------------------------------------------------------------
typedef struct rAudioBuffer    rAudioBuffer;
typedef struct rAudioProcessor rAudioProcessor;

//----------------------------------------------------------------------------------
// Structures
//----------------------------------------------------------------------------------
${structs.join('\n\n')}

//----------------------------------------------------------------------------------
// Enumerators
//----------------------------------------------------------------------------------
${enums.join('\n\n')}

//----------------------------------------------------------------------------------
// Defines and constants
//----------------------------------------------------------------------------------
${defines.join('\n')}

//----------------------------------------------------------------------------------
// Cart memory — malloc/free exported via linker flags, not this header.
// Compile with: -Wl,--export=malloc -Wl,--export=free
//----------------------------------------------------------------------------------
#include <stdlib.h>

//----------------------------------------------------------------------------------
// Cart lifecycle macros
//
//   Use these to declare the three lifecycle functions the host calls.
//
//   main() is still the WASI entry point — call InitWindow() there.
//   All three lifecycle functions are optional
//----------------------------------------------------------------------------------

// Called every frame while loading. Draw a progress screen here.
void CartPreload(float progress);

// Called once after CartPreload finishes. Set up game state.
void CartInit();

// Called every frame. Main game loop body.
void CartUpdate();

//----------------------------------------------------------------------------------
// Raylib API imports
//----------------------------------------------------------------------------------
${functions.join('\n\n')}
`

await mkdir(join(ROOT, 'include'), { recursive: true })
await writeFile(join(ROOT, 'include', 'raylib.h'), header)

const direct = api.functions.filter((f) => {
  if (EXCLUDE.has(f.name)) return false
  if (f.name === 'TextFormat') return false
  const params = (f.params || []).filter((p) => p.type !== '...')
  return !isStructByValue(f.returnType) && !params.some((p) => isStructByValue(p.type)) && !isVariadic(f)
}).length

console.log(`Generated include/raylib.h`)
console.log(`  Structs:        ${api.structs.length}`)
console.log(`  Aliases:        ${api.aliases?.length || 0}`)
console.log(`  Enums:          ${api.enums.length}`)
console.log(`  Defines:        ${defines.length}`)
console.log(`  Functions:      ${functions.length} / ${api.functions.length}`)
console.log(`    direct import:  ${direct}`)
console.log(`    wrapped:        ${functions.length - direct - 1} (+ 1 TextFormat cart-only)`)
console.log(`  Excluded:       ${excluded.join(', ')}`)
