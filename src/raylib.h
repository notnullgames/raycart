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
typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

typedef Vector4 Quaternion;

typedef struct Matrix {
    float m0;
    float m4;
    float m8;
    float m12;
    float m1;
    float m5;
    float m9;
    float m13;
    float m2;
    float m6;
    float m10;
    float m14;
    float m3;
    float m7;
    float m11;
    float m15;
} Matrix;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

typedef struct Rectangle {
    float x;
    float y;
    float width;
    float height;
} Rectangle;

typedef struct Image {
    void * data;
    int width;
    int height;
    int mipmaps;
    int format;
} Image;

typedef struct Texture {
    unsigned int id;
    int width;
    int height;
    int mipmaps;
    int format;
} Texture;

typedef Texture Texture2D;

typedef Texture TextureCubemap;

typedef struct RenderTexture {
    unsigned int id;
    Texture texture;
    Texture depth;
} RenderTexture;

typedef RenderTexture RenderTexture2D;

typedef struct NPatchInfo {
    Rectangle source;
    int left;
    int top;
    int right;
    int bottom;
    int layout;
} NPatchInfo;

typedef struct GlyphInfo {
    int value;
    int offsetX;
    int offsetY;
    int advanceX;
    Image image;
} GlyphInfo;

typedef struct Font {
    int baseSize;
    int glyphCount;
    int glyphPadding;
    Texture2D texture;
    Rectangle * recs;
    GlyphInfo * glyphs;
} Font;

typedef struct Camera3D {
    Vector3 position;
    Vector3 target;
    Vector3 up;
    float fovy;
    int projection;
} Camera3D;

typedef Camera3D Camera;

typedef struct Camera2D {
    Vector2 offset;
    Vector2 target;
    float rotation;
    float zoom;
} Camera2D;

typedef struct Mesh {
    int vertexCount;
    int triangleCount;
    float * vertices;
    float * texcoords;
    float * texcoords2;
    float * normals;
    float * tangents;
    unsigned char * colors;
    unsigned short * indices;
    int boneCount;
    unsigned char * boneIndices;
    float * boneWeights;
    float * animVertices;
    float * animNormals;
    unsigned int vaoId;
    unsigned int * vboId;
} Mesh;

typedef struct Shader {
    unsigned int id;
    int * locs;
} Shader;

typedef struct MaterialMap {
    Texture2D texture;
    Color color;
    float value;
} MaterialMap;

typedef struct Material {
    Shader shader;
    MaterialMap * maps;
    float params[4];
} Material;

typedef struct Transform {
    Vector3 translation;
    Quaternion rotation;
    Vector3 scale;
} Transform;

typedef Transform *ModelAnimPose;

typedef struct BoneInfo {
    char name[32];
    int parent;
} BoneInfo;

typedef struct ModelSkeleton {
    int boneCount;
    BoneInfo * bones;
    ModelAnimPose bindPose;
} ModelSkeleton;

typedef struct Model {
    Matrix transform;
    int meshCount;
    int materialCount;
    Mesh * meshes;
    Material * materials;
    int * meshMaterial;
    ModelSkeleton skeleton;
    ModelAnimPose currentPose;
    Matrix * boneMatrices;
} Model;

typedef struct ModelAnimation {
    char name[32];
    int boneCount;
    int keyframeCount;
    ModelAnimPose * keyframePoses;
} ModelAnimation;

typedef struct Ray {
    Vector3 position;
    Vector3 direction;
} Ray;

typedef struct RayCollision {
    bool hit;
    float distance;
    Vector3 point;
    Vector3 normal;
} RayCollision;

typedef struct BoundingBox {
    Vector3 min;
    Vector3 max;
} BoundingBox;

typedef struct Wave {
    unsigned int frameCount;
    unsigned int sampleRate;
    unsigned int sampleSize;
    unsigned int channels;
    void * data;
} Wave;

typedef struct AudioStream {
    rAudioBuffer * buffer;
    rAudioProcessor * processor;
    unsigned int sampleRate;
    unsigned int sampleSize;
    unsigned int channels;
} AudioStream;

typedef struct Sound {
    AudioStream stream;
    unsigned int frameCount;
} Sound;

typedef struct Music {
    AudioStream stream;
    unsigned int frameCount;
    bool looping;
    int ctxType;
    void * ctxData;
} Music;

typedef struct VrDeviceInfo {
    int hResolution;
    int vResolution;
    float hScreenSize;
    float vScreenSize;
    float eyeToScreenDistance;
    float lensSeparationDistance;
    float interpupillaryDistance;
    float lensDistortionValues[4];
    float chromaAbCorrection[4];
} VrDeviceInfo;

typedef struct VrStereoConfig {
    Matrix projection[2];
    Matrix viewOffset[2];
    float leftLensCenter[2];
    float rightLensCenter[2];
    float leftScreenCenter[2];
    float rightScreenCenter[2];
    float scale[2];
    float scaleIn[2];
} VrStereoConfig;

typedef struct FilePathList {
    unsigned int count;
    char ** paths;
} FilePathList;

typedef struct AutomationEvent {
    unsigned int frame;
    unsigned int type;
    int params[4];
} AutomationEvent;

typedef struct AutomationEventList {
    unsigned int capacity;
    unsigned int count;
    AutomationEvent * events;
} AutomationEventList;

//----------------------------------------------------------------------------------
// Enumerators
//----------------------------------------------------------------------------------
typedef enum {
    FLAG_VSYNC_HINT = 64,
    FLAG_FULLSCREEN_MODE = 2,
    FLAG_WINDOW_RESIZABLE = 4,
    FLAG_WINDOW_UNDECORATED = 8,
    FLAG_WINDOW_HIDDEN = 128,
    FLAG_WINDOW_MINIMIZED = 512,
    FLAG_WINDOW_MAXIMIZED = 1024,
    FLAG_WINDOW_UNFOCUSED = 2048,
    FLAG_WINDOW_TOPMOST = 4096,
    FLAG_WINDOW_ALWAYS_RUN = 256,
    FLAG_WINDOW_TRANSPARENT = 16,
    FLAG_WINDOW_HIGHDPI = 8192,
    FLAG_WINDOW_MOUSE_PASSTHROUGH = 16384,
    FLAG_BORDERLESS_WINDOWED_MODE = 32768,
    FLAG_MSAA_4X_HINT = 32,
    FLAG_INTERLACED_HINT = 65536,
} ConfigFlags;

typedef enum {
    LOG_ALL = 0,
    LOG_TRACE = 1,
    LOG_DEBUG = 2,
    LOG_INFO = 3,
    LOG_WARNING = 4,
    LOG_ERROR = 5,
    LOG_FATAL = 6,
    LOG_NONE = 7,
} TraceLogLevel;

typedef enum {
    KEY_NULL = 0,
    KEY_APOSTROPHE = 39,
    KEY_COMMA = 44,
    KEY_MINUS = 45,
    KEY_PERIOD = 46,
    KEY_SLASH = 47,
    KEY_ZERO = 48,
    KEY_ONE = 49,
    KEY_TWO = 50,
    KEY_THREE = 51,
    KEY_FOUR = 52,
    KEY_FIVE = 53,
    KEY_SIX = 54,
    KEY_SEVEN = 55,
    KEY_EIGHT = 56,
    KEY_NINE = 57,
    KEY_SEMICOLON = 59,
    KEY_EQUAL = 61,
    KEY_A = 65,
    KEY_B = 66,
    KEY_C = 67,
    KEY_D = 68,
    KEY_E = 69,
    KEY_F = 70,
    KEY_G = 71,
    KEY_H = 72,
    KEY_I = 73,
    KEY_J = 74,
    KEY_K = 75,
    KEY_L = 76,
    KEY_M = 77,
    KEY_N = 78,
    KEY_O = 79,
    KEY_P = 80,
    KEY_Q = 81,
    KEY_R = 82,
    KEY_S = 83,
    KEY_T = 84,
    KEY_U = 85,
    KEY_V = 86,
    KEY_W = 87,
    KEY_X = 88,
    KEY_Y = 89,
    KEY_Z = 90,
    KEY_LEFT_BRACKET = 91,
    KEY_BACKSLASH = 92,
    KEY_RIGHT_BRACKET = 93,
    KEY_GRAVE = 96,
    KEY_SPACE = 32,
    KEY_ESCAPE = 256,
    KEY_ENTER = 257,
    KEY_TAB = 258,
    KEY_BACKSPACE = 259,
    KEY_INSERT = 260,
    KEY_DELETE = 261,
    KEY_RIGHT = 262,
    KEY_LEFT = 263,
    KEY_DOWN = 264,
    KEY_UP = 265,
    KEY_PAGE_UP = 266,
    KEY_PAGE_DOWN = 267,
    KEY_HOME = 268,
    KEY_END = 269,
    KEY_CAPS_LOCK = 280,
    KEY_SCROLL_LOCK = 281,
    KEY_NUM_LOCK = 282,
    KEY_PRINT_SCREEN = 283,
    KEY_PAUSE = 284,
    KEY_F1 = 290,
    KEY_F2 = 291,
    KEY_F3 = 292,
    KEY_F4 = 293,
    KEY_F5 = 294,
    KEY_F6 = 295,
    KEY_F7 = 296,
    KEY_F8 = 297,
    KEY_F9 = 298,
    KEY_F10 = 299,
    KEY_F11 = 300,
    KEY_F12 = 301,
    KEY_LEFT_SHIFT = 340,
    KEY_LEFT_CONTROL = 341,
    KEY_LEFT_ALT = 342,
    KEY_LEFT_SUPER = 343,
    KEY_RIGHT_SHIFT = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT = 346,
    KEY_RIGHT_SUPER = 347,
    KEY_KB_MENU = 348,
    KEY_KP_0 = 320,
    KEY_KP_1 = 321,
    KEY_KP_2 = 322,
    KEY_KP_3 = 323,
    KEY_KP_4 = 324,
    KEY_KP_5 = 325,
    KEY_KP_6 = 326,
    KEY_KP_7 = 327,
    KEY_KP_8 = 328,
    KEY_KP_9 = 329,
    KEY_KP_DECIMAL = 330,
    KEY_KP_DIVIDE = 331,
    KEY_KP_MULTIPLY = 332,
    KEY_KP_SUBTRACT = 333,
    KEY_KP_ADD = 334,
    KEY_KP_ENTER = 335,
    KEY_KP_EQUAL = 336,
    KEY_BACK = 4,
    KEY_MENU = 5,
    KEY_VOLUME_UP = 24,
    KEY_VOLUME_DOWN = 25,
} KeyboardKey;

typedef enum {
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_RIGHT = 1,
    MOUSE_BUTTON_MIDDLE = 2,
    MOUSE_BUTTON_SIDE = 3,
    MOUSE_BUTTON_EXTRA = 4,
    MOUSE_BUTTON_FORWARD = 5,
    MOUSE_BUTTON_BACK = 6,
} MouseButton;

typedef enum {
    MOUSE_CURSOR_DEFAULT = 0,
    MOUSE_CURSOR_ARROW = 1,
    MOUSE_CURSOR_IBEAM = 2,
    MOUSE_CURSOR_CROSSHAIR = 3,
    MOUSE_CURSOR_POINTING_HAND = 4,
    MOUSE_CURSOR_RESIZE_EW = 5,
    MOUSE_CURSOR_RESIZE_NS = 6,
    MOUSE_CURSOR_RESIZE_NWSE = 7,
    MOUSE_CURSOR_RESIZE_NESW = 8,
    MOUSE_CURSOR_RESIZE_ALL = 9,
    MOUSE_CURSOR_NOT_ALLOWED = 10,
} MouseCursor;

typedef enum {
    GAMEPAD_BUTTON_UNKNOWN = 0,
    GAMEPAD_BUTTON_LEFT_FACE_UP = 1,
    GAMEPAD_BUTTON_LEFT_FACE_RIGHT = 2,
    GAMEPAD_BUTTON_LEFT_FACE_DOWN = 3,
    GAMEPAD_BUTTON_LEFT_FACE_LEFT = 4,
    GAMEPAD_BUTTON_RIGHT_FACE_UP = 5,
    GAMEPAD_BUTTON_RIGHT_FACE_RIGHT = 6,
    GAMEPAD_BUTTON_RIGHT_FACE_DOWN = 7,
    GAMEPAD_BUTTON_RIGHT_FACE_LEFT = 8,
    GAMEPAD_BUTTON_LEFT_TRIGGER_1 = 9,
    GAMEPAD_BUTTON_LEFT_TRIGGER_2 = 10,
    GAMEPAD_BUTTON_RIGHT_TRIGGER_1 = 11,
    GAMEPAD_BUTTON_RIGHT_TRIGGER_2 = 12,
    GAMEPAD_BUTTON_MIDDLE_LEFT = 13,
    GAMEPAD_BUTTON_MIDDLE = 14,
    GAMEPAD_BUTTON_MIDDLE_RIGHT = 15,
    GAMEPAD_BUTTON_LEFT_THUMB = 16,
    GAMEPAD_BUTTON_RIGHT_THUMB = 17,
} GamepadButton;

typedef enum {
    GAMEPAD_AXIS_LEFT_X = 0,
    GAMEPAD_AXIS_LEFT_Y = 1,
    GAMEPAD_AXIS_RIGHT_X = 2,
    GAMEPAD_AXIS_RIGHT_Y = 3,
    GAMEPAD_AXIS_LEFT_TRIGGER = 4,
    GAMEPAD_AXIS_RIGHT_TRIGGER = 5,
} GamepadAxis;

typedef enum {
    MATERIAL_MAP_ALBEDO = 0,
    MATERIAL_MAP_METALNESS = 1,
    MATERIAL_MAP_NORMAL = 2,
    MATERIAL_MAP_ROUGHNESS = 3,
    MATERIAL_MAP_OCCLUSION = 4,
    MATERIAL_MAP_EMISSION = 5,
    MATERIAL_MAP_HEIGHT = 6,
    MATERIAL_MAP_CUBEMAP = 7,
    MATERIAL_MAP_IRRADIANCE = 8,
    MATERIAL_MAP_PREFILTER = 9,
    MATERIAL_MAP_BRDF = 10,
} MaterialMapIndex;

typedef enum {
    SHADER_LOC_VERTEX_POSITION = 0,
    SHADER_LOC_VERTEX_TEXCOORD01 = 1,
    SHADER_LOC_VERTEX_TEXCOORD02 = 2,
    SHADER_LOC_VERTEX_NORMAL = 3,
    SHADER_LOC_VERTEX_TANGENT = 4,
    SHADER_LOC_VERTEX_COLOR = 5,
    SHADER_LOC_MATRIX_MVP = 6,
    SHADER_LOC_MATRIX_VIEW = 7,
    SHADER_LOC_MATRIX_PROJECTION = 8,
    SHADER_LOC_MATRIX_MODEL = 9,
    SHADER_LOC_MATRIX_NORMAL = 10,
    SHADER_LOC_VECTOR_VIEW = 11,
    SHADER_LOC_COLOR_DIFFUSE = 12,
    SHADER_LOC_COLOR_SPECULAR = 13,
    SHADER_LOC_COLOR_AMBIENT = 14,
    SHADER_LOC_MAP_ALBEDO = 15,
    SHADER_LOC_MAP_METALNESS = 16,
    SHADER_LOC_MAP_NORMAL = 17,
    SHADER_LOC_MAP_ROUGHNESS = 18,
    SHADER_LOC_MAP_OCCLUSION = 19,
    SHADER_LOC_MAP_EMISSION = 20,
    SHADER_LOC_MAP_HEIGHT = 21,
    SHADER_LOC_MAP_CUBEMAP = 22,
    SHADER_LOC_MAP_IRRADIANCE = 23,
    SHADER_LOC_MAP_PREFILTER = 24,
    SHADER_LOC_MAP_BRDF = 25,
    SHADER_LOC_VERTEX_BONEIDS = 26,
    SHADER_LOC_VERTEX_BONEWEIGHTS = 27,
    SHADER_LOC_MATRIX_BONETRANSFORMS = 28,
    SHADER_LOC_VERTEX_INSTANCETRANSFORM = 29,
} ShaderLocationIndex;

typedef enum {
    SHADER_UNIFORM_FLOAT = 0,
    SHADER_UNIFORM_VEC2 = 1,
    SHADER_UNIFORM_VEC3 = 2,
    SHADER_UNIFORM_VEC4 = 3,
    SHADER_UNIFORM_INT = 4,
    SHADER_UNIFORM_IVEC2 = 5,
    SHADER_UNIFORM_IVEC3 = 6,
    SHADER_UNIFORM_IVEC4 = 7,
    SHADER_UNIFORM_UINT = 8,
    SHADER_UNIFORM_UIVEC2 = 9,
    SHADER_UNIFORM_UIVEC3 = 10,
    SHADER_UNIFORM_UIVEC4 = 11,
    SHADER_UNIFORM_SAMPLER2D = 12,
} ShaderUniformDataType;

typedef enum {
    SHADER_ATTRIB_FLOAT = 0,
    SHADER_ATTRIB_VEC2 = 1,
    SHADER_ATTRIB_VEC3 = 2,
    SHADER_ATTRIB_VEC4 = 3,
} ShaderAttributeDataType;

typedef enum {
    PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1,
    PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA = 2,
    PIXELFORMAT_UNCOMPRESSED_R5G6B5 = 3,
    PIXELFORMAT_UNCOMPRESSED_R8G8B8 = 4,
    PIXELFORMAT_UNCOMPRESSED_R5G5B5A1 = 5,
    PIXELFORMAT_UNCOMPRESSED_R4G4B4A4 = 6,
    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 = 7,
    PIXELFORMAT_UNCOMPRESSED_R32 = 8,
    PIXELFORMAT_UNCOMPRESSED_R32G32B32 = 9,
    PIXELFORMAT_UNCOMPRESSED_R32G32B32A32 = 10,
    PIXELFORMAT_UNCOMPRESSED_R16 = 11,
    PIXELFORMAT_UNCOMPRESSED_R16G16B16 = 12,
    PIXELFORMAT_UNCOMPRESSED_R16G16B16A16 = 13,
    PIXELFORMAT_COMPRESSED_DXT1_RGB = 14,
    PIXELFORMAT_COMPRESSED_DXT1_RGBA = 15,
    PIXELFORMAT_COMPRESSED_DXT3_RGBA = 16,
    PIXELFORMAT_COMPRESSED_DXT5_RGBA = 17,
    PIXELFORMAT_COMPRESSED_ETC1_RGB = 18,
    PIXELFORMAT_COMPRESSED_ETC2_RGB = 19,
    PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA = 20,
    PIXELFORMAT_COMPRESSED_PVRT_RGB = 21,
    PIXELFORMAT_COMPRESSED_PVRT_RGBA = 22,
    PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA = 23,
    PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA = 24,
} PixelFormat;

typedef enum {
    TEXTURE_FILTER_POINT = 0,
    TEXTURE_FILTER_BILINEAR = 1,
    TEXTURE_FILTER_TRILINEAR = 2,
    TEXTURE_FILTER_ANISOTROPIC_4X = 3,
    TEXTURE_FILTER_ANISOTROPIC_8X = 4,
    TEXTURE_FILTER_ANISOTROPIC_16X = 5,
} TextureFilter;

typedef enum {
    TEXTURE_WRAP_REPEAT = 0,
    TEXTURE_WRAP_CLAMP = 1,
    TEXTURE_WRAP_MIRROR_REPEAT = 2,
    TEXTURE_WRAP_MIRROR_CLAMP = 3,
} TextureWrap;

