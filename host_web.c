// emcc -O3 -sSINGLE_FILE=1 -sUSE_GLFW=3 -sFULL_ES2=1 -sINITIAL_MEMORY=67108864 -sALLOW_MEMORY_GROWTH=1 -Iraylib-6.0_webassembly/include -Lraylib-6.0_webassembly/lib -lraylib.web -sEXPORTED_FUNCTIONS=_placeholder -sEXPORTED_RUNTIME_METHODS=wasmMemory,FS host_web.c -o host/web/raylib_em.mjs

#include <emscripten.h>
#define PLATFORM_WEB 1
#include <raylib.h>
