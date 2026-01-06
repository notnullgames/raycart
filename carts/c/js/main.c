#include "raycart.h"
#include "quickjs.h"
#include "quickjs-libc.h"

JSRuntime *rt;
JSContext *ctx;
JSValue global;
JSValue args[2];

#include "raycart_bindings.c"

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
  // this makes console.logs work right
  fflush(stdout);
}

void CartClose() {
  cart("CartClose", NULL, 0);
}