typedef enum {
    CUBEMAP_LAYOUT_AUTO_DETECT = 0,
    CUBEMAP_LAYOUT_LINE_VERTICAL = 1,
    CUBEMAP_LAYOUT_LINE_HORIZONTAL = 2,
    CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR = 3,
    CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE = 4,
} CubemapLayout;

typedef enum {
    FONT_DEFAULT = 0,
    FONT_BITMAP = 1,
    FONT_SDF = 2,
} FontType;

typedef enum {
    BLEND_ALPHA = 0,
    BLEND_ADDITIVE = 1,
    BLEND_MULTIPLIED = 2,
    BLEND_ADD_COLORS = 3,
    BLEND_SUBTRACT_COLORS = 4,
    BLEND_ALPHA_PREMULTIPLY = 5,
    BLEND_CUSTOM = 6,
    BLEND_CUSTOM_SEPARATE = 7,
} BlendMode;

typedef enum {
    GESTURE_NONE = 0,
    GESTURE_TAP = 1,
    GESTURE_DOUBLETAP = 2,
    GESTURE_HOLD = 4,
    GESTURE_DRAG = 8,
    GESTURE_SWIPE_RIGHT = 16,
    GESTURE_SWIPE_LEFT = 32,
    GESTURE_SWIPE_UP = 64,
    GESTURE_SWIPE_DOWN = 128,
    GESTURE_PINCH_IN = 256,
    GESTURE_PINCH_OUT = 512,
} Gesture;

typedef enum {
    CAMERA_CUSTOM = 0,
    CAMERA_FREE = 1,
    CAMERA_ORBITAL = 2,
    CAMERA_FIRST_PERSON = 3,
    CAMERA_THIRD_PERSON = 4,
} CameraMode;

typedef enum {
    CAMERA_PERSPECTIVE = 0,
    CAMERA_ORTHOGRAPHIC = 1,
} CameraProjection;

typedef enum {
    NPATCH_NINE_PATCH = 0,
    NPATCH_THREE_PATCH_VERTICAL = 1,
    NPATCH_THREE_PATCH_HORIZONTAL = 2,
} NPatchLayout;

//----------------------------------------------------------------------------------
// Defines and constants
//----------------------------------------------------------------------------------
#define RAYLIB_VERSION_MAJOR     6
#define RAYLIB_VERSION_MINOR     1
#define RAYLIB_VERSION_PATCH     0
#define RAYLIB_VERSION           6.1-dev
#define PI                       3.141592653589793
#define DEG2RAD                  (PI/180.0f)
#define RAD2DEG                  (180.0f/PI)
#define LIGHTGRAY                (Color){ 200, 200, 200, 255 }
#define GRAY                     (Color){ 130, 130, 130, 255 }
#define DARKGRAY                 (Color){ 80, 80, 80, 255 }
#define YELLOW                   (Color){ 253, 249, 0, 255 }
#define GOLD                     (Color){ 255, 203, 0, 255 }
#define ORANGE                   (Color){ 255, 161, 0, 255 }
#define PINK                     (Color){ 255, 109, 194, 255 }
#define RED                      (Color){ 230, 41, 55, 255 }
#define MAROON                   (Color){ 190, 33, 55, 255 }
#define GREEN                    (Color){ 0, 228, 48, 255 }
#define LIME                     (Color){ 0, 158, 47, 255 }
#define DARKGREEN                (Color){ 0, 117, 44, 255 }
#define SKYBLUE                  (Color){ 102, 191, 255, 255 }
#define BLUE                     (Color){ 0, 121, 241, 255 }
#define DARKBLUE                 (Color){ 0, 82, 172, 255 }
#define PURPLE                   (Color){ 200, 122, 255, 255 }
#define VIOLET                   (Color){ 135, 60, 190, 255 }
#define DARKPURPLE               (Color){ 112, 31, 126, 255 }
#define BEIGE                    (Color){ 211, 176, 131, 255 }
#define BROWN                    (Color){ 127, 106, 79, 255 }
#define DARKBROWN                (Color){ 76, 63, 47, 255 }
#define WHITE                    (Color){ 255, 255, 255, 255 }
#define BLACK                    (Color){ 0, 0, 0, 255 }
#define BLANK                    (Color){ 0, 0, 0, 0 }
#define MAGENTA                  (Color){ 255, 0, 255, 255 }
#define RAYWHITE                 (Color){ 245, 245, 245, 255 }

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
__attribute__((import_module("raylib"), import_name("InitWindow")))
void InitWindow(int width, int height, const char * title);

__attribute__((import_module("raylib"), import_name("CloseWindow")))
void CloseWindow(void);

__attribute__((import_module("raylib"), import_name("WindowShouldClose")))
bool WindowShouldClose(void);

__attribute__((import_module("raylib"), import_name("IsWindowReady")))
bool IsWindowReady(void);

__attribute__((import_module("raylib"), import_name("IsWindowFullscreen")))
bool IsWindowFullscreen(void);

__attribute__((import_module("raylib"), import_name("IsWindowHidden")))
bool IsWindowHidden(void);

__attribute__((import_module("raylib"), import_name("IsWindowMinimized")))
bool IsWindowMinimized(void);

__attribute__((import_module("raylib"), import_name("IsWindowMaximized")))
bool IsWindowMaximized(void);

__attribute__((import_module("raylib"), import_name("IsWindowFocused")))
bool IsWindowFocused(void);

__attribute__((import_module("raylib"), import_name("IsWindowResized")))
bool IsWindowResized(void);

__attribute__((import_module("raylib"), import_name("IsWindowState")))
bool IsWindowState(unsigned int flag);

__attribute__((import_module("raylib"), import_name("SetWindowState")))
void SetWindowState(unsigned int flags);

__attribute__((import_module("raylib"), import_name("ClearWindowState")))
void ClearWindowState(unsigned int flags);

__attribute__((import_module("raylib"), import_name("ToggleFullscreen")))
void ToggleFullscreen(void);

__attribute__((import_module("raylib"), import_name("ToggleBorderlessWindowed")))
void ToggleBorderlessWindowed(void);

__attribute__((import_module("raylib"), import_name("MaximizeWindow")))
void MaximizeWindow(void);

__attribute__((import_module("raylib"), import_name("MinimizeWindow")))
void MinimizeWindow(void);

__attribute__((import_module("raylib"), import_name("RestoreWindow")))
void RestoreWindow(void);

__attribute__((import_module("raylib"), import_name("SetWindowIcon")))
void __raylib_SetWindowIcon(const Image* image);

static inline void SetWindowIcon(Image image) {
    __raylib_SetWindowIcon(&image);
}

__attribute__((import_module("raylib"), import_name("SetWindowIcons")))
void SetWindowIcons(Image * images, int count);

__attribute__((import_module("raylib"), import_name("SetWindowTitle")))
void SetWindowTitle(const char * title);

__attribute__((import_module("raylib"), import_name("SetWindowPosition")))
void SetWindowPosition(int x, int y);

__attribute__((import_module("raylib"), import_name("SetWindowMonitor")))
void SetWindowMonitor(int monitor);

__attribute__((import_module("raylib"), import_name("SetWindowMinSize")))
void SetWindowMinSize(int width, int height);

__attribute__((import_module("raylib"), import_name("SetWindowMaxSize")))
void SetWindowMaxSize(int width, int height);

__attribute__((import_module("raylib"), import_name("SetWindowSize")))
void SetWindowSize(int width, int height);

__attribute__((import_module("raylib"), import_name("SetWindowOpacity")))
void SetWindowOpacity(float opacity);

__attribute__((import_module("raylib"), import_name("SetWindowFocused")))
void SetWindowFocused(void);

__attribute__((import_module("raylib"), import_name("GetWindowHandle")))
void * GetWindowHandle(void);

__attribute__((import_module("raylib"), import_name("GetScreenWidth")))
int GetScreenWidth(void);

__attribute__((import_module("raylib"), import_name("GetScreenHeight")))
int GetScreenHeight(void);

__attribute__((import_module("raylib"), import_name("GetRenderWidth")))
int GetRenderWidth(void);

__attribute__((import_module("raylib"), import_name("GetRenderHeight")))
int GetRenderHeight(void);

__attribute__((import_module("raylib"), import_name("GetMonitorCount")))
int GetMonitorCount(void);

__attribute__((import_module("raylib"), import_name("GetCurrentMonitor")))
int GetCurrentMonitor(void);

__attribute__((import_module("raylib"), import_name("GetMonitorPosition")))
void __raylib_GetMonitorPosition(Vector2* __result, int monitor);

