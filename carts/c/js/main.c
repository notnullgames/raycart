#include "raycart.h"
#include "quickjs.h"
#include "quickjs-libc.h"

JSRuntime *rt;
JSContext *ctx;
JSValue global;
JSValue args[2];

void expose_things_to_js();

// callback something in cart
void cart(const char* func_name, JSValue* args, int argc) {
  JSValue cart = JS_GetPropertyStr(ctx, global, "cart");
  if (!JS_IsUndefined(cart)) {
    JSValue func = JS_GetPropertyStr(ctx, cart, func_name);
    if (JS_IsFunction(ctx, func)) {
      JSValue result = JS_Call(ctx, func, cart, argc, args);
      if (JS_IsException(result)) {
        js_std_dump_error(ctx);
      }
      JS_FreeValue(ctx, result);
    } else {
      // printf("cart.%s() not defined\n", func_name);
    }
    JS_FreeValue(ctx, func);
  } else {
    printf("cart not defined when calling %s\n", func_name);
  }
  JS_FreeValue(ctx, cart);
}

void CartInit(){
  rt = JS_NewRuntime();
  ctx = JS_NewContext(rt);
  global = JS_GetGlobalObject(ctx);

  JS_SetMemoryLimit(rt, 0x4000000); // 64 Mb
  JS_SetMaxStackSize(rt, 0x10000); // 64 Kb
  JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
  js_std_add_helpers(ctx, 0, NULL);

  expose_things_to_js();

  // setup std/os globals/modules
  js_init_module_std(ctx, "std");
  js_init_module_os(ctx, "os");
  const char *str = "import * as std from 'std';\n"
    "import * as os from 'os';\n"
    "import * as cart from './main.js';\n"
    "globalThis.cart = cart;\n"
    "globalThis.std = std;\n"
    "globalThis.os = os;\n"
    "cart?.CartInit && cart.CartInit();\n";
  
  JSValue std_val = JS_Eval(ctx, str, strlen(str), "<CART>", JS_EVAL_TYPE_MODULE);
  std_val = js_std_await(ctx, std_val);
  
  if (!JS_IsException(std_val)) {
    js_module_set_import_meta(ctx, std_val, 1, 1);
    std_val = JS_EvalFunction(ctx, std_val);
  } else {
    js_std_dump_error(ctx);
  }

  JS_FreeValue(ctx, std_val);
}

void CartUpdate() {
  cart("CartUpdate", NULL, 0);
  fflush(stdout);
}

// type-converters

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
  // For values that fit in a regular number, use that
  if (value <= 0x1FFFFFFFFFFFFF) { // 2^53 - 1 (safe integer range)
    return JS_NewFloat64(ctx, (double)value);
  }
  // For larger values, use BigInt
  return JS_NewBigInt64(ctx, (int64_t)value);
}

