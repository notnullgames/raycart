#ifdef EMSCRIPTEN

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
    const cartImage = ptr => copyFromCart(ptr, 20);
    const cartVector2 = ptr => copyFromCart(ptr, 8);
    const cartColor = ptr => copyFromCart(ptr, 4);
    const cartCamera2D = ptr => copyFromCart(ptr, 24);
    const cartCamera3D = ptr => copyFromCart(ptr, 44);
    const cartRenderTexture = ptr => copyFromCart(ptr, 44);
    const cartShader = ptr => copyFromCart(ptr, 8);
    const cartVrStereoConfig = ptr => copyFromCart(ptr, 0);
    const cartVrDeviceInfo = ptr => copyFromCart(ptr, 28);
    const cartMatrix = ptr => copyFromCart(ptr, 64);
    const cartTexture = ptr => copyFromCart(ptr, 20);
    const cartRay = ptr => copyFromCart(ptr, 24);
    const cartVector3 = ptr => copyFromCart(ptr, 12);
    const cartFilePathList = ptr => copyFromCart(ptr, 12);
    const cartAutomationEventList = ptr => copyFromCart(ptr, 12);
    const cartAutomationEvent = ptr => copyFromCart(ptr, 8);
    const cartRectangle = ptr => copyFromCart(ptr, 16);
    const cartFont = ptr => copyFromCart(ptr, 40);
    const cartNPatchInfo = ptr => copyFromCart(ptr, 36);
    const cartVector4 = ptr => copyFromCart(ptr, 16);
    const cartGlyphInfo = ptr => copyFromCart(ptr, 36);
    const cartModel = ptr => copyFromCart(ptr, 96);
    const cartMesh = ptr => copyFromCart(ptr, 68);
    const cartBoundingBox = ptr => copyFromCart(ptr, 24);
    const cartMaterial = ptr => copyFromCart(ptr, 12);
    const cartModelAnimation = ptr => copyFromCart(ptr, 16);
    const cartRayCollision = ptr => copyFromCart(ptr, 28);
    const cartWave = ptr => copyFromCart(ptr, 20);
    const cartSound = ptr => copyFromCart(ptr, 24);
    const cartMusic = ptr => copyFromCart(ptr, 32);
    const cartAudioStream = ptr => copyFromCart(ptr, 20);

    // copy from host to cart
    const hostString = (ptr) => copyToCart(ptr, hostStringLen(ptr) + 1);
    const hostImage = ptr => copyToCart(ptr, 20);
    const hostVector2 = ptr => copyToCart(ptr, 8);
    const hostColor = ptr => copyToCart(ptr, 4);
    const hostCamera2D = ptr => copyToCart(ptr, 24);
    const hostCamera3D = ptr => copyToCart(ptr, 44);
    const hostRenderTexture = ptr => copyToCart(ptr, 44);
    const hostShader = ptr => copyToCart(ptr, 8);
    const hostVrStereoConfig = ptr => copyToCart(ptr, 0);
    const hostVrDeviceInfo = ptr => copyToCart(ptr, 28);
    const hostMatrix = ptr => copyToCart(ptr, 64);
    const hostTexture = ptr => copyToCart(ptr, 20);
    const hostRay = ptr => copyToCart(ptr, 24);
    const hostVector3 = ptr => copyToCart(ptr, 12);
    const hostFilePathList = ptr => copyToCart(ptr, 12);
    const hostAutomationEventList = ptr => copyToCart(ptr, 12);
    const hostAutomationEvent = ptr => copyToCart(ptr, 8);
    const hostRectangle = ptr => copyToCart(ptr, 16);
    const hostFont = ptr => copyToCart(ptr, 40);
    const hostNPatchInfo = ptr => copyToCart(ptr, 36);
    const hostVector4 = ptr => copyToCart(ptr, 16);
    const hostGlyphInfo = ptr => copyToCart(ptr, 36);
    const hostModel = ptr => copyToCart(ptr, 96);
    const hostMesh = ptr => copyToCart(ptr, 68);
    const hostBoundingBox = ptr => copyToCart(ptr, 24);
    const hostMaterial = ptr => copyToCart(ptr, 12);
    const hostModelAnimation = ptr => copyToCart(ptr, 16);
    const hostRayCollision = ptr => copyToCart(ptr, 28);
    const hostWave = ptr => copyToCart(ptr, 20);
    const hostSound = ptr => copyToCart(ptr, 24);
    const hostMusic = ptr => copyToCart(ptr, 32);
    const hostAudioStream = ptr => copyToCart(ptr, 20);

    // Generated raylib bindings
    const raycart = {
        InitWindow(width, height, title) {
            const title_h = cartString(title);
            Module._InitWindow(width, height, title_h);
            Module._MemFree(title_h);
        },

        CloseWindow() {
            Module._CloseWindow();
        },

        WindowShouldClose() {
            const result = Module._WindowShouldClose();
            return result;
        },

        IsWindowReady() {
            const result = Module._IsWindowReady();
            return result;
        },

        IsWindowFullscreen() {
            const result = Module._IsWindowFullscreen();
            return result;
        },

        IsWindowHidden() {
            const result = Module._IsWindowHidden();
            return result;
        },

        IsWindowMinimized() {
            const result = Module._IsWindowMinimized();
            return result;
        },

        IsWindowMaximized() {
            const result = Module._IsWindowMaximized();
            return result;
        },

        IsWindowFocused() {
            const result = Module._IsWindowFocused();
            return result;
        },

        IsWindowResized() {
            const result = Module._IsWindowResized();
            return result;
        },

        IsWindowState(flag) {
            const result = Module._IsWindowState(flag);
            return result;
        },

        SetWindowState(flags) {
            Module._SetWindowState(flags);
        },

        ClearWindowState(flags) {
            Module._ClearWindowState(flags);
        },

        ToggleFullscreen() {
            Module._ToggleFullscreen();
        },

        ToggleBorderlessWindowed() {
            Module._ToggleBorderlessWindowed();
        },

        MaximizeWindow() {
            Module._MaximizeWindow();
        },

        MinimizeWindow() {
            Module._MinimizeWindow();
        },

        RestoreWindow() {
            Module._RestoreWindow();
        },

        SetWindowIcon(image) {
            const image_h = cartImage(image);
            Module._SetWindowIcon(image_h);
            Module._MemFree(image_h);
        },

        SetWindowIcons(images, count) {
            const images_h = cartImage(images);
            Module._SetWindowIcons(images_h, count);
            copyHostToCart(images_h, images, 20);
            Module._MemFree(images_h);
        },

        SetWindowTitle(title) {
            const title_h = cartString(title);
            Module._SetWindowTitle(title_h);
            Module._MemFree(title_h);
        },

        SetWindowPosition(x, y) {
            Module._SetWindowPosition(x, y);
        },

        SetWindowMonitor(monitor) {
            Module._SetWindowMonitor(monitor);
        },

        SetWindowMinSize(width, height) {
            Module._SetWindowMinSize(width, height);
        },

        SetWindowMaxSize(width, height) {
            Module._SetWindowMaxSize(width, height);
        },

        SetWindowSize(width, height) {
            Module._SetWindowSize(width, height);
        },

        SetWindowOpacity(opacity) {
            Module._SetWindowOpacity(opacity);
        },

        SetWindowFocused() {
            Module._SetWindowFocused();
        },

        GetWindowHandle() {
            const result = Module._GetWindowHandle();
            return result;
        },

        GetScreenWidth() {
            const result = Module._GetScreenWidth();
            return result;
        },

        GetScreenHeight() {
            const result = Module._GetScreenHeight();
            return result;
        },

        GetRenderWidth() {
            const result = Module._GetRenderWidth();
            return result;
        },

        GetRenderHeight() {
            const result = Module._GetRenderHeight();
            return result;
        },

        GetMonitorCount() {
            const result = Module._GetMonitorCount();
            return result;
        },

        GetCurrentMonitor() {
            const result = Module._GetCurrentMonitor();
            return result;
        },

        GetMonitorPosition(resultPtr, monitor) {
            const result_h = Module._MemAlloc(8);
            Module._GetMonitorPosition(result_h, monitor);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetMonitorWidth(monitor) {
            const result = Module._GetMonitorWidth(monitor);
            return result;
        },

        GetMonitorHeight(monitor) {
            const result = Module._GetMonitorHeight(monitor);
            return result;
        },

        GetMonitorPhysicalWidth(monitor) {
            const result = Module._GetMonitorPhysicalWidth(monitor);
            return result;
        },

        GetMonitorPhysicalHeight(monitor) {
            const result = Module._GetMonitorPhysicalHeight(monitor);
            return result;
        },

        GetMonitorRefreshRate(monitor) {
            const result = Module._GetMonitorRefreshRate(monitor);
            return result;
        },

        GetWindowPosition(resultPtr) {
            const result_h = Module._MemAlloc(8);
            Module._GetWindowPosition(result_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetWindowScaleDPI(resultPtr) {
            const result_h = Module._MemAlloc(8);
            Module._GetWindowScaleDPI(result_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetMonitorName(monitor) {
            const result = Module._GetMonitorName(monitor);
            return result;
        },

        SetClipboardText(text) {
            const text_h = cartString(text);
            Module._SetClipboardText(text_h);
            Module._MemFree(text_h);
        },

        GetClipboardText() {
            const result = Module._GetClipboardText();
            return result;
        },

        EnableEventWaiting() {
            Module._EnableEventWaiting();
        },

        DisableEventWaiting() {
            Module._DisableEventWaiting();
        },

        ShowCursor() {
            Module._ShowCursor();
        },

        HideCursor() {
            Module._HideCursor();
        },

        IsCursorHidden() {
            const result = Module._IsCursorHidden();
            return result;
        },

        EnableCursor() {
            Module._EnableCursor();
        },

        DisableCursor() {
            Module._DisableCursor();
        },

        IsCursorOnScreen() {
            const result = Module._IsCursorOnScreen();
            return result;
        },

        ClearBackground(color) {
            const color_h = cartColor(color);
            Module._ClearBackground(color_h);
            Module._MemFree(color_h);
        },

        BeginDrawing() {
            Module._BeginDrawing();
        },

        EndDrawing() {
            Module._EndDrawing();
        },

        BeginMode2D(camera) {
            const camera_h = cartCamera2D(camera);
            Module._BeginMode2D(camera_h);
            Module._MemFree(camera_h);
        },

        EndMode2D() {
            Module._EndMode2D();
        },

        BeginMode3D(camera) {
            const camera_h = cartCamera3D(camera);
            Module._BeginMode3D(camera_h);
            Module._MemFree(camera_h);
        },

        EndMode3D() {
            Module._EndMode3D();
        },

        BeginTextureMode(target) {
            const target_h = cartRenderTexture(target);
            Module._BeginTextureMode(target_h);
            Module._MemFree(target_h);
        },

        EndTextureMode() {
            Module._EndTextureMode();
        },

        BeginShaderMode(shader) {
            const shader_h = cartShader(shader);
            Module._BeginShaderMode(shader_h);
            Module._MemFree(shader_h);
        },

        EndShaderMode() {
            Module._EndShaderMode();
        },

        BeginBlendMode(mode) {
            Module._BeginBlendMode(mode);
        },

        EndBlendMode() {
            Module._EndBlendMode();
        },

        BeginScissorMode(x, y, width, height) {
            Module._BeginScissorMode(x, y, width, height);
        },

        EndScissorMode() {
            Module._EndScissorMode();
        },

        BeginVrStereoMode(config) {
            const config_h = cartVrStereoConfig(config);
            Module._BeginVrStereoMode(config_h);
            Module._MemFree(config_h);
        },

        EndVrStereoMode() {
            Module._EndVrStereoMode();
        },

        LoadVrStereoConfig(resultPtr, device) {
            const device_h = cartVrDeviceInfo(device);
            const result_h = Module._MemAlloc(0);
            Module._LoadVrStereoConfig(result_h, device_h);
            Module._MemFree(device_h);
            copyHostToCart(result_h, resultPtr, 0);
            Module._MemFree(result_h);
        },

        UnloadVrStereoConfig(config) {
            const config_h = cartVrStereoConfig(config);
            Module._UnloadVrStereoConfig(config_h);
            Module._MemFree(config_h);
        },

        LoadShader(resultPtr, vsFileName, fsFileName) {
            const vsFileName_h = cartString(vsFileName);
            const fsFileName_h = cartString(fsFileName);
            const result_h = Module._MemAlloc(8);
            Module._LoadShader(result_h, vsFileName_h, fsFileName_h);
            Module._MemFree(vsFileName_h);
            Module._MemFree(fsFileName_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        LoadShaderFromMemory(resultPtr, vsCode, fsCode) {
            const vsCode_h = cartString(vsCode);
            const fsCode_h = cartString(fsCode);
            const result_h = Module._MemAlloc(8);
            Module._LoadShaderFromMemory(result_h, vsCode_h, fsCode_h);
            Module._MemFree(vsCode_h);
            Module._MemFree(fsCode_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        IsShaderValid(shader) {
            const shader_h = cartShader(shader);
            const result = Module._IsShaderValid(shader_h);
            Module._MemFree(shader_h);
            return result;
        },

        GetShaderLocation(shader, uniformName) {
            const shader_h = cartShader(shader);
            const uniformName_h = cartString(uniformName);
            const result = Module._GetShaderLocation(shader_h, uniformName_h);
            Module._MemFree(shader_h);
            Module._MemFree(uniformName_h);
            return result;
        },

        GetShaderLocationAttrib(shader, attribName) {
            const shader_h = cartShader(shader);
            const attribName_h = cartString(attribName);
            const result = Module._GetShaderLocationAttrib(shader_h, attribName_h);
            Module._MemFree(shader_h);
            Module._MemFree(attribName_h);
            return result;
        },

        SetShaderValue(shader, locIndex, value, uniformType) {
            const shader_h = cartShader(shader);
            Module._SetShaderValue(shader_h, locIndex, value, uniformType);
            Module._MemFree(shader_h);
        },

        SetShaderValueV(shader, locIndex, value, uniformType, count) {
            const shader_h = cartShader(shader);
            Module._SetShaderValueV(shader_h, locIndex, value, uniformType, count);
            Module._MemFree(shader_h);
        },

        SetShaderValueMatrix(shader, locIndex, mat) {
            const shader_h = cartShader(shader);
            const mat_h = cartMatrix(mat);
            Module._SetShaderValueMatrix(shader_h, locIndex, mat_h);
            Module._MemFree(shader_h);
            Module._MemFree(mat_h);
        },

        SetShaderValueTexture(shader, locIndex, texture) {
            const shader_h = cartShader(shader);
            const texture_h = cartTexture(texture);
            Module._SetShaderValueTexture(shader_h, locIndex, texture_h);
            Module._MemFree(shader_h);
            Module._MemFree(texture_h);
        },

        UnloadShader(shader) {
            const shader_h = cartShader(shader);
            Module._UnloadShader(shader_h);
            Module._MemFree(shader_h);
        },

        GetScreenToWorldRay(resultPtr, position, camera) {
            const position_h = cartVector2(position);
            const camera_h = cartCamera3D(camera);
            const result_h = Module._MemAlloc(24);
            Module._GetScreenToWorldRay(result_h, position_h, camera_h);
            Module._MemFree(position_h);
            Module._MemFree(camera_h);
            copyHostToCart(result_h, resultPtr, 24);
            Module._MemFree(result_h);
        },

        GetScreenToWorldRayEx(resultPtr, position, camera, width, height) {
            const position_h = cartVector2(position);
            const camera_h = cartCamera3D(camera);
            const result_h = Module._MemAlloc(24);
            Module._GetScreenToWorldRayEx(result_h, position_h, camera_h, width, height);
            Module._MemFree(position_h);
            Module._MemFree(camera_h);
            copyHostToCart(result_h, resultPtr, 24);
            Module._MemFree(result_h);
        },

        GetWorldToScreen(resultPtr, position, camera) {
            const position_h = cartVector3(position);
            const camera_h = cartCamera3D(camera);
            const result_h = Module._MemAlloc(8);
            Module._GetWorldToScreen(result_h, position_h, camera_h);
            Module._MemFree(position_h);
            Module._MemFree(camera_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetWorldToScreenEx(resultPtr, position, camera, width, height) {
            const position_h = cartVector3(position);
            const camera_h = cartCamera3D(camera);
            const result_h = Module._MemAlloc(8);
            Module._GetWorldToScreenEx(result_h, position_h, camera_h, width, height);
            Module._MemFree(position_h);
            Module._MemFree(camera_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetWorldToScreen2D(resultPtr, position, camera) {
            const position_h = cartVector2(position);
            const camera_h = cartCamera2D(camera);
            const result_h = Module._MemAlloc(8);
            Module._GetWorldToScreen2D(result_h, position_h, camera_h);
            Module._MemFree(position_h);
            Module._MemFree(camera_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetScreenToWorld2D(resultPtr, position, camera) {
            const position_h = cartVector2(position);
            const camera_h = cartCamera2D(camera);
            const result_h = Module._MemAlloc(8);
            Module._GetScreenToWorld2D(result_h, position_h, camera_h);
            Module._MemFree(position_h);
            Module._MemFree(camera_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetCameraMatrix(resultPtr, camera) {
            const camera_h = cartCamera3D(camera);
            const result_h = Module._MemAlloc(64);
            Module._GetCameraMatrix(result_h, camera_h);
            Module._MemFree(camera_h);
            copyHostToCart(result_h, resultPtr, 64);
            Module._MemFree(result_h);
        },

        GetCameraMatrix2D(resultPtr, camera) {
            const camera_h = cartCamera2D(camera);
            const result_h = Module._MemAlloc(64);
            Module._GetCameraMatrix2D(result_h, camera_h);
            Module._MemFree(camera_h);
            copyHostToCart(result_h, resultPtr, 64);
            Module._MemFree(result_h);
        },

        SetTargetFPS(fps) {
            Module._SetTargetFPS(fps);
        },

        GetFrameTime() {
            const result = Module._GetFrameTime();
            return result;
        },

        GetTime() {
            const result = Module._GetTime();
            return result;
        },

        GetFPS() {
            const result = Module._GetFPS();
            return result;
        },

        SwapScreenBuffer() {
            Module._SwapScreenBuffer();
        },

        PollInputEvents() {
            Module._PollInputEvents();
        },

        WaitTime(seconds) {
            Module._WaitTime(seconds);
        },

        SetRandomSeed(seed) {
            Module._SetRandomSeed(seed);
        },

        GetRandomValue(min, max) {
            const result = Module._GetRandomValue(min, max);
            return result;
        },

        LoadRandomSequence(count, min, max) {
            const result = Module._LoadRandomSequence(count, min, max);
            return result;
        },

        UnloadRandomSequence(sequence) {
            Module._UnloadRandomSequence(sequence);
        },

        TakeScreenshot(fileName) {
            const fileName_h = cartString(fileName);
            Module._TakeScreenshot(fileName_h);
            Module._MemFree(fileName_h);
        },

        SetConfigFlags(flags) {
            Module._SetConfigFlags(flags);
        },

        OpenURL(url) {
            const url_h = cartString(url);
            Module._OpenURL(url_h);
            Module._MemFree(url_h);
        },

        TraceLog(logLevel, text, args) {
            const text_h = cartString(text);
            Module._TraceLog(logLevel, text_h, args);
            Module._MemFree(text_h);
        },

        SetTraceLogLevel(logLevel) {
            Module._SetTraceLogLevel(logLevel);
        },

        LoadFileData(fileName, dataSize) {
            const fileName_h = cartString(fileName);
            const result = Module._LoadFileData(fileName_h, dataSize);
            Module._MemFree(fileName_h);
            return result;
        },

        UnloadFileData(data) {
            const data_h = cartString(data);
            Module._UnloadFileData(data_h);
            Module._MemFree(data_h);
        },

        SaveFileData(fileName, data, dataSize) {
            const fileName_h = cartString(fileName);
            const result = Module._SaveFileData(fileName_h, data, dataSize);
            Module._MemFree(fileName_h);
            return result;
        },

        ExportDataAsCode(data, dataSize, fileName) {
            const data_h = cartString(data);
            const fileName_h = cartString(fileName);
            const result = Module._ExportDataAsCode(data_h, dataSize, fileName_h);
            Module._MemFree(data_h);
            Module._MemFree(fileName_h);
            return result;
        },

        LoadFileText(fileName) {
            const fileName_h = cartString(fileName);
            const result = Module._LoadFileText(fileName_h);
            Module._MemFree(fileName_h);
            return result;
        },

        UnloadFileText(text) {
            const text_h = cartString(text);
            Module._UnloadFileText(text_h);
            Module._MemFree(text_h);
        },

        SaveFileText(fileName, text) {
            const fileName_h = cartString(fileName);
            const text_h = cartString(text);
            const result = Module._SaveFileText(fileName_h, text_h);
            Module._MemFree(fileName_h);
            Module._MemFree(text_h);
            return result;
        },

        FileExists(fileName) {
            const fileName_h = cartString(fileName);
            const result = Module._FileExists(fileName_h);
            Module._MemFree(fileName_h);
            return result;
        },

        DirectoryExists(dirPath) {
            const dirPath_h = cartString(dirPath);
            const result = Module._DirectoryExists(dirPath_h);
            Module._MemFree(dirPath_h);
            return result;
        },

        IsFileExtension(fileName, ext) {
            const fileName_h = cartString(fileName);
            const ext_h = cartString(ext);
            const result = Module._IsFileExtension(fileName_h, ext_h);
            Module._MemFree(fileName_h);
            Module._MemFree(ext_h);
            return result;
        },

        GetFileLength(fileName) {
            const fileName_h = cartString(fileName);
            const result = Module._GetFileLength(fileName_h);
            Module._MemFree(fileName_h);
            return result;
        },

        GetFileModTime(fileName) {
            const fileName_h = cartString(fileName);
            const result = Module._GetFileModTime(fileName_h);
            Module._MemFree(fileName_h);
            return result;
        },

        GetFileExtension(fileName) {
            const fileName_h = cartString(fileName);
            const result = Module._GetFileExtension(fileName_h);
            Module._MemFree(fileName_h);
            return result;
        },

        GetFileName(filePath) {
            const filePath_h = cartString(filePath);
            const result = Module._GetFileName(filePath_h);
            Module._MemFree(filePath_h);
            return result;
        },

        GetFileNameWithoutExt(filePath) {
            const filePath_h = cartString(filePath);
            const result = Module._GetFileNameWithoutExt(filePath_h);
            Module._MemFree(filePath_h);
            return result;
        },

        GetDirectoryPath(filePath) {
            const filePath_h = cartString(filePath);
            const result = Module._GetDirectoryPath(filePath_h);
            Module._MemFree(filePath_h);
            return result;
        },

        GetPrevDirectoryPath(dirPath) {
            const dirPath_h = cartString(dirPath);
            const result = Module._GetPrevDirectoryPath(dirPath_h);
            Module._MemFree(dirPath_h);
            return result;
        },

        GetWorkingDirectory() {
            const result = Module._GetWorkingDirectory();
            return result;
        },

        GetApplicationDirectory() {
            const result = Module._GetApplicationDirectory();
            return result;
        },

        MakeDirectory(dirPath) {
            const dirPath_h = cartString(dirPath);
            const result = Module._MakeDirectory(dirPath_h);
            Module._MemFree(dirPath_h);
            return result;
        },

        ChangeDirectory(dirPath) {
            const dirPath_h = cartString(dirPath);
            const result = Module._ChangeDirectory(dirPath_h);
            Module._MemFree(dirPath_h);
            return result;
        },

        IsPathFile(path) {
            const path_h = cartString(path);
            const result = Module._IsPathFile(path_h);
            Module._MemFree(path_h);
            return result;
        },

        IsFileNameValid(fileName) {
            const fileName_h = cartString(fileName);
            const result = Module._IsFileNameValid(fileName_h);
            Module._MemFree(fileName_h);
            return result;
        },

        LoadDirectoryFiles(resultPtr, dirPath) {
            const dirPath_h = cartString(dirPath);
            const result_h = Module._MemAlloc(12);
            Module._LoadDirectoryFiles(result_h, dirPath_h);
            Module._MemFree(dirPath_h);
            copyHostToCart(result_h, resultPtr, 12);
            Module._MemFree(result_h);
        },

        LoadDirectoryFilesEx(resultPtr, basePath, filter, scanSubdirs) {
            const basePath_h = cartString(basePath);
            const filter_h = cartString(filter);
            const result_h = Module._MemAlloc(12);
            Module._LoadDirectoryFilesEx(result_h, basePath_h, filter_h, scanSubdirs);
            Module._MemFree(basePath_h);
            Module._MemFree(filter_h);
            copyHostToCart(result_h, resultPtr, 12);
            Module._MemFree(result_h);
        },

        UnloadDirectoryFiles(files) {
            const files_h = cartFilePathList(files);
            Module._UnloadDirectoryFiles(files_h);
            Module._MemFree(files_h);
        },

        IsFileDropped() {
            const result = Module._IsFileDropped();
            return result;
        },

        LoadDroppedFiles(resultPtr) {
            const result_h = Module._MemAlloc(12);
            Module._LoadDroppedFiles(result_h);
            copyHostToCart(result_h, resultPtr, 12);
            Module._MemFree(result_h);
        },

        UnloadDroppedFiles(files) {
            const files_h = cartFilePathList(files);
            Module._UnloadDroppedFiles(files_h);
            Module._MemFree(files_h);
        },

        CompressData(data, dataSize, compDataSize) {
            const data_h = cartString(data);
            const result = Module._CompressData(data_h, dataSize, compDataSize);
            Module._MemFree(data_h);
            return result;
        },

        DecompressData(compData, compDataSize, dataSize) {
            const compData_h = cartString(compData);
            const result = Module._DecompressData(compData_h, compDataSize, dataSize);
            Module._MemFree(compData_h);
            return result;
        },

        EncodeDataBase64(data, dataSize, outputSize) {
            const data_h = cartString(data);
            const result = Module._EncodeDataBase64(data_h, dataSize, outputSize);
            Module._MemFree(data_h);
            return result;
        },

        DecodeDataBase64(text, outputSize) {
            const text_h = cartString(text);
            const result = Module._DecodeDataBase64(text_h, outputSize);
            Module._MemFree(text_h);
            return result;
        },

        ComputeCRC32(data, dataSize) {
            const data_h = cartString(data);
            const result = Module._ComputeCRC32(data_h, dataSize);
            Module._MemFree(data_h);
            return result;
        },

        ComputeMD5(data, dataSize) {
            const data_h = cartString(data);
            const result = Module._ComputeMD5(data_h, dataSize);
            Module._MemFree(data_h);
            return result;
        },

        ComputeSHA1(data, dataSize) {
            const data_h = cartString(data);
            const result = Module._ComputeSHA1(data_h, dataSize);
            Module._MemFree(data_h);
            return result;
        },

        LoadAutomationEventList(resultPtr, fileName) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(12);
            Module._LoadAutomationEventList(result_h, fileName_h);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 12);
            Module._MemFree(result_h);
        },

        UnloadAutomationEventList(list) {
            const list_h = cartAutomationEventList(list);
            Module._UnloadAutomationEventList(list_h);
            Module._MemFree(list_h);
        },

        ExportAutomationEventList(list, fileName) {
            const list_h = cartAutomationEventList(list);
            const fileName_h = cartString(fileName);
            const result = Module._ExportAutomationEventList(list_h, fileName_h);
            Module._MemFree(list_h);
            Module._MemFree(fileName_h);
            return result;
        },

        SetAutomationEventList(list) {
            const list_h = cartAutomationEventList(list);
            Module._SetAutomationEventList(list_h);
            copyHostToCart(list_h, list, 12);
            Module._MemFree(list_h);
        },

        SetAutomationEventBaseFrame(frame) {
            Module._SetAutomationEventBaseFrame(frame);
        },

        StartAutomationEventRecording() {
            Module._StartAutomationEventRecording();
        },

        StopAutomationEventRecording() {
            Module._StopAutomationEventRecording();
        },

        PlayAutomationEvent(event) {
            const event_h = cartAutomationEvent(event);
            Module._PlayAutomationEvent(event_h);
            Module._MemFree(event_h);
        },

        IsKeyPressed(key) {
            const result = Module._IsKeyPressed(key);
            return result;
        },

        IsKeyPressedRepeat(key) {
            const result = Module._IsKeyPressedRepeat(key);
            return result;
        },

        IsKeyDown(key) {
            const result = Module._IsKeyDown(key);
            return result;
        },

        IsKeyReleased(key) {
            const result = Module._IsKeyReleased(key);
            return result;
        },

        IsKeyUp(key) {
            const result = Module._IsKeyUp(key);
            return result;
        },

        GetKeyPressed() {
            const result = Module._GetKeyPressed();
            return result;
        },

        GetCharPressed() {
            const result = Module._GetCharPressed();
            return result;
        },

        GetKeyName(key) {
            const result = Module._GetKeyName(key);
            return result;
        },

        SetExitKey(key) {
            Module._SetExitKey(key);
        },

        IsGamepadAvailable(gamepad) {
            const result = Module._IsGamepadAvailable(gamepad);
            return result;
        },

        GetGamepadName(gamepad) {
            const result = Module._GetGamepadName(gamepad);
            return result;
        },

        IsGamepadButtonPressed(gamepad, button) {
            const result = Module._IsGamepadButtonPressed(gamepad, button);
            return result;
        },

        IsGamepadButtonDown(gamepad, button) {
            const result = Module._IsGamepadButtonDown(gamepad, button);
            return result;
        },

        IsGamepadButtonReleased(gamepad, button) {
            const result = Module._IsGamepadButtonReleased(gamepad, button);
            return result;
        },

        IsGamepadButtonUp(gamepad, button) {
            const result = Module._IsGamepadButtonUp(gamepad, button);
            return result;
        },

        GetGamepadButtonPressed() {
            const result = Module._GetGamepadButtonPressed();
            return result;
        },

        GetGamepadAxisCount(gamepad) {
            const result = Module._GetGamepadAxisCount(gamepad);
            return result;
        },

        GetGamepadAxisMovement(gamepad, axis) {
            const result = Module._GetGamepadAxisMovement(gamepad, axis);
            return result;
        },

        SetGamepadMappings(mappings) {
            const mappings_h = cartString(mappings);
            const result = Module._SetGamepadMappings(mappings_h);
            Module._MemFree(mappings_h);
            return result;
        },

        SetGamepadVibration(gamepad, leftMotor, rightMotor, duration) {
            Module._SetGamepadVibration(gamepad, leftMotor, rightMotor, duration);
        },

        IsMouseButtonPressed(button) {
            const result = Module._IsMouseButtonPressed(button);
            return result;
        },

        IsMouseButtonDown(button) {
            const result = Module._IsMouseButtonDown(button);
            return result;
        },

        IsMouseButtonReleased(button) {
            const result = Module._IsMouseButtonReleased(button);
            return result;
        },

        IsMouseButtonUp(button) {
            const result = Module._IsMouseButtonUp(button);
            return result;
        },

        GetMouseX() {
            const result = Module._GetMouseX();
            return result;
        },

        GetMouseY() {
            const result = Module._GetMouseY();
            return result;
        },

        GetMousePosition(resultPtr) {
            const result_h = Module._MemAlloc(8);
            Module._GetMousePosition(result_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetMouseDelta(resultPtr) {
            const result_h = Module._MemAlloc(8);
            Module._GetMouseDelta(result_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        SetMousePosition(x, y) {
            Module._SetMousePosition(x, y);
        },

        SetMouseOffset(offsetX, offsetY) {
            Module._SetMouseOffset(offsetX, offsetY);
        },

        SetMouseScale(scaleX, scaleY) {
            Module._SetMouseScale(scaleX, scaleY);
        },

        GetMouseWheelMove() {
            const result = Module._GetMouseWheelMove();
            return result;
        },

        GetMouseWheelMoveV(resultPtr) {
            const result_h = Module._MemAlloc(8);
            Module._GetMouseWheelMoveV(result_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        SetMouseCursor(cursor) {
            Module._SetMouseCursor(cursor);
        },

        GetTouchX() {
            const result = Module._GetTouchX();
            return result;
        },

        GetTouchY() {
            const result = Module._GetTouchY();
            return result;
        },

        GetTouchPosition(resultPtr, index) {
            const result_h = Module._MemAlloc(8);
            Module._GetTouchPosition(result_h, index);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetTouchPointId(index) {
            const result = Module._GetTouchPointId(index);
            return result;
        },

        GetTouchPointCount() {
            const result = Module._GetTouchPointCount();
            return result;
        },

        SetGesturesEnabled(flags) {
            Module._SetGesturesEnabled(flags);
        },

        IsGestureDetected(gesture) {
            const result = Module._IsGestureDetected(gesture);
            return result;
        },

        GetGestureDetected() {
            const result = Module._GetGestureDetected();
            return result;
        },

        GetGestureHoldDuration() {
            const result = Module._GetGestureHoldDuration();
            return result;
        },

        GetGestureDragVector(resultPtr) {
            const result_h = Module._MemAlloc(8);
            Module._GetGestureDragVector(result_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetGestureDragAngle() {
            const result = Module._GetGestureDragAngle();
            return result;
        },

        GetGesturePinchVector(resultPtr) {
            const result_h = Module._MemAlloc(8);
            Module._GetGesturePinchVector(result_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetGesturePinchAngle() {
            const result = Module._GetGesturePinchAngle();
            return result;
        },

        UpdateCamera(camera, mode) {
            const camera_h = cartCamera3D(camera);
            Module._UpdateCamera(camera_h, mode);
            copyHostToCart(camera_h, camera, 44);
            Module._MemFree(camera_h);
        },

        UpdateCameraPro(camera, movement, rotation, zoom) {
            const camera_h = cartCamera3D(camera);
            const movement_h = cartVector3(movement);
            const rotation_h = cartVector3(rotation);
            Module._UpdateCameraPro(camera_h, movement_h, rotation_h, zoom);
            copyHostToCart(camera_h, camera, 44);
            Module._MemFree(camera_h);
            Module._MemFree(movement_h);
            Module._MemFree(rotation_h);
        },

        SetShapesTexture(texture, source) {
            const texture_h = cartTexture(texture);
            const source_h = cartRectangle(source);
            Module._SetShapesTexture(texture_h, source_h);
            Module._MemFree(texture_h);
            Module._MemFree(source_h);
        },

        GetShapesTexture(resultPtr) {
            const result_h = Module._MemAlloc(20);
            Module._GetShapesTexture(result_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        GetShapesTextureRectangle(resultPtr) {
            const result_h = Module._MemAlloc(16);
            Module._GetShapesTextureRectangle(result_h);
            copyHostToCart(result_h, resultPtr, 16);
            Module._MemFree(result_h);
        },

        DrawPixel(posX, posY, color) {
            const color_h = cartColor(color);
            Module._DrawPixel(posX, posY, color_h);
            Module._MemFree(color_h);
        },

        DrawPixelV(position, color) {
            const position_h = cartVector2(position);
            const color_h = cartColor(color);
            Module._DrawPixelV(position_h, color_h);
            Module._MemFree(position_h);
            Module._MemFree(color_h);
        },

        DrawLine(startPosX, startPosY, endPosX, endPosY, color) {
            const color_h = cartColor(color);
            Module._DrawLine(startPosX, startPosY, endPosX, endPosY, color_h);
            Module._MemFree(color_h);
        },

        DrawLineV(startPos, endPos, color) {
            const startPos_h = cartVector2(startPos);
            const endPos_h = cartVector2(endPos);
            const color_h = cartColor(color);
            Module._DrawLineV(startPos_h, endPos_h, color_h);
            Module._MemFree(startPos_h);
            Module._MemFree(endPos_h);
            Module._MemFree(color_h);
        },

        DrawLineEx(startPos, endPos, thick, color) {
            const startPos_h = cartVector2(startPos);
            const endPos_h = cartVector2(endPos);
            const color_h = cartColor(color);
            Module._DrawLineEx(startPos_h, endPos_h, thick, color_h);
            Module._MemFree(startPos_h);
            Module._MemFree(endPos_h);
            Module._MemFree(color_h);
        },

        DrawLineStrip(points, pointCount, color) {
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._DrawLineStrip(points_h, pointCount, color_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        DrawLineBezier(startPos, endPos, thick, color) {
            const startPos_h = cartVector2(startPos);
            const endPos_h = cartVector2(endPos);
            const color_h = cartColor(color);
            Module._DrawLineBezier(startPos_h, endPos_h, thick, color_h);
            Module._MemFree(startPos_h);
            Module._MemFree(endPos_h);
            Module._MemFree(color_h);
        },

        DrawCircle(centerX, centerY, radius, color) {
            const color_h = cartColor(color);
            Module._DrawCircle(centerX, centerY, radius, color_h);
            Module._MemFree(color_h);
        },

        DrawCircleSector(center, radius, startAngle, endAngle, segments, color) {
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._DrawCircleSector(center_h, radius, startAngle, endAngle, segments, color_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        DrawCircleSectorLines(center, radius, startAngle, endAngle, segments, color) {
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._DrawCircleSectorLines(center_h, radius, startAngle, endAngle, segments, color_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        DrawCircleGradient(centerX, centerY, radius, inner, outer) {
            const inner_h = cartColor(inner);
            const outer_h = cartColor(outer);
            Module._DrawCircleGradient(centerX, centerY, radius, inner_h, outer_h);
            Module._MemFree(inner_h);
            Module._MemFree(outer_h);
        },

        DrawCircleV(center, radius, color) {
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._DrawCircleV(center_h, radius, color_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        DrawCircleLines(centerX, centerY, radius, color) {
            const color_h = cartColor(color);
            Module._DrawCircleLines(centerX, centerY, radius, color_h);
            Module._MemFree(color_h);
        },

        DrawCircleLinesV(center, radius, color) {
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._DrawCircleLinesV(center_h, radius, color_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        DrawEllipse(centerX, centerY, radiusH, radiusV, color) {
            const color_h = cartColor(color);
            Module._DrawEllipse(centerX, centerY, radiusH, radiusV, color_h);
            Module._MemFree(color_h);
        },

        DrawEllipseLines(centerX, centerY, radiusH, radiusV, color) {
            const color_h = cartColor(color);
            Module._DrawEllipseLines(centerX, centerY, radiusH, radiusV, color_h);
            Module._MemFree(color_h);
        },

        DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, color) {
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._DrawRing(center_h, innerRadius, outerRadius, startAngle, endAngle, segments, color_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, color) {
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._DrawRingLines(center_h, innerRadius, outerRadius, startAngle, endAngle, segments, color_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        DrawRectangle(posX, posY, width, height, color) {
            const color_h = cartColor(color);
            Module._DrawRectangle(posX, posY, width, height, color_h);
            Module._MemFree(color_h);
        },

        DrawRectangleV(position, size, color) {
            const position_h = cartVector2(position);
            const size_h = cartVector2(size);
            const color_h = cartColor(color);
            Module._DrawRectangleV(position_h, size_h, color_h);
            Module._MemFree(position_h);
            Module._MemFree(size_h);
            Module._MemFree(color_h);
        },

        DrawRectangleRec(rec, color) {
            const rec_h = cartRectangle(rec);
            const color_h = cartColor(color);
            Module._DrawRectangleRec(rec_h, color_h);
            Module._MemFree(rec_h);
            Module._MemFree(color_h);
        },

        DrawRectanglePro(rec, origin, rotation, color) {
            const rec_h = cartRectangle(rec);
            const origin_h = cartVector2(origin);
            const color_h = cartColor(color);
            Module._DrawRectanglePro(rec_h, origin_h, rotation, color_h);
            Module._MemFree(rec_h);
            Module._MemFree(origin_h);
            Module._MemFree(color_h);
        },

        DrawRectangleGradientV(posX, posY, width, height, top, bottom) {
            const top_h = cartColor(top);
            const bottom_h = cartColor(bottom);
            Module._DrawRectangleGradientV(posX, posY, width, height, top_h, bottom_h);
            Module._MemFree(top_h);
            Module._MemFree(bottom_h);
        },

        DrawRectangleGradientH(posX, posY, width, height, left, right) {
            const left_h = cartColor(left);
            const right_h = cartColor(right);
            Module._DrawRectangleGradientH(posX, posY, width, height, left_h, right_h);
            Module._MemFree(left_h);
            Module._MemFree(right_h);
        },

        DrawRectangleGradientEx(rec, topLeft, bottomLeft, bottomRight, topRight) {
            const rec_h = cartRectangle(rec);
            const topLeft_h = cartColor(topLeft);
            const bottomLeft_h = cartColor(bottomLeft);
            const bottomRight_h = cartColor(bottomRight);
            const topRight_h = cartColor(topRight);
            Module._DrawRectangleGradientEx(rec_h, topLeft_h, bottomLeft_h, bottomRight_h, topRight_h);
            Module._MemFree(rec_h);
            Module._MemFree(topLeft_h);
            Module._MemFree(bottomLeft_h);
            Module._MemFree(bottomRight_h);
            Module._MemFree(topRight_h);
        },

        DrawRectangleLines(posX, posY, width, height, color) {
            const color_h = cartColor(color);
            Module._DrawRectangleLines(posX, posY, width, height, color_h);
            Module._MemFree(color_h);
        },

        DrawRectangleLinesEx(rec, lineThick, color) {
            const rec_h = cartRectangle(rec);
            const color_h = cartColor(color);
            Module._DrawRectangleLinesEx(rec_h, lineThick, color_h);
            Module._MemFree(rec_h);
            Module._MemFree(color_h);
        },

        DrawRectangleRounded(rec, roundness, segments, color) {
            const rec_h = cartRectangle(rec);
            const color_h = cartColor(color);
            Module._DrawRectangleRounded(rec_h, roundness, segments, color_h);
            Module._MemFree(rec_h);
            Module._MemFree(color_h);
        },

        DrawRectangleRoundedLines(rec, roundness, segments, color) {
            const rec_h = cartRectangle(rec);
            const color_h = cartColor(color);
            Module._DrawRectangleRoundedLines(rec_h, roundness, segments, color_h);
            Module._MemFree(rec_h);
            Module._MemFree(color_h);
        },

        DrawRectangleRoundedLinesEx(rec, roundness, segments, lineThick, color) {
            const rec_h = cartRectangle(rec);
            const color_h = cartColor(color);
            Module._DrawRectangleRoundedLinesEx(rec_h, roundness, segments, lineThick, color_h);
            Module._MemFree(rec_h);
            Module._MemFree(color_h);
        },

        DrawTriangle(v1, v2, v3, color) {
            const v1_h = cartVector2(v1);
            const v2_h = cartVector2(v2);
            const v3_h = cartVector2(v3);
            const color_h = cartColor(color);
            Module._DrawTriangle(v1_h, v2_h, v3_h, color_h);
            Module._MemFree(v1_h);
            Module._MemFree(v2_h);
            Module._MemFree(v3_h);
            Module._MemFree(color_h);
        },

        DrawTriangleLines(v1, v2, v3, color) {
            const v1_h = cartVector2(v1);
            const v2_h = cartVector2(v2);
            const v3_h = cartVector2(v3);
            const color_h = cartColor(color);
            Module._DrawTriangleLines(v1_h, v2_h, v3_h, color_h);
            Module._MemFree(v1_h);
            Module._MemFree(v2_h);
            Module._MemFree(v3_h);
            Module._MemFree(color_h);
        },

        DrawTriangleFan(points, pointCount, color) {
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._DrawTriangleFan(points_h, pointCount, color_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        DrawTriangleStrip(points, pointCount, color) {
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._DrawTriangleStrip(points_h, pointCount, color_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        DrawPoly(center, sides, radius, rotation, color) {
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._DrawPoly(center_h, sides, radius, rotation, color_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        DrawPolyLines(center, sides, radius, rotation, color) {
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._DrawPolyLines(center_h, sides, radius, rotation, color_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        DrawPolyLinesEx(center, sides, radius, rotation, lineThick, color) {
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._DrawPolyLinesEx(center_h, sides, radius, rotation, lineThick, color_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        DrawSplineLinear(points, pointCount, thick, color) {
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._DrawSplineLinear(points_h, pointCount, thick, color_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        DrawSplineBasis(points, pointCount, thick, color) {
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._DrawSplineBasis(points_h, pointCount, thick, color_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        DrawSplineCatmullRom(points, pointCount, thick, color) {
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._DrawSplineCatmullRom(points_h, pointCount, thick, color_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        DrawSplineBezierQuadratic(points, pointCount, thick, color) {
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._DrawSplineBezierQuadratic(points_h, pointCount, thick, color_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        DrawSplineBezierCubic(points, pointCount, thick, color) {
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._DrawSplineBezierCubic(points_h, pointCount, thick, color_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        DrawSplineSegmentLinear(p1, p2, thick, color) {
            const p1_h = cartVector2(p1);
            const p2_h = cartVector2(p2);
            const color_h = cartColor(color);
            Module._DrawSplineSegmentLinear(p1_h, p2_h, thick, color_h);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            Module._MemFree(color_h);
        },

        DrawSplineSegmentBasis(p1, p2, p3, p4, thick, color) {
            const p1_h = cartVector2(p1);
            const p2_h = cartVector2(p2);
            const p3_h = cartVector2(p3);
            const p4_h = cartVector2(p4);
            const color_h = cartColor(color);
            Module._DrawSplineSegmentBasis(p1_h, p2_h, p3_h, p4_h, thick, color_h);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            Module._MemFree(p3_h);
            Module._MemFree(p4_h);
            Module._MemFree(color_h);
        },

        DrawSplineSegmentCatmullRom(p1, p2, p3, p4, thick, color) {
            const p1_h = cartVector2(p1);
            const p2_h = cartVector2(p2);
            const p3_h = cartVector2(p3);
            const p4_h = cartVector2(p4);
            const color_h = cartColor(color);
            Module._DrawSplineSegmentCatmullRom(p1_h, p2_h, p3_h, p4_h, thick, color_h);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            Module._MemFree(p3_h);
            Module._MemFree(p4_h);
            Module._MemFree(color_h);
        },

        DrawSplineSegmentBezierQuadratic(p1, c2, p3, thick, color) {
            const p1_h = cartVector2(p1);
            const c2_h = cartVector2(c2);
            const p3_h = cartVector2(p3);
            const color_h = cartColor(color);
            Module._DrawSplineSegmentBezierQuadratic(p1_h, c2_h, p3_h, thick, color_h);
            Module._MemFree(p1_h);
            Module._MemFree(c2_h);
            Module._MemFree(p3_h);
            Module._MemFree(color_h);
        },

        DrawSplineSegmentBezierCubic(p1, c2, c3, p4, thick, color) {
            const p1_h = cartVector2(p1);
            const c2_h = cartVector2(c2);
            const c3_h = cartVector2(c3);
            const p4_h = cartVector2(p4);
            const color_h = cartColor(color);
            Module._DrawSplineSegmentBezierCubic(p1_h, c2_h, c3_h, p4_h, thick, color_h);
            Module._MemFree(p1_h);
            Module._MemFree(c2_h);
            Module._MemFree(c3_h);
            Module._MemFree(p4_h);
            Module._MemFree(color_h);
        },

        GetSplinePointLinear(resultPtr, startPos, endPos, t) {
            const startPos_h = cartVector2(startPos);
            const endPos_h = cartVector2(endPos);
            const result_h = Module._MemAlloc(8);
            Module._GetSplinePointLinear(result_h, startPos_h, endPos_h, t);
            Module._MemFree(startPos_h);
            Module._MemFree(endPos_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetSplinePointBasis(resultPtr, p1, p2, p3, p4, t) {
            const p1_h = cartVector2(p1);
            const p2_h = cartVector2(p2);
            const p3_h = cartVector2(p3);
            const p4_h = cartVector2(p4);
            const result_h = Module._MemAlloc(8);
            Module._GetSplinePointBasis(result_h, p1_h, p2_h, p3_h, p4_h, t);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            Module._MemFree(p3_h);
            Module._MemFree(p4_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetSplinePointCatmullRom(resultPtr, p1, p2, p3, p4, t) {
            const p1_h = cartVector2(p1);
            const p2_h = cartVector2(p2);
            const p3_h = cartVector2(p3);
            const p4_h = cartVector2(p4);
            const result_h = Module._MemAlloc(8);
            Module._GetSplinePointCatmullRom(result_h, p1_h, p2_h, p3_h, p4_h, t);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            Module._MemFree(p3_h);
            Module._MemFree(p4_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetSplinePointBezierQuad(resultPtr, p1, c2, p3, t) {
            const p1_h = cartVector2(p1);
            const c2_h = cartVector2(c2);
            const p3_h = cartVector2(p3);
            const result_h = Module._MemAlloc(8);
            Module._GetSplinePointBezierQuad(result_h, p1_h, c2_h, p3_h, t);
            Module._MemFree(p1_h);
            Module._MemFree(c2_h);
            Module._MemFree(p3_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetSplinePointBezierCubic(resultPtr, p1, c2, c3, p4, t) {
            const p1_h = cartVector2(p1);
            const c2_h = cartVector2(c2);
            const c3_h = cartVector2(c3);
            const p4_h = cartVector2(p4);
            const result_h = Module._MemAlloc(8);
            Module._GetSplinePointBezierCubic(result_h, p1_h, c2_h, c3_h, p4_h, t);
            Module._MemFree(p1_h);
            Module._MemFree(c2_h);
            Module._MemFree(c3_h);
            Module._MemFree(p4_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        CheckCollisionRecs(rec1, rec2) {
            const rec1_h = cartRectangle(rec1);
            const rec2_h = cartRectangle(rec2);
            const result = Module._CheckCollisionRecs(rec1_h, rec2_h);
            Module._MemFree(rec1_h);
            Module._MemFree(rec2_h);
            return result;
        },

        CheckCollisionCircles(center1, radius1, center2, radius2) {
            const center1_h = cartVector2(center1);
            const center2_h = cartVector2(center2);
            const result = Module._CheckCollisionCircles(center1_h, radius1, center2_h, radius2);
            Module._MemFree(center1_h);
            Module._MemFree(center2_h);
            return result;
        },

        CheckCollisionCircleRec(center, radius, rec) {
            const center_h = cartVector2(center);
            const rec_h = cartRectangle(rec);
            const result = Module._CheckCollisionCircleRec(center_h, radius, rec_h);
            Module._MemFree(center_h);
            Module._MemFree(rec_h);
            return result;
        },

        CheckCollisionCircleLine(center, radius, p1, p2) {
            const center_h = cartVector2(center);
            const p1_h = cartVector2(p1);
            const p2_h = cartVector2(p2);
            const result = Module._CheckCollisionCircleLine(center_h, radius, p1_h, p2_h);
            Module._MemFree(center_h);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            return result;
        },

        CheckCollisionPointRec(point, rec) {
            const point_h = cartVector2(point);
            const rec_h = cartRectangle(rec);
            const result = Module._CheckCollisionPointRec(point_h, rec_h);
            Module._MemFree(point_h);
            Module._MemFree(rec_h);
            return result;
        },

        CheckCollisionPointCircle(point, center, radius) {
            const point_h = cartVector2(point);
            const center_h = cartVector2(center);
            const result = Module._CheckCollisionPointCircle(point_h, center_h, radius);
            Module._MemFree(point_h);
            Module._MemFree(center_h);
            return result;
        },

        CheckCollisionPointTriangle(point, p1, p2, p3) {
            const point_h = cartVector2(point);
            const p1_h = cartVector2(p1);
            const p2_h = cartVector2(p2);
            const p3_h = cartVector2(p3);
            const result = Module._CheckCollisionPointTriangle(point_h, p1_h, p2_h, p3_h);
            Module._MemFree(point_h);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            Module._MemFree(p3_h);
            return result;
        },

        CheckCollisionPointLine(point, p1, p2, threshold) {
            const point_h = cartVector2(point);
            const p1_h = cartVector2(p1);
            const p2_h = cartVector2(p2);
            const result = Module._CheckCollisionPointLine(point_h, p1_h, p2_h, threshold);
            Module._MemFree(point_h);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            return result;
        },

        CheckCollisionPointPoly(point, points, pointCount) {
            const point_h = cartVector2(point);
            const points_h = cartVector2(points);
            const result = Module._CheckCollisionPointPoly(point_h, points_h, pointCount);
            Module._MemFree(point_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            return result;
        },

        CheckCollisionLines(startPos1, endPos1, startPos2, endPos2, collisionPoint) {
            const startPos1_h = cartVector2(startPos1);
            const endPos1_h = cartVector2(endPos1);
            const startPos2_h = cartVector2(startPos2);
            const endPos2_h = cartVector2(endPos2);
            const collisionPoint_h = cartVector2(collisionPoint);
            const result = Module._CheckCollisionLines(startPos1_h, endPos1_h, startPos2_h, endPos2_h, collisionPoint_h);
            Module._MemFree(startPos1_h);
            Module._MemFree(endPos1_h);
            Module._MemFree(startPos2_h);
            Module._MemFree(endPos2_h);
            copyHostToCart(collisionPoint_h, collisionPoint, 8);
            Module._MemFree(collisionPoint_h);
            return result;
        },

        GetCollisionRec(resultPtr, rec1, rec2) {
            const rec1_h = cartRectangle(rec1);
            const rec2_h = cartRectangle(rec2);
            const result_h = Module._MemAlloc(16);
            Module._GetCollisionRec(result_h, rec1_h, rec2_h);
            Module._MemFree(rec1_h);
            Module._MemFree(rec2_h);
            copyHostToCart(result_h, resultPtr, 16);
            Module._MemFree(result_h);
        },

        LoadImage(resultPtr, fileName) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(20);
            Module._LoadImage(result_h, fileName_h);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadImageRaw(resultPtr, fileName, width, height, format, headerSize) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(20);
            Module._LoadImageRaw(result_h, fileName_h, width, height, format, headerSize);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadImageAnim(resultPtr, fileName, frames) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(20);
            Module._LoadImageAnim(result_h, fileName_h, frames);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadImageAnimFromMemory(resultPtr, fileType, fileData, dataSize, frames) {
            const fileType_h = cartString(fileType);
            const fileData_h = cartString(fileData);
            const result_h = Module._MemAlloc(20);
            Module._LoadImageAnimFromMemory(result_h, fileType_h, fileData_h, dataSize, frames);
            Module._MemFree(fileType_h);
            Module._MemFree(fileData_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadImageFromMemory(resultPtr, fileType, fileData, dataSize) {
            const fileType_h = cartString(fileType);
            const fileData_h = cartString(fileData);
            const result_h = Module._MemAlloc(20);
            Module._LoadImageFromMemory(result_h, fileType_h, fileData_h, dataSize);
            Module._MemFree(fileType_h);
            Module._MemFree(fileData_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadImageFromTexture(resultPtr, texture) {
            const texture_h = cartTexture(texture);
            const result_h = Module._MemAlloc(20);
            Module._LoadImageFromTexture(result_h, texture_h);
            Module._MemFree(texture_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadImageFromScreen(resultPtr) {
            const result_h = Module._MemAlloc(20);
            Module._LoadImageFromScreen(result_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        IsImageValid(image) {
            const image_h = cartImage(image);
            const result = Module._IsImageValid(image_h);
            Module._MemFree(image_h);
            return result;
        },

        UnloadImage(image) {
            const image_h = cartImage(image);
            Module._UnloadImage(image_h);
            Module._MemFree(image_h);
        },

        ExportImage(image, fileName) {
            const image_h = cartImage(image);
            const fileName_h = cartString(fileName);
            const result = Module._ExportImage(image_h, fileName_h);
            Module._MemFree(image_h);
            Module._MemFree(fileName_h);
            return result;
        },

        ExportImageToMemory(image, fileType, fileSize) {
            const image_h = cartImage(image);
            const fileType_h = cartString(fileType);
            const result = Module._ExportImageToMemory(image_h, fileType_h, fileSize);
            Module._MemFree(image_h);
            Module._MemFree(fileType_h);
            return result;
        },

        ExportImageAsCode(image, fileName) {
            const image_h = cartImage(image);
            const fileName_h = cartString(fileName);
            const result = Module._ExportImageAsCode(image_h, fileName_h);
            Module._MemFree(image_h);
            Module._MemFree(fileName_h);
            return result;
        },

        GenImageColor(resultPtr, width, height, color) {
            const color_h = cartColor(color);
            const result_h = Module._MemAlloc(20);
            Module._GenImageColor(result_h, width, height, color_h);
            Module._MemFree(color_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        GenImageGradientLinear(resultPtr, width, height, direction, start, end) {
            const start_h = cartColor(start);
            const end_h = cartColor(end);
            const result_h = Module._MemAlloc(20);
            Module._GenImageGradientLinear(result_h, width, height, direction, start_h, end_h);
            Module._MemFree(start_h);
            Module._MemFree(end_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        GenImageGradientRadial(resultPtr, width, height, density, inner, outer) {
            const inner_h = cartColor(inner);
            const outer_h = cartColor(outer);
            const result_h = Module._MemAlloc(20);
            Module._GenImageGradientRadial(result_h, width, height, density, inner_h, outer_h);
            Module._MemFree(inner_h);
            Module._MemFree(outer_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        GenImageGradientSquare(resultPtr, width, height, density, inner, outer) {
            const inner_h = cartColor(inner);
            const outer_h = cartColor(outer);
            const result_h = Module._MemAlloc(20);
            Module._GenImageGradientSquare(result_h, width, height, density, inner_h, outer_h);
            Module._MemFree(inner_h);
            Module._MemFree(outer_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        GenImageChecked(resultPtr, width, height, checksX, checksY, col1, col2) {
            const col1_h = cartColor(col1);
            const col2_h = cartColor(col2);
            const result_h = Module._MemAlloc(20);
            Module._GenImageChecked(result_h, width, height, checksX, checksY, col1_h, col2_h);
            Module._MemFree(col1_h);
            Module._MemFree(col2_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        GenImageWhiteNoise(resultPtr, width, height, factor) {
            const result_h = Module._MemAlloc(20);
            Module._GenImageWhiteNoise(result_h, width, height, factor);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        GenImagePerlinNoise(resultPtr, width, height, offsetX, offsetY, scale) {
            const result_h = Module._MemAlloc(20);
            Module._GenImagePerlinNoise(result_h, width, height, offsetX, offsetY, scale);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        GenImageCellular(resultPtr, width, height, tileSize) {
            const result_h = Module._MemAlloc(20);
            Module._GenImageCellular(result_h, width, height, tileSize);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        GenImageText(resultPtr, width, height, text) {
            const text_h = cartString(text);
            const result_h = Module._MemAlloc(20);
            Module._GenImageText(result_h, width, height, text_h);
            Module._MemFree(text_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        ImageCopy(resultPtr, image) {
            const image_h = cartImage(image);
            const result_h = Module._MemAlloc(20);
            Module._ImageCopy(result_h, image_h);
            Module._MemFree(image_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        ImageFromImage(resultPtr, image, rec) {
            const image_h = cartImage(image);
            const rec_h = cartRectangle(rec);
            const result_h = Module._MemAlloc(20);
            Module._ImageFromImage(result_h, image_h, rec_h);
            Module._MemFree(image_h);
            Module._MemFree(rec_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        ImageFromChannel(resultPtr, image, selectedChannel) {
            const image_h = cartImage(image);
            const result_h = Module._MemAlloc(20);
            Module._ImageFromChannel(result_h, image_h, selectedChannel);
            Module._MemFree(image_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        ImageText(resultPtr, text, fontSize, color) {
            const text_h = cartString(text);
            const color_h = cartColor(color);
            const result_h = Module._MemAlloc(20);
            Module._ImageText(result_h, text_h, fontSize, color_h);
            Module._MemFree(text_h);
            Module._MemFree(color_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        ImageTextEx(resultPtr, font, text, fontSize, spacing, tint) {
            const font_h = cartFont(font);
            const text_h = cartString(text);
            const tint_h = cartColor(tint);
            const result_h = Module._MemAlloc(20);
            Module._ImageTextEx(result_h, font_h, text_h, fontSize, spacing, tint_h);
            Module._MemFree(font_h);
            Module._MemFree(text_h);
            Module._MemFree(tint_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        ImageFormat(image, newFormat) {
            const image_h = cartImage(image);
            Module._ImageFormat(image_h, newFormat);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageToPOT(image, fill) {
            const image_h = cartImage(image);
            const fill_h = cartColor(fill);
            Module._ImageToPOT(image_h, fill_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
            Module._MemFree(fill_h);
        },

        ImageCrop(image, crop) {
            const image_h = cartImage(image);
            const crop_h = cartRectangle(crop);
            Module._ImageCrop(image_h, crop_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
            Module._MemFree(crop_h);
        },

        ImageAlphaCrop(image, threshold) {
            const image_h = cartImage(image);
            Module._ImageAlphaCrop(image_h, threshold);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageAlphaClear(image, color, threshold) {
            const image_h = cartImage(image);
            const color_h = cartColor(color);
            Module._ImageAlphaClear(image_h, color_h, threshold);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
            Module._MemFree(color_h);
        },

        ImageAlphaMask(image, alphaMask) {
            const image_h = cartImage(image);
            const alphaMask_h = cartImage(alphaMask);
            Module._ImageAlphaMask(image_h, alphaMask_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
            Module._MemFree(alphaMask_h);
        },

        ImageAlphaPremultiply(image) {
            const image_h = cartImage(image);
            Module._ImageAlphaPremultiply(image_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageBlurGaussian(image, blurSize) {
            const image_h = cartImage(image);
            Module._ImageBlurGaussian(image_h, blurSize);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageKernelConvolution(image, kernel, kernelSize) {
            const image_h = cartImage(image);
            Module._ImageKernelConvolution(image_h, kernel, kernelSize);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageResize(image, newWidth, newHeight) {
            const image_h = cartImage(image);
            Module._ImageResize(image_h, newWidth, newHeight);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageResizeNN(image, newWidth, newHeight) {
            const image_h = cartImage(image);
            Module._ImageResizeNN(image_h, newWidth, newHeight);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageResizeCanvas(image, newWidth, newHeight, offsetX, offsetY, fill) {
            const image_h = cartImage(image);
            const fill_h = cartColor(fill);
            Module._ImageResizeCanvas(image_h, newWidth, newHeight, offsetX, offsetY, fill_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
            Module._MemFree(fill_h);
        },

        ImageMipmaps(image) {
            const image_h = cartImage(image);
            Module._ImageMipmaps(image_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageDither(image, rBpp, gBpp, bBpp, aBpp) {
            const image_h = cartImage(image);
            Module._ImageDither(image_h, rBpp, gBpp, bBpp, aBpp);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageFlipVertical(image) {
            const image_h = cartImage(image);
            Module._ImageFlipVertical(image_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageFlipHorizontal(image) {
            const image_h = cartImage(image);
            Module._ImageFlipHorizontal(image_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageRotate(image, degrees) {
            const image_h = cartImage(image);
            Module._ImageRotate(image_h, degrees);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageRotateCW(image) {
            const image_h = cartImage(image);
            Module._ImageRotateCW(image_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageRotateCCW(image) {
            const image_h = cartImage(image);
            Module._ImageRotateCCW(image_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageColorTint(image, color) {
            const image_h = cartImage(image);
            const color_h = cartColor(color);
            Module._ImageColorTint(image_h, color_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
            Module._MemFree(color_h);
        },

        ImageColorInvert(image) {
            const image_h = cartImage(image);
            Module._ImageColorInvert(image_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageColorGrayscale(image) {
            const image_h = cartImage(image);
            Module._ImageColorGrayscale(image_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageColorContrast(image, contrast) {
            const image_h = cartImage(image);
            Module._ImageColorContrast(image_h, contrast);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageColorBrightness(image, brightness) {
            const image_h = cartImage(image);
            Module._ImageColorBrightness(image_h, brightness);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
        },

        ImageColorReplace(image, color, replace) {
            const image_h = cartImage(image);
            const color_h = cartColor(color);
            const replace_h = cartColor(replace);
            Module._ImageColorReplace(image_h, color_h, replace_h);
            copyHostToCart(image_h, image, 20);
            Module._MemFree(image_h);
            Module._MemFree(color_h);
            Module._MemFree(replace_h);
        },

        UnloadImageColors(colors) {
            const colors_h = cartColor(colors);
            Module._UnloadImageColors(colors_h);
            copyHostToCart(colors_h, colors, 4);
            Module._MemFree(colors_h);
        },

        UnloadImagePalette(colors) {
            const colors_h = cartColor(colors);
            Module._UnloadImagePalette(colors_h);
            copyHostToCart(colors_h, colors, 4);
            Module._MemFree(colors_h);
        },

        GetImageAlphaBorder(resultPtr, image, threshold) {
            const image_h = cartImage(image);
            const result_h = Module._MemAlloc(16);
            Module._GetImageAlphaBorder(result_h, image_h, threshold);
            Module._MemFree(image_h);
            copyHostToCart(result_h, resultPtr, 16);
            Module._MemFree(result_h);
        },

        GetImageColor(resultPtr, image, x, y) {
            const image_h = cartImage(image);
            const result_h = Module._MemAlloc(4);
            Module._GetImageColor(result_h, image_h, x, y);
            Module._MemFree(image_h);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        ImageClearBackground(dst, color) {
            const dst_h = cartImage(dst);
            const color_h = cartColor(color);
            Module._ImageClearBackground(dst_h, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(color_h);
        },

        ImageDrawPixel(dst, posX, posY, color) {
            const dst_h = cartImage(dst);
            const color_h = cartColor(color);
            Module._ImageDrawPixel(dst_h, posX, posY, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(color_h);
        },

        ImageDrawPixelV(dst, position, color) {
            const dst_h = cartImage(dst);
            const position_h = cartVector2(position);
            const color_h = cartColor(color);
            Module._ImageDrawPixelV(dst_h, position_h, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(position_h);
            Module._MemFree(color_h);
        },

        ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, color) {
            const dst_h = cartImage(dst);
            const color_h = cartColor(color);
            Module._ImageDrawLine(dst_h, startPosX, startPosY, endPosX, endPosY, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(color_h);
        },

        ImageDrawLineV(dst, start, end, color) {
            const dst_h = cartImage(dst);
            const start_h = cartVector2(start);
            const end_h = cartVector2(end);
            const color_h = cartColor(color);
            Module._ImageDrawLineV(dst_h, start_h, end_h, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(start_h);
            Module._MemFree(end_h);
            Module._MemFree(color_h);
        },

        ImageDrawLineEx(dst, start, end, thick, color) {
            const dst_h = cartImage(dst);
            const start_h = cartVector2(start);
            const end_h = cartVector2(end);
            const color_h = cartColor(color);
            Module._ImageDrawLineEx(dst_h, start_h, end_h, thick, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(start_h);
            Module._MemFree(end_h);
            Module._MemFree(color_h);
        },

        ImageDrawCircle(dst, centerX, centerY, radius, color) {
            const dst_h = cartImage(dst);
            const color_h = cartColor(color);
            Module._ImageDrawCircle(dst_h, centerX, centerY, radius, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(color_h);
        },

        ImageDrawCircleV(dst, center, radius, color) {
            const dst_h = cartImage(dst);
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._ImageDrawCircleV(dst_h, center_h, radius, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        ImageDrawCircleLines(dst, centerX, centerY, radius, color) {
            const dst_h = cartImage(dst);
            const color_h = cartColor(color);
            Module._ImageDrawCircleLines(dst_h, centerX, centerY, radius, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(color_h);
        },

        ImageDrawCircleLinesV(dst, center, radius, color) {
            const dst_h = cartImage(dst);
            const center_h = cartVector2(center);
            const color_h = cartColor(color);
            Module._ImageDrawCircleLinesV(dst_h, center_h, radius, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(center_h);
            Module._MemFree(color_h);
        },

        ImageDrawRectangle(dst, posX, posY, width, height, color) {
            const dst_h = cartImage(dst);
            const color_h = cartColor(color);
            Module._ImageDrawRectangle(dst_h, posX, posY, width, height, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(color_h);
        },

        ImageDrawRectangleV(dst, position, size, color) {
            const dst_h = cartImage(dst);
            const position_h = cartVector2(position);
            const size_h = cartVector2(size);
            const color_h = cartColor(color);
            Module._ImageDrawRectangleV(dst_h, position_h, size_h, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(position_h);
            Module._MemFree(size_h);
            Module._MemFree(color_h);
        },

        ImageDrawRectangleRec(dst, rec, color) {
            const dst_h = cartImage(dst);
            const rec_h = cartRectangle(rec);
            const color_h = cartColor(color);
            Module._ImageDrawRectangleRec(dst_h, rec_h, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(rec_h);
            Module._MemFree(color_h);
        },

        ImageDrawRectangleLines(dst, rec, thick, color) {
            const dst_h = cartImage(dst);
            const rec_h = cartRectangle(rec);
            const color_h = cartColor(color);
            Module._ImageDrawRectangleLines(dst_h, rec_h, thick, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(rec_h);
            Module._MemFree(color_h);
        },

        ImageDrawTriangle(dst, v1, v2, v3, color) {
            const dst_h = cartImage(dst);
            const v1_h = cartVector2(v1);
            const v2_h = cartVector2(v2);
            const v3_h = cartVector2(v3);
            const color_h = cartColor(color);
            Module._ImageDrawTriangle(dst_h, v1_h, v2_h, v3_h, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(v1_h);
            Module._MemFree(v2_h);
            Module._MemFree(v3_h);
            Module._MemFree(color_h);
        },

        ImageDrawTriangleEx(dst, v1, v2, v3, c1, c2, c3) {
            const dst_h = cartImage(dst);
            const v1_h = cartVector2(v1);
            const v2_h = cartVector2(v2);
            const v3_h = cartVector2(v3);
            const c1_h = cartColor(c1);
            const c2_h = cartColor(c2);
            const c3_h = cartColor(c3);
            Module._ImageDrawTriangleEx(dst_h, v1_h, v2_h, v3_h, c1_h, c2_h, c3_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(v1_h);
            Module._MemFree(v2_h);
            Module._MemFree(v3_h);
            Module._MemFree(c1_h);
            Module._MemFree(c2_h);
            Module._MemFree(c3_h);
        },

        ImageDrawTriangleLines(dst, v1, v2, v3, color) {
            const dst_h = cartImage(dst);
            const v1_h = cartVector2(v1);
            const v2_h = cartVector2(v2);
            const v3_h = cartVector2(v3);
            const color_h = cartColor(color);
            Module._ImageDrawTriangleLines(dst_h, v1_h, v2_h, v3_h, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(v1_h);
            Module._MemFree(v2_h);
            Module._MemFree(v3_h);
            Module._MemFree(color_h);
        },

        ImageDrawTriangleFan(dst, points, pointCount, color) {
            const dst_h = cartImage(dst);
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._ImageDrawTriangleFan(dst_h, points_h, pointCount, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        ImageDrawTriangleStrip(dst, points, pointCount, color) {
            const dst_h = cartImage(dst);
            const points_h = cartVector2(points);
            const color_h = cartColor(color);
            Module._ImageDrawTriangleStrip(dst_h, points_h, pointCount, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            copyHostToCart(points_h, points, 8);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        ImageDraw(dst, src, srcRec, dstRec, tint) {
            const dst_h = cartImage(dst);
            const src_h = cartImage(src);
            const srcRec_h = cartRectangle(srcRec);
            const dstRec_h = cartRectangle(dstRec);
            const tint_h = cartColor(tint);
            Module._ImageDraw(dst_h, src_h, srcRec_h, dstRec_h, tint_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(src_h);
            Module._MemFree(srcRec_h);
            Module._MemFree(dstRec_h);
            Module._MemFree(tint_h);
        },

        ImageDrawText(dst, text, posX, posY, fontSize, color) {
            const dst_h = cartImage(dst);
            const text_h = cartString(text);
            const color_h = cartColor(color);
            Module._ImageDrawText(dst_h, text_h, posX, posY, fontSize, color_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(text_h);
            Module._MemFree(color_h);
        },

        ImageDrawTextEx(dst, font, text, position, fontSize, spacing, tint) {
            const dst_h = cartImage(dst);
            const font_h = cartFont(font);
            const text_h = cartString(text);
            const position_h = cartVector2(position);
            const tint_h = cartColor(tint);
            Module._ImageDrawTextEx(dst_h, font_h, text_h, position_h, fontSize, spacing, tint_h);
            copyHostToCart(dst_h, dst, 20);
            Module._MemFree(dst_h);
            Module._MemFree(font_h);
            Module._MemFree(text_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        LoadTexture(resultPtr, fileName) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(20);
            Module._LoadTexture(result_h, fileName_h);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadTextureFromImage(resultPtr, image) {
            const image_h = cartImage(image);
            const result_h = Module._MemAlloc(20);
            Module._LoadTextureFromImage(result_h, image_h);
            Module._MemFree(image_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadTextureCubemap(resultPtr, image, layout) {
            const image_h = cartImage(image);
            const result_h = Module._MemAlloc(20);
            Module._LoadTextureCubemap(result_h, image_h, layout);
            Module._MemFree(image_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadRenderTexture(resultPtr, width, height) {
            const result_h = Module._MemAlloc(44);
            Module._LoadRenderTexture(result_h, width, height);
            copyHostToCart(result_h, resultPtr, 44);
            Module._MemFree(result_h);
        },

        IsTextureValid(texture) {
            const texture_h = cartTexture(texture);
            const result = Module._IsTextureValid(texture_h);
            Module._MemFree(texture_h);
            return result;
        },

        UnloadTexture(texture) {
            const texture_h = cartTexture(texture);
            Module._UnloadTexture(texture_h);
            Module._MemFree(texture_h);
        },

        IsRenderTextureValid(target) {
            const target_h = cartRenderTexture(target);
            const result = Module._IsRenderTextureValid(target_h);
            Module._MemFree(target_h);
            return result;
        },

        UnloadRenderTexture(target) {
            const target_h = cartRenderTexture(target);
            Module._UnloadRenderTexture(target_h);
            Module._MemFree(target_h);
        },

        UpdateTexture(texture, pixels) {
            const texture_h = cartTexture(texture);
            Module._UpdateTexture(texture_h, pixels);
            Module._MemFree(texture_h);
        },

        UpdateTextureRec(texture, rec, pixels) {
            const texture_h = cartTexture(texture);
            const rec_h = cartRectangle(rec);
            Module._UpdateTextureRec(texture_h, rec_h, pixels);
            Module._MemFree(texture_h);
            Module._MemFree(rec_h);
        },

        GenTextureMipmaps(texture) {
            const texture_h = cartTexture(texture);
            Module._GenTextureMipmaps(texture_h);
            copyHostToCart(texture_h, texture, 20);
            Module._MemFree(texture_h);
        },

        SetTextureFilter(texture, filter) {
            const texture_h = cartTexture(texture);
            Module._SetTextureFilter(texture_h, filter);
            Module._MemFree(texture_h);
        },

        SetTextureWrap(texture, wrap) {
            const texture_h = cartTexture(texture);
            Module._SetTextureWrap(texture_h, wrap);
            Module._MemFree(texture_h);
        },

        DrawTexture(texture, posX, posY, tint) {
            const texture_h = cartTexture(texture);
            const tint_h = cartColor(tint);
            Module._DrawTexture(texture_h, posX, posY, tint_h);
            Module._MemFree(texture_h);
            Module._MemFree(tint_h);
        },

        DrawTextureV(texture, position, tint) {
            const texture_h = cartTexture(texture);
            const position_h = cartVector2(position);
            const tint_h = cartColor(tint);
            Module._DrawTextureV(texture_h, position_h, tint_h);
            Module._MemFree(texture_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        DrawTextureEx(texture, position, rotation, scale, tint) {
            const texture_h = cartTexture(texture);
            const position_h = cartVector2(position);
            const tint_h = cartColor(tint);
            Module._DrawTextureEx(texture_h, position_h, rotation, scale, tint_h);
            Module._MemFree(texture_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        DrawTextureRec(texture, source, position, tint) {
            const texture_h = cartTexture(texture);
            const source_h = cartRectangle(source);
            const position_h = cartVector2(position);
            const tint_h = cartColor(tint);
            Module._DrawTextureRec(texture_h, source_h, position_h, tint_h);
            Module._MemFree(texture_h);
            Module._MemFree(source_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        DrawTexturePro(texture, source, dest, origin, rotation, tint) {
            const texture_h = cartTexture(texture);
            const source_h = cartRectangle(source);
            const dest_h = cartRectangle(dest);
            const origin_h = cartVector2(origin);
            const tint_h = cartColor(tint);
            Module._DrawTexturePro(texture_h, source_h, dest_h, origin_h, rotation, tint_h);
            Module._MemFree(texture_h);
            Module._MemFree(source_h);
            Module._MemFree(dest_h);
            Module._MemFree(origin_h);
            Module._MemFree(tint_h);
        },

        DrawTextureNPatch(texture, nPatchInfo, dest, origin, rotation, tint) {
            const texture_h = cartTexture(texture);
            const nPatchInfo_h = cartNPatchInfo(nPatchInfo);
            const dest_h = cartRectangle(dest);
            const origin_h = cartVector2(origin);
            const tint_h = cartColor(tint);
            Module._DrawTextureNPatch(texture_h, nPatchInfo_h, dest_h, origin_h, rotation, tint_h);
            Module._MemFree(texture_h);
            Module._MemFree(nPatchInfo_h);
            Module._MemFree(dest_h);
            Module._MemFree(origin_h);
            Module._MemFree(tint_h);
        },

        ColorIsEqual(col1, col2) {
            const col1_h = cartColor(col1);
            const col2_h = cartColor(col2);
            const result = Module._ColorIsEqual(col1_h, col2_h);
            Module._MemFree(col1_h);
            Module._MemFree(col2_h);
            return result;
        },

        Fade(resultPtr, color, alpha) {
            const color_h = cartColor(color);
            const result_h = Module._MemAlloc(4);
            Module._Fade(result_h, color_h, alpha);
            Module._MemFree(color_h);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        ColorToInt(color) {
            const color_h = cartColor(color);
            const result = Module._ColorToInt(color_h);
            Module._MemFree(color_h);
            return result;
        },

        ColorNormalize(resultPtr, color) {
            const color_h = cartColor(color);
            const result_h = Module._MemAlloc(16);
            Module._ColorNormalize(result_h, color_h);
            Module._MemFree(color_h);
            copyHostToCart(result_h, resultPtr, 16);
            Module._MemFree(result_h);
        },

        ColorFromNormalized(resultPtr, normalized) {
            const normalized_h = cartVector4(normalized);
            const result_h = Module._MemAlloc(4);
            Module._ColorFromNormalized(result_h, normalized_h);
            Module._MemFree(normalized_h);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        ColorToHSV(resultPtr, color) {
            const color_h = cartColor(color);
            const result_h = Module._MemAlloc(12);
            Module._ColorToHSV(result_h, color_h);
            Module._MemFree(color_h);
            copyHostToCart(result_h, resultPtr, 12);
            Module._MemFree(result_h);
        },

        ColorFromHSV(resultPtr, hue, saturation, value) {
            const result_h = Module._MemAlloc(4);
            Module._ColorFromHSV(result_h, hue, saturation, value);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        ColorTint(resultPtr, color, tint) {
            const color_h = cartColor(color);
            const tint_h = cartColor(tint);
            const result_h = Module._MemAlloc(4);
            Module._ColorTint(result_h, color_h, tint_h);
            Module._MemFree(color_h);
            Module._MemFree(tint_h);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        ColorBrightness(resultPtr, color, factor) {
            const color_h = cartColor(color);
            const result_h = Module._MemAlloc(4);
            Module._ColorBrightness(result_h, color_h, factor);
            Module._MemFree(color_h);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        ColorContrast(resultPtr, color, contrast) {
            const color_h = cartColor(color);
            const result_h = Module._MemAlloc(4);
            Module._ColorContrast(result_h, color_h, contrast);
            Module._MemFree(color_h);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        ColorAlpha(resultPtr, color, alpha) {
            const color_h = cartColor(color);
            const result_h = Module._MemAlloc(4);
            Module._ColorAlpha(result_h, color_h, alpha);
            Module._MemFree(color_h);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        ColorAlphaBlend(resultPtr, dst, src, tint) {
            const dst_h = cartColor(dst);
            const src_h = cartColor(src);
            const tint_h = cartColor(tint);
            const result_h = Module._MemAlloc(4);
            Module._ColorAlphaBlend(result_h, dst_h, src_h, tint_h);
            Module._MemFree(dst_h);
            Module._MemFree(src_h);
            Module._MemFree(tint_h);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        ColorLerp(resultPtr, color1, color2, factor) {
            const color1_h = cartColor(color1);
            const color2_h = cartColor(color2);
            const result_h = Module._MemAlloc(4);
            Module._ColorLerp(result_h, color1_h, color2_h, factor);
            Module._MemFree(color1_h);
            Module._MemFree(color2_h);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        GetColor(resultPtr, hexValue) {
            const result_h = Module._MemAlloc(4);
            Module._GetColor(result_h, hexValue);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        GetPixelColor(resultPtr, srcPtr, format) {
            const result_h = Module._MemAlloc(4);
            Module._GetPixelColor(result_h, srcPtr, format);
            copyHostToCart(result_h, resultPtr, 4);
            Module._MemFree(result_h);
        },

        SetPixelColor(dstPtr, color, format) {
            const color_h = cartColor(color);
            Module._SetPixelColor(dstPtr, color_h, format);
            Module._MemFree(color_h);
        },

        GetPixelDataSize(width, height, format) {
            const result = Module._GetPixelDataSize(width, height, format);
            return result;
        },

        GetFontDefault(resultPtr) {
            const result_h = Module._MemAlloc(40);
            Module._GetFontDefault(result_h);
            copyHostToCart(result_h, resultPtr, 40);
            Module._MemFree(result_h);
        },

        LoadFont(resultPtr, fileName) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(40);
            Module._LoadFont(result_h, fileName_h);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 40);
            Module._MemFree(result_h);
        },

        LoadFontEx(resultPtr, fileName, fontSize, codepoints, codepointCount) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(40);
            Module._LoadFontEx(result_h, fileName_h, fontSize, codepoints, codepointCount);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 40);
            Module._MemFree(result_h);
        },

        LoadFontFromImage(resultPtr, image, key, firstChar) {
            const image_h = cartImage(image);
            const key_h = cartColor(key);
            const result_h = Module._MemAlloc(40);
            Module._LoadFontFromImage(result_h, image_h, key_h, firstChar);
            Module._MemFree(image_h);
            Module._MemFree(key_h);
            copyHostToCart(result_h, resultPtr, 40);
            Module._MemFree(result_h);
        },

        LoadFontFromMemory(resultPtr, fileType, fileData, dataSize, fontSize, codepoints, codepointCount) {
            const fileType_h = cartString(fileType);
            const fileData_h = cartString(fileData);
            const result_h = Module._MemAlloc(40);
            Module._LoadFontFromMemory(result_h, fileType_h, fileData_h, dataSize, fontSize, codepoints, codepointCount);
            Module._MemFree(fileType_h);
            Module._MemFree(fileData_h);
            copyHostToCart(result_h, resultPtr, 40);
            Module._MemFree(result_h);
        },

        IsFontValid(font) {
            const font_h = cartFont(font);
            const result = Module._IsFontValid(font_h);
            Module._MemFree(font_h);
            return result;
        },

        LoadFontData(fileData, dataSize, fontSize, codepoints, codepointCount, type, glyphCount) {
            const fileData_h = cartString(fileData);
            const result = Module._LoadFontData(fileData_h, dataSize, fontSize, codepoints, codepointCount, type, glyphCount);
            Module._MemFree(fileData_h);
            return result;
        },

        GenImageFontAtlas(resultPtr, glyphs, glyphRecs, glyphCount, fontSize, padding, packMethod) {
            const glyphs_h = cartGlyphInfo(glyphs);
            const glyphRecs_h = cartRectangle(glyphRecs);
            const result_h = Module._MemAlloc(20);
            Module._GenImageFontAtlas(result_h, glyphs_h, glyphRecs_h, glyphCount, fontSize, padding, packMethod);
            copyHostToCart(glyphs_h, glyphs, 36);
            Module._MemFree(glyphs_h);
            copyHostToCart(glyphRecs_h, glyphRecs, 16);
            Module._MemFree(glyphRecs_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        UnloadFontData(glyphs, glyphCount) {
            const glyphs_h = cartGlyphInfo(glyphs);
            Module._UnloadFontData(glyphs_h, glyphCount);
            copyHostToCart(glyphs_h, glyphs, 36);
            Module._MemFree(glyphs_h);
        },

        UnloadFont(font) {
            const font_h = cartFont(font);
            Module._UnloadFont(font_h);
            Module._MemFree(font_h);
        },

        ExportFontAsCode(font, fileName) {
            const font_h = cartFont(font);
            const fileName_h = cartString(fileName);
            const result = Module._ExportFontAsCode(font_h, fileName_h);
            Module._MemFree(font_h);
            Module._MemFree(fileName_h);
            return result;
        },

        DrawFPS(posX, posY) {
            Module._DrawFPS(posX, posY);
        },

        DrawText(text, posX, posY, fontSize, color) {
            const text_h = cartString(text);
            const color_h = cartColor(color);
            Module._DrawText(text_h, posX, posY, fontSize, color_h);
            Module._MemFree(text_h);
            Module._MemFree(color_h);
        },

        DrawTextEx(font, text, position, fontSize, spacing, tint) {
            const font_h = cartFont(font);
            const text_h = cartString(text);
            const position_h = cartVector2(position);
            const tint_h = cartColor(tint);
            Module._DrawTextEx(font_h, text_h, position_h, fontSize, spacing, tint_h);
            Module._MemFree(font_h);
            Module._MemFree(text_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        DrawTextPro(font, text, position, origin, rotation, fontSize, spacing, tint) {
            const font_h = cartFont(font);
            const text_h = cartString(text);
            const position_h = cartVector2(position);
            const origin_h = cartVector2(origin);
            const tint_h = cartColor(tint);
            Module._DrawTextPro(font_h, text_h, position_h, origin_h, rotation, fontSize, spacing, tint_h);
            Module._MemFree(font_h);
            Module._MemFree(text_h);
            Module._MemFree(position_h);
            Module._MemFree(origin_h);
            Module._MemFree(tint_h);
        },

        DrawTextCodepoint(font, codepoint, position, fontSize, tint) {
            const font_h = cartFont(font);
            const position_h = cartVector2(position);
            const tint_h = cartColor(tint);
            Module._DrawTextCodepoint(font_h, codepoint, position_h, fontSize, tint_h);
            Module._MemFree(font_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        DrawTextCodepoints(font, codepoints, codepointCount, position, fontSize, spacing, tint) {
            const font_h = cartFont(font);
            const position_h = cartVector2(position);
            const tint_h = cartColor(tint);
            Module._DrawTextCodepoints(font_h, codepoints, codepointCount, position_h, fontSize, spacing, tint_h);
            Module._MemFree(font_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        SetTextLineSpacing(spacing) {
            Module._SetTextLineSpacing(spacing);
        },

        MeasureText(text, fontSize) {
            const text_h = cartString(text);
            const result = Module._MeasureText(text_h, fontSize);
            Module._MemFree(text_h);
            return result;
        },

        MeasureTextEx(resultPtr, font, text, fontSize, spacing) {
            const font_h = cartFont(font);
            const text_h = cartString(text);
            const result_h = Module._MemAlloc(8);
            Module._MeasureTextEx(result_h, font_h, text_h, fontSize, spacing);
            Module._MemFree(font_h);
            Module._MemFree(text_h);
            copyHostToCart(result_h, resultPtr, 8);
            Module._MemFree(result_h);
        },

        GetGlyphIndex(font, codepoint) {
            const font_h = cartFont(font);
            const result = Module._GetGlyphIndex(font_h, codepoint);
            Module._MemFree(font_h);
            return result;
        },

        GetGlyphInfo(resultPtr, font, codepoint) {
            const font_h = cartFont(font);
            const result_h = Module._MemAlloc(36);
            Module._GetGlyphInfo(result_h, font_h, codepoint);
            Module._MemFree(font_h);
            copyHostToCart(result_h, resultPtr, 36);
            Module._MemFree(result_h);
        },

        GetGlyphAtlasRec(resultPtr, font, codepoint) {
            const font_h = cartFont(font);
            const result_h = Module._MemAlloc(16);
            Module._GetGlyphAtlasRec(result_h, font_h, codepoint);
            Module._MemFree(font_h);
            copyHostToCart(result_h, resultPtr, 16);
            Module._MemFree(result_h);
        },

        LoadUTF8(codepoints, length) {
            const result = Module._LoadUTF8(codepoints, length);
            return result;
        },

        UnloadUTF8(text) {
            const text_h = cartString(text);
            Module._UnloadUTF8(text_h);
            Module._MemFree(text_h);
        },

        LoadCodepoints(text, count) {
            const text_h = cartString(text);
            const result = Module._LoadCodepoints(text_h, count);
            Module._MemFree(text_h);
            return result;
        },

        UnloadCodepoints(codepoints) {
            Module._UnloadCodepoints(codepoints);
        },

        GetCodepointCount(text) {
            const text_h = cartString(text);
            const result = Module._GetCodepointCount(text_h);
            Module._MemFree(text_h);
            return result;
        },

        GetCodepoint(text, codepointSize) {
            const text_h = cartString(text);
            const result = Module._GetCodepoint(text_h, codepointSize);
            Module._MemFree(text_h);
            return result;
        },

        GetCodepointNext(text, codepointSize) {
            const text_h = cartString(text);
            const result = Module._GetCodepointNext(text_h, codepointSize);
            Module._MemFree(text_h);
            return result;
        },

        GetCodepointPrevious(text, codepointSize) {
            const text_h = cartString(text);
            const result = Module._GetCodepointPrevious(text_h, codepointSize);
            Module._MemFree(text_h);
            return result;
        },

        CodepointToUTF8(codepoint, utf8Size) {
            const result = Module._CodepointToUTF8(codepoint, utf8Size);
            return result;
        },

        TextCopy(dst, src) {
            const dst_h = cartString(dst);
            const src_h = cartString(src);
            const result = Module._TextCopy(dst_h, src_h);
            Module._MemFree(dst_h);
            Module._MemFree(src_h);
            return result;
        },

        TextIsEqual(text1, text2) {
            const text1_h = cartString(text1);
            const text2_h = cartString(text2);
            const result = Module._TextIsEqual(text1_h, text2_h);
            Module._MemFree(text1_h);
            Module._MemFree(text2_h);
            return result;
        },

        TextLength(text) {
            const text_h = cartString(text);
            const result = Module._TextLength(text_h);
            Module._MemFree(text_h);
            return result;
        },

        TextFormat(text, args) {
            const text_h = cartString(text);
            const result = Module._TextFormat(text_h, args);
            Module._MemFree(text_h);
            return result;
        },

        TextSubtext(text, position, length) {
            const text_h = cartString(text);
            const result = Module._TextSubtext(text_h, position, length);
            Module._MemFree(text_h);
            return result;
        },

        TextReplace(text, search, replacement) {
            const text_h = cartString(text);
            const search_h = cartString(search);
            const replacement_h = cartString(replacement);
            const result = Module._TextReplace(text_h, search_h, replacement_h);
            Module._MemFree(text_h);
            Module._MemFree(search_h);
            Module._MemFree(replacement_h);
            return result;
        },

        TextInsert(text, insert, position) {
            const text_h = cartString(text);
            const insert_h = cartString(insert);
            const result = Module._TextInsert(text_h, insert_h, position);
            Module._MemFree(text_h);
            Module._MemFree(insert_h);
            return result;
        },

        TextJoin(textList, count, delimiter) {
            const textList_h = cartString(textList);
            const delimiter_h = cartString(delimiter);
            const result = Module._TextJoin(textList_h, count, delimiter_h);
            Module._MemFree(textList_h);
            Module._MemFree(delimiter_h);
            return result;
        },

        TextSplit(text, delimiter, count) {
            const text_h = cartString(text);
            const result = Module._TextSplit(text_h, delimiter, count);
            Module._MemFree(text_h);
            return result;
        },

        TextAppend(text, append, position) {
            const text_h = cartString(text);
            const append_h = cartString(append);
            Module._TextAppend(text_h, append_h, position);
            Module._MemFree(text_h);
            Module._MemFree(append_h);
        },

        TextFindIndex(text, search) {
            const text_h = cartString(text);
            const search_h = cartString(search);
            const result = Module._TextFindIndex(text_h, search_h);
            Module._MemFree(text_h);
            Module._MemFree(search_h);
            return result;
        },

        TextToUpper(text) {
            const text_h = cartString(text);
            const result = Module._TextToUpper(text_h);
            Module._MemFree(text_h);
            return result;
        },

        TextToLower(text) {
            const text_h = cartString(text);
            const result = Module._TextToLower(text_h);
            Module._MemFree(text_h);
            return result;
        },

        TextToPascal(text) {
            const text_h = cartString(text);
            const result = Module._TextToPascal(text_h);
            Module._MemFree(text_h);
            return result;
        },

        TextToSnake(text) {
            const text_h = cartString(text);
            const result = Module._TextToSnake(text_h);
            Module._MemFree(text_h);
            return result;
        },

        TextToCamel(text) {
            const text_h = cartString(text);
            const result = Module._TextToCamel(text_h);
            Module._MemFree(text_h);
            return result;
        },

        TextToInteger(text) {
            const text_h = cartString(text);
            const result = Module._TextToInteger(text_h);
            Module._MemFree(text_h);
            return result;
        },

        TextToFloat(text) {
            const text_h = cartString(text);
            const result = Module._TextToFloat(text_h);
            Module._MemFree(text_h);
            return result;
        },

        DrawLine3D(startPos, endPos, color) {
            const startPos_h = cartVector3(startPos);
            const endPos_h = cartVector3(endPos);
            const color_h = cartColor(color);
            Module._DrawLine3D(startPos_h, endPos_h, color_h);
            Module._MemFree(startPos_h);
            Module._MemFree(endPos_h);
            Module._MemFree(color_h);
        },

        DrawPoint3D(position, color) {
            const position_h = cartVector3(position);
            const color_h = cartColor(color);
            Module._DrawPoint3D(position_h, color_h);
            Module._MemFree(position_h);
            Module._MemFree(color_h);
        },

        DrawCircle3D(center, radius, rotationAxis, rotationAngle, color) {
            const center_h = cartVector3(center);
            const rotationAxis_h = cartVector3(rotationAxis);
            const color_h = cartColor(color);
            Module._DrawCircle3D(center_h, radius, rotationAxis_h, rotationAngle, color_h);
            Module._MemFree(center_h);
            Module._MemFree(rotationAxis_h);
            Module._MemFree(color_h);
        },

        DrawTriangle3D(v1, v2, v3, color) {
            const v1_h = cartVector3(v1);
            const v2_h = cartVector3(v2);
            const v3_h = cartVector3(v3);
            const color_h = cartColor(color);
            Module._DrawTriangle3D(v1_h, v2_h, v3_h, color_h);
            Module._MemFree(v1_h);
            Module._MemFree(v2_h);
            Module._MemFree(v3_h);
            Module._MemFree(color_h);
        },

        DrawTriangleStrip3D(points, pointCount, color) {
            const points_h = cartVector3(points);
            const color_h = cartColor(color);
            Module._DrawTriangleStrip3D(points_h, pointCount, color_h);
            copyHostToCart(points_h, points, 12);
            Module._MemFree(points_h);
            Module._MemFree(color_h);
        },

        DrawCube(position, width, height, length, color) {
            const position_h = cartVector3(position);
            const color_h = cartColor(color);
            Module._DrawCube(position_h, width, height, length, color_h);
            Module._MemFree(position_h);
            Module._MemFree(color_h);
        },

        DrawCubeV(position, size, color) {
            const position_h = cartVector3(position);
            const size_h = cartVector3(size);
            const color_h = cartColor(color);
            Module._DrawCubeV(position_h, size_h, color_h);
            Module._MemFree(position_h);
            Module._MemFree(size_h);
            Module._MemFree(color_h);
        },

        DrawCubeWires(position, width, height, length, color) {
            const position_h = cartVector3(position);
            const color_h = cartColor(color);
            Module._DrawCubeWires(position_h, width, height, length, color_h);
            Module._MemFree(position_h);
            Module._MemFree(color_h);
        },

        DrawCubeWiresV(position, size, color) {
            const position_h = cartVector3(position);
            const size_h = cartVector3(size);
            const color_h = cartColor(color);
            Module._DrawCubeWiresV(position_h, size_h, color_h);
            Module._MemFree(position_h);
            Module._MemFree(size_h);
            Module._MemFree(color_h);
        },

        DrawSphere(centerPos, radius, color) {
            const centerPos_h = cartVector3(centerPos);
            const color_h = cartColor(color);
            Module._DrawSphere(centerPos_h, radius, color_h);
            Module._MemFree(centerPos_h);
            Module._MemFree(color_h);
        },

        DrawSphereEx(centerPos, radius, rings, slices, color) {
            const centerPos_h = cartVector3(centerPos);
            const color_h = cartColor(color);
            Module._DrawSphereEx(centerPos_h, radius, rings, slices, color_h);
            Module._MemFree(centerPos_h);
            Module._MemFree(color_h);
        },

        DrawSphereWires(centerPos, radius, rings, slices, color) {
            const centerPos_h = cartVector3(centerPos);
            const color_h = cartColor(color);
            Module._DrawSphereWires(centerPos_h, radius, rings, slices, color_h);
            Module._MemFree(centerPos_h);
            Module._MemFree(color_h);
        },

        DrawCylinder(position, radiusTop, radiusBottom, height, slices, color) {
            const position_h = cartVector3(position);
            const color_h = cartColor(color);
            Module._DrawCylinder(position_h, radiusTop, radiusBottom, height, slices, color_h);
            Module._MemFree(position_h);
            Module._MemFree(color_h);
        },

        DrawCylinderEx(startPos, endPos, startRadius, endRadius, sides, color) {
            const startPos_h = cartVector3(startPos);
            const endPos_h = cartVector3(endPos);
            const color_h = cartColor(color);
            Module._DrawCylinderEx(startPos_h, endPos_h, startRadius, endRadius, sides, color_h);
            Module._MemFree(startPos_h);
            Module._MemFree(endPos_h);
            Module._MemFree(color_h);
        },

        DrawCylinderWires(position, radiusTop, radiusBottom, height, slices, color) {
            const position_h = cartVector3(position);
            const color_h = cartColor(color);
            Module._DrawCylinderWires(position_h, radiusTop, radiusBottom, height, slices, color_h);
            Module._MemFree(position_h);
            Module._MemFree(color_h);
        },

        DrawCylinderWiresEx(startPos, endPos, startRadius, endRadius, sides, color) {
            const startPos_h = cartVector3(startPos);
            const endPos_h = cartVector3(endPos);
            const color_h = cartColor(color);
            Module._DrawCylinderWiresEx(startPos_h, endPos_h, startRadius, endRadius, sides, color_h);
            Module._MemFree(startPos_h);
            Module._MemFree(endPos_h);
            Module._MemFree(color_h);
        },

        DrawCapsule(startPos, endPos, radius, slices, rings, color) {
            const startPos_h = cartVector3(startPos);
            const endPos_h = cartVector3(endPos);
            const color_h = cartColor(color);
            Module._DrawCapsule(startPos_h, endPos_h, radius, slices, rings, color_h);
            Module._MemFree(startPos_h);
            Module._MemFree(endPos_h);
            Module._MemFree(color_h);
        },

        DrawCapsuleWires(startPos, endPos, radius, slices, rings, color) {
            const startPos_h = cartVector3(startPos);
            const endPos_h = cartVector3(endPos);
            const color_h = cartColor(color);
            Module._DrawCapsuleWires(startPos_h, endPos_h, radius, slices, rings, color_h);
            Module._MemFree(startPos_h);
            Module._MemFree(endPos_h);
            Module._MemFree(color_h);
        },

        DrawPlane(centerPos, size, color) {
            const centerPos_h = cartVector3(centerPos);
            const size_h = cartVector2(size);
            const color_h = cartColor(color);
            Module._DrawPlane(centerPos_h, size_h, color_h);
            Module._MemFree(centerPos_h);
            Module._MemFree(size_h);
            Module._MemFree(color_h);
        },

        DrawRay(ray, color) {
            const ray_h = cartRay(ray);
            const color_h = cartColor(color);
            Module._DrawRay(ray_h, color_h);
            Module._MemFree(ray_h);
            Module._MemFree(color_h);
        },

        DrawGrid(slices, spacing) {
            Module._DrawGrid(slices, spacing);
        },

        LoadModel(resultPtr, fileName) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(96);
            Module._LoadModel(result_h, fileName_h);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 96);
            Module._MemFree(result_h);
        },

        LoadModelFromMesh(resultPtr, mesh) {
            const mesh_h = cartMesh(mesh);
            const result_h = Module._MemAlloc(96);
            Module._LoadModelFromMesh(result_h, mesh_h);
            Module._MemFree(mesh_h);
            copyHostToCart(result_h, resultPtr, 96);
            Module._MemFree(result_h);
        },

        IsModelValid(model) {
            const model_h = cartModel(model);
            const result = Module._IsModelValid(model_h);
            Module._MemFree(model_h);
            return result;
        },

        UnloadModel(model) {
            const model_h = cartModel(model);
            Module._UnloadModel(model_h);
            Module._MemFree(model_h);
        },

        GetModelBoundingBox(resultPtr, model) {
            const model_h = cartModel(model);
            const result_h = Module._MemAlloc(24);
            Module._GetModelBoundingBox(result_h, model_h);
            Module._MemFree(model_h);
            copyHostToCart(result_h, resultPtr, 24);
            Module._MemFree(result_h);
        },

        DrawModel(model, position, scale, tint) {
            const model_h = cartModel(model);
            const position_h = cartVector3(position);
            const tint_h = cartColor(tint);
            Module._DrawModel(model_h, position_h, scale, tint_h);
            Module._MemFree(model_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        DrawModelEx(model, position, rotationAxis, rotationAngle, scale, tint) {
            const model_h = cartModel(model);
            const position_h = cartVector3(position);
            const rotationAxis_h = cartVector3(rotationAxis);
            const scale_h = cartVector3(scale);
            const tint_h = cartColor(tint);
            Module._DrawModelEx(model_h, position_h, rotationAxis_h, rotationAngle, scale_h, tint_h);
            Module._MemFree(model_h);
            Module._MemFree(position_h);
            Module._MemFree(rotationAxis_h);
            Module._MemFree(scale_h);
            Module._MemFree(tint_h);
        },

        DrawModelWires(model, position, scale, tint) {
            const model_h = cartModel(model);
            const position_h = cartVector3(position);
            const tint_h = cartColor(tint);
            Module._DrawModelWires(model_h, position_h, scale, tint_h);
            Module._MemFree(model_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        DrawModelWiresEx(model, position, rotationAxis, rotationAngle, scale, tint) {
            const model_h = cartModel(model);
            const position_h = cartVector3(position);
            const rotationAxis_h = cartVector3(rotationAxis);
            const scale_h = cartVector3(scale);
            const tint_h = cartColor(tint);
            Module._DrawModelWiresEx(model_h, position_h, rotationAxis_h, rotationAngle, scale_h, tint_h);
            Module._MemFree(model_h);
            Module._MemFree(position_h);
            Module._MemFree(rotationAxis_h);
            Module._MemFree(scale_h);
            Module._MemFree(tint_h);
        },

        DrawModelPoints(model, position, scale, tint) {
            const model_h = cartModel(model);
            const position_h = cartVector3(position);
            const tint_h = cartColor(tint);
            Module._DrawModelPoints(model_h, position_h, scale, tint_h);
            Module._MemFree(model_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        DrawModelPointsEx(model, position, rotationAxis, rotationAngle, scale, tint) {
            const model_h = cartModel(model);
            const position_h = cartVector3(position);
            const rotationAxis_h = cartVector3(rotationAxis);
            const scale_h = cartVector3(scale);
            const tint_h = cartColor(tint);
            Module._DrawModelPointsEx(model_h, position_h, rotationAxis_h, rotationAngle, scale_h, tint_h);
            Module._MemFree(model_h);
            Module._MemFree(position_h);
            Module._MemFree(rotationAxis_h);
            Module._MemFree(scale_h);
            Module._MemFree(tint_h);
        },

        DrawBoundingBox(box, color) {
            const box_h = cartBoundingBox(box);
            const color_h = cartColor(color);
            Module._DrawBoundingBox(box_h, color_h);
            Module._MemFree(box_h);
            Module._MemFree(color_h);
        },

        DrawBillboard(camera, texture, position, scale, tint) {
            const camera_h = cartCamera3D(camera);
            const texture_h = cartTexture(texture);
            const position_h = cartVector3(position);
            const tint_h = cartColor(tint);
            Module._DrawBillboard(camera_h, texture_h, position_h, scale, tint_h);
            Module._MemFree(camera_h);
            Module._MemFree(texture_h);
            Module._MemFree(position_h);
            Module._MemFree(tint_h);
        },

        DrawBillboardRec(camera, texture, source, position, size, tint) {
            const camera_h = cartCamera3D(camera);
            const texture_h = cartTexture(texture);
            const source_h = cartRectangle(source);
            const position_h = cartVector3(position);
            const size_h = cartVector2(size);
            const tint_h = cartColor(tint);
            Module._DrawBillboardRec(camera_h, texture_h, source_h, position_h, size_h, tint_h);
            Module._MemFree(camera_h);
            Module._MemFree(texture_h);
            Module._MemFree(source_h);
            Module._MemFree(position_h);
            Module._MemFree(size_h);
            Module._MemFree(tint_h);
        },

        DrawBillboardPro(camera, texture, source, position, up, size, origin, rotation, tint) {
            const camera_h = cartCamera3D(camera);
            const texture_h = cartTexture(texture);
            const source_h = cartRectangle(source);
            const position_h = cartVector3(position);
            const up_h = cartVector3(up);
            const size_h = cartVector2(size);
            const origin_h = cartVector2(origin);
            const tint_h = cartColor(tint);
            Module._DrawBillboardPro(camera_h, texture_h, source_h, position_h, up_h, size_h, origin_h, rotation, tint_h);
            Module._MemFree(camera_h);
            Module._MemFree(texture_h);
            Module._MemFree(source_h);
            Module._MemFree(position_h);
            Module._MemFree(up_h);
            Module._MemFree(size_h);
            Module._MemFree(origin_h);
            Module._MemFree(tint_h);
        },

        UploadMesh(mesh, dynamic) {
            const mesh_h = cartMesh(mesh);
            Module._UploadMesh(mesh_h, dynamic);
            copyHostToCart(mesh_h, mesh, 68);
            Module._MemFree(mesh_h);
        },

        UpdateMeshBuffer(mesh, index, data, dataSize, offset) {
            const mesh_h = cartMesh(mesh);
            Module._UpdateMeshBuffer(mesh_h, index, data, dataSize, offset);
            Module._MemFree(mesh_h);
        },

        UnloadMesh(mesh) {
            const mesh_h = cartMesh(mesh);
            Module._UnloadMesh(mesh_h);
            Module._MemFree(mesh_h);
        },

        DrawMesh(mesh, material, transform) {
            const mesh_h = cartMesh(mesh);
            const material_h = cartMaterial(material);
            const transform_h = cartMatrix(transform);
            Module._DrawMesh(mesh_h, material_h, transform_h);
            Module._MemFree(mesh_h);
            Module._MemFree(material_h);
            Module._MemFree(transform_h);
        },

        DrawMeshInstanced(mesh, material, transforms, instances) {
            const mesh_h = cartMesh(mesh);
            const material_h = cartMaterial(material);
            const transforms_h = cartMatrix(transforms);
            Module._DrawMeshInstanced(mesh_h, material_h, transforms_h, instances);
            Module._MemFree(mesh_h);
            Module._MemFree(material_h);
            copyHostToCart(transforms_h, transforms, 64);
            Module._MemFree(transforms_h);
        },

        GetMeshBoundingBox(resultPtr, mesh) {
            const mesh_h = cartMesh(mesh);
            const result_h = Module._MemAlloc(24);
            Module._GetMeshBoundingBox(result_h, mesh_h);
            Module._MemFree(mesh_h);
            copyHostToCart(result_h, resultPtr, 24);
            Module._MemFree(result_h);
        },

        GenMeshTangents(mesh) {
            const mesh_h = cartMesh(mesh);
            Module._GenMeshTangents(mesh_h);
            copyHostToCart(mesh_h, mesh, 68);
            Module._MemFree(mesh_h);
        },

        ExportMesh(mesh, fileName) {
            const mesh_h = cartMesh(mesh);
            const fileName_h = cartString(fileName);
            const result = Module._ExportMesh(mesh_h, fileName_h);
            Module._MemFree(mesh_h);
            Module._MemFree(fileName_h);
            return result;
        },

        ExportMeshAsCode(mesh, fileName) {
            const mesh_h = cartMesh(mesh);
            const fileName_h = cartString(fileName);
            const result = Module._ExportMeshAsCode(mesh_h, fileName_h);
            Module._MemFree(mesh_h);
            Module._MemFree(fileName_h);
            return result;
        },

        GenMeshPoly(resultPtr, sides, radius) {
            const result_h = Module._MemAlloc(68);
            Module._GenMeshPoly(result_h, sides, radius);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshPlane(resultPtr, width, length, resX, resZ) {
            const result_h = Module._MemAlloc(68);
            Module._GenMeshPlane(result_h, width, length, resX, resZ);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshCube(resultPtr, width, height, length) {
            const result_h = Module._MemAlloc(68);
            Module._GenMeshCube(result_h, width, height, length);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshSphere(resultPtr, radius, rings, slices) {
            const result_h = Module._MemAlloc(68);
            Module._GenMeshSphere(result_h, radius, rings, slices);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshHemiSphere(resultPtr, radius, rings, slices) {
            const result_h = Module._MemAlloc(68);
            Module._GenMeshHemiSphere(result_h, radius, rings, slices);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshCylinder(resultPtr, radius, height, slices) {
            const result_h = Module._MemAlloc(68);
            Module._GenMeshCylinder(result_h, radius, height, slices);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshCone(resultPtr, radius, height, slices) {
            const result_h = Module._MemAlloc(68);
            Module._GenMeshCone(result_h, radius, height, slices);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshTorus(resultPtr, radius, size, radSeg, sides) {
            const result_h = Module._MemAlloc(68);
            Module._GenMeshTorus(result_h, radius, size, radSeg, sides);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshKnot(resultPtr, radius, size, radSeg, sides) {
            const result_h = Module._MemAlloc(68);
            Module._GenMeshKnot(result_h, radius, size, radSeg, sides);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshHeightmap(resultPtr, heightmap, size) {
            const heightmap_h = cartImage(heightmap);
            const size_h = cartVector3(size);
            const result_h = Module._MemAlloc(68);
            Module._GenMeshHeightmap(result_h, heightmap_h, size_h);
            Module._MemFree(heightmap_h);
            Module._MemFree(size_h);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        GenMeshCubicmap(resultPtr, cubicmap, cubeSize) {
            const cubicmap_h = cartImage(cubicmap);
            const cubeSize_h = cartVector3(cubeSize);
            const result_h = Module._MemAlloc(68);
            Module._GenMeshCubicmap(result_h, cubicmap_h, cubeSize_h);
            Module._MemFree(cubicmap_h);
            Module._MemFree(cubeSize_h);
            copyHostToCart(result_h, resultPtr, 68);
            Module._MemFree(result_h);
        },

        LoadMaterialDefault(resultPtr) {
            const result_h = Module._MemAlloc(12);
            Module._LoadMaterialDefault(result_h);
            copyHostToCart(result_h, resultPtr, 12);
            Module._MemFree(result_h);
        },

        IsMaterialValid(material) {
            const material_h = cartMaterial(material);
            const result = Module._IsMaterialValid(material_h);
            Module._MemFree(material_h);
            return result;
        },

        UnloadMaterial(material) {
            const material_h = cartMaterial(material);
            Module._UnloadMaterial(material_h);
            Module._MemFree(material_h);
        },

        SetMaterialTexture(material, mapType, texture) {
            const material_h = cartMaterial(material);
            const texture_h = cartTexture(texture);
            Module._SetMaterialTexture(material_h, mapType, texture_h);
            copyHostToCart(material_h, material, 12);
            Module._MemFree(material_h);
            Module._MemFree(texture_h);
        },

        SetModelMeshMaterial(model, meshId, materialId) {
            const model_h = cartModel(model);
            Module._SetModelMeshMaterial(model_h, meshId, materialId);
            copyHostToCart(model_h, model, 96);
            Module._MemFree(model_h);
        },

        UnloadModelAnimations(animations, animCount) {
            const animations_h = cartModelAnimation(animations);
            Module._UnloadModelAnimations(animations_h, animCount);
            copyHostToCart(animations_h, animations, 16);
            Module._MemFree(animations_h);
        },

        CheckCollisionSpheres(center1, radius1, center2, radius2) {
            const center1_h = cartVector3(center1);
            const center2_h = cartVector3(center2);
            const result = Module._CheckCollisionSpheres(center1_h, radius1, center2_h, radius2);
            Module._MemFree(center1_h);
            Module._MemFree(center2_h);
            return result;
        },

        CheckCollisionBoxes(box1, box2) {
            const box1_h = cartBoundingBox(box1);
            const box2_h = cartBoundingBox(box2);
            const result = Module._CheckCollisionBoxes(box1_h, box2_h);
            Module._MemFree(box1_h);
            Module._MemFree(box2_h);
            return result;
        },

        CheckCollisionBoxSphere(box, center, radius) {
            const box_h = cartBoundingBox(box);
            const center_h = cartVector3(center);
            const result = Module._CheckCollisionBoxSphere(box_h, center_h, radius);
            Module._MemFree(box_h);
            Module._MemFree(center_h);
            return result;
        },

        GetRayCollisionSphere(resultPtr, ray, center, radius) {
            const ray_h = cartRay(ray);
            const center_h = cartVector3(center);
            const result_h = Module._MemAlloc(28);
            Module._GetRayCollisionSphere(result_h, ray_h, center_h, radius);
            Module._MemFree(ray_h);
            Module._MemFree(center_h);
            copyHostToCart(result_h, resultPtr, 28);
            Module._MemFree(result_h);
        },

        GetRayCollisionBox(resultPtr, ray, box) {
            const ray_h = cartRay(ray);
            const box_h = cartBoundingBox(box);
            const result_h = Module._MemAlloc(28);
            Module._GetRayCollisionBox(result_h, ray_h, box_h);
            Module._MemFree(ray_h);
            Module._MemFree(box_h);
            copyHostToCart(result_h, resultPtr, 28);
            Module._MemFree(result_h);
        },

        GetRayCollisionMesh(resultPtr, ray, mesh, transform) {
            const ray_h = cartRay(ray);
            const mesh_h = cartMesh(mesh);
            const transform_h = cartMatrix(transform);
            const result_h = Module._MemAlloc(28);
            Module._GetRayCollisionMesh(result_h, ray_h, mesh_h, transform_h);
            Module._MemFree(ray_h);
            Module._MemFree(mesh_h);
            Module._MemFree(transform_h);
            copyHostToCart(result_h, resultPtr, 28);
            Module._MemFree(result_h);
        },

        GetRayCollisionTriangle(resultPtr, ray, p1, p2, p3) {
            const ray_h = cartRay(ray);
            const p1_h = cartVector3(p1);
            const p2_h = cartVector3(p2);
            const p3_h = cartVector3(p3);
            const result_h = Module._MemAlloc(28);
            Module._GetRayCollisionTriangle(result_h, ray_h, p1_h, p2_h, p3_h);
            Module._MemFree(ray_h);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            Module._MemFree(p3_h);
            copyHostToCart(result_h, resultPtr, 28);
            Module._MemFree(result_h);
        },

        GetRayCollisionQuad(resultPtr, ray, p1, p2, p3, p4) {
            const ray_h = cartRay(ray);
            const p1_h = cartVector3(p1);
            const p2_h = cartVector3(p2);
            const p3_h = cartVector3(p3);
            const p4_h = cartVector3(p4);
            const result_h = Module._MemAlloc(28);
            Module._GetRayCollisionQuad(result_h, ray_h, p1_h, p2_h, p3_h, p4_h);
            Module._MemFree(ray_h);
            Module._MemFree(p1_h);
            Module._MemFree(p2_h);
            Module._MemFree(p3_h);
            Module._MemFree(p4_h);
            copyHostToCart(result_h, resultPtr, 28);
            Module._MemFree(result_h);
        },

        InitAudioDevice() {
            Module._InitAudioDevice();
        },

        CloseAudioDevice() {
            Module._CloseAudioDevice();
        },

        IsAudioDeviceReady() {
            const result = Module._IsAudioDeviceReady();
            return result;
        },

        SetMasterVolume(volume) {
            Module._SetMasterVolume(volume);
        },

        GetMasterVolume() {
            const result = Module._GetMasterVolume();
            return result;
        },

        LoadWave(resultPtr, fileName) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(20);
            Module._LoadWave(result_h, fileName_h);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        LoadWaveFromMemory(resultPtr, fileType, fileData, dataSize) {
            const fileType_h = cartString(fileType);
            const fileData_h = cartString(fileData);
            const result_h = Module._MemAlloc(20);
            Module._LoadWaveFromMemory(result_h, fileType_h, fileData_h, dataSize);
            Module._MemFree(fileType_h);
            Module._MemFree(fileData_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        IsWaveValid(wave) {
            const wave_h = cartWave(wave);
            const result = Module._IsWaveValid(wave_h);
            Module._MemFree(wave_h);
            return result;
        },

        LoadSound(resultPtr, fileName) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(24);
            Module._LoadSound(result_h, fileName_h);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 24);
            Module._MemFree(result_h);
        },

        LoadSoundFromWave(resultPtr, wave) {
            const wave_h = cartWave(wave);
            const result_h = Module._MemAlloc(24);
            Module._LoadSoundFromWave(result_h, wave_h);
            Module._MemFree(wave_h);
            copyHostToCart(result_h, resultPtr, 24);
            Module._MemFree(result_h);
        },

        LoadSoundAlias(resultPtr, source) {
            const source_h = cartSound(source);
            const result_h = Module._MemAlloc(24);
            Module._LoadSoundAlias(result_h, source_h);
            Module._MemFree(source_h);
            copyHostToCart(result_h, resultPtr, 24);
            Module._MemFree(result_h);
        },

        IsSoundValid(sound) {
            const sound_h = cartSound(sound);
            const result = Module._IsSoundValid(sound_h);
            Module._MemFree(sound_h);
            return result;
        },

        UpdateSound(sound, data, sampleCount) {
            const sound_h = cartSound(sound);
            Module._UpdateSound(sound_h, data, sampleCount);
            Module._MemFree(sound_h);
        },

        UnloadWave(wave) {
            const wave_h = cartWave(wave);
            Module._UnloadWave(wave_h);
            Module._MemFree(wave_h);
        },

        UnloadSound(sound) {
            const sound_h = cartSound(sound);
            Module._UnloadSound(sound_h);
            Module._MemFree(sound_h);
        },

        UnloadSoundAlias(alias) {
            const alias_h = cartSound(alias);
            Module._UnloadSoundAlias(alias_h);
            Module._MemFree(alias_h);
        },

        ExportWave(wave, fileName) {
            const wave_h = cartWave(wave);
            const fileName_h = cartString(fileName);
            const result = Module._ExportWave(wave_h, fileName_h);
            Module._MemFree(wave_h);
            Module._MemFree(fileName_h);
            return result;
        },

        ExportWaveAsCode(wave, fileName) {
            const wave_h = cartWave(wave);
            const fileName_h = cartString(fileName);
            const result = Module._ExportWaveAsCode(wave_h, fileName_h);
            Module._MemFree(wave_h);
            Module._MemFree(fileName_h);
            return result;
        },

        PlaySound(sound) {
            const sound_h = cartSound(sound);
            Module._PlaySound(sound_h);
            Module._MemFree(sound_h);
        },

        StopSound(sound) {
            const sound_h = cartSound(sound);
            Module._StopSound(sound_h);
            Module._MemFree(sound_h);
        },

        PauseSound(sound) {
            const sound_h = cartSound(sound);
            Module._PauseSound(sound_h);
            Module._MemFree(sound_h);
        },

        ResumeSound(sound) {
            const sound_h = cartSound(sound);
            Module._ResumeSound(sound_h);
            Module._MemFree(sound_h);
        },

        IsSoundPlaying(sound) {
            const sound_h = cartSound(sound);
            const result = Module._IsSoundPlaying(sound_h);
            Module._MemFree(sound_h);
            return result;
        },

        SetSoundVolume(sound, volume) {
            const sound_h = cartSound(sound);
            Module._SetSoundVolume(sound_h, volume);
            Module._MemFree(sound_h);
        },

        SetSoundPitch(sound, pitch) {
            const sound_h = cartSound(sound);
            Module._SetSoundPitch(sound_h, pitch);
            Module._MemFree(sound_h);
        },

        SetSoundPan(sound, pan) {
            const sound_h = cartSound(sound);
            Module._SetSoundPan(sound_h, pan);
            Module._MemFree(sound_h);
        },

        WaveCopy(resultPtr, wave) {
            const wave_h = cartWave(wave);
            const result_h = Module._MemAlloc(20);
            Module._WaveCopy(result_h, wave_h);
            Module._MemFree(wave_h);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        WaveCrop(wave, initFrame, finalFrame) {
            const wave_h = cartWave(wave);
            Module._WaveCrop(wave_h, initFrame, finalFrame);
            copyHostToCart(wave_h, wave, 20);
            Module._MemFree(wave_h);
        },

        WaveFormat(wave, sampleRate, sampleSize, channels) {
            const wave_h = cartWave(wave);
            Module._WaveFormat(wave_h, sampleRate, sampleSize, channels);
            copyHostToCart(wave_h, wave, 20);
            Module._MemFree(wave_h);
        },

        LoadWaveSamples(wave) {
            const wave_h = cartWave(wave);
            const result = Module._LoadWaveSamples(wave_h);
            Module._MemFree(wave_h);
            return result;
        },

        UnloadWaveSamples(samples) {
            Module._UnloadWaveSamples(samples);
        },

        LoadMusicStream(resultPtr, fileName) {
            const fileName_h = cartString(fileName);
            const result_h = Module._MemAlloc(32);
            Module._LoadMusicStream(result_h, fileName_h);
            Module._MemFree(fileName_h);
            copyHostToCart(result_h, resultPtr, 32);
            Module._MemFree(result_h);
        },

        LoadMusicStreamFromMemory(resultPtr, fileType, data, dataSize) {
            const fileType_h = cartString(fileType);
            const data_h = cartString(data);
            const result_h = Module._MemAlloc(32);
            Module._LoadMusicStreamFromMemory(result_h, fileType_h, data_h, dataSize);
            Module._MemFree(fileType_h);
            Module._MemFree(data_h);
            copyHostToCart(result_h, resultPtr, 32);
            Module._MemFree(result_h);
        },

        IsMusicValid(music) {
            const music_h = cartMusic(music);
            const result = Module._IsMusicValid(music_h);
            Module._MemFree(music_h);
            return result;
        },

        UnloadMusicStream(music) {
            const music_h = cartMusic(music);
            Module._UnloadMusicStream(music_h);
            Module._MemFree(music_h);
        },

        PlayMusicStream(music) {
            const music_h = cartMusic(music);
            Module._PlayMusicStream(music_h);
            Module._MemFree(music_h);
        },

        IsMusicStreamPlaying(music) {
            const music_h = cartMusic(music);
            const result = Module._IsMusicStreamPlaying(music_h);
            Module._MemFree(music_h);
            return result;
        },

        UpdateMusicStream(music) {
            const music_h = cartMusic(music);
            Module._UpdateMusicStream(music_h);
            Module._MemFree(music_h);
        },

        StopMusicStream(music) {
            const music_h = cartMusic(music);
            Module._StopMusicStream(music_h);
            Module._MemFree(music_h);
        },

        PauseMusicStream(music) {
            const music_h = cartMusic(music);
            Module._PauseMusicStream(music_h);
            Module._MemFree(music_h);
        },

        ResumeMusicStream(music) {
            const music_h = cartMusic(music);
            Module._ResumeMusicStream(music_h);
            Module._MemFree(music_h);
        },

        SeekMusicStream(music, position) {
            const music_h = cartMusic(music);
            Module._SeekMusicStream(music_h, position);
            Module._MemFree(music_h);
        },

        SetMusicVolume(music, volume) {
            const music_h = cartMusic(music);
            Module._SetMusicVolume(music_h, volume);
            Module._MemFree(music_h);
        },

        SetMusicPitch(music, pitch) {
            const music_h = cartMusic(music);
            Module._SetMusicPitch(music_h, pitch);
            Module._MemFree(music_h);
        },

        SetMusicPan(music, pan) {
            const music_h = cartMusic(music);
            Module._SetMusicPan(music_h, pan);
            Module._MemFree(music_h);
        },

        GetMusicTimeLength(music) {
            const music_h = cartMusic(music);
            const result = Module._GetMusicTimeLength(music_h);
            Module._MemFree(music_h);
            return result;
        },

        GetMusicTimePlayed(music) {
            const music_h = cartMusic(music);
            const result = Module._GetMusicTimePlayed(music_h);
            Module._MemFree(music_h);
            return result;
        },

        LoadAudioStream(resultPtr, sampleRate, sampleSize, channels) {
            const result_h = Module._MemAlloc(20);
            Module._LoadAudioStream(result_h, sampleRate, sampleSize, channels);
            copyHostToCart(result_h, resultPtr, 20);
            Module._MemFree(result_h);
        },

        IsAudioStreamValid(stream) {
            const stream_h = cartAudioStream(stream);
            const result = Module._IsAudioStreamValid(stream_h);
            Module._MemFree(stream_h);
            return result;
        },

        UnloadAudioStream(stream) {
            const stream_h = cartAudioStream(stream);
            Module._UnloadAudioStream(stream_h);
            Module._MemFree(stream_h);
        },

        UpdateAudioStream(stream, data, frameCount) {
            const stream_h = cartAudioStream(stream);
            Module._UpdateAudioStream(stream_h, data, frameCount);
            Module._MemFree(stream_h);
        },

        IsAudioStreamProcessed(stream) {
            const stream_h = cartAudioStream(stream);
            const result = Module._IsAudioStreamProcessed(stream_h);
            Module._MemFree(stream_h);
            return result;
        },

        PlayAudioStream(stream) {
            const stream_h = cartAudioStream(stream);
            Module._PlayAudioStream(stream_h);
            Module._MemFree(stream_h);
        },

        PauseAudioStream(stream) {
            const stream_h = cartAudioStream(stream);
            Module._PauseAudioStream(stream_h);
            Module._MemFree(stream_h);
        },

        ResumeAudioStream(stream) {
            const stream_h = cartAudioStream(stream);
            Module._ResumeAudioStream(stream_h);
            Module._MemFree(stream_h);
        },

        IsAudioStreamPlaying(stream) {
            const stream_h = cartAudioStream(stream);
            const result = Module._IsAudioStreamPlaying(stream_h);
            Module._MemFree(stream_h);
            return result;
        },

        StopAudioStream(stream) {
            const stream_h = cartAudioStream(stream);
            Module._StopAudioStream(stream_h);
            Module._MemFree(stream_h);
        },

        SetAudioStreamVolume(stream, volume) {
            const stream_h = cartAudioStream(stream);
            Module._SetAudioStreamVolume(stream_h, volume);
            Module._MemFree(stream_h);
        },

        SetAudioStreamPitch(stream, pitch) {
            const stream_h = cartAudioStream(stream);
            Module._SetAudioStreamPitch(stream_h, pitch);
            Module._MemFree(stream_h);
        },

        SetAudioStreamPan(stream, pan) {
            const stream_h = cartAudioStream(stream);
            Module._SetAudioStreamPan(stream_h, pan);
            Module._MemFree(stream_h);
        },

        SetAudioStreamBufferSizeDefault(size) {
            Module._SetAudioStreamBufferSizeDefault(size);
        }
    };

    const { instance } = await WebAssembly.instantiate(wasmBytes, {raycart, wasi_snapshot_preview1: Module.wasi_snapshot_preview1});
    const cart = Module.cart = instance.exports;
    Module.wasi_snapshot_preview1.start(cart);

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