static inline Vector2 GetMonitorPosition(int monitor) {
    Vector2 __r;
    __raylib_GetMonitorPosition(&__r, monitor);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetMonitorWidth")))
int GetMonitorWidth(int monitor);

__attribute__((import_module("raylib"), import_name("GetMonitorHeight")))
int GetMonitorHeight(int monitor);

__attribute__((import_module("raylib"), import_name("GetMonitorPhysicalWidth")))
int GetMonitorPhysicalWidth(int monitor);

__attribute__((import_module("raylib"), import_name("GetMonitorPhysicalHeight")))
int GetMonitorPhysicalHeight(int monitor);

__attribute__((import_module("raylib"), import_name("GetMonitorRefreshRate")))
int GetMonitorRefreshRate(int monitor);

__attribute__((import_module("raylib"), import_name("GetWindowPosition")))
void __raylib_GetWindowPosition(Vector2* __result);

static inline Vector2 GetWindowPosition(void) {
    Vector2 __r;
    __raylib_GetWindowPosition(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetWindowScaleDPI")))
void __raylib_GetWindowScaleDPI(Vector2* __result);

static inline Vector2 GetWindowScaleDPI(void) {
    Vector2 __r;
    __raylib_GetWindowScaleDPI(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetMonitorName")))
const char * GetMonitorName(int monitor);

__attribute__((import_module("raylib"), import_name("SetClipboardText")))
void SetClipboardText(const char * text);

__attribute__((import_module("raylib"), import_name("GetClipboardText")))
const char * GetClipboardText(void);

__attribute__((import_module("raylib"), import_name("GetClipboardImage")))
void __raylib_GetClipboardImage(Image* __result);

static inline Image GetClipboardImage(void) {
    Image __r;
    __raylib_GetClipboardImage(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("EnableEventWaiting")))
void EnableEventWaiting(void);

__attribute__((import_module("raylib"), import_name("DisableEventWaiting")))
void DisableEventWaiting(void);

__attribute__((import_module("raylib"), import_name("ShowCursor")))
void ShowCursor(void);

__attribute__((import_module("raylib"), import_name("HideCursor")))
void HideCursor(void);

__attribute__((import_module("raylib"), import_name("IsCursorHidden")))
bool IsCursorHidden(void);

__attribute__((import_module("raylib"), import_name("EnableCursor")))
void EnableCursor(void);

__attribute__((import_module("raylib"), import_name("DisableCursor")))
void DisableCursor(void);

__attribute__((import_module("raylib"), import_name("IsCursorOnScreen")))
bool IsCursorOnScreen(void);

__attribute__((import_module("raylib"), import_name("ClearBackground")))
void __raylib_ClearBackground(const Color* color);

static inline void ClearBackground(Color color) {
    __raylib_ClearBackground(&color);
}

__attribute__((import_module("raylib"), import_name("BeginDrawing")))
void BeginDrawing(void);

__attribute__((import_module("raylib"), import_name("EndDrawing")))
void EndDrawing(void);

__attribute__((import_module("raylib"), import_name("BeginMode2D")))
void __raylib_BeginMode2D(const Camera2D* camera);

static inline void BeginMode2D(Camera2D camera) {
    __raylib_BeginMode2D(&camera);
}

__attribute__((import_module("raylib"), import_name("EndMode2D")))
void EndMode2D(void);

__attribute__((import_module("raylib"), import_name("BeginMode3D")))
void __raylib_BeginMode3D(const Camera3D* camera);

static inline void BeginMode3D(Camera3D camera) {
    __raylib_BeginMode3D(&camera);
}

__attribute__((import_module("raylib"), import_name("EndMode3D")))
void EndMode3D(void);

__attribute__((import_module("raylib"), import_name("BeginTextureMode")))
void __raylib_BeginTextureMode(const RenderTexture2D* target);

static inline void BeginTextureMode(RenderTexture2D target) {
    __raylib_BeginTextureMode(&target);
}

__attribute__((import_module("raylib"), import_name("EndTextureMode")))
void EndTextureMode(void);

__attribute__((import_module("raylib"), import_name("BeginShaderMode")))
void __raylib_BeginShaderMode(const Shader* shader);

static inline void BeginShaderMode(Shader shader) {
    __raylib_BeginShaderMode(&shader);
}

__attribute__((import_module("raylib"), import_name("EndShaderMode")))
void EndShaderMode(void);

__attribute__((import_module("raylib"), import_name("BeginBlendMode")))
void BeginBlendMode(int mode);

__attribute__((import_module("raylib"), import_name("EndBlendMode")))
void EndBlendMode(void);

__attribute__((import_module("raylib"), import_name("BeginScissorMode")))
void BeginScissorMode(int x, int y, int width, int height);

__attribute__((import_module("raylib"), import_name("EndScissorMode")))
void EndScissorMode(void);

__attribute__((import_module("raylib"), import_name("BeginVrStereoMode")))
void __raylib_BeginVrStereoMode(const VrStereoConfig* config);

static inline void BeginVrStereoMode(VrStereoConfig config) {
    __raylib_BeginVrStereoMode(&config);
}

__attribute__((import_module("raylib"), import_name("EndVrStereoMode")))
void EndVrStereoMode(void);

__attribute__((import_module("raylib"), import_name("LoadVrStereoConfig")))
void __raylib_LoadVrStereoConfig(VrStereoConfig* __result, const VrDeviceInfo* device);

static inline VrStereoConfig LoadVrStereoConfig(VrDeviceInfo device) {
    VrStereoConfig __r;
    __raylib_LoadVrStereoConfig(&__r, &device);
    return __r;
}

__attribute__((import_module("raylib"), import_name("UnloadVrStereoConfig")))
void __raylib_UnloadVrStereoConfig(const VrStereoConfig* config);

static inline void UnloadVrStereoConfig(VrStereoConfig config) {
    __raylib_UnloadVrStereoConfig(&config);
}

__attribute__((import_module("raylib"), import_name("LoadShader")))
void __raylib_LoadShader(Shader* __result, const char * vsFileName, const char * fsFileName);

static inline Shader LoadShader(const char * vsFileName, const char * fsFileName) {
    Shader __r;
    __raylib_LoadShader(&__r, vsFileName, fsFileName);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadShaderFromMemory")))
void __raylib_LoadShaderFromMemory(Shader* __result, const char * vsCode, const char * fsCode);

static inline Shader LoadShaderFromMemory(const char * vsCode, const char * fsCode) {
    Shader __r;
    __raylib_LoadShaderFromMemory(&__r, vsCode, fsCode);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsShaderValid")))
bool __raylib_IsShaderValid(const Shader* shader);

static inline bool IsShaderValid(Shader shader) {
    return __raylib_IsShaderValid(&shader);
}

__attribute__((import_module("raylib"), import_name("GetShaderLocation")))
int __raylib_GetShaderLocation(const Shader* shader, const char * uniformName);

static inline int GetShaderLocation(Shader shader, const char * uniformName) {
    return __raylib_GetShaderLocation(&shader, uniformName);
}

__attribute__((import_module("raylib"), import_name("GetShaderLocationAttrib")))
int __raylib_GetShaderLocationAttrib(const Shader* shader, const char * attribName);

static inline int GetShaderLocationAttrib(Shader shader, const char * attribName) {
    return __raylib_GetShaderLocationAttrib(&shader, attribName);
}

__attribute__((import_module("raylib"), import_name("SetShaderValue")))
void __raylib_SetShaderValue(const Shader* shader, int locIndex, const void * value, int uniformType);

static inline void SetShaderValue(Shader shader, int locIndex, const void * value, int uniformType) {
    __raylib_SetShaderValue(&shader, locIndex, value, uniformType);
}

__attribute__((import_module("raylib"), import_name("SetShaderValueV")))
void __raylib_SetShaderValueV(const Shader* shader, int locIndex, const void * value, int uniformType, int count);

static inline void SetShaderValueV(Shader shader, int locIndex, const void * value, int uniformType, int count) {
    __raylib_SetShaderValueV(&shader, locIndex, value, uniformType, count);
}

__attribute__((import_module("raylib"), import_name("SetShaderValueMatrix")))
void __raylib_SetShaderValueMatrix(const Shader* shader, int locIndex, const Matrix* mat);

static inline void SetShaderValueMatrix(Shader shader, int locIndex, Matrix mat) {
    __raylib_SetShaderValueMatrix(&shader, locIndex, &mat);
}

__attribute__((import_module("raylib"), import_name("SetShaderValueTexture")))
void __raylib_SetShaderValueTexture(const Shader* shader, int locIndex, const Texture2D* texture);

static inline void SetShaderValueTexture(Shader shader, int locIndex, Texture2D texture) {
    __raylib_SetShaderValueTexture(&shader, locIndex, &texture);
}

__attribute__((import_module("raylib"), import_name("UnloadShader")))
void __raylib_UnloadShader(const Shader* shader);

static inline void UnloadShader(Shader shader) {
    __raylib_UnloadShader(&shader);
}

__attribute__((import_module("raylib"), import_name("GetScreenToWorldRay")))
void __raylib_GetScreenToWorldRay(Ray* __result, const Vector2* position, const Camera* camera);

static inline Ray GetScreenToWorldRay(Vector2 position, Camera camera) {
    Ray __r;
    __raylib_GetScreenToWorldRay(&__r, &position, &camera);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetScreenToWorldRayEx")))
void __raylib_GetScreenToWorldRayEx(Ray* __result, const Vector2* position, const Camera* camera, int width, int height);

static inline Ray GetScreenToWorldRayEx(Vector2 position, Camera camera, int width, int height) {
    Ray __r;
    __raylib_GetScreenToWorldRayEx(&__r, &position, &camera, width, height);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetWorldToScreen")))
void __raylib_GetWorldToScreen(Vector2* __result, const Vector3* position, const Camera* camera);

static inline Vector2 GetWorldToScreen(Vector3 position, Camera camera) {
    Vector2 __r;
    __raylib_GetWorldToScreen(&__r, &position, &camera);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetWorldToScreenEx")))
void __raylib_GetWorldToScreenEx(Vector2* __result, const Vector3* position, const Camera* camera, int width, int height);

static inline Vector2 GetWorldToScreenEx(Vector3 position, Camera camera, int width, int height) {
    Vector2 __r;
    __raylib_GetWorldToScreenEx(&__r, &position, &camera, width, height);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetWorldToScreen2D")))
void __raylib_GetWorldToScreen2D(Vector2* __result, const Vector2* position, const Camera2D* camera);

static inline Vector2 GetWorldToScreen2D(Vector2 position, Camera2D camera) {
    Vector2 __r;
    __raylib_GetWorldToScreen2D(&__r, &position, &camera);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetScreenToWorld2D")))
void __raylib_GetScreenToWorld2D(Vector2* __result, const Vector2* position, const Camera2D* camera);

static inline Vector2 GetScreenToWorld2D(Vector2 position, Camera2D camera) {
    Vector2 __r;
    __raylib_GetScreenToWorld2D(&__r, &position, &camera);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetCameraMatrix")))
void __raylib_GetCameraMatrix(Matrix* __result, const Camera* camera);

static inline Matrix GetCameraMatrix(Camera camera) {
    Matrix __r;
    __raylib_GetCameraMatrix(&__r, &camera);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetCameraMatrix2D")))
void __raylib_GetCameraMatrix2D(Matrix* __result, const Camera2D* camera);

static inline Matrix GetCameraMatrix2D(Camera2D camera) {
    Matrix __r;
    __raylib_GetCameraMatrix2D(&__r, &camera);
    return __r;
}

__attribute__((import_module("raylib"), import_name("SetTargetFPS")))
void SetTargetFPS(int fps);

__attribute__((import_module("raylib"), import_name("GetFrameTime")))
float GetFrameTime(void);

__attribute__((import_module("raylib"), import_name("GetTime")))
double GetTime(void);

__attribute__((import_module("raylib"), import_name("GetFPS")))
int GetFPS(void);

__attribute__((import_module("raylib"), import_name("SwapScreenBuffer")))
void SwapScreenBuffer(void);

__attribute__((import_module("raylib"), import_name("PollInputEvents")))
void PollInputEvents(void);

__attribute__((import_module("raylib"), import_name("WaitTime")))
void WaitTime(double seconds);

__attribute__((import_module("raylib"), import_name("SetRandomSeed")))
void SetRandomSeed(unsigned int seed);

__attribute__((import_module("raylib"), import_name("GetRandomValue")))
int GetRandomValue(int min, int max);

__attribute__((import_module("raylib"), import_name("LoadRandomSequence")))
int * LoadRandomSequence(unsigned int count, int min, int max);

__attribute__((import_module("raylib"), import_name("UnloadRandomSequence")))
void UnloadRandomSequence(int * sequence);

__attribute__((import_module("raylib"), import_name("TakeScreenshot")))
void TakeScreenshot(const char * fileName);

__attribute__((import_module("raylib"), import_name("SetConfigFlags")))
void SetConfigFlags(unsigned int flags);

__attribute__((import_module("raylib"), import_name("OpenURL")))
void OpenURL(const char * url);

__attribute__((import_module("raylib"), import_name("SetTraceLogLevel")))
void SetTraceLogLevel(int logLevel);

__attribute__((import_module("raylib"), import_name("TraceLog")))
void __raylib_TraceLog(int logLevel, const char * text);

static inline void TraceLog(int logLevel, const char * text, ...) {
    char __buf[1024];
    va_list __ap;
    va_start(__ap, text);
    vsnprintf(__buf, sizeof(__buf), text, __ap);
    va_end(__ap);
    __raylib_TraceLog(logLevel, __buf);
}

__attribute__((import_module("raylib"), import_name("LoadFileData")))
unsigned char * LoadFileData(const char * fileName, int * dataSize);

__attribute__((import_module("raylib"), import_name("UnloadFileData")))
void UnloadFileData(unsigned char * data);

__attribute__((import_module("raylib"), import_name("SaveFileData")))
bool SaveFileData(const char * fileName, const void * data, int dataSize);

__attribute__((import_module("raylib"), import_name("ExportDataAsCode")))
bool ExportDataAsCode(const unsigned char * data, int dataSize, const char * fileName);

__attribute__((import_module("raylib"), import_name("LoadFileText")))
char * LoadFileText(const char * fileName);

__attribute__((import_module("raylib"), import_name("UnloadFileText")))
void UnloadFileText(char * text);

__attribute__((import_module("raylib"), import_name("SaveFileText")))
bool SaveFileText(const char * fileName, const char * text);

__attribute__((import_module("raylib"), import_name("FileRename")))
int FileRename(const char * fileName, const char * fileRename);

__attribute__((import_module("raylib"), import_name("FileRemove")))
int FileRemove(const char * fileName);

__attribute__((import_module("raylib"), import_name("FileCopy")))
int FileCopy(const char * srcPath, const char * dstPath);

__attribute__((import_module("raylib"), import_name("FileMove")))
int FileMove(const char * srcPath, const char * dstPath);

__attribute__((import_module("raylib"), import_name("FileTextReplace")))
int FileTextReplace(const char * fileName, const char * search, const char * replacement);

__attribute__((import_module("raylib"), import_name("FileTextFindIndex")))
int FileTextFindIndex(const char * fileName, const char * search);

__attribute__((import_module("raylib"), import_name("FileExists")))
bool FileExists(const char * fileName);

__attribute__((import_module("raylib"), import_name("DirectoryExists")))
bool DirectoryExists(const char * dirPath);

__attribute__((import_module("raylib"), import_name("IsFileExtension")))
bool IsFileExtension(const char * fileName, const char * ext);

__attribute__((import_module("raylib"), import_name("GetFileLength")))
int GetFileLength(const char * fileName);

__attribute__((import_module("raylib"), import_name("GetFileModTime")))
long GetFileModTime(const char * fileName);

__attribute__((import_module("raylib"), import_name("GetFileExtension")))
const char * GetFileExtension(const char * fileName);

__attribute__((import_module("raylib"), import_name("GetFileName")))
const char * GetFileName(const char * filePath);

__attribute__((import_module("raylib"), import_name("GetFileNameWithoutExt")))
const char * GetFileNameWithoutExt(const char * filePath);

__attribute__((import_module("raylib"), import_name("GetDirectoryPath")))
const char * GetDirectoryPath(const char * filePath);

__attribute__((import_module("raylib"), import_name("GetPrevDirectoryPath")))
const char * GetPrevDirectoryPath(const char * dirPath);

__attribute__((import_module("raylib"), import_name("GetWorkingDirectory")))
const char * GetWorkingDirectory(void);

__attribute__((import_module("raylib"), import_name("GetApplicationDirectory")))
const char * GetApplicationDirectory(void);

__attribute__((import_module("raylib"), import_name("MakeDirectory")))
int MakeDirectory(const char * dirPath);

__attribute__((import_module("raylib"), import_name("ChangeDirectory")))
bool ChangeDirectory(const char * dirPath);

__attribute__((import_module("raylib"), import_name("IsPathFile")))
bool IsPathFile(const char * path);

__attribute__((import_module("raylib"), import_name("IsFileNameValid")))
bool IsFileNameValid(const char * fileName);

__attribute__((import_module("raylib"), import_name("LoadDirectoryFiles")))
void __raylib_LoadDirectoryFiles(FilePathList* __result, const char * dirPath);

static inline FilePathList LoadDirectoryFiles(const char * dirPath) {
    FilePathList __r;
    __raylib_LoadDirectoryFiles(&__r, dirPath);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadDirectoryFilesEx")))
void __raylib_LoadDirectoryFilesEx(FilePathList* __result, const char * basePath, const char * filter, bool scanSubdirs);

static inline FilePathList LoadDirectoryFilesEx(const char * basePath, const char * filter, bool scanSubdirs) {
    FilePathList __r;
    __raylib_LoadDirectoryFilesEx(&__r, basePath, filter, scanSubdirs);
    return __r;
}

__attribute__((import_module("raylib"), import_name("UnloadDirectoryFiles")))
void __raylib_UnloadDirectoryFiles(const FilePathList* files);

static inline void UnloadDirectoryFiles(FilePathList files) {
    __raylib_UnloadDirectoryFiles(&files);
}

__attribute__((import_module("raylib"), import_name("IsFileDropped")))
bool IsFileDropped(void);

__attribute__((import_module("raylib"), import_name("LoadDroppedFiles")))
void __raylib_LoadDroppedFiles(FilePathList* __result);

static inline FilePathList LoadDroppedFiles(void) {
    FilePathList __r;
    __raylib_LoadDroppedFiles(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("UnloadDroppedFiles")))
void __raylib_UnloadDroppedFiles(const FilePathList* files);

static inline void UnloadDroppedFiles(FilePathList files) {
    __raylib_UnloadDroppedFiles(&files);
}

__attribute__((import_module("raylib"), import_name("GetDirectoryFileCount")))
unsigned int GetDirectoryFileCount(const char * dirPath);

__attribute__((import_module("raylib"), import_name("GetDirectoryFileCountEx")))
unsigned int GetDirectoryFileCountEx(const char * basePath, const char * filter, bool scanSubdirs);

__attribute__((import_module("raylib"), import_name("CompressData")))
unsigned char * CompressData(const unsigned char * data, int dataSize, int * compDataSize);

__attribute__((import_module("raylib"), import_name("DecompressData")))
unsigned char * DecompressData(const unsigned char * compData, int compDataSize, int * dataSize);

__attribute__((import_module("raylib"), import_name("EncodeDataBase64")))
char * EncodeDataBase64(const unsigned char * data, int dataSize, int * outputSize);

__attribute__((import_module("raylib"), import_name("DecodeDataBase64")))
unsigned char * DecodeDataBase64(const char * text, int * outputSize);

__attribute__((import_module("raylib"), import_name("ComputeCRC32")))
unsigned int ComputeCRC32(const unsigned char * data, int dataSize);

__attribute__((import_module("raylib"), import_name("ComputeMD5")))
unsigned int * ComputeMD5(const unsigned char * data, int dataSize);

__attribute__((import_module("raylib"), import_name("ComputeSHA1")))
unsigned int * ComputeSHA1(const unsigned char * data, int dataSize);

__attribute__((import_module("raylib"), import_name("ComputeSHA256")))
unsigned int * ComputeSHA256(const unsigned char * data, int dataSize);

__attribute__((import_module("raylib"), import_name("LoadAutomationEventList")))
void __raylib_LoadAutomationEventList(AutomationEventList* __result, const char * fileName);

static inline AutomationEventList LoadAutomationEventList(const char * fileName) {
    AutomationEventList __r;
    __raylib_LoadAutomationEventList(&__r, fileName);
    return __r;
}

__attribute__((import_module("raylib"), import_name("UnloadAutomationEventList")))
void __raylib_UnloadAutomationEventList(const AutomationEventList* list);

static inline void UnloadAutomationEventList(AutomationEventList list) {
    __raylib_UnloadAutomationEventList(&list);
}

__attribute__((import_module("raylib"), import_name("ExportAutomationEventList")))
bool __raylib_ExportAutomationEventList(const AutomationEventList* list, const char * fileName);

static inline bool ExportAutomationEventList(AutomationEventList list, const char * fileName) {
    return __raylib_ExportAutomationEventList(&list, fileName);
}

__attribute__((import_module("raylib"), import_name("SetAutomationEventList")))
void SetAutomationEventList(AutomationEventList * list);

__attribute__((import_module("raylib"), import_name("SetAutomationEventBaseFrame")))
void SetAutomationEventBaseFrame(int frame);

__attribute__((import_module("raylib"), import_name("StartAutomationEventRecording")))
void StartAutomationEventRecording(void);

__attribute__((import_module("raylib"), import_name("StopAutomationEventRecording")))
void StopAutomationEventRecording(void);

__attribute__((import_module("raylib"), import_name("PlayAutomationEvent")))
void __raylib_PlayAutomationEvent(const AutomationEvent* event);

static inline void PlayAutomationEvent(AutomationEvent event) {
    __raylib_PlayAutomationEvent(&event);
}

__attribute__((import_module("raylib"), import_name("IsKeyPressed")))
bool IsKeyPressed(int key);

__attribute__((import_module("raylib"), import_name("IsKeyPressedRepeat")))
bool IsKeyPressedRepeat(int key);

__attribute__((import_module("raylib"), import_name("IsKeyDown")))
bool IsKeyDown(int key);

__attribute__((import_module("raylib"), import_name("IsKeyReleased")))
bool IsKeyReleased(int key);

__attribute__((import_module("raylib"), import_name("IsKeyUp")))
bool IsKeyUp(int key);

__attribute__((import_module("raylib"), import_name("GetKeyPressed")))
int GetKeyPressed(void);

__attribute__((import_module("raylib"), import_name("GetCharPressed")))
int GetCharPressed(void);

__attribute__((import_module("raylib"), import_name("GetKeyName")))
const char * GetKeyName(int key);

__attribute__((import_module("raylib"), import_name("SetExitKey")))
void SetExitKey(int key);

__attribute__((import_module("raylib"), import_name("IsGamepadAvailable")))
bool IsGamepadAvailable(int gamepad);

__attribute__((import_module("raylib"), import_name("GetGamepadName")))
const char * GetGamepadName(int gamepad);

__attribute__((import_module("raylib"), import_name("IsGamepadButtonPressed")))
bool IsGamepadButtonPressed(int gamepad, int button);

__attribute__((import_module("raylib"), import_name("IsGamepadButtonDown")))
bool IsGamepadButtonDown(int gamepad, int button);

__attribute__((import_module("raylib"), import_name("IsGamepadButtonReleased")))
bool IsGamepadButtonReleased(int gamepad, int button);

__attribute__((import_module("raylib"), import_name("IsGamepadButtonUp")))
bool IsGamepadButtonUp(int gamepad, int button);

__attribute__((import_module("raylib"), import_name("GetGamepadButtonPressed")))
int GetGamepadButtonPressed(void);

__attribute__((import_module("raylib"), import_name("GetGamepadAxisCount")))
int GetGamepadAxisCount(int gamepad);

__attribute__((import_module("raylib"), import_name("GetGamepadAxisMovement")))
float GetGamepadAxisMovement(int gamepad, int axis);

__attribute__((import_module("raylib"), import_name("SetGamepadMappings")))
int SetGamepadMappings(const char * mappings);

__attribute__((import_module("raylib"), import_name("SetGamepadVibration")))
void SetGamepadVibration(int gamepad, float leftMotor, float rightMotor, float duration);

__attribute__((import_module("raylib"), import_name("IsMouseButtonPressed")))
bool IsMouseButtonPressed(int button);

__attribute__((import_module("raylib"), import_name("IsMouseButtonDown")))
bool IsMouseButtonDown(int button);

__attribute__((import_module("raylib"), import_name("IsMouseButtonReleased")))
bool IsMouseButtonReleased(int button);

__attribute__((import_module("raylib"), import_name("IsMouseButtonUp")))
bool IsMouseButtonUp(int button);

__attribute__((import_module("raylib"), import_name("GetMouseX")))
int GetMouseX(void);

__attribute__((import_module("raylib"), import_name("GetMouseY")))
int GetMouseY(void);

__attribute__((import_module("raylib"), import_name("GetMousePosition")))
void __raylib_GetMousePosition(Vector2* __result);

static inline Vector2 GetMousePosition(void) {
    Vector2 __r;
    __raylib_GetMousePosition(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetMouseDelta")))
void __raylib_GetMouseDelta(Vector2* __result);

static inline Vector2 GetMouseDelta(void) {
    Vector2 __r;
    __raylib_GetMouseDelta(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("SetMousePosition")))
void SetMousePosition(int x, int y);

__attribute__((import_module("raylib"), import_name("SetMouseOffset")))
void SetMouseOffset(int offsetX, int offsetY);

__attribute__((import_module("raylib"), import_name("SetMouseScale")))
void SetMouseScale(float scaleX, float scaleY);

__attribute__((import_module("raylib"), import_name("GetMouseWheelMove")))
float GetMouseWheelMove(void);

__attribute__((import_module("raylib"), import_name("GetMouseWheelMoveV")))
void __raylib_GetMouseWheelMoveV(Vector2* __result);

static inline Vector2 GetMouseWheelMoveV(void) {
    Vector2 __r;
    __raylib_GetMouseWheelMoveV(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("SetMouseCursor")))
void SetMouseCursor(int cursor);

__attribute__((import_module("raylib"), import_name("GetTouchX")))
int GetTouchX(void);

__attribute__((import_module("raylib"), import_name("GetTouchY")))
int GetTouchY(void);

__attribute__((import_module("raylib"), import_name("GetTouchPosition")))
void __raylib_GetTouchPosition(Vector2* __result, int index);

static inline Vector2 GetTouchPosition(int index) {
    Vector2 __r;
    __raylib_GetTouchPosition(&__r, index);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetTouchPointId")))
int GetTouchPointId(int index);

__attribute__((import_module("raylib"), import_name("GetTouchPointCount")))
int GetTouchPointCount(void);

__attribute__((import_module("raylib"), import_name("SetGesturesEnabled")))
void SetGesturesEnabled(unsigned int flags);

__attribute__((import_module("raylib"), import_name("IsGestureDetected")))
bool IsGestureDetected(unsigned int gesture);

__attribute__((import_module("raylib"), import_name("GetGestureDetected")))
int GetGestureDetected(void);

__attribute__((import_module("raylib"), import_name("GetGestureHoldDuration")))
float GetGestureHoldDuration(void);

__attribute__((import_module("raylib"), import_name("GetGestureDragVector")))
void __raylib_GetGestureDragVector(Vector2* __result);

static inline Vector2 GetGestureDragVector(void) {
    Vector2 __r;
    __raylib_GetGestureDragVector(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetGestureDragAngle")))
float GetGestureDragAngle(void);

__attribute__((import_module("raylib"), import_name("GetGesturePinchVector")))
void __raylib_GetGesturePinchVector(Vector2* __result);

static inline Vector2 GetGesturePinchVector(void) {
    Vector2 __r;
    __raylib_GetGesturePinchVector(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetGesturePinchAngle")))
float GetGesturePinchAngle(void);

__attribute__((import_module("raylib"), import_name("UpdateCamera")))
void UpdateCamera(Camera * camera, int mode);

__attribute__((import_module("raylib"), import_name("UpdateCameraPro")))
void __raylib_UpdateCameraPro(Camera * camera, const Vector3* movement, const Vector3* rotation, float zoom);

static inline void UpdateCameraPro(Camera * camera, Vector3 movement, Vector3 rotation, float zoom) {
    __raylib_UpdateCameraPro(camera, &movement, &rotation, zoom);
}

__attribute__((import_module("raylib"), import_name("SetShapesTexture")))
void __raylib_SetShapesTexture(const Texture2D* texture, const Rectangle* source);

static inline void SetShapesTexture(Texture2D texture, Rectangle source) {
    __raylib_SetShapesTexture(&texture, &source);
}

__attribute__((import_module("raylib"), import_name("GetShapesTexture")))
void __raylib_GetShapesTexture(Texture2D* __result);

static inline Texture2D GetShapesTexture(void) {
    Texture2D __r;
    __raylib_GetShapesTexture(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetShapesTextureRectangle")))
void __raylib_GetShapesTextureRectangle(Rectangle* __result);

static inline Rectangle GetShapesTextureRectangle(void) {
    Rectangle __r;
    __raylib_GetShapesTextureRectangle(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("DrawPixel")))
void __raylib_DrawPixel(int posX, int posY, const Color* color);

static inline void DrawPixel(int posX, int posY, Color color) {
    __raylib_DrawPixel(posX, posY, &color);
}

__attribute__((import_module("raylib"), import_name("DrawPixelV")))
void __raylib_DrawPixelV(const Vector2* position, const Color* color);

static inline void DrawPixelV(Vector2 position, Color color) {
    __raylib_DrawPixelV(&position, &color);
}

__attribute__((import_module("raylib"), import_name("DrawLine")))
void __raylib_DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, const Color* color);

static inline void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color) {
    __raylib_DrawLine(startPosX, startPosY, endPosX, endPosY, &color);
}

__attribute__((import_module("raylib"), import_name("DrawLineV")))
void __raylib_DrawLineV(const Vector2* startPos, const Vector2* endPos, const Color* color);

static inline void DrawLineV(Vector2 startPos, Vector2 endPos, Color color) {
    __raylib_DrawLineV(&startPos, &endPos, &color);
}

__attribute__((import_module("raylib"), import_name("DrawLineEx")))
void __raylib_DrawLineEx(const Vector2* startPos, const Vector2* endPos, float thick, const Color* color);

static inline void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color) {
    __raylib_DrawLineEx(&startPos, &endPos, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawLineStrip")))
void __raylib_DrawLineStrip(const Vector2 * points, int pointCount, const Color* color);

static inline void DrawLineStrip(const Vector2 * points, int pointCount, Color color) {
    __raylib_DrawLineStrip(points, pointCount, &color);
}

__attribute__((import_module("raylib"), import_name("DrawLineBezier")))
void __raylib_DrawLineBezier(const Vector2* startPos, const Vector2* endPos, float thick, const Color* color);

static inline void DrawLineBezier(Vector2 startPos, Vector2 endPos, float thick, Color color) {
    __raylib_DrawLineBezier(&startPos, &endPos, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawLineDashed")))
void __raylib_DrawLineDashed(const Vector2* startPos, const Vector2* endPos, int dashSize, int spaceSize, const Color* color);

static inline void DrawLineDashed(Vector2 startPos, Vector2 endPos, int dashSize, int spaceSize, Color color) {
    __raylib_DrawLineDashed(&startPos, &endPos, dashSize, spaceSize, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCircle")))
void __raylib_DrawCircle(int centerX, int centerY, float radius, const Color* color);

static inline void DrawCircle(int centerX, int centerY, float radius, Color color) {
    __raylib_DrawCircle(centerX, centerY, radius, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCircleV")))
void __raylib_DrawCircleV(const Vector2* center, float radius, const Color* color);

static inline void DrawCircleV(Vector2 center, float radius, Color color) {
    __raylib_DrawCircleV(&center, radius, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCircleGradient")))
void __raylib_DrawCircleGradient(const Vector2* center, float radius, const Color* inner, const Color* outer);

static inline void DrawCircleGradient(Vector2 center, float radius, Color inner, Color outer) {
    __raylib_DrawCircleGradient(&center, radius, &inner, &outer);
}

__attribute__((import_module("raylib"), import_name("DrawCircleSector")))
void __raylib_DrawCircleSector(const Vector2* center, float radius, float startAngle, float endAngle, int segments, const Color* color);

static inline void DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color) {
    __raylib_DrawCircleSector(&center, radius, startAngle, endAngle, segments, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCircleSectorLines")))
void __raylib_DrawCircleSectorLines(const Vector2* center, float radius, float startAngle, float endAngle, int segments, const Color* color);

static inline void DrawCircleSectorLines(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color) {
    __raylib_DrawCircleSectorLines(&center, radius, startAngle, endAngle, segments, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCircleLines")))
void __raylib_DrawCircleLines(int centerX, int centerY, float radius, const Color* color);

static inline void DrawCircleLines(int centerX, int centerY, float radius, Color color) {
    __raylib_DrawCircleLines(centerX, centerY, radius, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCircleLinesV")))
void __raylib_DrawCircleLinesV(const Vector2* center, float radius, const Color* color);

static inline void DrawCircleLinesV(Vector2 center, float radius, Color color) {
    __raylib_DrawCircleLinesV(&center, radius, &color);
}

__attribute__((import_module("raylib"), import_name("DrawEllipse")))
void __raylib_DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, const Color* color);

static inline void DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, Color color) {
    __raylib_DrawEllipse(centerX, centerY, radiusH, radiusV, &color);
}

__attribute__((import_module("raylib"), import_name("DrawEllipseV")))
void __raylib_DrawEllipseV(const Vector2* center, float radiusH, float radiusV, const Color* color);

static inline void DrawEllipseV(Vector2 center, float radiusH, float radiusV, Color color) {
    __raylib_DrawEllipseV(&center, radiusH, radiusV, &color);
}

__attribute__((import_module("raylib"), import_name("DrawEllipseLines")))
void __raylib_DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, const Color* color);

static inline void DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color) {
    __raylib_DrawEllipseLines(centerX, centerY, radiusH, radiusV, &color);
}

__attribute__((import_module("raylib"), import_name("DrawEllipseLinesV")))
void __raylib_DrawEllipseLinesV(const Vector2* center, float radiusH, float radiusV, const Color* color);

static inline void DrawEllipseLinesV(Vector2 center, float radiusH, float radiusV, Color color) {
    __raylib_DrawEllipseLinesV(&center, radiusH, radiusV, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRing")))
void __raylib_DrawRing(const Vector2* center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, const Color* color);

static inline void DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color) {
    __raylib_DrawRing(&center, innerRadius, outerRadius, startAngle, endAngle, segments, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRingLines")))
void __raylib_DrawRingLines(const Vector2* center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, const Color* color);

static inline void DrawRingLines(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color) {
    __raylib_DrawRingLines(&center, innerRadius, outerRadius, startAngle, endAngle, segments, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRectangle")))
void __raylib_DrawRectangle(int posX, int posY, int width, int height, const Color* color);

static inline void DrawRectangle(int posX, int posY, int width, int height, Color color) {
    __raylib_DrawRectangle(posX, posY, width, height, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleV")))
void __raylib_DrawRectangleV(const Vector2* position, const Vector2* size, const Color* color);

static inline void DrawRectangleV(Vector2 position, Vector2 size, Color color) {
    __raylib_DrawRectangleV(&position, &size, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleRec")))
void __raylib_DrawRectangleRec(const Rectangle* rec, const Color* color);

static inline void DrawRectangleRec(Rectangle rec, Color color) {
    __raylib_DrawRectangleRec(&rec, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRectanglePro")))
void __raylib_DrawRectanglePro(const Rectangle* rec, const Vector2* origin, float rotation, const Color* color);

static inline void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color) {
    __raylib_DrawRectanglePro(&rec, &origin, rotation, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleGradientV")))
void __raylib_DrawRectangleGradientV(int posX, int posY, int width, int height, const Color* top, const Color* bottom);

static inline void DrawRectangleGradientV(int posX, int posY, int width, int height, Color top, Color bottom) {
    __raylib_DrawRectangleGradientV(posX, posY, width, height, &top, &bottom);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleGradientH")))
void __raylib_DrawRectangleGradientH(int posX, int posY, int width, int height, const Color* left, const Color* right);

static inline void DrawRectangleGradientH(int posX, int posY, int width, int height, Color left, Color right) {
    __raylib_DrawRectangleGradientH(posX, posY, width, height, &left, &right);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleGradientEx")))
void __raylib_DrawRectangleGradientEx(const Rectangle* rec, const Color* topLeft, const Color* bottomLeft, const Color* bottomRight, const Color* topRight);

static inline void DrawRectangleGradientEx(Rectangle rec, Color topLeft, Color bottomLeft, Color bottomRight, Color topRight) {
    __raylib_DrawRectangleGradientEx(&rec, &topLeft, &bottomLeft, &bottomRight, &topRight);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleLines")))
void __raylib_DrawRectangleLines(int posX, int posY, int width, int height, const Color* color);

static inline void DrawRectangleLines(int posX, int posY, int width, int height, Color color) {
    __raylib_DrawRectangleLines(posX, posY, width, height, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleLinesEx")))
void __raylib_DrawRectangleLinesEx(const Rectangle* rec, float lineThick, const Color* color);

static inline void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color) {
    __raylib_DrawRectangleLinesEx(&rec, lineThick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleRounded")))
void __raylib_DrawRectangleRounded(const Rectangle* rec, float roundness, int segments, const Color* color);

static inline void DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color) {
    __raylib_DrawRectangleRounded(&rec, roundness, segments, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleRoundedLines")))
void __raylib_DrawRectangleRoundedLines(const Rectangle* rec, float roundness, int segments, const Color* color);

static inline void DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, Color color) {
    __raylib_DrawRectangleRoundedLines(&rec, roundness, segments, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRectangleRoundedLinesEx")))
void __raylib_DrawRectangleRoundedLinesEx(const Rectangle* rec, float roundness, int segments, float lineThick, const Color* color);

static inline void DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, float lineThick, Color color) {
    __raylib_DrawRectangleRoundedLinesEx(&rec, roundness, segments, lineThick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawTriangle")))
void __raylib_DrawTriangle(const Vector2* v1, const Vector2* v2, const Vector2* v3, const Color* color);

static inline void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color) {
    __raylib_DrawTriangle(&v1, &v2, &v3, &color);
}

__attribute__((import_module("raylib"), import_name("DrawTriangleGradient")))
void __raylib_DrawTriangleGradient(const Vector2* v1, const Vector2* v2, const Vector2* v3, const Color* c1, const Color* c2, const Color* c3);

static inline void DrawTriangleGradient(Vector2 v1, Vector2 v2, Vector2 v3, Color c1, Color c2, Color c3) {
    __raylib_DrawTriangleGradient(&v1, &v2, &v3, &c1, &c2, &c3);
}

__attribute__((import_module("raylib"), import_name("DrawTriangleLines")))
void __raylib_DrawTriangleLines(const Vector2* v1, const Vector2* v2, const Vector2* v3, const Color* color);

static inline void DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color) {
    __raylib_DrawTriangleLines(&v1, &v2, &v3, &color);
}

__attribute__((import_module("raylib"), import_name("DrawTriangleFan")))
void __raylib_DrawTriangleFan(const Vector2 * points, int pointCount, const Color* color);

static inline void DrawTriangleFan(const Vector2 * points, int pointCount, Color color) {
    __raylib_DrawTriangleFan(points, pointCount, &color);
}

__attribute__((import_module("raylib"), import_name("DrawTriangleStrip")))
void __raylib_DrawTriangleStrip(const Vector2 * points, int pointCount, const Color* color);

static inline void DrawTriangleStrip(const Vector2 * points, int pointCount, Color color) {
    __raylib_DrawTriangleStrip(points, pointCount, &color);
}

__attribute__((import_module("raylib"), import_name("DrawPoly")))
void __raylib_DrawPoly(const Vector2* center, int sides, float radius, float rotation, const Color* color);

static inline void DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color) {
    __raylib_DrawPoly(&center, sides, radius, rotation, &color);
}

__attribute__((import_module("raylib"), import_name("DrawPolyLines")))
void __raylib_DrawPolyLines(const Vector2* center, int sides, float radius, float rotation, const Color* color);

static inline void DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color) {
    __raylib_DrawPolyLines(&center, sides, radius, rotation, &color);
}

__attribute__((import_module("raylib"), import_name("DrawPolyLinesEx")))
void __raylib_DrawPolyLinesEx(const Vector2* center, int sides, float radius, float rotation, float lineThick, const Color* color);

static inline void DrawPolyLinesEx(Vector2 center, int sides, float radius, float rotation, float lineThick, Color color) {
    __raylib_DrawPolyLinesEx(&center, sides, radius, rotation, lineThick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineLinear")))
void __raylib_DrawSplineLinear(const Vector2 * points, int pointCount, float thick, const Color* color);

static inline void DrawSplineLinear(const Vector2 * points, int pointCount, float thick, Color color) {
    __raylib_DrawSplineLinear(points, pointCount, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineBasis")))
void __raylib_DrawSplineBasis(const Vector2 * points, int pointCount, float thick, const Color* color);

static inline void DrawSplineBasis(const Vector2 * points, int pointCount, float thick, Color color) {
    __raylib_DrawSplineBasis(points, pointCount, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineCatmullRom")))
void __raylib_DrawSplineCatmullRom(const Vector2 * points, int pointCount, float thick, const Color* color);

static inline void DrawSplineCatmullRom(const Vector2 * points, int pointCount, float thick, Color color) {
    __raylib_DrawSplineCatmullRom(points, pointCount, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineBezierQuadratic")))
void __raylib_DrawSplineBezierQuadratic(const Vector2 * points, int pointCount, float thick, const Color* color);

static inline void DrawSplineBezierQuadratic(const Vector2 * points, int pointCount, float thick, Color color) {
    __raylib_DrawSplineBezierQuadratic(points, pointCount, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineBezierCubic")))
void __raylib_DrawSplineBezierCubic(const Vector2 * points, int pointCount, float thick, const Color* color);

static inline void DrawSplineBezierCubic(const Vector2 * points, int pointCount, float thick, Color color) {
    __raylib_DrawSplineBezierCubic(points, pointCount, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineSegmentLinear")))
void __raylib_DrawSplineSegmentLinear(const Vector2* p1, const Vector2* p2, float thick, const Color* color);

static inline void DrawSplineSegmentLinear(Vector2 p1, Vector2 p2, float thick, Color color) {
    __raylib_DrawSplineSegmentLinear(&p1, &p2, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineSegmentBasis")))
void __raylib_DrawSplineSegmentBasis(const Vector2* p1, const Vector2* p2, const Vector2* p3, const Vector2* p4, float thick, const Color* color);

static inline void DrawSplineSegmentBasis(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float thick, Color color) {
    __raylib_DrawSplineSegmentBasis(&p1, &p2, &p3, &p4, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineSegmentCatmullRom")))
void __raylib_DrawSplineSegmentCatmullRom(const Vector2* p1, const Vector2* p2, const Vector2* p3, const Vector2* p4, float thick, const Color* color);

static inline void DrawSplineSegmentCatmullRom(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float thick, Color color) {
    __raylib_DrawSplineSegmentCatmullRom(&p1, &p2, &p3, &p4, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineSegmentBezierQuadratic")))
void __raylib_DrawSplineSegmentBezierQuadratic(const Vector2* p1, const Vector2* c2, const Vector2* p3, float thick, const Color* color);

static inline void DrawSplineSegmentBezierQuadratic(Vector2 p1, Vector2 c2, Vector2 p3, float thick, Color color) {
    __raylib_DrawSplineSegmentBezierQuadratic(&p1, &c2, &p3, thick, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSplineSegmentBezierCubic")))
void __raylib_DrawSplineSegmentBezierCubic(const Vector2* p1, const Vector2* c2, const Vector2* c3, const Vector2* p4, float thick, const Color* color);

static inline void DrawSplineSegmentBezierCubic(Vector2 p1, Vector2 c2, Vector2 c3, Vector2 p4, float thick, Color color) {
    __raylib_DrawSplineSegmentBezierCubic(&p1, &c2, &c3, &p4, thick, &color);
}

__attribute__((import_module("raylib"), import_name("GetSplinePointLinear")))
void __raylib_GetSplinePointLinear(Vector2* __result, const Vector2* startPos, const Vector2* endPos, float t);

static inline Vector2 GetSplinePointLinear(Vector2 startPos, Vector2 endPos, float t) {
    Vector2 __r;
    __raylib_GetSplinePointLinear(&__r, &startPos, &endPos, t);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetSplinePointBasis")))
void __raylib_GetSplinePointBasis(Vector2* __result, const Vector2* p1, const Vector2* p2, const Vector2* p3, const Vector2* p4, float t);

static inline Vector2 GetSplinePointBasis(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float t) {
    Vector2 __r;
    __raylib_GetSplinePointBasis(&__r, &p1, &p2, &p3, &p4, t);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetSplinePointCatmullRom")))
void __raylib_GetSplinePointCatmullRom(Vector2* __result, const Vector2* p1, const Vector2* p2, const Vector2* p3, const Vector2* p4, float t);

static inline Vector2 GetSplinePointCatmullRom(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float t) {
    Vector2 __r;
    __raylib_GetSplinePointCatmullRom(&__r, &p1, &p2, &p3, &p4, t);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetSplinePointBezierQuadratic")))
void __raylib_GetSplinePointBezierQuadratic(Vector2* __result, const Vector2* p1, const Vector2* c2, const Vector2* p3, float t);

static inline Vector2 GetSplinePointBezierQuadratic(Vector2 p1, Vector2 c2, Vector2 p3, float t) {
    Vector2 __r;
    __raylib_GetSplinePointBezierQuadratic(&__r, &p1, &c2, &p3, t);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetSplinePointBezierCubic")))
void __raylib_GetSplinePointBezierCubic(Vector2* __result, const Vector2* p1, const Vector2* c2, const Vector2* c3, const Vector2* p4, float t);

static inline Vector2 GetSplinePointBezierCubic(Vector2 p1, Vector2 c2, Vector2 c3, Vector2 p4, float t) {
    Vector2 __r;
    __raylib_GetSplinePointBezierCubic(&__r, &p1, &c2, &c3, &p4, t);
    return __r;
}

__attribute__((import_module("raylib"), import_name("CheckCollisionRecs")))
bool __raylib_CheckCollisionRecs(const Rectangle* rec1, const Rectangle* rec2);

static inline bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2) {
    return __raylib_CheckCollisionRecs(&rec1, &rec2);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionCircles")))
bool __raylib_CheckCollisionCircles(const Vector2* center1, float radius1, const Vector2* center2, float radius2);

static inline bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2) {
    return __raylib_CheckCollisionCircles(&center1, radius1, &center2, radius2);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionCircleRec")))
bool __raylib_CheckCollisionCircleRec(const Vector2* center, float radius, const Rectangle* rec);

static inline bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec) {
    return __raylib_CheckCollisionCircleRec(&center, radius, &rec);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionCircleLine")))
bool __raylib_CheckCollisionCircleLine(const Vector2* center, float radius, const Vector2* p1, const Vector2* p2);

static inline bool CheckCollisionCircleLine(Vector2 center, float radius, Vector2 p1, Vector2 p2) {
    return __raylib_CheckCollisionCircleLine(&center, radius, &p1, &p2);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionPointRec")))
bool __raylib_CheckCollisionPointRec(const Vector2* point, const Rectangle* rec);

static inline bool CheckCollisionPointRec(Vector2 point, Rectangle rec) {
    return __raylib_CheckCollisionPointRec(&point, &rec);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionPointCircle")))
bool __raylib_CheckCollisionPointCircle(const Vector2* point, const Vector2* center, float radius);

static inline bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius) {
    return __raylib_CheckCollisionPointCircle(&point, &center, radius);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionPointTriangle")))
bool __raylib_CheckCollisionPointTriangle(const Vector2* point, const Vector2* p1, const Vector2* p2, const Vector2* p3);

static inline bool CheckCollisionPointTriangle(Vector2 point, Vector2 p1, Vector2 p2, Vector2 p3) {
    return __raylib_CheckCollisionPointTriangle(&point, &p1, &p2, &p3);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionPointLine")))
bool __raylib_CheckCollisionPointLine(const Vector2* point, const Vector2* p1, const Vector2* p2, int threshold);

static inline bool CheckCollisionPointLine(Vector2 point, Vector2 p1, Vector2 p2, int threshold) {
    return __raylib_CheckCollisionPointLine(&point, &p1, &p2, threshold);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionPointPoly")))
bool __raylib_CheckCollisionPointPoly(const Vector2* point, const Vector2 * points, int pointCount);

static inline bool CheckCollisionPointPoly(Vector2 point, const Vector2 * points, int pointCount) {
    return __raylib_CheckCollisionPointPoly(&point, points, pointCount);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionLines")))
bool __raylib_CheckCollisionLines(const Vector2* startPos1, const Vector2* endPos1, const Vector2* startPos2, const Vector2* endPos2, Vector2 * collisionPoint);

static inline bool CheckCollisionLines(Vector2 startPos1, Vector2 endPos1, Vector2 startPos2, Vector2 endPos2, Vector2 * collisionPoint) {
    return __raylib_CheckCollisionLines(&startPos1, &endPos1, &startPos2, &endPos2, collisionPoint);
}

__attribute__((import_module("raylib"), import_name("GetCollisionRec")))
void __raylib_GetCollisionRec(Rectangle* __result, const Rectangle* rec1, const Rectangle* rec2);

static inline Rectangle GetCollisionRec(Rectangle rec1, Rectangle rec2) {
    Rectangle __r;
    __raylib_GetCollisionRec(&__r, &rec1, &rec2);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadImage")))
void __raylib_LoadImage(Image* __result, const char * fileName);

static inline Image LoadImage(const char * fileName) {
    Image __r;
    __raylib_LoadImage(&__r, fileName);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadImageRaw")))
void __raylib_LoadImageRaw(Image* __result, const char * fileName, int width, int height, int format, int headerSize);

static inline Image LoadImageRaw(const char * fileName, int width, int height, int format, int headerSize) {
    Image __r;
    __raylib_LoadImageRaw(&__r, fileName, width, height, format, headerSize);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadImageAnim")))
void __raylib_LoadImageAnim(Image* __result, const char * fileName, int * frames);

static inline Image LoadImageAnim(const char * fileName, int * frames) {
    Image __r;
    __raylib_LoadImageAnim(&__r, fileName, frames);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadImageAnimFromMemory")))
void __raylib_LoadImageAnimFromMemory(Image* __result, const char * fileType, const unsigned char * fileData, int dataSize, int * frames);

static inline Image LoadImageAnimFromMemory(const char * fileType, const unsigned char * fileData, int dataSize, int * frames) {
    Image __r;
    __raylib_LoadImageAnimFromMemory(&__r, fileType, fileData, dataSize, frames);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadImageFromMemory")))
void __raylib_LoadImageFromMemory(Image* __result, const char * fileType, const unsigned char * fileData, int dataSize);

static inline Image LoadImageFromMemory(const char * fileType, const unsigned char * fileData, int dataSize) {
    Image __r;
    __raylib_LoadImageFromMemory(&__r, fileType, fileData, dataSize);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadImageFromTexture")))
void __raylib_LoadImageFromTexture(Image* __result, const Texture2D* texture);

static inline Image LoadImageFromTexture(Texture2D texture) {
    Image __r;
    __raylib_LoadImageFromTexture(&__r, &texture);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadImageFromScreen")))
void __raylib_LoadImageFromScreen(Image* __result);

static inline Image LoadImageFromScreen(void) {
    Image __r;
    __raylib_LoadImageFromScreen(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsImageValid")))
bool __raylib_IsImageValid(const Image* image);

static inline bool IsImageValid(Image image) {
    return __raylib_IsImageValid(&image);
}

__attribute__((import_module("raylib"), import_name("UnloadImage")))
void __raylib_UnloadImage(const Image* image);

static inline void UnloadImage(Image image) {
    __raylib_UnloadImage(&image);
}

__attribute__((import_module("raylib"), import_name("ExportImage")))
bool __raylib_ExportImage(const Image* image, const char * fileName);

static inline bool ExportImage(Image image, const char * fileName) {
    return __raylib_ExportImage(&image, fileName);
}

__attribute__((import_module("raylib"), import_name("ExportImageToMemory")))
unsigned char * __raylib_ExportImageToMemory(const Image* image, const char * fileType, int * fileSize);

static inline unsigned char * ExportImageToMemory(Image image, const char * fileType, int * fileSize) {
    return __raylib_ExportImageToMemory(&image, fileType, fileSize);
}

__attribute__((import_module("raylib"), import_name("ExportImageAsCode")))
bool __raylib_ExportImageAsCode(const Image* image, const char * fileName);

static inline bool ExportImageAsCode(Image image, const char * fileName) {
    return __raylib_ExportImageAsCode(&image, fileName);
}

__attribute__((import_module("raylib"), import_name("GenImageColor")))
void __raylib_GenImageColor(Image* __result, int width, int height, const Color* color);

static inline Image GenImageColor(int width, int height, Color color) {
    Image __r;
    __raylib_GenImageColor(&__r, width, height, &color);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenImageGradientLinear")))
void __raylib_GenImageGradientLinear(Image* __result, int width, int height, int direction, const Color* start, const Color* end);

static inline Image GenImageGradientLinear(int width, int height, int direction, Color start, Color end) {
    Image __r;
    __raylib_GenImageGradientLinear(&__r, width, height, direction, &start, &end);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenImageGradientRadial")))
void __raylib_GenImageGradientRadial(Image* __result, int width, int height, float density, const Color* inner, const Color* outer);

static inline Image GenImageGradientRadial(int width, int height, float density, Color inner, Color outer) {
    Image __r;
    __raylib_GenImageGradientRadial(&__r, width, height, density, &inner, &outer);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenImageGradientSquare")))
void __raylib_GenImageGradientSquare(Image* __result, int width, int height, float density, const Color* inner, const Color* outer);

static inline Image GenImageGradientSquare(int width, int height, float density, Color inner, Color outer) {
    Image __r;
    __raylib_GenImageGradientSquare(&__r, width, height, density, &inner, &outer);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenImageChecked")))
void __raylib_GenImageChecked(Image* __result, int width, int height, int checksX, int checksY, const Color* col1, const Color* col2);

static inline Image GenImageChecked(int width, int height, int checksX, int checksY, Color col1, Color col2) {
    Image __r;
    __raylib_GenImageChecked(&__r, width, height, checksX, checksY, &col1, &col2);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenImageWhiteNoise")))
void __raylib_GenImageWhiteNoise(Image* __result, int width, int height, float factor);

static inline Image GenImageWhiteNoise(int width, int height, float factor) {
    Image __r;
    __raylib_GenImageWhiteNoise(&__r, width, height, factor);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenImagePerlinNoise")))
void __raylib_GenImagePerlinNoise(Image* __result, int width, int height, int offsetX, int offsetY, float scale);

static inline Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale) {
    Image __r;
    __raylib_GenImagePerlinNoise(&__r, width, height, offsetX, offsetY, scale);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenImageCellular")))
void __raylib_GenImageCellular(Image* __result, int width, int height, int tileSize);

static inline Image GenImageCellular(int width, int height, int tileSize) {
    Image __r;
    __raylib_GenImageCellular(&__r, width, height, tileSize);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenImageText")))
void __raylib_GenImageText(Image* __result, int width, int height, const char * text);

static inline Image GenImageText(int width, int height, const char * text) {
    Image __r;
    __raylib_GenImageText(&__r, width, height, text);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ImageCopy")))
void __raylib_ImageCopy(Image* __result, const Image* image);

static inline Image ImageCopy(Image image) {
    Image __r;
    __raylib_ImageCopy(&__r, &image);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ImageFromImage")))
void __raylib_ImageFromImage(Image* __result, const Image* image, const Rectangle* rec);

static inline Image ImageFromImage(Image image, Rectangle rec) {
    Image __r;
    __raylib_ImageFromImage(&__r, &image, &rec);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ImageFromChannel")))
void __raylib_ImageFromChannel(Image* __result, const Image* image, int selectedChannel);

static inline Image ImageFromChannel(Image image, int selectedChannel) {
    Image __r;
    __raylib_ImageFromChannel(&__r, &image, selectedChannel);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ImageText")))
void __raylib_ImageText(Image* __result, const char * text, int fontSize, const Color* color);

static inline Image ImageText(const char * text, int fontSize, Color color) {
    Image __r;
    __raylib_ImageText(&__r, text, fontSize, &color);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ImageTextEx")))
void __raylib_ImageTextEx(Image* __result, const Font* font, const char * text, float fontSize, float spacing, const Color* tint);

static inline Image ImageTextEx(Font font, const char * text, float fontSize, float spacing, Color tint) {
    Image __r;
    __raylib_ImageTextEx(&__r, &font, text, fontSize, spacing, &tint);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ImageFormat")))
void ImageFormat(Image * image, int newFormat);

__attribute__((import_module("raylib"), import_name("ImageToPOT")))
void __raylib_ImageToPOT(Image * image, const Color* fill);

static inline void ImageToPOT(Image * image, Color fill) {
    __raylib_ImageToPOT(image, &fill);
}

__attribute__((import_module("raylib"), import_name("ImageCrop")))
void __raylib_ImageCrop(Image * image, const Rectangle* crop);

static inline void ImageCrop(Image * image, Rectangle crop) {
    __raylib_ImageCrop(image, &crop);
}

__attribute__((import_module("raylib"), import_name("ImageAlphaCrop")))
void ImageAlphaCrop(Image * image, float threshold);

__attribute__((import_module("raylib"), import_name("ImageAlphaClear")))
void __raylib_ImageAlphaClear(Image * image, const Color* color, float threshold);

static inline void ImageAlphaClear(Image * image, Color color, float threshold) {
    __raylib_ImageAlphaClear(image, &color, threshold);
}

__attribute__((import_module("raylib"), import_name("ImageAlphaMask")))
void __raylib_ImageAlphaMask(Image * image, const Image* alphaMask);

static inline void ImageAlphaMask(Image * image, Image alphaMask) {
    __raylib_ImageAlphaMask(image, &alphaMask);
}

__attribute__((import_module("raylib"), import_name("ImageAlphaPremultiply")))
void ImageAlphaPremultiply(Image * image);

__attribute__((import_module("raylib"), import_name("ImageBlurGaussian")))
void ImageBlurGaussian(Image * image, int blurSize);

__attribute__((import_module("raylib"), import_name("ImageKernelConvolution")))
void ImageKernelConvolution(Image * image, const float * kernel, int kernelSize);

__attribute__((import_module("raylib"), import_name("ImageResize")))
void ImageResize(Image * image, int newWidth, int newHeight);

__attribute__((import_module("raylib"), import_name("ImageResizeNN")))
void ImageResizeNN(Image * image, int newWidth, int newHeight);

__attribute__((import_module("raylib"), import_name("ImageResizeCanvas")))
void __raylib_ImageResizeCanvas(Image * image, int newWidth, int newHeight, int offsetX, int offsetY, const Color* fill);

static inline void ImageResizeCanvas(Image * image, int newWidth, int newHeight, int offsetX, int offsetY, Color fill) {
    __raylib_ImageResizeCanvas(image, newWidth, newHeight, offsetX, offsetY, &fill);
}

__attribute__((import_module("raylib"), import_name("ImageMipmaps")))
void ImageMipmaps(Image * image);

__attribute__((import_module("raylib"), import_name("ImageDither")))
void ImageDither(Image * image, int rBpp, int gBpp, int bBpp, int aBpp);

__attribute__((import_module("raylib"), import_name("ImageFlipVertical")))
void ImageFlipVertical(Image * image);

__attribute__((import_module("raylib"), import_name("ImageFlipHorizontal")))
void ImageFlipHorizontal(Image * image);

__attribute__((import_module("raylib"), import_name("ImageRotate")))
void ImageRotate(Image * image, int degrees);

__attribute__((import_module("raylib"), import_name("ImageRotateCW")))
void ImageRotateCW(Image * image);

__attribute__((import_module("raylib"), import_name("ImageRotateCCW")))
void ImageRotateCCW(Image * image);

__attribute__((import_module("raylib"), import_name("ImageColorTint")))
void __raylib_ImageColorTint(Image * image, const Color* color);

static inline void ImageColorTint(Image * image, Color color) {
    __raylib_ImageColorTint(image, &color);
}

__attribute__((import_module("raylib"), import_name("ImageColorInvert")))
void ImageColorInvert(Image * image);

__attribute__((import_module("raylib"), import_name("ImageColorGrayscale")))
void ImageColorGrayscale(Image * image);

__attribute__((import_module("raylib"), import_name("ImageColorContrast")))
void ImageColorContrast(Image * image, int contrast);

__attribute__((import_module("raylib"), import_name("ImageColorBrightness")))
void ImageColorBrightness(Image * image, int brightness);

__attribute__((import_module("raylib"), import_name("ImageColorReplace")))
void __raylib_ImageColorReplace(Image * image, const Color* color, const Color* replace);

static inline void ImageColorReplace(Image * image, Color color, Color replace) {
    __raylib_ImageColorReplace(image, &color, &replace);
}

__attribute__((import_module("raylib"), import_name("LoadImageColors")))
Color * __raylib_LoadImageColors(const Image* image);

static inline Color * LoadImageColors(Image image) {
    return __raylib_LoadImageColors(&image);
}

__attribute__((import_module("raylib"), import_name("LoadImagePalette")))
Color * __raylib_LoadImagePalette(const Image* image, int maxPaletteSize, int * colorCount);

static inline Color * LoadImagePalette(Image image, int maxPaletteSize, int * colorCount) {
    return __raylib_LoadImagePalette(&image, maxPaletteSize, colorCount);
}

__attribute__((import_module("raylib"), import_name("UnloadImageColors")))
void UnloadImageColors(Color * colors);

__attribute__((import_module("raylib"), import_name("UnloadImagePalette")))
void UnloadImagePalette(Color * colors);

__attribute__((import_module("raylib"), import_name("GetImageAlphaBorder")))
void __raylib_GetImageAlphaBorder(Rectangle* __result, const Image* image, float threshold);

static inline Rectangle GetImageAlphaBorder(Image image, float threshold) {
    Rectangle __r;
    __raylib_GetImageAlphaBorder(&__r, &image, threshold);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetImageColor")))
void __raylib_GetImageColor(Color* __result, const Image* image, int x, int y);

static inline Color GetImageColor(Image image, int x, int y) {
    Color __r;
    __raylib_GetImageColor(&__r, &image, x, y);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ImageClearBackground")))
void __raylib_ImageClearBackground(Image * dst, const Color* color);

static inline void ImageClearBackground(Image * dst, Color color) {
    __raylib_ImageClearBackground(dst, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawPixel")))
void __raylib_ImageDrawPixel(Image * dst, int posX, int posY, const Color* color);

static inline void ImageDrawPixel(Image * dst, int posX, int posY, Color color) {
    __raylib_ImageDrawPixel(dst, posX, posY, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawPixelV")))
void __raylib_ImageDrawPixelV(Image * dst, const Vector2* position, const Color* color);

static inline void ImageDrawPixelV(Image * dst, Vector2 position, Color color) {
    __raylib_ImageDrawPixelV(dst, &position, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawLine")))
void __raylib_ImageDrawLine(Image * dst, int startPosX, int startPosY, int endPosX, int endPosY, const Color* color);

static inline void ImageDrawLine(Image * dst, int startPosX, int startPosY, int endPosX, int endPosY, Color color) {
    __raylib_ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawLineV")))
void __raylib_ImageDrawLineV(Image * dst, const Vector2* start, const Vector2* end, const Color* color);

static inline void ImageDrawLineV(Image * dst, Vector2 start, Vector2 end, Color color) {
    __raylib_ImageDrawLineV(dst, &start, &end, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawLineEx")))
void __raylib_ImageDrawLineEx(Image * dst, const Vector2* start, const Vector2* end, int thick, const Color* color);

static inline void ImageDrawLineEx(Image * dst, Vector2 start, Vector2 end, int thick, Color color) {
    __raylib_ImageDrawLineEx(dst, &start, &end, thick, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawCircle")))
void __raylib_ImageDrawCircle(Image * dst, int centerX, int centerY, int radius, const Color* color);

static inline void ImageDrawCircle(Image * dst, int centerX, int centerY, int radius, Color color) {
    __raylib_ImageDrawCircle(dst, centerX, centerY, radius, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawCircleV")))
void __raylib_ImageDrawCircleV(Image * dst, const Vector2* center, int radius, const Color* color);

static inline void ImageDrawCircleV(Image * dst, Vector2 center, int radius, Color color) {
    __raylib_ImageDrawCircleV(dst, &center, radius, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawCircleLines")))
void __raylib_ImageDrawCircleLines(Image * dst, int centerX, int centerY, int radius, const Color* color);

static inline void ImageDrawCircleLines(Image * dst, int centerX, int centerY, int radius, Color color) {
    __raylib_ImageDrawCircleLines(dst, centerX, centerY, radius, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawCircleLinesV")))
void __raylib_ImageDrawCircleLinesV(Image * dst, const Vector2* center, int radius, const Color* color);

static inline void ImageDrawCircleLinesV(Image * dst, Vector2 center, int radius, Color color) {
    __raylib_ImageDrawCircleLinesV(dst, &center, radius, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawRectangle")))
void __raylib_ImageDrawRectangle(Image * dst, int posX, int posY, int width, int height, const Color* color);

static inline void ImageDrawRectangle(Image * dst, int posX, int posY, int width, int height, Color color) {
    __raylib_ImageDrawRectangle(dst, posX, posY, width, height, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawRectangleV")))
void __raylib_ImageDrawRectangleV(Image * dst, const Vector2* position, const Vector2* size, const Color* color);

static inline void ImageDrawRectangleV(Image * dst, Vector2 position, Vector2 size, Color color) {
    __raylib_ImageDrawRectangleV(dst, &position, &size, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawRectangleRec")))
void __raylib_ImageDrawRectangleRec(Image * dst, const Rectangle* rec, const Color* color);

static inline void ImageDrawRectangleRec(Image * dst, Rectangle rec, Color color) {
    __raylib_ImageDrawRectangleRec(dst, &rec, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawRectangleLines")))
void __raylib_ImageDrawRectangleLines(Image * dst, int posX, int posY, int width, int height, const Color* color);

static inline void ImageDrawRectangleLines(Image * dst, int posX, int posY, int width, int height, Color color) {
    __raylib_ImageDrawRectangleLines(dst, posX, posY, width, height, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawRectangleLinesEx")))
void __raylib_ImageDrawRectangleLinesEx(Image * dst, const Rectangle* rec, int thick, const Color* color);

static inline void ImageDrawRectangleLinesEx(Image * dst, Rectangle rec, int thick, Color color) {
    __raylib_ImageDrawRectangleLinesEx(dst, &rec, thick, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawTriangle")))
void __raylib_ImageDrawTriangle(Image * dst, const Vector2* v1, const Vector2* v2, const Vector2* v3, const Color* color);

static inline void ImageDrawTriangle(Image * dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color) {
    __raylib_ImageDrawTriangle(dst, &v1, &v2, &v3, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawTriangleGradient")))
void __raylib_ImageDrawTriangleGradient(Image * dst, const Vector2* v1, const Vector2* v2, const Vector2* v3, const Color* c1, const Color* c2, const Color* c3);

static inline void ImageDrawTriangleGradient(Image * dst, Vector2 v1, Vector2 v2, Vector2 v3, Color c1, Color c2, Color c3) {
    __raylib_ImageDrawTriangleGradient(dst, &v1, &v2, &v3, &c1, &c2, &c3);
}

__attribute__((import_module("raylib"), import_name("ImageDrawTriangleLines")))
void __raylib_ImageDrawTriangleLines(Image * dst, const Vector2* v1, const Vector2* v2, const Vector2* v3, const Color* color);

static inline void ImageDrawTriangleLines(Image * dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color) {
    __raylib_ImageDrawTriangleLines(dst, &v1, &v2, &v3, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawTriangleFan")))
void __raylib_ImageDrawTriangleFan(Image * dst, const Vector2 * points, int pointCount, const Color* color);

static inline void ImageDrawTriangleFan(Image * dst, const Vector2 * points, int pointCount, Color color) {
    __raylib_ImageDrawTriangleFan(dst, points, pointCount, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawTriangleStrip")))
void __raylib_ImageDrawTriangleStrip(Image * dst, const Vector2 * points, int pointCount, const Color* color);

static inline void ImageDrawTriangleStrip(Image * dst, const Vector2 * points, int pointCount, Color color) {
    __raylib_ImageDrawTriangleStrip(dst, points, pointCount, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDraw")))
void __raylib_ImageDraw(Image * dst, const Image* src, const Rectangle* srcRec, const Rectangle* dstRec, const Color* tint);

static inline void ImageDraw(Image * dst, Image src, Rectangle srcRec, Rectangle dstRec, Color tint) {
    __raylib_ImageDraw(dst, &src, &srcRec, &dstRec, &tint);
}

__attribute__((import_module("raylib"), import_name("ImageDrawText")))
void __raylib_ImageDrawText(Image * dst, const char * text, int posX, int posY, int fontSize, const Color* color);

static inline void ImageDrawText(Image * dst, const char * text, int posX, int posY, int fontSize, Color color) {
    __raylib_ImageDrawText(dst, text, posX, posY, fontSize, &color);
}

__attribute__((import_module("raylib"), import_name("ImageDrawTextEx")))
void __raylib_ImageDrawTextEx(Image * dst, const Font* font, const char * text, const Vector2* position, float fontSize, float spacing, const Color* tint);

static inline void ImageDrawTextEx(Image * dst, Font font, const char * text, Vector2 position, float fontSize, float spacing, Color tint) {
    __raylib_ImageDrawTextEx(dst, &font, text, &position, fontSize, spacing, &tint);
}

__attribute__((import_module("raylib"), import_name("LoadTexture")))
void __raylib_LoadTexture(Texture2D* __result, const char * fileName);

static inline Texture2D LoadTexture(const char * fileName) {
    Texture2D __r;
    __raylib_LoadTexture(&__r, fileName);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadTextureFromImage")))
void __raylib_LoadTextureFromImage(Texture2D* __result, const Image* image);

static inline Texture2D LoadTextureFromImage(Image image) {
    Texture2D __r;
    __raylib_LoadTextureFromImage(&__r, &image);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadTextureCubemap")))
void __raylib_LoadTextureCubemap(TextureCubemap* __result, const Image* image, int layout);

static inline TextureCubemap LoadTextureCubemap(Image image, int layout) {
    TextureCubemap __r;
    __raylib_LoadTextureCubemap(&__r, &image, layout);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadRenderTexture")))
void __raylib_LoadRenderTexture(RenderTexture2D* __result, int width, int height);

static inline RenderTexture2D LoadRenderTexture(int width, int height) {
    RenderTexture2D __r;
    __raylib_LoadRenderTexture(&__r, width, height);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsTextureValid")))
bool __raylib_IsTextureValid(const Texture2D* texture);

static inline bool IsTextureValid(Texture2D texture) {
    return __raylib_IsTextureValid(&texture);
}

__attribute__((import_module("raylib"), import_name("UnloadTexture")))
void __raylib_UnloadTexture(const Texture2D* texture);

static inline void UnloadTexture(Texture2D texture) {
    __raylib_UnloadTexture(&texture);
}

__attribute__((import_module("raylib"), import_name("IsRenderTextureValid")))
bool __raylib_IsRenderTextureValid(const RenderTexture2D* target);

static inline bool IsRenderTextureValid(RenderTexture2D target) {
    return __raylib_IsRenderTextureValid(&target);
}

__attribute__((import_module("raylib"), import_name("UnloadRenderTexture")))
void __raylib_UnloadRenderTexture(const RenderTexture2D* target);

static inline void UnloadRenderTexture(RenderTexture2D target) {
    __raylib_UnloadRenderTexture(&target);
}

__attribute__((import_module("raylib"), import_name("UpdateTexture")))
void __raylib_UpdateTexture(const Texture2D* texture, const void * pixels);

static inline void UpdateTexture(Texture2D texture, const void * pixels) {
    __raylib_UpdateTexture(&texture, pixels);
}

__attribute__((import_module("raylib"), import_name("UpdateTextureRec")))
void __raylib_UpdateTextureRec(const Texture2D* texture, const Rectangle* rec, const void * pixels);

static inline void UpdateTextureRec(Texture2D texture, Rectangle rec, const void * pixels) {
    __raylib_UpdateTextureRec(&texture, &rec, pixels);
}

__attribute__((import_module("raylib"), import_name("GenTextureMipmaps")))
void GenTextureMipmaps(Texture2D * texture);

__attribute__((import_module("raylib"), import_name("SetTextureFilter")))
void __raylib_SetTextureFilter(const Texture2D* texture, int filter);

static inline void SetTextureFilter(Texture2D texture, int filter) {
    __raylib_SetTextureFilter(&texture, filter);
}

__attribute__((import_module("raylib"), import_name("SetTextureWrap")))
void __raylib_SetTextureWrap(const Texture2D* texture, int wrap);

static inline void SetTextureWrap(Texture2D texture, int wrap) {
    __raylib_SetTextureWrap(&texture, wrap);
}

__attribute__((import_module("raylib"), import_name("DrawTexture")))
void __raylib_DrawTexture(const Texture2D* texture, int posX, int posY, const Color* tint);

static inline void DrawTexture(Texture2D texture, int posX, int posY, Color tint) {
    __raylib_DrawTexture(&texture, posX, posY, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawTextureV")))
void __raylib_DrawTextureV(const Texture2D* texture, const Vector2* position, const Color* tint);

static inline void DrawTextureV(Texture2D texture, Vector2 position, Color tint) {
    __raylib_DrawTextureV(&texture, &position, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawTextureEx")))
void __raylib_DrawTextureEx(const Texture2D* texture, const Vector2* position, float rotation, float scale, const Color* tint);

static inline void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint) {
    __raylib_DrawTextureEx(&texture, &position, rotation, scale, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawTextureRec")))
void __raylib_DrawTextureRec(const Texture2D* texture, const Rectangle* source, const Vector2* position, const Color* tint);

static inline void DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint) {
    __raylib_DrawTextureRec(&texture, &source, &position, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawTexturePro")))
void __raylib_DrawTexturePro(const Texture2D* texture, const Rectangle* source, const Rectangle* dest, const Vector2* origin, float rotation, const Color* tint);

static inline void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint) {
    __raylib_DrawTexturePro(&texture, &source, &dest, &origin, rotation, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawTextureNPatch")))
void __raylib_DrawTextureNPatch(const Texture2D* texture, const NPatchInfo* nPatchInfo, const Rectangle* dest, const Vector2* origin, float rotation, const Color* tint);

static inline void DrawTextureNPatch(Texture2D texture, NPatchInfo nPatchInfo, Rectangle dest, Vector2 origin, float rotation, Color tint) {
    __raylib_DrawTextureNPatch(&texture, &nPatchInfo, &dest, &origin, rotation, &tint);
}

__attribute__((import_module("raylib"), import_name("ColorIsEqual")))
bool __raylib_ColorIsEqual(const Color* col1, const Color* col2);

static inline bool ColorIsEqual(Color col1, Color col2) {
    return __raylib_ColorIsEqual(&col1, &col2);
}

__attribute__((import_module("raylib"), import_name("Fade")))
void __raylib_Fade(Color* __result, const Color* color, float alpha);

static inline Color Fade(Color color, float alpha) {
    Color __r;
    __raylib_Fade(&__r, &color, alpha);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorToInt")))
int __raylib_ColorToInt(const Color* color);

static inline int ColorToInt(Color color) {
    return __raylib_ColorToInt(&color);
}

__attribute__((import_module("raylib"), import_name("ColorNormalize")))
void __raylib_ColorNormalize(Vector4* __result, const Color* color);

static inline Vector4 ColorNormalize(Color color) {
    Vector4 __r;
    __raylib_ColorNormalize(&__r, &color);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorFromNormalized")))
void __raylib_ColorFromNormalized(Color* __result, const Vector4* normalized);

static inline Color ColorFromNormalized(Vector4 normalized) {
    Color __r;
    __raylib_ColorFromNormalized(&__r, &normalized);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorToHSV")))
void __raylib_ColorToHSV(Vector3* __result, const Color* color);

static inline Vector3 ColorToHSV(Color color) {
    Vector3 __r;
    __raylib_ColorToHSV(&__r, &color);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorFromHSV")))
void __raylib_ColorFromHSV(Color* __result, float hue, float saturation, float value);

static inline Color ColorFromHSV(float hue, float saturation, float value) {
    Color __r;
    __raylib_ColorFromHSV(&__r, hue, saturation, value);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorTint")))
void __raylib_ColorTint(Color* __result, const Color* color, const Color* tint);

static inline Color ColorTint(Color color, Color tint) {
    Color __r;
    __raylib_ColorTint(&__r, &color, &tint);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorBrightness")))
void __raylib_ColorBrightness(Color* __result, const Color* color, float factor);

static inline Color ColorBrightness(Color color, float factor) {
    Color __r;
    __raylib_ColorBrightness(&__r, &color, factor);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorContrast")))
void __raylib_ColorContrast(Color* __result, const Color* color, float contrast);

static inline Color ColorContrast(Color color, float contrast) {
    Color __r;
    __raylib_ColorContrast(&__r, &color, contrast);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorAlpha")))
void __raylib_ColorAlpha(Color* __result, const Color* color, float alpha);

static inline Color ColorAlpha(Color color, float alpha) {
    Color __r;
    __raylib_ColorAlpha(&__r, &color, alpha);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorAlphaBlend")))
void __raylib_ColorAlphaBlend(Color* __result, const Color* dst, const Color* src, const Color* tint);

static inline Color ColorAlphaBlend(Color dst, Color src, Color tint) {
    Color __r;
    __raylib_ColorAlphaBlend(&__r, &dst, &src, &tint);
    return __r;
}

__attribute__((import_module("raylib"), import_name("ColorLerp")))
void __raylib_ColorLerp(Color* __result, const Color* color1, const Color* color2, float factor);

static inline Color ColorLerp(Color color1, Color color2, float factor) {
    Color __r;
    __raylib_ColorLerp(&__r, &color1, &color2, factor);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetColor")))
void __raylib_GetColor(Color* __result, unsigned int hexValue);

static inline Color GetColor(unsigned int hexValue) {
    Color __r;
    __raylib_GetColor(&__r, hexValue);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetPixelColor")))
void __raylib_GetPixelColor(Color* __result, void * srcPtr, int format);

static inline Color GetPixelColor(void * srcPtr, int format) {
    Color __r;
    __raylib_GetPixelColor(&__r, srcPtr, format);
    return __r;
}

__attribute__((import_module("raylib"), import_name("SetPixelColor")))
void __raylib_SetPixelColor(void * dstPtr, const Color* color, int format);

static inline void SetPixelColor(void * dstPtr, Color color, int format) {
    __raylib_SetPixelColor(dstPtr, &color, format);
}

__attribute__((import_module("raylib"), import_name("GetPixelDataSize")))
int GetPixelDataSize(int width, int height, int format);

__attribute__((import_module("raylib"), import_name("GetFontDefault")))
void __raylib_GetFontDefault(Font* __result);

static inline Font GetFontDefault(void) {
    Font __r;
    __raylib_GetFontDefault(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadFont")))
void __raylib_LoadFont(Font* __result, const char * fileName);

static inline Font LoadFont(const char * fileName) {
    Font __r;
    __raylib_LoadFont(&__r, fileName);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadFontEx")))
void __raylib_LoadFontEx(Font* __result, const char * fileName, int fontSize, const int * codepoints, int codepointCount);

static inline Font LoadFontEx(const char * fileName, int fontSize, const int * codepoints, int codepointCount) {
    Font __r;
    __raylib_LoadFontEx(&__r, fileName, fontSize, codepoints, codepointCount);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadFontFromImage")))
void __raylib_LoadFontFromImage(Font* __result, const Image* image, const Color* key, int firstChar);

static inline Font LoadFontFromImage(Image image, Color key, int firstChar) {
    Font __r;
    __raylib_LoadFontFromImage(&__r, &image, &key, firstChar);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadFontFromMemory")))
void __raylib_LoadFontFromMemory(Font* __result, const char * fileType, const unsigned char * fileData, int dataSize, int fontSize, const int * codepoints, int codepointCount);

static inline Font LoadFontFromMemory(const char * fileType, const unsigned char * fileData, int dataSize, int fontSize, const int * codepoints, int codepointCount) {
    Font __r;
    __raylib_LoadFontFromMemory(&__r, fileType, fileData, dataSize, fontSize, codepoints, codepointCount);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsFontValid")))
bool __raylib_IsFontValid(const Font* font);

static inline bool IsFontValid(Font font) {
    return __raylib_IsFontValid(&font);
}

__attribute__((import_module("raylib"), import_name("LoadFontData")))
GlyphInfo * LoadFontData(const unsigned char * fileData, int dataSize, int fontSize, const int * codepoints, int codepointCount, int type, int * glyphCount);

__attribute__((import_module("raylib"), import_name("GenImageFontAtlas")))
void __raylib_GenImageFontAtlas(Image* __result, const GlyphInfo * glyphs, Rectangle ** glyphRecs, int glyphCount, int fontSize, int padding, int packMethod);

static inline Image GenImageFontAtlas(const GlyphInfo * glyphs, Rectangle ** glyphRecs, int glyphCount, int fontSize, int padding, int packMethod) {
    Image __r;
    __raylib_GenImageFontAtlas(&__r, glyphs, glyphRecs, glyphCount, fontSize, padding, packMethod);
    return __r;
}

__attribute__((import_module("raylib"), import_name("UnloadFontData")))
void UnloadFontData(GlyphInfo * glyphs, int glyphCount);

__attribute__((import_module("raylib"), import_name("UnloadFont")))
void __raylib_UnloadFont(const Font* font);

static inline void UnloadFont(Font font) {
    __raylib_UnloadFont(&font);
}

__attribute__((import_module("raylib"), import_name("ExportFontAsCode")))
bool __raylib_ExportFontAsCode(const Font* font, const char * fileName);

static inline bool ExportFontAsCode(Font font, const char * fileName) {
    return __raylib_ExportFontAsCode(&font, fileName);
}

__attribute__((import_module("raylib"), import_name("DrawFPS")))
void DrawFPS(int posX, int posY);

__attribute__((import_module("raylib"), import_name("DrawText")))
void __raylib_DrawText(const char * text, int posX, int posY, int fontSize, const Color* color);

static inline void DrawText(const char * text, int posX, int posY, int fontSize, Color color) {
    __raylib_DrawText(text, posX, posY, fontSize, &color);
}

__attribute__((import_module("raylib"), import_name("DrawTextEx")))
void __raylib_DrawTextEx(const Font* font, const char * text, const Vector2* position, float fontSize, float spacing, const Color* tint);

static inline void DrawTextEx(Font font, const char * text, Vector2 position, float fontSize, float spacing, Color tint) {
    __raylib_DrawTextEx(&font, text, &position, fontSize, spacing, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawTextPro")))
void __raylib_DrawTextPro(const Font* font, const char * text, const Vector2* position, const Vector2* origin, float rotation, float fontSize, float spacing, const Color* tint);

static inline void DrawTextPro(Font font, const char * text, Vector2 position, Vector2 origin, float rotation, float fontSize, float spacing, Color tint) {
    __raylib_DrawTextPro(&font, text, &position, &origin, rotation, fontSize, spacing, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawTextCodepoint")))
void __raylib_DrawTextCodepoint(const Font* font, int codepoint, const Vector2* position, float fontSize, const Color* tint);

static inline void DrawTextCodepoint(Font font, int codepoint, Vector2 position, float fontSize, Color tint) {
    __raylib_DrawTextCodepoint(&font, codepoint, &position, fontSize, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawTextCodepoints")))
void __raylib_DrawTextCodepoints(const Font* font, const int * codepoints, int codepointCount, const Vector2* position, float fontSize, float spacing, const Color* tint);

static inline void DrawTextCodepoints(Font font, const int * codepoints, int codepointCount, Vector2 position, float fontSize, float spacing, Color tint) {
    __raylib_DrawTextCodepoints(&font, codepoints, codepointCount, &position, fontSize, spacing, &tint);
}

__attribute__((import_module("raylib"), import_name("SetTextLineSpacing")))
void SetTextLineSpacing(int spacing);

__attribute__((import_module("raylib"), import_name("MeasureText")))
int MeasureText(const char * text, int fontSize);

__attribute__((import_module("raylib"), import_name("MeasureTextEx")))
void __raylib_MeasureTextEx(Vector2* __result, const Font* font, const char * text, float fontSize, float spacing);

static inline Vector2 MeasureTextEx(Font font, const char * text, float fontSize, float spacing) {
    Vector2 __r;
    __raylib_MeasureTextEx(&__r, &font, text, fontSize, spacing);
    return __r;
}

__attribute__((import_module("raylib"), import_name("MeasureTextCodepoints")))
void __raylib_MeasureTextCodepoints(Vector2* __result, const Font* font, const int * codepoints, int length, float fontSize, float spacing);

static inline Vector2 MeasureTextCodepoints(Font font, const int * codepoints, int length, float fontSize, float spacing) {
    Vector2 __r;
    __raylib_MeasureTextCodepoints(&__r, &font, codepoints, length, fontSize, spacing);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetGlyphIndex")))
int __raylib_GetGlyphIndex(const Font* font, int codepoint);

static inline int GetGlyphIndex(Font font, int codepoint) {
    return __raylib_GetGlyphIndex(&font, codepoint);
}

__attribute__((import_module("raylib"), import_name("GetGlyphInfo")))
void __raylib_GetGlyphInfo(GlyphInfo* __result, const Font* font, int codepoint);

static inline GlyphInfo GetGlyphInfo(Font font, int codepoint) {
    GlyphInfo __r;
    __raylib_GetGlyphInfo(&__r, &font, codepoint);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetGlyphAtlasRec")))
void __raylib_GetGlyphAtlasRec(Rectangle* __result, const Font* font, int codepoint);

static inline Rectangle GetGlyphAtlasRec(Font font, int codepoint) {
    Rectangle __r;
    __raylib_GetGlyphAtlasRec(&__r, &font, codepoint);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadUTF8")))
char * LoadUTF8(const int * codepoints, int length);

__attribute__((import_module("raylib"), import_name("UnloadUTF8")))
void UnloadUTF8(char * text);

__attribute__((import_module("raylib"), import_name("LoadCodepoints")))
int * LoadCodepoints(const char * text, int * count);

__attribute__((import_module("raylib"), import_name("UnloadCodepoints")))
void UnloadCodepoints(int * codepoints);

__attribute__((import_module("raylib"), import_name("GetCodepointCount")))
int GetCodepointCount(const char * text);

__attribute__((import_module("raylib"), import_name("GetCodepoint")))
int GetCodepoint(const char * text, int * codepointSize);

__attribute__((import_module("raylib"), import_name("GetCodepointNext")))
int GetCodepointNext(const char * text, int * codepointSize);

__attribute__((import_module("raylib"), import_name("GetCodepointPrevious")))
int GetCodepointPrevious(const char * text, int * codepointSize);

__attribute__((import_module("raylib"), import_name("CodepointToUTF8")))
const char * CodepointToUTF8(int codepoint, int * utf8Size);

__attribute__((import_module("raylib"), import_name("LoadTextLines")))
char ** LoadTextLines(const char * text, int * count);

__attribute__((import_module("raylib"), import_name("UnloadTextLines")))
void UnloadTextLines(char ** text, int lineCount);

__attribute__((import_module("raylib"), import_name("TextCopy")))
int TextCopy(char * dst, const char * src);

__attribute__((import_module("raylib"), import_name("TextIsEqual")))
bool TextIsEqual(const char * text1, const char * text2);

__attribute__((import_module("raylib"), import_name("TextLength")))
unsigned int TextLength(const char * text);

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
}

__attribute__((import_module("raylib"), import_name("TextSubtext")))
const char * TextSubtext(const char * text, int position, int length);

__attribute__((import_module("raylib"), import_name("TextRemoveSpaces")))
const char * TextRemoveSpaces(const char * text);

__attribute__((import_module("raylib"), import_name("GetTextBetween")))
char * GetTextBetween(const char * text, const char * begin, const char * end);

__attribute__((import_module("raylib"), import_name("TextReplace")))
char * TextReplace(const char * text, const char * search, const char * replacement);

__attribute__((import_module("raylib"), import_name("TextReplaceAlloc")))
char * TextReplaceAlloc(const char * text, const char * search, const char * replacement);

__attribute__((import_module("raylib"), import_name("TextReplaceBetween")))
char * TextReplaceBetween(const char * text, const char * begin, const char * end, const char * replacement);

__attribute__((import_module("raylib"), import_name("TextReplaceBetweenAlloc")))
char * TextReplaceBetweenAlloc(const char * text, const char * begin, const char * end, const char * replacement);

__attribute__((import_module("raylib"), import_name("TextInsert")))
char * TextInsert(const char * text, const char * insert, int position);

__attribute__((import_module("raylib"), import_name("TextInsertAlloc")))
char * TextInsertAlloc(const char * text, const char * insert, int position);

__attribute__((import_module("raylib"), import_name("TextJoin")))
char * TextJoin(char ** textList, int count, const char * delimiter);

__attribute__((import_module("raylib"), import_name("TextSplit")))
char ** TextSplit(const char * text, char delimiter, int * count);

__attribute__((import_module("raylib"), import_name("TextAppend")))
void TextAppend(char * text, const char * append, int * position);

__attribute__((import_module("raylib"), import_name("TextFindIndex")))
int TextFindIndex(const char * text, const char * search);

__attribute__((import_module("raylib"), import_name("TextToUpper")))
char * TextToUpper(const char * text);

__attribute__((import_module("raylib"), import_name("TextToLower")))
char * TextToLower(const char * text);

__attribute__((import_module("raylib"), import_name("TextToPascal")))
char * TextToPascal(const char * text);

__attribute__((import_module("raylib"), import_name("TextToSnake")))
char * TextToSnake(const char * text);

__attribute__((import_module("raylib"), import_name("TextToCamel")))
char * TextToCamel(const char * text);

__attribute__((import_module("raylib"), import_name("TextToInteger")))
int TextToInteger(const char * text);

__attribute__((import_module("raylib"), import_name("TextToFloat")))
float TextToFloat(const char * text);

__attribute__((import_module("raylib"), import_name("DrawLine3D")))
void __raylib_DrawLine3D(const Vector3* startPos, const Vector3* endPos, const Color* color);

static inline void DrawLine3D(Vector3 startPos, Vector3 endPos, Color color) {
    __raylib_DrawLine3D(&startPos, &endPos, &color);
}

__attribute__((import_module("raylib"), import_name("DrawPoint3D")))
void __raylib_DrawPoint3D(const Vector3* position, const Color* color);

static inline void DrawPoint3D(Vector3 position, Color color) {
    __raylib_DrawPoint3D(&position, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCircle3D")))
void __raylib_DrawCircle3D(const Vector3* center, float radius, const Vector3* rotationAxis, float rotationAngle, const Color* color);

static inline void DrawCircle3D(Vector3 center, float radius, Vector3 rotationAxis, float rotationAngle, Color color) {
    __raylib_DrawCircle3D(&center, radius, &rotationAxis, rotationAngle, &color);
}

__attribute__((import_module("raylib"), import_name("DrawTriangle3D")))
void __raylib_DrawTriangle3D(const Vector3* v1, const Vector3* v2, const Vector3* v3, const Color* color);

static inline void DrawTriangle3D(Vector3 v1, Vector3 v2, Vector3 v3, Color color) {
    __raylib_DrawTriangle3D(&v1, &v2, &v3, &color);
}

__attribute__((import_module("raylib"), import_name("DrawTriangleStrip3D")))
void __raylib_DrawTriangleStrip3D(const Vector3 * points, int pointCount, const Color* color);

static inline void DrawTriangleStrip3D(const Vector3 * points, int pointCount, Color color) {
    __raylib_DrawTriangleStrip3D(points, pointCount, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCube")))
void __raylib_DrawCube(const Vector3* position, float width, float height, float length, const Color* color);

static inline void DrawCube(Vector3 position, float width, float height, float length, Color color) {
    __raylib_DrawCube(&position, width, height, length, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCubeV")))
void __raylib_DrawCubeV(const Vector3* position, const Vector3* size, const Color* color);

static inline void DrawCubeV(Vector3 position, Vector3 size, Color color) {
    __raylib_DrawCubeV(&position, &size, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCubeWires")))
void __raylib_DrawCubeWires(const Vector3* position, float width, float height, float length, const Color* color);

static inline void DrawCubeWires(Vector3 position, float width, float height, float length, Color color) {
    __raylib_DrawCubeWires(&position, width, height, length, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCubeWiresV")))
void __raylib_DrawCubeWiresV(const Vector3* position, const Vector3* size, const Color* color);

static inline void DrawCubeWiresV(Vector3 position, Vector3 size, Color color) {
    __raylib_DrawCubeWiresV(&position, &size, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSphere")))
void __raylib_DrawSphere(const Vector3* centerPos, float radius, const Color* color);

static inline void DrawSphere(Vector3 centerPos, float radius, Color color) {
    __raylib_DrawSphere(&centerPos, radius, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSphereEx")))
void __raylib_DrawSphereEx(const Vector3* centerPos, float radius, int rings, int slices, const Color* color);

static inline void DrawSphereEx(Vector3 centerPos, float radius, int rings, int slices, Color color) {
    __raylib_DrawSphereEx(&centerPos, radius, rings, slices, &color);
}

__attribute__((import_module("raylib"), import_name("DrawSphereWires")))
void __raylib_DrawSphereWires(const Vector3* centerPos, float radius, int rings, int slices, const Color* color);

static inline void DrawSphereWires(Vector3 centerPos, float radius, int rings, int slices, Color color) {
    __raylib_DrawSphereWires(&centerPos, radius, rings, slices, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCylinder")))
void __raylib_DrawCylinder(const Vector3* position, float radiusTop, float radiusBottom, float height, int slices, const Color* color);

static inline void DrawCylinder(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color) {
    __raylib_DrawCylinder(&position, radiusTop, radiusBottom, height, slices, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCylinderEx")))
void __raylib_DrawCylinderEx(const Vector3* startPos, const Vector3* endPos, float startRadius, float endRadius, int sides, const Color* color);

static inline void DrawCylinderEx(Vector3 startPos, Vector3 endPos, float startRadius, float endRadius, int sides, Color color) {
    __raylib_DrawCylinderEx(&startPos, &endPos, startRadius, endRadius, sides, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCylinderWires")))
void __raylib_DrawCylinderWires(const Vector3* position, float radiusTop, float radiusBottom, float height, int slices, const Color* color);

static inline void DrawCylinderWires(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color) {
    __raylib_DrawCylinderWires(&position, radiusTop, radiusBottom, height, slices, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCylinderWiresEx")))
void __raylib_DrawCylinderWiresEx(const Vector3* startPos, const Vector3* endPos, float startRadius, float endRadius, int slices, const Color* color);

static inline void DrawCylinderWiresEx(Vector3 startPos, Vector3 endPos, float startRadius, float endRadius, int slices, Color color) {
    __raylib_DrawCylinderWiresEx(&startPos, &endPos, startRadius, endRadius, slices, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCapsule")))
void __raylib_DrawCapsule(const Vector3* startPos, const Vector3* endPos, float radius, int rings, int slices, const Color* color);

static inline void DrawCapsule(Vector3 startPos, Vector3 endPos, float radius, int rings, int slices, Color color) {
    __raylib_DrawCapsule(&startPos, &endPos, radius, rings, slices, &color);
}

__attribute__((import_module("raylib"), import_name("DrawCapsuleWires")))
void __raylib_DrawCapsuleWires(const Vector3* startPos, const Vector3* endPos, float radius, int rings, int slices, const Color* color);

static inline void DrawCapsuleWires(Vector3 startPos, Vector3 endPos, float radius, int rings, int slices, Color color) {
    __raylib_DrawCapsuleWires(&startPos, &endPos, radius, rings, slices, &color);
}

__attribute__((import_module("raylib"), import_name("DrawPlane")))
void __raylib_DrawPlane(const Vector3* centerPos, const Vector2* size, const Color* color);

static inline void DrawPlane(Vector3 centerPos, Vector2 size, Color color) {
    __raylib_DrawPlane(&centerPos, &size, &color);
}

__attribute__((import_module("raylib"), import_name("DrawRay")))
void __raylib_DrawRay(const Ray* ray, const Color* color);

static inline void DrawRay(Ray ray, Color color) {
    __raylib_DrawRay(&ray, &color);
}

__attribute__((import_module("raylib"), import_name("DrawGrid")))
void DrawGrid(int slices, float spacing);

__attribute__((import_module("raylib"), import_name("LoadModel")))
void __raylib_LoadModel(Model* __result, const char * fileName);

static inline Model LoadModel(const char * fileName) {
    Model __r;
    __raylib_LoadModel(&__r, fileName);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadModelFromMesh")))
void __raylib_LoadModelFromMesh(Model* __result, const Mesh* mesh);

static inline Model LoadModelFromMesh(Mesh mesh) {
    Model __r;
    __raylib_LoadModelFromMesh(&__r, &mesh);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsModelValid")))
bool __raylib_IsModelValid(const Model* model);

static inline bool IsModelValid(Model model) {
    return __raylib_IsModelValid(&model);
}

__attribute__((import_module("raylib"), import_name("UnloadModel")))
void __raylib_UnloadModel(const Model* model);

static inline void UnloadModel(Model model) {
    __raylib_UnloadModel(&model);
}

__attribute__((import_module("raylib"), import_name("GetModelBoundingBox")))
void __raylib_GetModelBoundingBox(BoundingBox* __result, const Model* model);

static inline BoundingBox GetModelBoundingBox(Model model) {
    BoundingBox __r;
    __raylib_GetModelBoundingBox(&__r, &model);
    return __r;
}

__attribute__((import_module("raylib"), import_name("DrawModel")))
void __raylib_DrawModel(const Model* model, const Vector3* position, float scale, const Color* tint);

static inline void DrawModel(Model model, Vector3 position, float scale, Color tint) {
    __raylib_DrawModel(&model, &position, scale, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawModelEx")))
void __raylib_DrawModelEx(const Model* model, const Vector3* position, const Vector3* rotationAxis, float rotationAngle, const Vector3* scale, const Color* tint);

static inline void DrawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint) {
    __raylib_DrawModelEx(&model, &position, &rotationAxis, rotationAngle, &scale, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawModelWires")))
void __raylib_DrawModelWires(const Model* model, const Vector3* position, float scale, const Color* tint);

static inline void DrawModelWires(Model model, Vector3 position, float scale, Color tint) {
    __raylib_DrawModelWires(&model, &position, scale, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawModelWiresEx")))
void __raylib_DrawModelWiresEx(const Model* model, const Vector3* position, const Vector3* rotationAxis, float rotationAngle, const Vector3* scale, const Color* tint);

static inline void DrawModelWiresEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint) {
    __raylib_DrawModelWiresEx(&model, &position, &rotationAxis, rotationAngle, &scale, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawBoundingBox")))
void __raylib_DrawBoundingBox(const BoundingBox* box, const Color* color);

static inline void DrawBoundingBox(BoundingBox box, Color color) {
    __raylib_DrawBoundingBox(&box, &color);
}

__attribute__((import_module("raylib"), import_name("DrawBillboard")))
void __raylib_DrawBillboard(const Camera* camera, const Texture2D* texture, const Vector3* position, float scale, const Color* tint);

static inline void DrawBillboard(Camera camera, Texture2D texture, Vector3 position, float scale, Color tint) {
    __raylib_DrawBillboard(&camera, &texture, &position, scale, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawBillboardRec")))
void __raylib_DrawBillboardRec(const Camera* camera, const Texture2D* texture, const Rectangle* source, const Vector3* position, const Vector2* size, const Color* tint);

static inline void DrawBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint) {
    __raylib_DrawBillboardRec(&camera, &texture, &source, &position, &size, &tint);
}

__attribute__((import_module("raylib"), import_name("DrawBillboardPro")))
void __raylib_DrawBillboardPro(const Camera* camera, const Texture2D* texture, const Rectangle* source, const Vector3* position, const Vector3* up, const Vector2* size, const Vector2* origin, float rotation, const Color* tint);

static inline void DrawBillboardPro(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector3 up, Vector2 size, Vector2 origin, float rotation, Color tint) {
    __raylib_DrawBillboardPro(&camera, &texture, &source, &position, &up, &size, &origin, rotation, &tint);
}

__attribute__((import_module("raylib"), import_name("UploadMesh")))
void UploadMesh(Mesh * mesh, bool dynamic);

__attribute__((import_module("raylib"), import_name("UpdateMeshBuffer")))
void __raylib_UpdateMeshBuffer(const Mesh* mesh, int index, const void * data, int dataSize, int offset);

static inline void UpdateMeshBuffer(Mesh mesh, int index, const void * data, int dataSize, int offset) {
    __raylib_UpdateMeshBuffer(&mesh, index, data, dataSize, offset);
}

__attribute__((import_module("raylib"), import_name("UnloadMesh")))
void __raylib_UnloadMesh(const Mesh* mesh);

static inline void UnloadMesh(Mesh mesh) {
    __raylib_UnloadMesh(&mesh);
}

__attribute__((import_module("raylib"), import_name("DrawMesh")))
void __raylib_DrawMesh(const Mesh* mesh, const Material* material, const Matrix* transform);

static inline void DrawMesh(Mesh mesh, Material material, Matrix transform) {
    __raylib_DrawMesh(&mesh, &material, &transform);
}

__attribute__((import_module("raylib"), import_name("DrawMeshInstanced")))
void __raylib_DrawMeshInstanced(const Mesh* mesh, const Material* material, const Matrix * transforms, int instances);

static inline void DrawMeshInstanced(Mesh mesh, Material material, const Matrix * transforms, int instances) {
    __raylib_DrawMeshInstanced(&mesh, &material, transforms, instances);
}

__attribute__((import_module("raylib"), import_name("GetMeshBoundingBox")))
void __raylib_GetMeshBoundingBox(BoundingBox* __result, const Mesh* mesh);

static inline BoundingBox GetMeshBoundingBox(Mesh mesh) {
    BoundingBox __r;
    __raylib_GetMeshBoundingBox(&__r, &mesh);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshTangents")))
void GenMeshTangents(Mesh * mesh);

__attribute__((import_module("raylib"), import_name("ExportMesh")))
bool __raylib_ExportMesh(const Mesh* mesh, const char * fileName);

static inline bool ExportMesh(Mesh mesh, const char * fileName) {
    return __raylib_ExportMesh(&mesh, fileName);
}

__attribute__((import_module("raylib"), import_name("ExportMeshAsCode")))
bool __raylib_ExportMeshAsCode(const Mesh* mesh, const char * fileName);

static inline bool ExportMeshAsCode(Mesh mesh, const char * fileName) {
    return __raylib_ExportMeshAsCode(&mesh, fileName);
}

__attribute__((import_module("raylib"), import_name("GenMeshPoly")))
void __raylib_GenMeshPoly(Mesh* __result, int sides, float radius);

static inline Mesh GenMeshPoly(int sides, float radius) {
    Mesh __r;
    __raylib_GenMeshPoly(&__r, sides, radius);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshPlane")))
void __raylib_GenMeshPlane(Mesh* __result, float width, float length, int resX, int resZ);

static inline Mesh GenMeshPlane(float width, float length, int resX, int resZ) {
    Mesh __r;
    __raylib_GenMeshPlane(&__r, width, length, resX, resZ);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshCube")))
void __raylib_GenMeshCube(Mesh* __result, float width, float height, float length);

static inline Mesh GenMeshCube(float width, float height, float length) {
    Mesh __r;
    __raylib_GenMeshCube(&__r, width, height, length);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshSphere")))
void __raylib_GenMeshSphere(Mesh* __result, float radius, int rings, int slices);

static inline Mesh GenMeshSphere(float radius, int rings, int slices) {
    Mesh __r;
    __raylib_GenMeshSphere(&__r, radius, rings, slices);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshHemiSphere")))
void __raylib_GenMeshHemiSphere(Mesh* __result, float radius, int rings, int slices);

static inline Mesh GenMeshHemiSphere(float radius, int rings, int slices) {
    Mesh __r;
    __raylib_GenMeshHemiSphere(&__r, radius, rings, slices);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshCylinder")))
void __raylib_GenMeshCylinder(Mesh* __result, float radius, float height, int slices);

static inline Mesh GenMeshCylinder(float radius, float height, int slices) {
    Mesh __r;
    __raylib_GenMeshCylinder(&__r, radius, height, slices);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshCone")))
void __raylib_GenMeshCone(Mesh* __result, float radius, float height, int slices);

static inline Mesh GenMeshCone(float radius, float height, int slices) {
    Mesh __r;
    __raylib_GenMeshCone(&__r, radius, height, slices);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshTorus")))
void __raylib_GenMeshTorus(Mesh* __result, float radius, float size, int radSeg, int sides);

static inline Mesh GenMeshTorus(float radius, float size, int radSeg, int sides) {
    Mesh __r;
    __raylib_GenMeshTorus(&__r, radius, size, radSeg, sides);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshKnot")))
void __raylib_GenMeshKnot(Mesh* __result, float radius, float size, int radSeg, int sides);

static inline Mesh GenMeshKnot(float radius, float size, int radSeg, int sides) {
    Mesh __r;
    __raylib_GenMeshKnot(&__r, radius, size, radSeg, sides);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshHeightmap")))
void __raylib_GenMeshHeightmap(Mesh* __result, const Image* heightmap, const Vector3* size);

static inline Mesh GenMeshHeightmap(Image heightmap, Vector3 size) {
    Mesh __r;
    __raylib_GenMeshHeightmap(&__r, &heightmap, &size);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GenMeshCubicmap")))
void __raylib_GenMeshCubicmap(Mesh* __result, const Image* cubicmap, const Vector3* cubeSize);

static inline Mesh GenMeshCubicmap(Image cubicmap, Vector3 cubeSize) {
    Mesh __r;
    __raylib_GenMeshCubicmap(&__r, &cubicmap, &cubeSize);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadMaterials")))
Material * LoadMaterials(const char * fileName, int * materialCount);

__attribute__((import_module("raylib"), import_name("LoadMaterialDefault")))
void __raylib_LoadMaterialDefault(Material* __result);

static inline Material LoadMaterialDefault(void) {
    Material __r;
    __raylib_LoadMaterialDefault(&__r);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsMaterialValid")))
bool __raylib_IsMaterialValid(const Material* material);

static inline bool IsMaterialValid(Material material) {
    return __raylib_IsMaterialValid(&material);
}

__attribute__((import_module("raylib"), import_name("UnloadMaterial")))
void __raylib_UnloadMaterial(const Material* material);

static inline void UnloadMaterial(Material material) {
    __raylib_UnloadMaterial(&material);
}

__attribute__((import_module("raylib"), import_name("SetMaterialTexture")))
void __raylib_SetMaterialTexture(Material * material, int mapType, const Texture2D* texture);

static inline void SetMaterialTexture(Material * material, int mapType, Texture2D texture) {
    __raylib_SetMaterialTexture(material, mapType, &texture);
}

__attribute__((import_module("raylib"), import_name("SetModelMeshMaterial")))
void SetModelMeshMaterial(Model * model, int meshId, int materialId);

__attribute__((import_module("raylib"), import_name("LoadModelAnimations")))
ModelAnimation * LoadModelAnimations(const char * fileName, int * animCount);

__attribute__((import_module("raylib"), import_name("UpdateModelAnimation")))
void __raylib_UpdateModelAnimation(const Model* model, const ModelAnimation* anim, float frame);

static inline void UpdateModelAnimation(Model model, ModelAnimation anim, float frame) {
    __raylib_UpdateModelAnimation(&model, &anim, frame);
}

__attribute__((import_module("raylib"), import_name("UpdateModelAnimationEx")))
void __raylib_UpdateModelAnimationEx(const Model* model, const ModelAnimation* animA, float frameA, const ModelAnimation* animB, float frameB, float blend);

static inline void UpdateModelAnimationEx(Model model, ModelAnimation animA, float frameA, ModelAnimation animB, float frameB, float blend) {
    __raylib_UpdateModelAnimationEx(&model, &animA, frameA, &animB, frameB, blend);
}

__attribute__((import_module("raylib"), import_name("UnloadModelAnimations")))
void UnloadModelAnimations(ModelAnimation * animations, int animCount);

__attribute__((import_module("raylib"), import_name("IsModelAnimationValid")))
bool __raylib_IsModelAnimationValid(const Model* model, const ModelAnimation* anim);

static inline bool IsModelAnimationValid(Model model, ModelAnimation anim) {
    return __raylib_IsModelAnimationValid(&model, &anim);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionSpheres")))
bool __raylib_CheckCollisionSpheres(const Vector3* center1, float radius1, const Vector3* center2, float radius2);

static inline bool CheckCollisionSpheres(Vector3 center1, float radius1, Vector3 center2, float radius2) {
    return __raylib_CheckCollisionSpheres(&center1, radius1, &center2, radius2);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionBoxes")))
bool __raylib_CheckCollisionBoxes(const BoundingBox* box1, const BoundingBox* box2);

static inline bool CheckCollisionBoxes(BoundingBox box1, BoundingBox box2) {
    return __raylib_CheckCollisionBoxes(&box1, &box2);
}

__attribute__((import_module("raylib"), import_name("CheckCollisionBoxSphere")))
bool __raylib_CheckCollisionBoxSphere(const BoundingBox* box, const Vector3* center, float radius);

static inline bool CheckCollisionBoxSphere(BoundingBox box, Vector3 center, float radius) {
    return __raylib_CheckCollisionBoxSphere(&box, &center, radius);
}

__attribute__((import_module("raylib"), import_name("GetRayCollisionSphere")))
void __raylib_GetRayCollisionSphere(RayCollision* __result, const Ray* ray, const Vector3* center, float radius);

static inline RayCollision GetRayCollisionSphere(Ray ray, Vector3 center, float radius) {
    RayCollision __r;
    __raylib_GetRayCollisionSphere(&__r, &ray, &center, radius);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetRayCollisionBox")))
void __raylib_GetRayCollisionBox(RayCollision* __result, const Ray* ray, const BoundingBox* box);

static inline RayCollision GetRayCollisionBox(Ray ray, BoundingBox box) {
    RayCollision __r;
    __raylib_GetRayCollisionBox(&__r, &ray, &box);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetRayCollisionMesh")))
void __raylib_GetRayCollisionMesh(RayCollision* __result, const Ray* ray, const Mesh* mesh, const Matrix* transform);

static inline RayCollision GetRayCollisionMesh(Ray ray, Mesh mesh, Matrix transform) {
    RayCollision __r;
    __raylib_GetRayCollisionMesh(&__r, &ray, &mesh, &transform);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetRayCollisionTriangle")))
void __raylib_GetRayCollisionTriangle(RayCollision* __result, const Ray* ray, const Vector3* p1, const Vector3* p2, const Vector3* p3);

static inline RayCollision GetRayCollisionTriangle(Ray ray, Vector3 p1, Vector3 p2, Vector3 p3) {
    RayCollision __r;
    __raylib_GetRayCollisionTriangle(&__r, &ray, &p1, &p2, &p3);
    return __r;
}

__attribute__((import_module("raylib"), import_name("GetRayCollisionQuad")))
void __raylib_GetRayCollisionQuad(RayCollision* __result, const Ray* ray, const Vector3* p1, const Vector3* p2, const Vector3* p3, const Vector3* p4);

static inline RayCollision GetRayCollisionQuad(Ray ray, Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4) {
    RayCollision __r;
    __raylib_GetRayCollisionQuad(&__r, &ray, &p1, &p2, &p3, &p4);
    return __r;
}

__attribute__((import_module("raylib"), import_name("InitAudioDevice")))
void InitAudioDevice(void);

__attribute__((import_module("raylib"), import_name("CloseAudioDevice")))
void CloseAudioDevice(void);

__attribute__((import_module("raylib"), import_name("IsAudioDeviceReady")))
bool IsAudioDeviceReady(void);

__attribute__((import_module("raylib"), import_name("SetMasterVolume")))
void SetMasterVolume(float volume);

__attribute__((import_module("raylib"), import_name("GetMasterVolume")))
float GetMasterVolume(void);

__attribute__((import_module("raylib"), import_name("LoadWave")))
void __raylib_LoadWave(Wave* __result, const char * fileName);

static inline Wave LoadWave(const char * fileName) {
    Wave __r;
    __raylib_LoadWave(&__r, fileName);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadWaveFromMemory")))
void __raylib_LoadWaveFromMemory(Wave* __result, const char * fileType, const unsigned char * fileData, int dataSize);

static inline Wave LoadWaveFromMemory(const char * fileType, const unsigned char * fileData, int dataSize) {
    Wave __r;
    __raylib_LoadWaveFromMemory(&__r, fileType, fileData, dataSize);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsWaveValid")))
bool __raylib_IsWaveValid(const Wave* wave);

static inline bool IsWaveValid(Wave wave) {
    return __raylib_IsWaveValid(&wave);
}

__attribute__((import_module("raylib"), import_name("LoadSound")))
void __raylib_LoadSound(Sound* __result, const char * fileName);

static inline Sound LoadSound(const char * fileName) {
    Sound __r;
    __raylib_LoadSound(&__r, fileName);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadSoundFromWave")))
void __raylib_LoadSoundFromWave(Sound* __result, const Wave* wave);

static inline Sound LoadSoundFromWave(Wave wave) {
    Sound __r;
    __raylib_LoadSoundFromWave(&__r, &wave);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadSoundAlias")))
void __raylib_LoadSoundAlias(Sound* __result, const Sound* source);

static inline Sound LoadSoundAlias(Sound source) {
    Sound __r;
    __raylib_LoadSoundAlias(&__r, &source);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsSoundValid")))
bool __raylib_IsSoundValid(const Sound* sound);

static inline bool IsSoundValid(Sound sound) {
    return __raylib_IsSoundValid(&sound);
}

__attribute__((import_module("raylib"), import_name("UpdateSound")))
void __raylib_UpdateSound(const Sound* sound, const void * data, int frameCount);

static inline void UpdateSound(Sound sound, const void * data, int frameCount) {
    __raylib_UpdateSound(&sound, data, frameCount);
}

__attribute__((import_module("raylib"), import_name("UnloadWave")))
void __raylib_UnloadWave(const Wave* wave);

static inline void UnloadWave(Wave wave) {
    __raylib_UnloadWave(&wave);
}

__attribute__((import_module("raylib"), import_name("UnloadSound")))
void __raylib_UnloadSound(const Sound* sound);

static inline void UnloadSound(Sound sound) {
    __raylib_UnloadSound(&sound);
}

__attribute__((import_module("raylib"), import_name("UnloadSoundAlias")))
void __raylib_UnloadSoundAlias(const Sound* alias);

static inline void UnloadSoundAlias(Sound alias) {
    __raylib_UnloadSoundAlias(&alias);
}

__attribute__((import_module("raylib"), import_name("ExportWave")))
bool __raylib_ExportWave(const Wave* wave, const char * fileName);

static inline bool ExportWave(Wave wave, const char * fileName) {
    return __raylib_ExportWave(&wave, fileName);
}

__attribute__((import_module("raylib"), import_name("ExportWaveAsCode")))
bool __raylib_ExportWaveAsCode(const Wave* wave, const char * fileName);

static inline bool ExportWaveAsCode(Wave wave, const char * fileName) {
    return __raylib_ExportWaveAsCode(&wave, fileName);
}

__attribute__((import_module("raylib"), import_name("PlaySound")))
void __raylib_PlaySound(const Sound* sound);

static inline void PlaySound(Sound sound) {
    __raylib_PlaySound(&sound);
}

__attribute__((import_module("raylib"), import_name("StopSound")))
void __raylib_StopSound(const Sound* sound);

static inline void StopSound(Sound sound) {
    __raylib_StopSound(&sound);
}

__attribute__((import_module("raylib"), import_name("PauseSound")))
void __raylib_PauseSound(const Sound* sound);

static inline void PauseSound(Sound sound) {
    __raylib_PauseSound(&sound);
}

__attribute__((import_module("raylib"), import_name("ResumeSound")))
void __raylib_ResumeSound(const Sound* sound);

static inline void ResumeSound(Sound sound) {
    __raylib_ResumeSound(&sound);
}

__attribute__((import_module("raylib"), import_name("IsSoundPlaying")))
bool __raylib_IsSoundPlaying(const Sound* sound);

static inline bool IsSoundPlaying(Sound sound) {
    return __raylib_IsSoundPlaying(&sound);
}

__attribute__((import_module("raylib"), import_name("SetSoundVolume")))
void __raylib_SetSoundVolume(const Sound* sound, float volume);

static inline void SetSoundVolume(Sound sound, float volume) {
    __raylib_SetSoundVolume(&sound, volume);
}

__attribute__((import_module("raylib"), import_name("SetSoundPitch")))
void __raylib_SetSoundPitch(const Sound* sound, float pitch);

static inline void SetSoundPitch(Sound sound, float pitch) {
    __raylib_SetSoundPitch(&sound, pitch);
}

__attribute__((import_module("raylib"), import_name("SetSoundPan")))
void __raylib_SetSoundPan(const Sound* sound, float pan);

static inline void SetSoundPan(Sound sound, float pan) {
    __raylib_SetSoundPan(&sound, pan);
}

__attribute__((import_module("raylib"), import_name("WaveCopy")))
void __raylib_WaveCopy(Wave* __result, const Wave* wave);

static inline Wave WaveCopy(Wave wave) {
    Wave __r;
    __raylib_WaveCopy(&__r, &wave);
    return __r;
}

__attribute__((import_module("raylib"), import_name("WaveCrop")))
void WaveCrop(Wave * wave, int initFrame, int finalFrame);

__attribute__((import_module("raylib"), import_name("WaveFormat")))
void WaveFormat(Wave * wave, int sampleRate, int sampleSize, int channels);

__attribute__((import_module("raylib"), import_name("LoadWaveSamples")))
float * __raylib_LoadWaveSamples(const Wave* wave);

static inline float * LoadWaveSamples(Wave wave) {
    return __raylib_LoadWaveSamples(&wave);
}

__attribute__((import_module("raylib"), import_name("UnloadWaveSamples")))
void UnloadWaveSamples(float * samples);

__attribute__((import_module("raylib"), import_name("LoadMusicStream")))
void __raylib_LoadMusicStream(Music* __result, const char * fileName);

static inline Music LoadMusicStream(const char * fileName) {
    Music __r;
    __raylib_LoadMusicStream(&__r, fileName);
    return __r;
}

__attribute__((import_module("raylib"), import_name("LoadMusicStreamFromMemory")))
void __raylib_LoadMusicStreamFromMemory(Music* __result, const char * fileType, const unsigned char * data, int dataSize);

static inline Music LoadMusicStreamFromMemory(const char * fileType, const unsigned char * data, int dataSize) {
    Music __r;
    __raylib_LoadMusicStreamFromMemory(&__r, fileType, data, dataSize);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsMusicValid")))
bool __raylib_IsMusicValid(const Music* music);

static inline bool IsMusicValid(Music music) {
    return __raylib_IsMusicValid(&music);
}

__attribute__((import_module("raylib"), import_name("UnloadMusicStream")))
void __raylib_UnloadMusicStream(const Music* music);

static inline void UnloadMusicStream(Music music) {
    __raylib_UnloadMusicStream(&music);
}

__attribute__((import_module("raylib"), import_name("PlayMusicStream")))
void __raylib_PlayMusicStream(const Music* music);

static inline void PlayMusicStream(Music music) {
    __raylib_PlayMusicStream(&music);
}

__attribute__((import_module("raylib"), import_name("IsMusicStreamPlaying")))
bool __raylib_IsMusicStreamPlaying(const Music* music);

static inline bool IsMusicStreamPlaying(Music music) {
    return __raylib_IsMusicStreamPlaying(&music);
}

__attribute__((import_module("raylib"), import_name("UpdateMusicStream")))
void __raylib_UpdateMusicStream(const Music* music);

static inline void UpdateMusicStream(Music music) {
    __raylib_UpdateMusicStream(&music);
}

__attribute__((import_module("raylib"), import_name("StopMusicStream")))
void __raylib_StopMusicStream(const Music* music);

static inline void StopMusicStream(Music music) {
    __raylib_StopMusicStream(&music);
}

__attribute__((import_module("raylib"), import_name("PauseMusicStream")))
void __raylib_PauseMusicStream(const Music* music);

static inline void PauseMusicStream(Music music) {
    __raylib_PauseMusicStream(&music);
}

__attribute__((import_module("raylib"), import_name("ResumeMusicStream")))
void __raylib_ResumeMusicStream(const Music* music);

static inline void ResumeMusicStream(Music music) {
    __raylib_ResumeMusicStream(&music);
}

__attribute__((import_module("raylib"), import_name("SeekMusicStream")))
void __raylib_SeekMusicStream(const Music* music, float position);

static inline void SeekMusicStream(Music music, float position) {
    __raylib_SeekMusicStream(&music, position);
}

__attribute__((import_module("raylib"), import_name("SetMusicVolume")))
void __raylib_SetMusicVolume(const Music* music, float volume);

static inline void SetMusicVolume(Music music, float volume) {
    __raylib_SetMusicVolume(&music, volume);
}

__attribute__((import_module("raylib"), import_name("SetMusicPitch")))
void __raylib_SetMusicPitch(const Music* music, float pitch);

static inline void SetMusicPitch(Music music, float pitch) {
    __raylib_SetMusicPitch(&music, pitch);
}

__attribute__((import_module("raylib"), import_name("SetMusicPan")))
void __raylib_SetMusicPan(const Music* music, float pan);

static inline void SetMusicPan(Music music, float pan) {
    __raylib_SetMusicPan(&music, pan);
}

__attribute__((import_module("raylib"), import_name("GetMusicTimeLength")))
float __raylib_GetMusicTimeLength(const Music* music);

static inline float GetMusicTimeLength(Music music) {
    return __raylib_GetMusicTimeLength(&music);
}

__attribute__((import_module("raylib"), import_name("GetMusicTimePlayed")))
float __raylib_GetMusicTimePlayed(const Music* music);

static inline float GetMusicTimePlayed(Music music) {
    return __raylib_GetMusicTimePlayed(&music);
}

__attribute__((import_module("raylib"), import_name("LoadAudioStream")))
void __raylib_LoadAudioStream(AudioStream* __result, unsigned int sampleRate, unsigned int sampleSize, unsigned int channels);

static inline AudioStream LoadAudioStream(unsigned int sampleRate, unsigned int sampleSize, unsigned int channels) {
    AudioStream __r;
    __raylib_LoadAudioStream(&__r, sampleRate, sampleSize, channels);
    return __r;
}

__attribute__((import_module("raylib"), import_name("IsAudioStreamValid")))
bool __raylib_IsAudioStreamValid(const AudioStream* stream);

static inline bool IsAudioStreamValid(AudioStream stream) {
    return __raylib_IsAudioStreamValid(&stream);
}

__attribute__((import_module("raylib"), import_name("UnloadAudioStream")))
void __raylib_UnloadAudioStream(const AudioStream* stream);

static inline void UnloadAudioStream(AudioStream stream) {
    __raylib_UnloadAudioStream(&stream);
}

__attribute__((import_module("raylib"), import_name("UpdateAudioStream")))
void __raylib_UpdateAudioStream(const AudioStream* stream, const void * data, int frameCount);

static inline void UpdateAudioStream(AudioStream stream, const void * data, int frameCount) {
    __raylib_UpdateAudioStream(&stream, data, frameCount);
}

__attribute__((import_module("raylib"), import_name("IsAudioStreamProcessed")))
bool __raylib_IsAudioStreamProcessed(const AudioStream* stream);

static inline bool IsAudioStreamProcessed(AudioStream stream) {
    return __raylib_IsAudioStreamProcessed(&stream);
}

__attribute__((import_module("raylib"), import_name("PlayAudioStream")))
void __raylib_PlayAudioStream(const AudioStream* stream);

static inline void PlayAudioStream(AudioStream stream) {
    __raylib_PlayAudioStream(&stream);
}

__attribute__((import_module("raylib"), import_name("PauseAudioStream")))
void __raylib_PauseAudioStream(const AudioStream* stream);

static inline void PauseAudioStream(AudioStream stream) {
    __raylib_PauseAudioStream(&stream);
}

__attribute__((import_module("raylib"), import_name("ResumeAudioStream")))
void __raylib_ResumeAudioStream(const AudioStream* stream);

static inline void ResumeAudioStream(AudioStream stream) {
    __raylib_ResumeAudioStream(&stream);
}

__attribute__((import_module("raylib"), import_name("IsAudioStreamPlaying")))
bool __raylib_IsAudioStreamPlaying(const AudioStream* stream);

static inline bool IsAudioStreamPlaying(AudioStream stream) {
    return __raylib_IsAudioStreamPlaying(&stream);
}

__attribute__((import_module("raylib"), import_name("StopAudioStream")))
void __raylib_StopAudioStream(const AudioStream* stream);

static inline void StopAudioStream(AudioStream stream) {
    __raylib_StopAudioStream(&stream);
}

__attribute__((import_module("raylib"), import_name("SetAudioStreamVolume")))
void __raylib_SetAudioStreamVolume(const AudioStream* stream, float volume);

static inline void SetAudioStreamVolume(AudioStream stream, float volume) {
    __raylib_SetAudioStreamVolume(&stream, volume);
}

__attribute__((import_module("raylib"), import_name("SetAudioStreamPitch")))
void __raylib_SetAudioStreamPitch(const AudioStream* stream, float pitch);

static inline void SetAudioStreamPitch(AudioStream stream, float pitch) {
    __raylib_SetAudioStreamPitch(&stream, pitch);
}

__attribute__((import_module("raylib"), import_name("SetAudioStreamPan")))
void __raylib_SetAudioStreamPan(const AudioStream* stream, float pan);

static inline void SetAudioStreamPan(AudioStream stream, float pan) {
    __raylib_SetAudioStreamPan(&stream, pan);
}

__attribute__((import_module("raylib"), import_name("SetAudioStreamBufferSizeDefault")))
void SetAudioStreamBufferSizeDefault(int size);