static JSValue color_to_js(Color color) {
  JSValue obj = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, obj, "r", JS_NewInt32(ctx, color.r));
  JS_SetPropertyStr(ctx, obj, "g", JS_NewInt32(ctx, color.g));
  JS_SetPropertyStr(ctx, obj, "b", JS_NewInt32(ctx, color.b));
  JS_SetPropertyStr(ctx, obj, "a", JS_NewInt32(ctx, color.a));
  return obj;
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
  
  // Try to convert as regular number first (for small values)
  if (JS_IsNumber(val)) {
    double d;
    if (JS_ToFloat64(ctx, &d, val) == 0) {
      result = (uint64_t)d;
    }
  }
  // Handle BigInt values
  else if (JS_IsBigInt(ctx, val)) {
    // For BigInt, we need to use a different approach
    // Convert to string and parse, or use JS_ToBigInt64 if available
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

static Color color_from_js(JSValue obj) {
  Color color = {0, 0, 0, 255}; // Default to opaque black
  
  JSValue r_val = JS_GetPropertyStr(ctx, obj, "r");
  JSValue g_val = JS_GetPropertyStr(ctx, obj, "g");
  JSValue b_val = JS_GetPropertyStr(ctx, obj, "b");
  JSValue a_val = JS_GetPropertyStr(ctx, obj, "a");
  
  uint32_t r, g, b, a;
  if (JS_ToUint32(ctx, &r, r_val) >= 0 && r <= 255) color.r = (uint8_t)r;
  if (JS_ToUint32(ctx, &g, g_val) >= 0 && g <= 255) color.g = (uint8_t)g;
  if (JS_ToUint32(ctx, &b, b_val) >= 0 && b <= 255) color.b = (uint8_t)b;
  if (JS_ToUint32(ctx, &a, a_val) >= 0 && a <= 255) color.a = (uint8_t)a;
  
  JS_FreeValue(ctx, r_val);
  JS_FreeValue(ctx, g_val);
  JS_FreeValue(ctx, b_val);
  JS_FreeValue(ctx, a_val);
  return color;
}


static JSValue js_InitWindow(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  InitWindow(i32_from_js(argv[0]), i32_from_js(argv[1]), string_from_js(argv[2]));
  return JS_UNDEFINED;
}

static JSValue js_ClearBackground(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  ClearBackground(color_from_js(argv[0]));
  return JS_UNDEFINED;
}

static JSValue js_DrawText(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  DrawText(string_from_js(argv[0]), i32_from_js(argv[1]), i32_from_js(argv[2]), i32_from_js(argv[3]), color_from_js(argv[4]));
  return JS_UNDEFINED;
}

void expose_things_to_js(){
  JS_SetPropertyStr(ctx, global, "LIGHTGRAY", color_to_js(LIGHTGRAY));
  JS_SetPropertyStr(ctx, global, "GRAY", color_to_js(GRAY));
  JS_SetPropertyStr(ctx, global, "DARKGRAY", color_to_js(DARKGRAY));
  JS_SetPropertyStr(ctx, global, "YELLOW", color_to_js(YELLOW));
  JS_SetPropertyStr(ctx, global, "GOLD", color_to_js(GOLD));
  JS_SetPropertyStr(ctx, global, "ORANGE", color_to_js(ORANGE));
  JS_SetPropertyStr(ctx, global, "PINK", color_to_js(PINK));
  JS_SetPropertyStr(ctx, global, "RED", color_to_js(RED));
  JS_SetPropertyStr(ctx, global, "MAROON", color_to_js(MAROON));
  JS_SetPropertyStr(ctx, global, "GREEN", color_to_js(GREEN));
  JS_SetPropertyStr(ctx, global, "LIME", color_to_js(LIME));
  JS_SetPropertyStr(ctx, global, "DARKGREEN", color_to_js(DARKGREEN));
  JS_SetPropertyStr(ctx, global, "SKYBLUE", color_to_js(SKYBLUE));
  JS_SetPropertyStr(ctx, global, "BLUE", color_to_js(BLUE));
  JS_SetPropertyStr(ctx, global, "DARKBLUE", color_to_js(DARKBLUE));
  JS_SetPropertyStr(ctx, global, "PURPLE", color_to_js(PURPLE));
  JS_SetPropertyStr(ctx, global, "VIOLET", color_to_js(VIOLET));
  JS_SetPropertyStr(ctx, global, "DARKPURPLE", color_to_js(DARKPURPLE));
  JS_SetPropertyStr(ctx, global, "BEIGE", color_to_js(BEIGE));
  JS_SetPropertyStr(ctx, global, "BROWN", color_to_js(BROWN));
  JS_SetPropertyStr(ctx, global, "DARKBROWN", color_to_js(DARKBROWN));
  JS_SetPropertyStr(ctx, global, "WHITE", color_to_js(WHITE));
  JS_SetPropertyStr(ctx, global, "BLACK", color_to_js(BLACK));
  JS_SetPropertyStr(ctx, global, "BLANK", color_to_js(BLANK));
  JS_SetPropertyStr(ctx, global, "MAGENTA", color_to_js(MAGENTA));
  JS_SetPropertyStr(ctx, global, "RAYWHITE", color_to_js(RAYWHITE));

  JS_SetPropertyStr(ctx, global, "InitWindow", JS_NewCFunction(ctx, js_InitWindow, "InitWindow", 3));
  JS_SetPropertyStr(ctx, global, "ClearBackground", JS_NewCFunction(ctx, js_ClearBackground, "ClearBackground", 1));
  JS_SetPropertyStr(ctx, global, "DrawText", JS_NewCFunction(ctx, js_DrawText, "DrawText", 5));
}
