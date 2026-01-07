#include <stdarg.h>
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
// Vector2, 2 components
typedef struct Vector2 {
    float x; // Vector x component
    float y; // Vector y component
} Vector2;

// Vector3, 3 components
typedef struct Vector3 {
    float x; // Vector x component
    float y; // Vector y component
    float z; // Vector z component
} Vector3;

// Vector4, 4 components
typedef struct Vector4 {
    float x; // Vector x component
    float y; // Vector y component
    float z; // Vector z component
    float w; // Vector w component
} Vector4;


// Quaternion, 4 components (Vector4 alias)
typedef Vector4 Quaternion;

// Matrix, 4x4 components, column major, OpenGL style, right-handed
typedef struct Matrix {
    float m0; // Matrix first row (4 components)
    float m4; // Matrix first row (4 components)
    float m8; // Matrix first row (4 components)
    float m12; // Matrix first row (4 components)
    float m1; // Matrix second row (4 components)
    float m5; // Matrix second row (4 components)
    float m9; // Matrix second row (4 components)
    float m13; // Matrix second row (4 components)
    float m2; // Matrix third row (4 components)
    float m6; // Matrix third row (4 components)
    float m10; // Matrix third row (4 components)
    float m14; // Matrix third row (4 components)
    float m3; // Matrix fourth row (4 components)
    float m7; // Matrix fourth row (4 components)
    float m11; // Matrix fourth row (4 components)
    float m15; // Matrix fourth row (4 components)
} Matrix;

// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color {
    unsigned char r; // Color red value
    unsigned char g; // Color green value
    unsigned char b; // Color blue value
    unsigned char a; // Color alpha value
} Color;

// Rectangle, 4 components
typedef struct Rectangle {
    float x; // Rectangle top-left corner position x
    float y; // Rectangle top-left corner position y
    float width; // Rectangle width
    float height; // Rectangle height
} Rectangle;

// Image, pixel data stored in CPU memory (RAM)
typedef struct Image {
    void * data; // Image raw data
    int width; // Image base width
    int height; // Image base height
    int mipmaps; // Mipmap levels, 1 by default
    int format; // Data format (PixelFormat type)
} Image;

// Texture, tex data stored in GPU memory (VRAM)
typedef struct Texture {
    unsigned int id; // OpenGL texture id
    int width; // Texture base width
    int height; // Texture base height
    int mipmaps; // Mipmap levels, 1 by default
    int format; // Data format (PixelFormat type)
} Texture;


// Texture2D, same as Texture
typedef Texture Texture2D;


// TextureCubemap, same as Texture
typedef Texture TextureCubemap;

// RenderTexture, fbo for texture rendering
typedef struct RenderTexture {
    unsigned int id; // OpenGL framebuffer object id
    Texture texture; // Color buffer attachment texture
    Texture depth; // Depth buffer attachment texture
} RenderTexture;


// RenderTexture2D, same as RenderTexture
typedef RenderTexture RenderTexture2D;

// NPatchInfo, n-patch layout info
typedef struct NPatchInfo {
    Rectangle source; // Texture source rectangle
    int left; // Left border offset
    int top; // Top border offset
    int right; // Right border offset
    int bottom; // Bottom border offset
    int layout; // Layout of the n-patch: 3x3, 1x3 or 3x1
} NPatchInfo;

// GlyphInfo, font characters glyphs info
typedef struct GlyphInfo {
    int value; // Character value (Unicode)
    int offsetX; // Character offset X when drawing
    int offsetY; // Character offset Y when drawing
    int advanceX; // Character advance position X
    Image image; // Character image data
} GlyphInfo;

// Font, font texture and GlyphInfo array data
typedef struct Font {
    int baseSize; // Base size (default chars height)
    int glyphCount; // Number of glyph characters
    int glyphPadding; // Padding around the glyph characters
    Texture2D texture; // Texture atlas containing the glyphs
    Rectangle * recs; // Rectangles in texture for the glyphs
    GlyphInfo * glyphs; // Glyphs info data
} Font;

// Camera, defines position/orientation in 3d space
typedef struct Camera3D {
    Vector3 position; // Camera position
    Vector3 target; // Camera target it looks-at
    Vector3 up; // Camera up vector (rotation over its axis)
    float fovy; // Camera field-of-view aperture in Y (degrees) in perspective, used as near plane height in world units in orthographic
    int projection; // Camera projection: CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC
} Camera3D;


// Camera type fallback, defaults to Camera3D
typedef Camera3D Camera;

// Camera2D, defines position/orientation in 2d space
typedef struct Camera2D {
    Vector2 offset; // Camera offset (screen space offset from window origin)
    Vector2 target; // Camera target (world space target point that is mapped to screen space offset)
    float rotation; // Camera rotation in degrees (pivots around target)
    float zoom; // Camera zoom (scaling around target), must not be set to 0, set to 1.0f for no scale
} Camera2D;

// Mesh, vertex data and vao/vbo
typedef struct Mesh {
    int vertexCount; // Number of vertices stored in arrays
    int triangleCount; // Number of triangles stored (indexed or not)
    float * vertices; // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float * texcoords; // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float * texcoords2; // Vertex texture second coordinates (UV - 2 components per vertex) (shader-location = 5)
    float * normals; // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
    float * tangents; // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
    unsigned char * colors; // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
    unsigned short * indices; // Vertex indices (in case vertex data comes indexed)
    float * animVertices; // Animated vertex positions (after bones transformations)
    float * animNormals; // Animated normals (after bones transformations)
    unsigned char * boneIds; // Vertex bone ids, max 255 bone ids, up to 4 bones influence by vertex (skinning) (shader-location = 6)
    float * boneWeights; // Vertex bone weight, up to 4 bones influence by vertex (skinning) (shader-location = 7)
    Matrix * boneMatrices; // Bones animated transformation matrices
    int boneCount; // Number of bones
    unsigned int vaoId; // OpenGL Vertex Array Object id
    unsigned int * vboId; // OpenGL Vertex Buffer Objects id (default vertex data)
} Mesh;

// Shader
typedef struct Shader {
    unsigned int id; // Shader program id
    int * locs; // Shader locations array (RL_MAX_SHADER_LOCATIONS)
} Shader;

// MaterialMap
typedef struct MaterialMap {
    Texture2D texture; // Material map texture
    Color color; // Material map color
    float value; // Material map value
} MaterialMap;

// Material, includes shader and maps
typedef struct Material {
    Shader shader; // Material shader
    MaterialMap * maps; // Material maps array (MAX_MATERIAL_MAPS)
    float params[4]; // Material generic parameters (if required)
} Material;

// Transform, vertex transformation data
typedef struct Transform {
    Vector3 translation; // Translation
    Quaternion rotation; // Rotation
    Vector3 scale; // Scale
} Transform;

// Bone, skeletal animation bone
typedef struct BoneInfo {
    char name[32]; // Bone name
    int parent; // Bone parent
} BoneInfo;

// Model, meshes, materials and animation data
typedef struct Model {
    Matrix transform; // Local transform matrix
    int meshCount; // Number of meshes
    int materialCount; // Number of materials
    Mesh * meshes; // Meshes array
    Material * materials; // Materials array
    int * meshMaterial; // Mesh material number
    int boneCount; // Number of bones
    BoneInfo * bones; // Bones information (skeleton)
    Transform * bindPose; // Bones base transformation (pose)
} Model;

// ModelAnimation
typedef struct ModelAnimation {
    int boneCount; // Number of bones
    int frameCount; // Number of animation frames
    BoneInfo * bones; // Bones information (skeleton)
    Transform ** framePoses; // Poses array by frame
    char name[32]; // Animation name
} ModelAnimation;

// Ray, ray for raycasting
typedef struct Ray {
    Vector3 position; // Ray position (origin)
    Vector3 direction; // Ray direction (normalized)
} Ray;

// RayCollision, ray hit information
typedef struct RayCollision {
    bool hit; // Did the ray hit something?
    float distance; // Distance to the nearest hit
    Vector3 point; // Point of the nearest hit
    Vector3 normal; // Surface normal of hit
} RayCollision;

// BoundingBox
typedef struct BoundingBox {
    Vector3 min; // Minimum vertex box-corner
    Vector3 max; // Maximum vertex box-corner
} BoundingBox;

// Wave, audio wave data
typedef struct Wave {
    unsigned int frameCount; // Total number of frames (considering channels)
    unsigned int sampleRate; // Frequency (samples per second)
    unsigned int sampleSize; // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels; // Number of channels (1-mono, 2-stereo, ...)
    void * data; // Buffer data pointer
} Wave;

// AudioStream, custom audio stream
typedef struct AudioStream {
    rAudioBuffer * buffer; // Pointer to internal data used by the audio system
    rAudioProcessor * processor; // Pointer to internal data processor, useful for audio effects
    unsigned int sampleRate; // Frequency (samples per second)
    unsigned int sampleSize; // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels; // Number of channels (1-mono, 2-stereo, ...)
} AudioStream;

// Sound
typedef struct Sound {
    AudioStream stream; // Audio stream
    unsigned int frameCount; // Total number of frames (considering channels)
} Sound;

// Music, audio stream, anything longer than ~10 seconds should be streamed
typedef struct Music {
    AudioStream stream; // Audio stream
    unsigned int frameCount; // Total number of frames (considering channels)
    bool looping; // Music looping enable
    int ctxType; // Type of music context (audio filetype)
    void * ctxData; // Audio context data, depends on type
} Music;

// VrDeviceInfo, Head-Mounted-Display device parameters
typedef struct VrDeviceInfo {
    int hResolution; // Horizontal resolution in pixels
    int vResolution; // Vertical resolution in pixels
    float hScreenSize; // Horizontal size in meters
    float vScreenSize; // Vertical size in meters
    float eyeToScreenDistance; // Distance between eye and display in meters
    float lensSeparationDistance; // Lens separation distance in meters
    float interpupillaryDistance; // IPD (distance between pupils) in meters
    float lensDistortionValues[4]; // Lens distortion constant parameters
    float chromaAbCorrection[4]; // Chromatic aberration correction parameters
} VrDeviceInfo;

// VrStereoConfig, VR stereo rendering configuration for simulator
typedef struct VrStereoConfig {
    Matrix projection[2]; // VR projection matrices (per eye)
    Matrix viewOffset[2]; // VR view offset matrices (per eye)
    float leftLensCenter[2]; // VR left lens center
    float rightLensCenter[2]; // VR right lens center
    float leftScreenCenter[2]; // VR left screen center
    float rightScreenCenter[2]; // VR right screen center
    float scale[2]; // VR distortion scale
    float scaleIn[2]; // VR distortion scale in
} VrStereoConfig;

// File path list
typedef struct FilePathList {
    unsigned int capacity; // Filepaths max entries
    unsigned int count; // Filepaths entries count
    char ** paths; // Filepaths entries
} FilePathList;

// Automation event
typedef struct AutomationEvent {
    unsigned int frame; // Event frame
    unsigned int type; // Event type (AutomationEventType)
    int params[4]; // Event parameters (if required)
} AutomationEvent;

// Automation event list
typedef struct AutomationEventList {
    unsigned int capacity; // Events max entries (MAX_AUTOMATION_EVENTS)
    unsigned int count; // Events entries count
    AutomationEvent * events; // Events entries
} AutomationEventList;

//----------------------------------------------------------------------------------
// Enumerators Definition
//----------------------------------------------------------------------------------
// System/Window config flags
typedef enum {
    FLAG_VSYNC_HINT = 64, // Set to try enabling V-Sync on GPU
    FLAG_FULLSCREEN_MODE = 2, // Set to run program in fullscreen
    FLAG_WINDOW_RESIZABLE = 4, // Set to allow resizable window
    FLAG_WINDOW_UNDECORATED = 8, // Set to disable window decoration (frame and buttons)
    FLAG_WINDOW_HIDDEN = 128, // Set to hide window
    FLAG_WINDOW_MINIMIZED = 512, // Set to minimize window (iconify)
    FLAG_WINDOW_MAXIMIZED = 1024, // Set to maximize window (expanded to monitor)
    FLAG_WINDOW_UNFOCUSED = 2048, // Set to window non focused
    FLAG_WINDOW_TOPMOST = 4096, // Set to window always on top
    FLAG_WINDOW_ALWAYS_RUN = 256, // Set to allow windows running while minimized
    FLAG_WINDOW_TRANSPARENT = 16, // Set to allow transparent framebuffer
    FLAG_WINDOW_HIGHDPI = 8192, // Set to support HighDPI
    FLAG_WINDOW_MOUSE_PASSTHROUGH = 16384, // Set to support mouse passthrough, only supported when FLAG_WINDOW_UNDECORATED
    FLAG_BORDERLESS_WINDOWED_MODE = 32768, // Set to run program in borderless windowed mode
    FLAG_MSAA_4X_HINT = 32, // Set to try enabling MSAA 4X
    FLAG_INTERLACED_HINT = 65536, // Set to try enabling interlaced video format (for V3D)
} ConfigFlags;

// Trace log level
typedef enum {
    LOG_ALL = 0, // Display all logs
    LOG_TRACE = 1, // Trace logging, intended for internal use only
    LOG_DEBUG = 2, // Debug logging, used for internal debugging, it should be disabled on release builds
    LOG_INFO = 3, // Info logging, used for program execution info
    LOG_WARNING = 4, // Warning logging, used on recoverable failures
    LOG_ERROR = 5, // Error logging, used on unrecoverable failures
    LOG_FATAL = 6, // Fatal logging, used to abort program: exit(EXIT_FAILURE)
    LOG_NONE = 7, // Disable logging
} TraceLogLevel;

// Keyboard keys (US keyboard layout)
typedef enum {
    KEY_NULL = 0, // Key: NULL, used for no key pressed
    KEY_APOSTROPHE = 39, // Key: '
    KEY_COMMA = 44, // Key: ,
    KEY_MINUS = 45, // Key: -
    KEY_PERIOD = 46, // Key: .
    KEY_SLASH = 47, // Key: /
    KEY_ZERO = 48, // Key: 0
    KEY_ONE = 49, // Key: 1
    KEY_TWO = 50, // Key: 2
    KEY_THREE = 51, // Key: 3
    KEY_FOUR = 52, // Key: 4
    KEY_FIVE = 53, // Key: 5
    KEY_SIX = 54, // Key: 6
    KEY_SEVEN = 55, // Key: 7
    KEY_EIGHT = 56, // Key: 8
    KEY_NINE = 57, // Key: 9
    KEY_SEMICOLON = 59, // Key: ;
    KEY_EQUAL = 61, // Key: =
    KEY_A = 65, // Key: A | a
    KEY_B = 66, // Key: B | b
    KEY_C = 67, // Key: C | c
    KEY_D = 68, // Key: D | d
    KEY_E = 69, // Key: E | e
    KEY_F = 70, // Key: F | f
    KEY_G = 71, // Key: G | g
    KEY_H = 72, // Key: H | h
    KEY_I = 73, // Key: I | i
    KEY_J = 74, // Key: J | j
    KEY_K = 75, // Key: K | k
    KEY_L = 76, // Key: L | l
    KEY_M = 77, // Key: M | m
    KEY_N = 78, // Key: N | n
    KEY_O = 79, // Key: O | o
    KEY_P = 80, // Key: P | p
    KEY_Q = 81, // Key: Q | q
    KEY_R = 82, // Key: R | r
    KEY_S = 83, // Key: S | s
    KEY_T = 84, // Key: T | t
    KEY_U = 85, // Key: U | u
    KEY_V = 86, // Key: V | v
    KEY_W = 87, // Key: W | w
    KEY_X = 88, // Key: X | x
    KEY_Y = 89, // Key: Y | y
    KEY_Z = 90, // Key: Z | z
    KEY_LEFT_BRACKET = 91, // Key: [
    KEY_BACKSLASH = 92, // Key: '\'
    KEY_RIGHT_BRACKET = 93, // Key: ]
    KEY_GRAVE = 96, // Key: `
    KEY_SPACE = 32, // Key: Space
    KEY_ESCAPE = 256, // Key: Esc
    KEY_ENTER = 257, // Key: Enter
    KEY_TAB = 258, // Key: Tab
    KEY_BACKSPACE = 259, // Key: Backspace
    KEY_INSERT = 260, // Key: Ins
    KEY_DELETE = 261, // Key: Del
    KEY_RIGHT = 262, // Key: Cursor right
    KEY_LEFT = 263, // Key: Cursor left
    KEY_DOWN = 264, // Key: Cursor down
    KEY_UP = 265, // Key: Cursor up
    KEY_PAGE_UP = 266, // Key: Page up
    KEY_PAGE_DOWN = 267, // Key: Page down
    KEY_HOME = 268, // Key: Home
    KEY_END = 269, // Key: End
    KEY_CAPS_LOCK = 280, // Key: Caps lock
    KEY_SCROLL_LOCK = 281, // Key: Scroll down
    KEY_NUM_LOCK = 282, // Key: Num lock
    KEY_PRINT_SCREEN = 283, // Key: Print screen
    KEY_PAUSE = 284, // Key: Pause
    KEY_F1 = 290, // Key: F1
    KEY_F2 = 291, // Key: F2
    KEY_F3 = 292, // Key: F3
    KEY_F4 = 293, // Key: F4
    KEY_F5 = 294, // Key: F5
    KEY_F6 = 295, // Key: F6
    KEY_F7 = 296, // Key: F7
    KEY_F8 = 297, // Key: F8
    KEY_F9 = 298, // Key: F9
    KEY_F10 = 299, // Key: F10
    KEY_F11 = 300, // Key: F11
    KEY_F12 = 301, // Key: F12
    KEY_LEFT_SHIFT = 340, // Key: Shift left
    KEY_LEFT_CONTROL = 341, // Key: Control left
    KEY_LEFT_ALT = 342, // Key: Alt left
    KEY_LEFT_SUPER = 343, // Key: Super left
    KEY_RIGHT_SHIFT = 344, // Key: Shift right
    KEY_RIGHT_CONTROL = 345, // Key: Control right
    KEY_RIGHT_ALT = 346, // Key: Alt right
    KEY_RIGHT_SUPER = 347, // Key: Super right
    KEY_KB_MENU = 348, // Key: KB menu
    KEY_KP_0 = 320, // Key: Keypad 0
    KEY_KP_1 = 321, // Key: Keypad 1
    KEY_KP_2 = 322, // Key: Keypad 2
    KEY_KP_3 = 323, // Key: Keypad 3
    KEY_KP_4 = 324, // Key: Keypad 4
    KEY_KP_5 = 325, // Key: Keypad 5
    KEY_KP_6 = 326, // Key: Keypad 6
    KEY_KP_7 = 327, // Key: Keypad 7
    KEY_KP_8 = 328, // Key: Keypad 8
    KEY_KP_9 = 329, // Key: Keypad 9
    KEY_KP_DECIMAL = 330, // Key: Keypad .
    KEY_KP_DIVIDE = 331, // Key: Keypad /
    KEY_KP_MULTIPLY = 332, // Key: Keypad *
    KEY_KP_SUBTRACT = 333, // Key: Keypad -
    KEY_KP_ADD = 334, // Key: Keypad +
    KEY_KP_ENTER = 335, // Key: Keypad Enter
    KEY_KP_EQUAL = 336, // Key: Keypad =
    KEY_BACK = 4, // Key: Android back button
    KEY_MENU = 5, // Key: Android menu button
    KEY_VOLUME_UP = 24, // Key: Android volume up button
    KEY_VOLUME_DOWN = 25, // Key: Android volume down button
} KeyboardKey;

// Mouse buttons
typedef enum {
    MOUSE_BUTTON_LEFT = 0, // Mouse button left
    MOUSE_BUTTON_RIGHT = 1, // Mouse button right
    MOUSE_BUTTON_MIDDLE = 2, // Mouse button middle (pressed wheel)
    MOUSE_BUTTON_SIDE = 3, // Mouse button side (advanced mouse device)
    MOUSE_BUTTON_EXTRA = 4, // Mouse button extra (advanced mouse device)
    MOUSE_BUTTON_FORWARD = 5, // Mouse button forward (advanced mouse device)
    MOUSE_BUTTON_BACK = 6, // Mouse button back (advanced mouse device)
} MouseButton;

// Mouse cursor
typedef enum {
    MOUSE_CURSOR_DEFAULT = 0, // Default pointer shape
    MOUSE_CURSOR_ARROW = 1, // Arrow shape
    MOUSE_CURSOR_IBEAM = 2, // Text writing cursor shape
    MOUSE_CURSOR_CROSSHAIR = 3, // Cross shape
    MOUSE_CURSOR_POINTING_HAND = 4, // Pointing hand cursor
    MOUSE_CURSOR_RESIZE_EW = 5, // Horizontal resize/move arrow shape
    MOUSE_CURSOR_RESIZE_NS = 6, // Vertical resize/move arrow shape
    MOUSE_CURSOR_RESIZE_NWSE = 7, // Top-left to bottom-right diagonal resize/move arrow shape
    MOUSE_CURSOR_RESIZE_NESW = 8, // The top-right to bottom-left diagonal resize/move arrow shape
    MOUSE_CURSOR_RESIZE_ALL = 9, // The omnidirectional resize/move cursor shape
    MOUSE_CURSOR_NOT_ALLOWED = 10, // The operation-not-allowed shape
} MouseCursor;

// Gamepad buttons
typedef enum {
    GAMEPAD_BUTTON_UNKNOWN = 0, // Unknown button, just for error checking
    GAMEPAD_BUTTON_LEFT_FACE_UP = 1, // Gamepad left DPAD up button
    GAMEPAD_BUTTON_LEFT_FACE_RIGHT = 2, // Gamepad left DPAD right button
    GAMEPAD_BUTTON_LEFT_FACE_DOWN = 3, // Gamepad left DPAD down button
    GAMEPAD_BUTTON_LEFT_FACE_LEFT = 4, // Gamepad left DPAD left button
    GAMEPAD_BUTTON_RIGHT_FACE_UP = 5, // Gamepad right button up (i.e. PS3: Triangle, Xbox: Y)
    GAMEPAD_BUTTON_RIGHT_FACE_RIGHT = 6, // Gamepad right button right (i.e. PS3: Circle, Xbox: B)
    GAMEPAD_BUTTON_RIGHT_FACE_DOWN = 7, // Gamepad right button down (i.e. PS3: Cross, Xbox: A)
    GAMEPAD_BUTTON_RIGHT_FACE_LEFT = 8, // Gamepad right button left (i.e. PS3: Square, Xbox: X)
    GAMEPAD_BUTTON_LEFT_TRIGGER_1 = 9, // Gamepad top/back trigger left (first), it could be a trailing button
    GAMEPAD_BUTTON_LEFT_TRIGGER_2 = 10, // Gamepad top/back trigger left (second), it could be a trailing button
    GAMEPAD_BUTTON_RIGHT_TRIGGER_1 = 11, // Gamepad top/back trigger right (first), it could be a trailing button
    GAMEPAD_BUTTON_RIGHT_TRIGGER_2 = 12, // Gamepad top/back trigger right (second), it could be a trailing button
    GAMEPAD_BUTTON_MIDDLE_LEFT = 13, // Gamepad center buttons, left one (i.e. PS3: Select)
    GAMEPAD_BUTTON_MIDDLE = 14, // Gamepad center buttons, middle one (i.e. PS3: PS, Xbox: XBOX)
    GAMEPAD_BUTTON_MIDDLE_RIGHT = 15, // Gamepad center buttons, right one (i.e. PS3: Start)
    GAMEPAD_BUTTON_LEFT_THUMB = 16, // Gamepad joystick pressed button left
    GAMEPAD_BUTTON_RIGHT_THUMB = 17, // Gamepad joystick pressed button right
} GamepadButton;

// Gamepad axes
typedef enum {
    GAMEPAD_AXIS_LEFT_X = 0, // Gamepad left stick X axis
    GAMEPAD_AXIS_LEFT_Y = 1, // Gamepad left stick Y axis
    GAMEPAD_AXIS_RIGHT_X = 2, // Gamepad right stick X axis
    GAMEPAD_AXIS_RIGHT_Y = 3, // Gamepad right stick Y axis
    GAMEPAD_AXIS_LEFT_TRIGGER = 4, // Gamepad back trigger left, pressure level: [1..-1]
    GAMEPAD_AXIS_RIGHT_TRIGGER = 5, // Gamepad back trigger right, pressure level: [1..-1]
} GamepadAxis;

// Material map index
typedef enum {
    MATERIAL_MAP_ALBEDO = 0, // Albedo material (same as: MATERIAL_MAP_DIFFUSE)
    MATERIAL_MAP_METALNESS = 1, // Metalness material (same as: MATERIAL_MAP_SPECULAR)
    MATERIAL_MAP_NORMAL = 2, // Normal material
    MATERIAL_MAP_ROUGHNESS = 3, // Roughness material
    MATERIAL_MAP_OCCLUSION = 4, // Ambient occlusion material
    MATERIAL_MAP_EMISSION = 5, // Emission material
    MATERIAL_MAP_HEIGHT = 6, // Heightmap material
    MATERIAL_MAP_CUBEMAP = 7, // Cubemap material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    MATERIAL_MAP_IRRADIANCE = 8, // Irradiance material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    MATERIAL_MAP_PREFILTER = 9, // Prefilter material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    MATERIAL_MAP_BRDF = 10, // Brdf material
} MaterialMapIndex;

// Shader location index
typedef enum {
    SHADER_LOC_VERTEX_POSITION = 0, // Shader location: vertex attribute: position
    SHADER_LOC_VERTEX_TEXCOORD01 = 1, // Shader location: vertex attribute: texcoord01
    SHADER_LOC_VERTEX_TEXCOORD02 = 2, // Shader location: vertex attribute: texcoord02
    SHADER_LOC_VERTEX_NORMAL = 3, // Shader location: vertex attribute: normal
    SHADER_LOC_VERTEX_TANGENT = 4, // Shader location: vertex attribute: tangent
    SHADER_LOC_VERTEX_COLOR = 5, // Shader location: vertex attribute: color
    SHADER_LOC_MATRIX_MVP = 6, // Shader location: matrix uniform: model-view-projection
    SHADER_LOC_MATRIX_VIEW = 7, // Shader location: matrix uniform: view (camera transform)
    SHADER_LOC_MATRIX_PROJECTION = 8, // Shader location: matrix uniform: projection
    SHADER_LOC_MATRIX_MODEL = 9, // Shader location: matrix uniform: model (transform)
    SHADER_LOC_MATRIX_NORMAL = 10, // Shader location: matrix uniform: normal
    SHADER_LOC_VECTOR_VIEW = 11, // Shader location: vector uniform: view
    SHADER_LOC_COLOR_DIFFUSE = 12, // Shader location: vector uniform: diffuse color
    SHADER_LOC_COLOR_SPECULAR = 13, // Shader location: vector uniform: specular color
    SHADER_LOC_COLOR_AMBIENT = 14, // Shader location: vector uniform: ambient color
    SHADER_LOC_MAP_ALBEDO = 15, // Shader location: sampler2d texture: albedo (same as: SHADER_LOC_MAP_DIFFUSE)
    SHADER_LOC_MAP_METALNESS = 16, // Shader location: sampler2d texture: metalness (same as: SHADER_LOC_MAP_SPECULAR)
    SHADER_LOC_MAP_NORMAL = 17, // Shader location: sampler2d texture: normal
    SHADER_LOC_MAP_ROUGHNESS = 18, // Shader location: sampler2d texture: roughness
    SHADER_LOC_MAP_OCCLUSION = 19, // Shader location: sampler2d texture: occlusion
    SHADER_LOC_MAP_EMISSION = 20, // Shader location: sampler2d texture: emission
    SHADER_LOC_MAP_HEIGHT = 21, // Shader location: sampler2d texture: height
    SHADER_LOC_MAP_CUBEMAP = 22, // Shader location: samplerCube texture: cubemap
    SHADER_LOC_MAP_IRRADIANCE = 23, // Shader location: samplerCube texture: irradiance
    SHADER_LOC_MAP_PREFILTER = 24, // Shader location: samplerCube texture: prefilter
    SHADER_LOC_MAP_BRDF = 25, // Shader location: sampler2d texture: brdf
    SHADER_LOC_VERTEX_BONEIDS = 26, // Shader location: vertex attribute: boneIds
    SHADER_LOC_VERTEX_BONEWEIGHTS = 27, // Shader location: vertex attribute: boneWeights
    SHADER_LOC_BONE_MATRICES = 28, // Shader location: array of matrices uniform: boneMatrices
    SHADER_LOC_VERTEX_INSTANCE_TX = 29, // Shader location: vertex attribute: instanceTransform
} ShaderLocationIndex;

// Shader uniform data type
typedef enum {
    SHADER_UNIFORM_FLOAT = 0, // Shader uniform type: float
    SHADER_UNIFORM_VEC2 = 1, // Shader uniform type: vec2 (2 float)
    SHADER_UNIFORM_VEC3 = 2, // Shader uniform type: vec3 (3 float)
    SHADER_UNIFORM_VEC4 = 3, // Shader uniform type: vec4 (4 float)
    SHADER_UNIFORM_INT = 4, // Shader uniform type: int
    SHADER_UNIFORM_IVEC2 = 5, // Shader uniform type: ivec2 (2 int)
    SHADER_UNIFORM_IVEC3 = 6, // Shader uniform type: ivec3 (3 int)
    SHADER_UNIFORM_IVEC4 = 7, // Shader uniform type: ivec4 (4 int)
    SHADER_UNIFORM_UINT = 8, // Shader uniform type: unsigned int
    SHADER_UNIFORM_UIVEC2 = 9, // Shader uniform type: uivec2 (2 unsigned int)
    SHADER_UNIFORM_UIVEC3 = 10, // Shader uniform type: uivec3 (3 unsigned int)
    SHADER_UNIFORM_UIVEC4 = 11, // Shader uniform type: uivec4 (4 unsigned int)
    SHADER_UNIFORM_SAMPLER2D = 12, // Shader uniform type: sampler2d
} ShaderUniformDataType;

// Shader attribute data types
typedef enum {
    SHADER_ATTRIB_FLOAT = 0, // Shader attribute type: float
    SHADER_ATTRIB_VEC2 = 1, // Shader attribute type: vec2 (2 float)
    SHADER_ATTRIB_VEC3 = 2, // Shader attribute type: vec3 (3 float)
    SHADER_ATTRIB_VEC4 = 3, // Shader attribute type: vec4 (4 float)
} ShaderAttributeDataType;

// Pixel formats
typedef enum {
    PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1, // 8 bit per pixel (no alpha)
    PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA = 2, // 8*2 bpp (2 channels)
    PIXELFORMAT_UNCOMPRESSED_R5G6B5 = 3, // 16 bpp
    PIXELFORMAT_UNCOMPRESSED_R8G8B8 = 4, // 24 bpp
    PIXELFORMAT_UNCOMPRESSED_R5G5B5A1 = 5, // 16 bpp (1 bit alpha)
    PIXELFORMAT_UNCOMPRESSED_R4G4B4A4 = 6, // 16 bpp (4 bit alpha)
    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 = 7, // 32 bpp
    PIXELFORMAT_UNCOMPRESSED_R32 = 8, // 32 bpp (1 channel - float)
    PIXELFORMAT_UNCOMPRESSED_R32G32B32 = 9, // 32*3 bpp (3 channels - float)
    PIXELFORMAT_UNCOMPRESSED_R32G32B32A32 = 10, // 32*4 bpp (4 channels - float)
    PIXELFORMAT_UNCOMPRESSED_R16 = 11, // 16 bpp (1 channel - half float)
    PIXELFORMAT_UNCOMPRESSED_R16G16B16 = 12, // 16*3 bpp (3 channels - half float)
    PIXELFORMAT_UNCOMPRESSED_R16G16B16A16 = 13, // 16*4 bpp (4 channels - half float)
    PIXELFORMAT_COMPRESSED_DXT1_RGB = 14, // 4 bpp (no alpha)
    PIXELFORMAT_COMPRESSED_DXT1_RGBA = 15, // 4 bpp (1 bit alpha)
    PIXELFORMAT_COMPRESSED_DXT3_RGBA = 16, // 8 bpp
    PIXELFORMAT_COMPRESSED_DXT5_RGBA = 17, // 8 bpp
    PIXELFORMAT_COMPRESSED_ETC1_RGB = 18, // 4 bpp
    PIXELFORMAT_COMPRESSED_ETC2_RGB = 19, // 4 bpp
    PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA = 20, // 8 bpp
    PIXELFORMAT_COMPRESSED_PVRT_RGB = 21, // 4 bpp
    PIXELFORMAT_COMPRESSED_PVRT_RGBA = 22, // 4 bpp
    PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA = 23, // 8 bpp
    PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA = 24, // 2 bpp
} PixelFormat;

// Texture parameters: filter mode
typedef enum {
    TEXTURE_FILTER_POINT = 0, // No filter, just pixel approximation
    TEXTURE_FILTER_BILINEAR = 1, // Linear filtering
    TEXTURE_FILTER_TRILINEAR = 2, // Trilinear filtering (linear with mipmaps)
    TEXTURE_FILTER_ANISOTROPIC_4X = 3, // Anisotropic filtering 4x
    TEXTURE_FILTER_ANISOTROPIC_8X = 4, // Anisotropic filtering 8x
    TEXTURE_FILTER_ANISOTROPIC_16X = 5, // Anisotropic filtering 16x
} TextureFilter;

// Texture parameters: wrap mode
typedef enum {
    TEXTURE_WRAP_REPEAT = 0, // Repeats texture in tiled mode
    TEXTURE_WRAP_CLAMP = 1, // Clamps texture to edge pixel in tiled mode
    TEXTURE_WRAP_MIRROR_REPEAT = 2, // Mirrors and repeats the texture in tiled mode
    TEXTURE_WRAP_MIRROR_CLAMP = 3, // Mirrors and clamps to border the texture in tiled mode
} TextureWrap;

// Cubemap layouts
typedef enum {
    CUBEMAP_LAYOUT_AUTO_DETECT = 0, // Automatically detect layout type
    CUBEMAP_LAYOUT_LINE_VERTICAL = 1, // Layout is defined by a vertical line with faces
    CUBEMAP_LAYOUT_LINE_HORIZONTAL = 2, // Layout is defined by a horizontal line with faces
    CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR = 3, // Layout is defined by a 3x4 cross with cubemap faces
    CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE = 4, // Layout is defined by a 4x3 cross with cubemap faces
} CubemapLayout;

// Font type, defines generation method
typedef enum {
    FONT_DEFAULT = 0, // Default font generation, anti-aliased
    FONT_BITMAP = 1, // Bitmap font generation, no anti-aliasing
    FONT_SDF = 2, // SDF font generation, requires external shader
} FontType;

// Color blending modes (pre-defined)
typedef enum {
    BLEND_ALPHA = 0, // Blend textures considering alpha (default)
    BLEND_ADDITIVE = 1, // Blend textures adding colors
    BLEND_MULTIPLIED = 2, // Blend textures multiplying colors
    BLEND_ADD_COLORS = 3, // Blend textures adding colors (alternative)
    BLEND_SUBTRACT_COLORS = 4, // Blend textures subtracting colors (alternative)
    BLEND_ALPHA_PREMULTIPLY = 5, // Blend premultiplied textures considering alpha
    BLEND_CUSTOM = 6, // Blend textures using custom src/dst factors (use rlSetBlendFactors())
    BLEND_CUSTOM_SEPARATE = 7, // Blend textures using custom rgb/alpha separate src/dst factors (use rlSetBlendFactorsSeparate())
} BlendMode;

// Gesture
typedef enum {
    GESTURE_NONE = 0, // No gesture
    GESTURE_TAP = 1, // Tap gesture
    GESTURE_DOUBLETAP = 2, // Double tap gesture
    GESTURE_HOLD = 4, // Hold gesture
    GESTURE_DRAG = 8, // Drag gesture
    GESTURE_SWIPE_RIGHT = 16, // Swipe right gesture
    GESTURE_SWIPE_LEFT = 32, // Swipe left gesture
    GESTURE_SWIPE_UP = 64, // Swipe up gesture
    GESTURE_SWIPE_DOWN = 128, // Swipe down gesture
    GESTURE_PINCH_IN = 256, // Pinch in gesture
    GESTURE_PINCH_OUT = 512, // Pinch out gesture
} Gesture;

// Camera system modes
typedef enum {
    CAMERA_CUSTOM = 0, // Camera custom, controlled by user (UpdateCamera() does nothing)
    CAMERA_FREE = 1, // Camera free mode
    CAMERA_ORBITAL = 2, // Camera orbital, around target, zoom supported
    CAMERA_FIRST_PERSON = 3, // Camera first person
    CAMERA_THIRD_PERSON = 4, // Camera third person
} CameraMode;

// Camera projection
typedef enum {
    CAMERA_PERSPECTIVE = 0, // Perspective projection
    CAMERA_ORTHOGRAPHIC = 1, // Orthographic projection
} CameraProjection;

// N-patch layout
typedef enum {
    NPATCH_NINE_PATCH = 0, // Npatch layout: 3x3 tiles
    NPATCH_THREE_PATCH_VERTICAL = 1, // Npatch layout: 1x3 tiles
    NPATCH_THREE_PATCH_HORIZONTAL = 2, // Npatch layout: 3x1 tiles
} NPatchLayout;

//----------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------
#define RAYLIB_VERSION_MAJOR 5
#define RAYLIB_VERSION_MINOR 6
#define RAYLIB_VERSION_PATCH 0
#define RAYLIB_VERSION       5.6-dev
#define PI                   3.141592653589793
#define DEG2RAD              (PI/180.0f)
#define RAD2DEG              (180.0f/PI)
#define LIGHTGRAY            (Color){ 200, 200, 200, 255 }   // Light Gray
#define GRAY                 (Color){ 130, 130, 130, 255 }   // Gray
#define DARKGRAY             (Color){ 80, 80, 80, 255 }   // Dark Gray
#define YELLOW               (Color){ 253, 249, 0, 255 }   // Yellow
#define GOLD                 (Color){ 255, 203, 0, 255 }   // Gold
#define ORANGE               (Color){ 255, 161, 0, 255 }   // Orange
#define PINK                 (Color){ 255, 109, 194, 255 }   // Pink
#define RED                  (Color){ 230, 41, 55, 255 }   // Red
#define MAROON               (Color){ 190, 33, 55, 255 }   // Maroon
#define GREEN                (Color){ 0, 228, 48, 255 }   // Green
#define LIME                 (Color){ 0, 158, 47, 255 }   // Lime
#define DARKGREEN            (Color){ 0, 117, 44, 255 }   // Dark Green
#define SKYBLUE              (Color){ 102, 191, 255, 255 }   // Sky Blue
#define BLUE                 (Color){ 0, 121, 241, 255 }   // Blue
#define DARKBLUE             (Color){ 0, 82, 172, 255 }   // Dark Blue
#define PURPLE               (Color){ 200, 122, 255, 255 }   // Purple
#define VIOLET               (Color){ 135, 60, 190, 255 }   // Violet
#define DARKPURPLE           (Color){ 112, 31, 126, 255 }   // Dark Purple
#define BEIGE                (Color){ 211, 176, 131, 255 }   // Beige
#define BROWN                (Color){ 127, 106, 79, 255 }   // Brown
#define DARKBROWN            (Color){ 76, 63, 47, 255 }   // Dark Brown
#define WHITE                (Color){ 255, 255, 255, 255 }   // White
#define BLACK                (Color){ 0, 0, 0, 255 }   // Black
#define BLANK                (Color){ 0, 0, 0, 0 }   // Blank (Transparent)
#define MAGENTA              (Color){ 255, 0, 255, 255 }   // Magenta
#define RAYWHITE             (Color){ 245, 245, 245, 255 }   // My own White (raylib logo)

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
  fprintf(stderr, "\n");
}

//----------------------------------------------------------------------------------
// Raylib Function Imports (from host)
//----------------------------------------------------------------------------------
RC_IMPORT("InitWindow")
void InitWindow(int width, int height, const char * title);

RC_IMPORT("CloseWindow")
void CloseWindow(void);

RC_IMPORT("WindowShouldClose")
bool WindowShouldClose(void);

RC_IMPORT("IsWindowReady")
bool IsWindowReady(void);

RC_IMPORT("IsWindowFullscreen")
bool IsWindowFullscreen(void);

RC_IMPORT("IsWindowHidden")
bool IsWindowHidden(void);

RC_IMPORT("IsWindowMinimized")
bool IsWindowMinimized(void);

RC_IMPORT("IsWindowMaximized")
bool IsWindowMaximized(void);

RC_IMPORT("IsWindowFocused")
bool IsWindowFocused(void);

RC_IMPORT("IsWindowResized")
bool IsWindowResized(void);

RC_IMPORT("IsWindowState")
bool IsWindowState(unsigned int flag);

RC_IMPORT("SetWindowState")
void SetWindowState(unsigned int flags);

RC_IMPORT("ClearWindowState")
void ClearWindowState(unsigned int flags);

RC_IMPORT("ToggleFullscreen")
void ToggleFullscreen(void);

RC_IMPORT("ToggleBorderlessWindowed")
void ToggleBorderlessWindowed(void);

RC_IMPORT("MaximizeWindow")
void MaximizeWindow(void);

RC_IMPORT("MinimizeWindow")
void MinimizeWindow(void);

RC_IMPORT("RestoreWindow")
void RestoreWindow(void);

RC_IMPORT("SetWindowIcon")
void SetWindowIcon(Image image);

RC_IMPORT("SetWindowIcons")
void SetWindowIcons(Image * images, int count);

RC_IMPORT("SetWindowTitle")
void SetWindowTitle(const char * title);

RC_IMPORT("SetWindowPosition")
void SetWindowPosition(int x, int y);

RC_IMPORT("SetWindowMonitor")
void SetWindowMonitor(int monitor);

RC_IMPORT("SetWindowMinSize")
void SetWindowMinSize(int width, int height);

RC_IMPORT("SetWindowMaxSize")
void SetWindowMaxSize(int width, int height);

RC_IMPORT("SetWindowSize")
void SetWindowSize(int width, int height);

RC_IMPORT("SetWindowOpacity")
void SetWindowOpacity(float opacity);

RC_IMPORT("SetWindowFocused")
void SetWindowFocused(void);

RC_IMPORT("GetWindowHandle")
void * GetWindowHandle(void);

RC_IMPORT("GetScreenWidth")
int GetScreenWidth(void);

RC_IMPORT("GetScreenHeight")
int GetScreenHeight(void);

RC_IMPORT("GetRenderWidth")
int GetRenderWidth(void);

RC_IMPORT("GetRenderHeight")
int GetRenderHeight(void);

RC_IMPORT("GetMonitorCount")
int GetMonitorCount(void);

RC_IMPORT("GetCurrentMonitor")
int GetCurrentMonitor(void);

RC_IMPORT("GetMonitorPosition")
Vector2 GetMonitorPosition(int monitor);

RC_IMPORT("GetMonitorWidth")
int GetMonitorWidth(int monitor);

RC_IMPORT("GetMonitorHeight")
int GetMonitorHeight(int monitor);

RC_IMPORT("GetMonitorPhysicalWidth")
int GetMonitorPhysicalWidth(int monitor);

RC_IMPORT("GetMonitorPhysicalHeight")
int GetMonitorPhysicalHeight(int monitor);

RC_IMPORT("GetMonitorRefreshRate")
int GetMonitorRefreshRate(int monitor);

RC_IMPORT("GetWindowPosition")
Vector2 GetWindowPosition(void);

RC_IMPORT("GetWindowScaleDPI")
Vector2 GetWindowScaleDPI(void);

RC_IMPORT("GetMonitorName")
const char * GetMonitorName(int monitor);

RC_IMPORT("SetClipboardText")
void SetClipboardText(const char * text);

RC_IMPORT("GetClipboardText")
const char * GetClipboardText(void);

RC_IMPORT("EnableEventWaiting")
void EnableEventWaiting(void);

RC_IMPORT("DisableEventWaiting")
void DisableEventWaiting(void);

RC_IMPORT("ShowCursor")
void ShowCursor(void);

RC_IMPORT("HideCursor")
void HideCursor(void);

RC_IMPORT("IsCursorHidden")
bool IsCursorHidden(void);

RC_IMPORT("EnableCursor")
void EnableCursor(void);

RC_IMPORT("DisableCursor")
void DisableCursor(void);

RC_IMPORT("IsCursorOnScreen")
bool IsCursorOnScreen(void);

RC_IMPORT("ClearBackground")
void ClearBackground(Color color);

RC_IMPORT("BeginDrawing")
void BeginDrawing(void);

RC_IMPORT("EndDrawing")
void EndDrawing(void);

RC_IMPORT("BeginMode2D")
void BeginMode2D(Camera2D camera);

RC_IMPORT("EndMode2D")
void EndMode2D(void);

RC_IMPORT("BeginMode3D")
void BeginMode3D(Camera3D camera);

RC_IMPORT("EndMode3D")
void EndMode3D(void);

RC_IMPORT("BeginTextureMode")
void BeginTextureMode(RenderTexture2D target);

RC_IMPORT("EndTextureMode")
void EndTextureMode(void);

RC_IMPORT("BeginShaderMode")
void BeginShaderMode(Shader shader);

RC_IMPORT("EndShaderMode")
void EndShaderMode(void);

RC_IMPORT("BeginBlendMode")
void BeginBlendMode(int mode);

RC_IMPORT("EndBlendMode")
void EndBlendMode(void);

RC_IMPORT("BeginScissorMode")
void BeginScissorMode(int x, int y, int width, int height);

RC_IMPORT("EndScissorMode")
void EndScissorMode(void);

RC_IMPORT("BeginVrStereoMode")
void BeginVrStereoMode(VrStereoConfig config);

RC_IMPORT("EndVrStereoMode")
void EndVrStereoMode(void);

RC_IMPORT("LoadVrStereoConfig")
VrStereoConfig LoadVrStereoConfig(VrDeviceInfo device);

RC_IMPORT("UnloadVrStereoConfig")
void UnloadVrStereoConfig(VrStereoConfig config);

RC_IMPORT("LoadShader")
Shader LoadShader(const char * vsFileName, const char * fsFileName);

RC_IMPORT("LoadShaderFromMemory")
Shader LoadShaderFromMemory(const char * vsCode, const char * fsCode);

RC_IMPORT("IsShaderValid")
bool IsShaderValid(Shader shader);

RC_IMPORT("GetShaderLocation")
int GetShaderLocation(Shader shader, const char * uniformName);

RC_IMPORT("GetShaderLocationAttrib")
int GetShaderLocationAttrib(Shader shader, const char * attribName);

RC_IMPORT("SetShaderValue")
void SetShaderValue(Shader shader, int locIndex, const void * value, int uniformType);

RC_IMPORT("SetShaderValueV")
void SetShaderValueV(Shader shader, int locIndex, const void * value, int uniformType, int count);

RC_IMPORT("SetShaderValueMatrix")
void SetShaderValueMatrix(Shader shader, int locIndex, Matrix mat);

RC_IMPORT("SetShaderValueTexture")
void SetShaderValueTexture(Shader shader, int locIndex, Texture2D texture);

RC_IMPORT("UnloadShader")
void UnloadShader(Shader shader);

RC_IMPORT("GetScreenToWorldRay")
Ray GetScreenToWorldRay(Vector2 position, Camera camera);

RC_IMPORT("GetScreenToWorldRayEx")
Ray GetScreenToWorldRayEx(Vector2 position, Camera camera, int width, int height);

RC_IMPORT("GetWorldToScreen")
Vector2 GetWorldToScreen(Vector3 position, Camera camera);

RC_IMPORT("GetWorldToScreenEx")
Vector2 GetWorldToScreenEx(Vector3 position, Camera camera, int width, int height);

RC_IMPORT("GetWorldToScreen2D")
Vector2 GetWorldToScreen2D(Vector2 position, Camera2D camera);

RC_IMPORT("GetScreenToWorld2D")
Vector2 GetScreenToWorld2D(Vector2 position, Camera2D camera);

RC_IMPORT("GetCameraMatrix")
Matrix GetCameraMatrix(Camera camera);

RC_IMPORT("GetCameraMatrix2D")
Matrix GetCameraMatrix2D(Camera2D camera);

RC_IMPORT("SetTargetFPS")
void SetTargetFPS(int fps);

RC_IMPORT("GetFrameTime")
float GetFrameTime(void);

RC_IMPORT("GetTime")
double GetTime(void);

RC_IMPORT("GetFPS")
int GetFPS(void);

RC_IMPORT("SwapScreenBuffer")
void SwapScreenBuffer(void);

RC_IMPORT("PollInputEvents")
void PollInputEvents(void);

RC_IMPORT("WaitTime")
void WaitTime(double seconds);

RC_IMPORT("SetRandomSeed")
void SetRandomSeed(unsigned int seed);

RC_IMPORT("GetRandomValue")
int GetRandomValue(int min, int max);

RC_IMPORT("LoadRandomSequence")
int * LoadRandomSequence(unsigned int count, int min, int max);

RC_IMPORT("UnloadRandomSequence")
void UnloadRandomSequence(int * sequence);

RC_IMPORT("TakeScreenshot")
void TakeScreenshot(const char * fileName);

RC_IMPORT("SetConfigFlags")
void SetConfigFlags(unsigned int flags);

RC_IMPORT("OpenURL")
void OpenURL(const char * url);

RC_IMPORT("SetTraceLogLevel")
void SetTraceLogLevel(int logLevel);

RC_IMPORT("LoadFileData")
unsigned char * LoadFileData(const char * fileName, int * dataSize);

RC_IMPORT("UnloadFileData")
void UnloadFileData(unsigned char * data);

RC_IMPORT("SaveFileData")
bool SaveFileData(const char * fileName, void * data, int dataSize);

RC_IMPORT("ExportDataAsCode")
bool ExportDataAsCode(const unsigned char * data, int dataSize, const char * fileName);

RC_IMPORT("LoadFileText")
char * LoadFileText(const char * fileName);

RC_IMPORT("UnloadFileText")
void UnloadFileText(char * text);

RC_IMPORT("SaveFileText")
bool SaveFileText(const char * fileName, const char * text);

RC_IMPORT("FileExists")
bool FileExists(const char * fileName);

RC_IMPORT("DirectoryExists")
bool DirectoryExists(const char * dirPath);

RC_IMPORT("IsFileExtension")
bool IsFileExtension(const char * fileName, const char * ext);

RC_IMPORT("GetFileLength")
int GetFileLength(const char * fileName);

RC_IMPORT("GetFileModTime")
long GetFileModTime(const char * fileName);

RC_IMPORT("GetFileExtension")
const char * GetFileExtension(const char * fileName);

RC_IMPORT("GetFileName")
const char * GetFileName(const char * filePath);

RC_IMPORT("GetFileNameWithoutExt")
const char * GetFileNameWithoutExt(const char * filePath);

RC_IMPORT("GetDirectoryPath")
const char * GetDirectoryPath(const char * filePath);

RC_IMPORT("GetPrevDirectoryPath")
const char * GetPrevDirectoryPath(const char * dirPath);

RC_IMPORT("GetWorkingDirectory")
const char * GetWorkingDirectory(void);

RC_IMPORT("GetApplicationDirectory")
const char * GetApplicationDirectory(void);

RC_IMPORT("MakeDirectory")
int MakeDirectory(const char * dirPath);

RC_IMPORT("ChangeDirectory")
bool ChangeDirectory(const char * dirPath);

RC_IMPORT("IsPathFile")
bool IsPathFile(const char * path);

RC_IMPORT("IsFileNameValid")
bool IsFileNameValid(const char * fileName);

RC_IMPORT("LoadDirectoryFiles")
FilePathList LoadDirectoryFiles(const char * dirPath);

RC_IMPORT("LoadDirectoryFilesEx")
FilePathList LoadDirectoryFilesEx(const char * basePath, const char * filter, bool scanSubdirs);

RC_IMPORT("UnloadDirectoryFiles")
void UnloadDirectoryFiles(FilePathList files);

RC_IMPORT("IsFileDropped")
bool IsFileDropped(void);

RC_IMPORT("LoadDroppedFiles")
FilePathList LoadDroppedFiles(void);

RC_IMPORT("UnloadDroppedFiles")
void UnloadDroppedFiles(FilePathList files);

RC_IMPORT("CompressData")
unsigned char * CompressData(const unsigned char * data, int dataSize, int * compDataSize);

RC_IMPORT("DecompressData")
unsigned char * DecompressData(const unsigned char * compData, int compDataSize, int * dataSize);

RC_IMPORT("EncodeDataBase64")
char * EncodeDataBase64(const unsigned char * data, int dataSize, int * outputSize);

RC_IMPORT("DecodeDataBase64")
unsigned char * DecodeDataBase64(const char * text, int * outputSize);

RC_IMPORT("ComputeCRC32")
unsigned int ComputeCRC32(unsigned char * data, int dataSize);

RC_IMPORT("ComputeMD5")
unsigned int * ComputeMD5(unsigned char * data, int dataSize);

RC_IMPORT("ComputeSHA1")
unsigned int * ComputeSHA1(unsigned char * data, int dataSize);

RC_IMPORT("LoadAutomationEventList")
AutomationEventList LoadAutomationEventList(const char * fileName);

RC_IMPORT("UnloadAutomationEventList")
void UnloadAutomationEventList(AutomationEventList list);

RC_IMPORT("ExportAutomationEventList")
bool ExportAutomationEventList(AutomationEventList list, const char * fileName);

RC_IMPORT("SetAutomationEventList")
void SetAutomationEventList(AutomationEventList * list);

RC_IMPORT("SetAutomationEventBaseFrame")
void SetAutomationEventBaseFrame(int frame);

RC_IMPORT("StartAutomationEventRecording")
void StartAutomationEventRecording(void);

RC_IMPORT("StopAutomationEventRecording")
void StopAutomationEventRecording(void);

RC_IMPORT("PlayAutomationEvent")
void PlayAutomationEvent(AutomationEvent event);

RC_IMPORT("IsKeyPressed")
bool IsKeyPressed(int key);

RC_IMPORT("IsKeyPressedRepeat")
bool IsKeyPressedRepeat(int key);

RC_IMPORT("IsKeyDown")
bool IsKeyDown(int key);

RC_IMPORT("IsKeyReleased")
bool IsKeyReleased(int key);

RC_IMPORT("IsKeyUp")
bool IsKeyUp(int key);

RC_IMPORT("GetKeyPressed")
int GetKeyPressed(void);

RC_IMPORT("GetCharPressed")
int GetCharPressed(void);

RC_IMPORT("GetKeyName")
const char * GetKeyName(int key);

RC_IMPORT("SetExitKey")
void SetExitKey(int key);

RC_IMPORT("IsGamepadAvailable")
bool IsGamepadAvailable(int gamepad);

RC_IMPORT("GetGamepadName")
const char * GetGamepadName(int gamepad);

RC_IMPORT("IsGamepadButtonPressed")
bool IsGamepadButtonPressed(int gamepad, int button);

RC_IMPORT("IsGamepadButtonDown")
bool IsGamepadButtonDown(int gamepad, int button);

RC_IMPORT("IsGamepadButtonReleased")
bool IsGamepadButtonReleased(int gamepad, int button);

RC_IMPORT("IsGamepadButtonUp")
bool IsGamepadButtonUp(int gamepad, int button);

RC_IMPORT("GetGamepadButtonPressed")
int GetGamepadButtonPressed(void);

RC_IMPORT("GetGamepadAxisCount")
int GetGamepadAxisCount(int gamepad);

RC_IMPORT("GetGamepadAxisMovement")
float GetGamepadAxisMovement(int gamepad, int axis);

RC_IMPORT("SetGamepadMappings")
int SetGamepadMappings(const char * mappings);

RC_IMPORT("SetGamepadVibration")
void SetGamepadVibration(int gamepad, float leftMotor, float rightMotor, float duration);

RC_IMPORT("IsMouseButtonPressed")
bool IsMouseButtonPressed(int button);

RC_IMPORT("IsMouseButtonDown")
bool IsMouseButtonDown(int button);

RC_IMPORT("IsMouseButtonReleased")
bool IsMouseButtonReleased(int button);

RC_IMPORT("IsMouseButtonUp")
bool IsMouseButtonUp(int button);

RC_IMPORT("GetMouseX")
int GetMouseX(void);

RC_IMPORT("GetMouseY")
int GetMouseY(void);

RC_IMPORT("GetMousePosition")
Vector2 GetMousePosition(void);

RC_IMPORT("GetMouseDelta")
Vector2 GetMouseDelta(void);

RC_IMPORT("SetMousePosition")
void SetMousePosition(int x, int y);

RC_IMPORT("SetMouseOffset")
void SetMouseOffset(int offsetX, int offsetY);

RC_IMPORT("SetMouseScale")
void SetMouseScale(float scaleX, float scaleY);

RC_IMPORT("GetMouseWheelMove")
float GetMouseWheelMove(void);

RC_IMPORT("GetMouseWheelMoveV")
Vector2 GetMouseWheelMoveV(void);

RC_IMPORT("SetMouseCursor")
void SetMouseCursor(int cursor);

RC_IMPORT("GetTouchX")
int GetTouchX(void);

RC_IMPORT("GetTouchY")
int GetTouchY(void);

RC_IMPORT("GetTouchPosition")
Vector2 GetTouchPosition(int index);

RC_IMPORT("GetTouchPointId")
int GetTouchPointId(int index);

RC_IMPORT("GetTouchPointCount")
int GetTouchPointCount(void);

RC_IMPORT("SetGesturesEnabled")
void SetGesturesEnabled(unsigned int flags);

RC_IMPORT("IsGestureDetected")
bool IsGestureDetected(unsigned int gesture);

RC_IMPORT("GetGestureDetected")
int GetGestureDetected(void);

RC_IMPORT("GetGestureHoldDuration")
float GetGestureHoldDuration(void);

RC_IMPORT("GetGestureDragVector")
Vector2 GetGestureDragVector(void);

RC_IMPORT("GetGestureDragAngle")
float GetGestureDragAngle(void);

RC_IMPORT("GetGesturePinchVector")
Vector2 GetGesturePinchVector(void);

RC_IMPORT("GetGesturePinchAngle")
float GetGesturePinchAngle(void);

RC_IMPORT("UpdateCamera")
void UpdateCamera(Camera * camera, int mode);

RC_IMPORT("UpdateCameraPro")
void UpdateCameraPro(Camera * camera, Vector3 movement, Vector3 rotation, float zoom);

RC_IMPORT("SetShapesTexture")
void SetShapesTexture(Texture2D texture, Rectangle source);

RC_IMPORT("GetShapesTexture")
Texture2D GetShapesTexture(void);

RC_IMPORT("GetShapesTextureRectangle")
Rectangle GetShapesTextureRectangle(void);

RC_IMPORT("DrawPixel")
void DrawPixel(int posX, int posY, Color color);

RC_IMPORT("DrawPixelV")
void DrawPixelV(Vector2 position, Color color);

RC_IMPORT("DrawLine")
void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color);

RC_IMPORT("DrawLineV")
void DrawLineV(Vector2 startPos, Vector2 endPos, Color color);

RC_IMPORT("DrawLineEx")
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color);

RC_IMPORT("DrawLineStrip")
void DrawLineStrip(const Vector2 * points, int pointCount, Color color);

RC_IMPORT("DrawLineBezier")
void DrawLineBezier(Vector2 startPos, Vector2 endPos, float thick, Color color);

RC_IMPORT("DrawCircle")
void DrawCircle(int centerX, int centerY, float radius, Color color);

RC_IMPORT("DrawCircleSector")
void DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color);

RC_IMPORT("DrawCircleSectorLines")
void DrawCircleSectorLines(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color);

RC_IMPORT("DrawCircleGradient")
void DrawCircleGradient(int centerX, int centerY, float radius, Color inner, Color outer);

RC_IMPORT("DrawCircleV")
void DrawCircleV(Vector2 center, float radius, Color color);

RC_IMPORT("DrawCircleLines")
void DrawCircleLines(int centerX, int centerY, float radius, Color color);

RC_IMPORT("DrawCircleLinesV")
void DrawCircleLinesV(Vector2 center, float radius, Color color);

RC_IMPORT("DrawEllipse")
void DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, Color color);

RC_IMPORT("DrawEllipseLines")
void DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color);

RC_IMPORT("DrawRing")
void DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color);

RC_IMPORT("DrawRingLines")
void DrawRingLines(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color);

RC_IMPORT("DrawRectangle")
void DrawRectangle(int posX, int posY, int width, int height, Color color);

RC_IMPORT("DrawRectangleV")
void DrawRectangleV(Vector2 position, Vector2 size, Color color);

RC_IMPORT("DrawRectangleRec")
void DrawRectangleRec(Rectangle rec, Color color);

RC_IMPORT("DrawRectanglePro")
void DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color);

RC_IMPORT("DrawRectangleGradientV")
void DrawRectangleGradientV(int posX, int posY, int width, int height, Color top, Color bottom);

RC_IMPORT("DrawRectangleGradientH")
void DrawRectangleGradientH(int posX, int posY, int width, int height, Color left, Color right);

RC_IMPORT("DrawRectangleGradientEx")
void DrawRectangleGradientEx(Rectangle rec, Color topLeft, Color bottomLeft, Color bottomRight, Color topRight);

RC_IMPORT("DrawRectangleLines")
void DrawRectangleLines(int posX, int posY, int width, int height, Color color);

RC_IMPORT("DrawRectangleLinesEx")
void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color);

RC_IMPORT("DrawRectangleRounded")
void DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color);

RC_IMPORT("DrawRectangleRoundedLines")
void DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, Color color);

RC_IMPORT("DrawRectangleRoundedLinesEx")
void DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, float lineThick, Color color);

RC_IMPORT("DrawTriangle")
void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);

RC_IMPORT("DrawTriangleLines")
void DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color);

RC_IMPORT("DrawTriangleFan")
void DrawTriangleFan(const Vector2 * points, int pointCount, Color color);

RC_IMPORT("DrawTriangleStrip")
void DrawTriangleStrip(const Vector2 * points, int pointCount, Color color);

RC_IMPORT("DrawPoly")
void DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color);

RC_IMPORT("DrawPolyLines")
void DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color);

RC_IMPORT("DrawPolyLinesEx")
void DrawPolyLinesEx(Vector2 center, int sides, float radius, float rotation, float lineThick, Color color);

RC_IMPORT("DrawSplineLinear")
void DrawSplineLinear(const Vector2 * points, int pointCount, float thick, Color color);

RC_IMPORT("DrawSplineBasis")
void DrawSplineBasis(const Vector2 * points, int pointCount, float thick, Color color);

RC_IMPORT("DrawSplineCatmullRom")
void DrawSplineCatmullRom(const Vector2 * points, int pointCount, float thick, Color color);

RC_IMPORT("DrawSplineBezierQuadratic")
void DrawSplineBezierQuadratic(const Vector2 * points, int pointCount, float thick, Color color);

RC_IMPORT("DrawSplineBezierCubic")
void DrawSplineBezierCubic(const Vector2 * points, int pointCount, float thick, Color color);

RC_IMPORT("DrawSplineSegmentLinear")
void DrawSplineSegmentLinear(Vector2 p1, Vector2 p2, float thick, Color color);

RC_IMPORT("DrawSplineSegmentBasis")
void DrawSplineSegmentBasis(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float thick, Color color);

RC_IMPORT("DrawSplineSegmentCatmullRom")
void DrawSplineSegmentCatmullRom(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float thick, Color color);

RC_IMPORT("DrawSplineSegmentBezierQuadratic")
void DrawSplineSegmentBezierQuadratic(Vector2 p1, Vector2 c2, Vector2 p3, float thick, Color color);

RC_IMPORT("DrawSplineSegmentBezierCubic")
void DrawSplineSegmentBezierCubic(Vector2 p1, Vector2 c2, Vector2 c3, Vector2 p4, float thick, Color color);

RC_IMPORT("GetSplinePointLinear")
Vector2 GetSplinePointLinear(Vector2 startPos, Vector2 endPos, float t);

RC_IMPORT("GetSplinePointBasis")
Vector2 GetSplinePointBasis(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float t);

RC_IMPORT("GetSplinePointCatmullRom")
Vector2 GetSplinePointCatmullRom(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float t);

RC_IMPORT("GetSplinePointBezierQuad")
Vector2 GetSplinePointBezierQuad(Vector2 p1, Vector2 c2, Vector2 p3, float t);

RC_IMPORT("GetSplinePointBezierCubic")
Vector2 GetSplinePointBezierCubic(Vector2 p1, Vector2 c2, Vector2 c3, Vector2 p4, float t);

RC_IMPORT("CheckCollisionRecs")
bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2);

RC_IMPORT("CheckCollisionCircles")
bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2);

RC_IMPORT("CheckCollisionCircleRec")
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec);

RC_IMPORT("CheckCollisionCircleLine")
bool CheckCollisionCircleLine(Vector2 center, float radius, Vector2 p1, Vector2 p2);

RC_IMPORT("CheckCollisionPointRec")
bool CheckCollisionPointRec(Vector2 point, Rectangle rec);

RC_IMPORT("CheckCollisionPointCircle")
bool CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius);

RC_IMPORT("CheckCollisionPointTriangle")
bool CheckCollisionPointTriangle(Vector2 point, Vector2 p1, Vector2 p2, Vector2 p3);

RC_IMPORT("CheckCollisionPointLine")
bool CheckCollisionPointLine(Vector2 point, Vector2 p1, Vector2 p2, int threshold);

RC_IMPORT("CheckCollisionPointPoly")
bool CheckCollisionPointPoly(Vector2 point, const Vector2 * points, int pointCount);

RC_IMPORT("CheckCollisionLines")
bool CheckCollisionLines(Vector2 startPos1, Vector2 endPos1, Vector2 startPos2, Vector2 endPos2, Vector2 * collisionPoint);

RC_IMPORT("GetCollisionRec")
Rectangle GetCollisionRec(Rectangle rec1, Rectangle rec2);

RC_IMPORT("LoadImage")
Image LoadImage(const char * fileName);

RC_IMPORT("LoadImageRaw")
Image LoadImageRaw(const char * fileName, int width, int height, int format, int headerSize);

RC_IMPORT("LoadImageAnim")
Image LoadImageAnim(const char * fileName, int * frames);

RC_IMPORT("LoadImageAnimFromMemory")
Image LoadImageAnimFromMemory(const char * fileType, const unsigned char * fileData, int dataSize, int * frames);

RC_IMPORT("LoadImageFromMemory")
Image LoadImageFromMemory(const char * fileType, const unsigned char * fileData, int dataSize);

RC_IMPORT("LoadImageFromTexture")
Image LoadImageFromTexture(Texture2D texture);

RC_IMPORT("LoadImageFromScreen")
Image LoadImageFromScreen(void);

RC_IMPORT("IsImageValid")
bool IsImageValid(Image image);

RC_IMPORT("UnloadImage")
void UnloadImage(Image image);

RC_IMPORT("ExportImage")
bool ExportImage(Image image, const char * fileName);

RC_IMPORT("ExportImageToMemory")
unsigned char * ExportImageToMemory(Image image, const char * fileType, int * fileSize);

RC_IMPORT("ExportImageAsCode")
bool ExportImageAsCode(Image image, const char * fileName);

RC_IMPORT("GenImageColor")
Image GenImageColor(int width, int height, Color color);

RC_IMPORT("GenImageGradientLinear")
Image GenImageGradientLinear(int width, int height, int direction, Color start, Color end);

RC_IMPORT("GenImageGradientRadial")
Image GenImageGradientRadial(int width, int height, float density, Color inner, Color outer);

RC_IMPORT("GenImageGradientSquare")
Image GenImageGradientSquare(int width, int height, float density, Color inner, Color outer);

RC_IMPORT("GenImageChecked")
Image GenImageChecked(int width, int height, int checksX, int checksY, Color col1, Color col2);

RC_IMPORT("GenImageWhiteNoise")
Image GenImageWhiteNoise(int width, int height, float factor);

RC_IMPORT("GenImagePerlinNoise")
Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);

RC_IMPORT("GenImageCellular")
Image GenImageCellular(int width, int height, int tileSize);

RC_IMPORT("GenImageText")
Image GenImageText(int width, int height, const char * text);

RC_IMPORT("ImageCopy")
Image ImageCopy(Image image);

RC_IMPORT("ImageFromImage")
Image ImageFromImage(Image image, Rectangle rec);

RC_IMPORT("ImageFromChannel")
Image ImageFromChannel(Image image, int selectedChannel);

RC_IMPORT("ImageText")
Image ImageText(const char * text, int fontSize, Color color);

RC_IMPORT("ImageTextEx")
Image ImageTextEx(Font font, const char * text, float fontSize, float spacing, Color tint);

RC_IMPORT("ImageFormat")
void ImageFormat(Image * image, int newFormat);

RC_IMPORT("ImageToPOT")
void ImageToPOT(Image * image, Color fill);

RC_IMPORT("ImageCrop")
void ImageCrop(Image * image, Rectangle crop);

RC_IMPORT("ImageAlphaCrop")
void ImageAlphaCrop(Image * image, float threshold);

RC_IMPORT("ImageAlphaClear")
void ImageAlphaClear(Image * image, Color color, float threshold);

RC_IMPORT("ImageAlphaMask")
void ImageAlphaMask(Image * image, Image alphaMask);

RC_IMPORT("ImageAlphaPremultiply")
void ImageAlphaPremultiply(Image * image);

RC_IMPORT("ImageBlurGaussian")
void ImageBlurGaussian(Image * image, int blurSize);

RC_IMPORT("ImageKernelConvolution")
void ImageKernelConvolution(Image * image, const float * kernel, int kernelSize);

RC_IMPORT("ImageResize")
void ImageResize(Image * image, int newWidth, int newHeight);

RC_IMPORT("ImageResizeNN")
void ImageResizeNN(Image * image, int newWidth, int newHeight);

RC_IMPORT("ImageResizeCanvas")
void ImageResizeCanvas(Image * image, int newWidth, int newHeight, int offsetX, int offsetY, Color fill);

RC_IMPORT("ImageMipmaps")
void ImageMipmaps(Image * image);

RC_IMPORT("ImageDither")
void ImageDither(Image * image, int rBpp, int gBpp, int bBpp, int aBpp);

RC_IMPORT("ImageFlipVertical")
void ImageFlipVertical(Image * image);

RC_IMPORT("ImageFlipHorizontal")
void ImageFlipHorizontal(Image * image);

RC_IMPORT("ImageRotate")
void ImageRotate(Image * image, int degrees);

RC_IMPORT("ImageRotateCW")
void ImageRotateCW(Image * image);

RC_IMPORT("ImageRotateCCW")
void ImageRotateCCW(Image * image);

RC_IMPORT("ImageColorTint")
void ImageColorTint(Image * image, Color color);

RC_IMPORT("ImageColorInvert")
void ImageColorInvert(Image * image);

RC_IMPORT("ImageColorGrayscale")
void ImageColorGrayscale(Image * image);

RC_IMPORT("ImageColorContrast")
void ImageColorContrast(Image * image, float contrast);

RC_IMPORT("ImageColorBrightness")
void ImageColorBrightness(Image * image, int brightness);

RC_IMPORT("ImageColorReplace")
void ImageColorReplace(Image * image, Color color, Color replace);

RC_IMPORT("LoadImageColors")
Color * LoadImageColors(Image image);

RC_IMPORT("LoadImagePalette")
Color * LoadImagePalette(Image image, int maxPaletteSize, int * colorCount);

RC_IMPORT("UnloadImageColors")
void UnloadImageColors(Color * colors);

RC_IMPORT("UnloadImagePalette")
void UnloadImagePalette(Color * colors);

RC_IMPORT("GetImageAlphaBorder")
Rectangle GetImageAlphaBorder(Image image, float threshold);

RC_IMPORT("GetImageColor")
Color GetImageColor(Image image, int x, int y);

RC_IMPORT("ImageClearBackground")
void ImageClearBackground(Image * dst, Color color);

RC_IMPORT("ImageDrawPixel")
void ImageDrawPixel(Image * dst, int posX, int posY, Color color);

RC_IMPORT("ImageDrawPixelV")
void ImageDrawPixelV(Image * dst, Vector2 position, Color color);

RC_IMPORT("ImageDrawLine")
void ImageDrawLine(Image * dst, int startPosX, int startPosY, int endPosX, int endPosY, Color color);

RC_IMPORT("ImageDrawLineV")
void ImageDrawLineV(Image * dst, Vector2 start, Vector2 end, Color color);

RC_IMPORT("ImageDrawLineEx")
void ImageDrawLineEx(Image * dst, Vector2 start, Vector2 end, int thick, Color color);

RC_IMPORT("ImageDrawCircle")
void ImageDrawCircle(Image * dst, int centerX, int centerY, int radius, Color color);

RC_IMPORT("ImageDrawCircleV")
void ImageDrawCircleV(Image * dst, Vector2 center, int radius, Color color);

RC_IMPORT("ImageDrawCircleLines")
void ImageDrawCircleLines(Image * dst, int centerX, int centerY, int radius, Color color);

RC_IMPORT("ImageDrawCircleLinesV")
void ImageDrawCircleLinesV(Image * dst, Vector2 center, int radius, Color color);

RC_IMPORT("ImageDrawRectangle")
void ImageDrawRectangle(Image * dst, int posX, int posY, int width, int height, Color color);

RC_IMPORT("ImageDrawRectangleV")
void ImageDrawRectangleV(Image * dst, Vector2 position, Vector2 size, Color color);

RC_IMPORT("ImageDrawRectangleRec")
void ImageDrawRectangleRec(Image * dst, Rectangle rec, Color color);

RC_IMPORT("ImageDrawRectangleLines")
void ImageDrawRectangleLines(Image * dst, Rectangle rec, int thick, Color color);

RC_IMPORT("ImageDrawTriangle")
void ImageDrawTriangle(Image * dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color);

RC_IMPORT("ImageDrawTriangleEx")
void ImageDrawTriangleEx(Image * dst, Vector2 v1, Vector2 v2, Vector2 v3, Color c1, Color c2, Color c3);

RC_IMPORT("ImageDrawTriangleLines")
void ImageDrawTriangleLines(Image * dst, Vector2 v1, Vector2 v2, Vector2 v3, Color color);

RC_IMPORT("ImageDrawTriangleFan")
void ImageDrawTriangleFan(Image * dst, const Vector2 * points, int pointCount, Color color);

RC_IMPORT("ImageDrawTriangleStrip")
void ImageDrawTriangleStrip(Image * dst, const Vector2 * points, int pointCount, Color color);

RC_IMPORT("ImageDraw")
void ImageDraw(Image * dst, Image src, Rectangle srcRec, Rectangle dstRec, Color tint);

RC_IMPORT("ImageDrawText")
void ImageDrawText(Image * dst, const char * text, int posX, int posY, int fontSize, Color color);

RC_IMPORT("ImageDrawTextEx")
void ImageDrawTextEx(Image * dst, Font font, const char * text, Vector2 position, float fontSize, float spacing, Color tint);

RC_IMPORT("LoadTexture")
Texture2D LoadTexture(const char * fileName);

RC_IMPORT("LoadTextureFromImage")
Texture2D LoadTextureFromImage(Image image);

RC_IMPORT("LoadTextureCubemap")
TextureCubemap LoadTextureCubemap(Image image, int layout);

RC_IMPORT("LoadRenderTexture")
RenderTexture2D LoadRenderTexture(int width, int height);

RC_IMPORT("IsTextureValid")
bool IsTextureValid(Texture2D texture);

RC_IMPORT("UnloadTexture")
void UnloadTexture(Texture2D texture);

RC_IMPORT("IsRenderTextureValid")
bool IsRenderTextureValid(RenderTexture2D target);

RC_IMPORT("UnloadRenderTexture")
void UnloadRenderTexture(RenderTexture2D target);

RC_IMPORT("UpdateTexture")
void UpdateTexture(Texture2D texture, const void * pixels);

RC_IMPORT("UpdateTextureRec")
void UpdateTextureRec(Texture2D texture, Rectangle rec, const void * pixels);

RC_IMPORT("GenTextureMipmaps")
void GenTextureMipmaps(Texture2D * texture);

RC_IMPORT("SetTextureFilter")
void SetTextureFilter(Texture2D texture, int filter);

RC_IMPORT("SetTextureWrap")
void SetTextureWrap(Texture2D texture, int wrap);

RC_IMPORT("DrawTexture")
void DrawTexture(Texture2D texture, int posX, int posY, Color tint);

RC_IMPORT("DrawTextureV")
void DrawTextureV(Texture2D texture, Vector2 position, Color tint);

RC_IMPORT("DrawTextureEx")
void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint);

RC_IMPORT("DrawTextureRec")
void DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint);

RC_IMPORT("DrawTexturePro")
void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);

RC_IMPORT("DrawTextureNPatch")
void DrawTextureNPatch(Texture2D texture, NPatchInfo nPatchInfo, Rectangle dest, Vector2 origin, float rotation, Color tint);

RC_IMPORT("ColorIsEqual")
bool ColorIsEqual(Color col1, Color col2);

RC_IMPORT("Fade")
Color Fade(Color color, float alpha);

RC_IMPORT("ColorToInt")
int ColorToInt(Color color);

RC_IMPORT("ColorNormalize")
Vector4 ColorNormalize(Color color);

RC_IMPORT("ColorFromNormalized")
Color ColorFromNormalized(Vector4 normalized);

RC_IMPORT("ColorToHSV")
Vector3 ColorToHSV(Color color);

RC_IMPORT("ColorFromHSV")
Color ColorFromHSV(float hue, float saturation, float value);

RC_IMPORT("ColorTint")
Color ColorTint(Color color, Color tint);

RC_IMPORT("ColorBrightness")
Color ColorBrightness(Color color, float factor);

RC_IMPORT("ColorContrast")
Color ColorContrast(Color color, float contrast);

RC_IMPORT("ColorAlpha")
Color ColorAlpha(Color color, float alpha);

RC_IMPORT("ColorAlphaBlend")
Color ColorAlphaBlend(Color dst, Color src, Color tint);

RC_IMPORT("ColorLerp")
Color ColorLerp(Color color1, Color color2, float factor);

RC_IMPORT("GetColor")
Color GetColor(unsigned int hexValue);

RC_IMPORT("GetPixelColor")
Color GetPixelColor(void * srcPtr, int format);

RC_IMPORT("SetPixelColor")
void SetPixelColor(void * dstPtr, Color color, int format);

RC_IMPORT("GetPixelDataSize")
int GetPixelDataSize(int width, int height, int format);

RC_IMPORT("GetFontDefault")
Font GetFontDefault(void);

RC_IMPORT("LoadFont")
Font LoadFont(const char * fileName);

RC_IMPORT("LoadFontEx")
Font LoadFontEx(const char * fileName, int fontSize, const int * codepoints, int codepointCount);

RC_IMPORT("LoadFontFromImage")
Font LoadFontFromImage(Image image, Color key, int firstChar);

RC_IMPORT("LoadFontFromMemory")
Font LoadFontFromMemory(const char * fileType, const unsigned char * fileData, int dataSize, int fontSize, const int * codepoints, int codepointCount);

RC_IMPORT("IsFontValid")
bool IsFontValid(Font font);

RC_IMPORT("LoadFontData")
GlyphInfo * LoadFontData(const unsigned char * fileData, int dataSize, int fontSize, const int * codepoints, int codepointCount, int type, int * glyphCount);

RC_IMPORT("GenImageFontAtlas")
Image GenImageFontAtlas(const GlyphInfo * glyphs, Rectangle ** glyphRecs, int glyphCount, int fontSize, int padding, int packMethod);

RC_IMPORT("UnloadFontData")
void UnloadFontData(GlyphInfo * glyphs, int glyphCount);

RC_IMPORT("UnloadFont")
void UnloadFont(Font font);

RC_IMPORT("ExportFontAsCode")
bool ExportFontAsCode(Font font, const char * fileName);

RC_IMPORT("DrawFPS")
void DrawFPS(int posX, int posY);

RC_IMPORT("DrawText")
void DrawText(const char * text, int posX, int posY, int fontSize, Color color);

RC_IMPORT("DrawTextEx")
void DrawTextEx(Font font, const char * text, Vector2 position, float fontSize, float spacing, Color tint);

RC_IMPORT("DrawTextPro")
void DrawTextPro(Font font, const char * text, Vector2 position, Vector2 origin, float rotation, float fontSize, float spacing, Color tint);

RC_IMPORT("DrawTextCodepoint")
void DrawTextCodepoint(Font font, int codepoint, Vector2 position, float fontSize, Color tint);

RC_IMPORT("DrawTextCodepoints")
void DrawTextCodepoints(Font font, const int * codepoints, int codepointCount, Vector2 position, float fontSize, float spacing, Color tint);

RC_IMPORT("SetTextLineSpacing")
void SetTextLineSpacing(int spacing);

RC_IMPORT("MeasureText")
int MeasureText(const char * text, int fontSize);

RC_IMPORT("MeasureTextEx")
Vector2 MeasureTextEx(Font font, const char * text, float fontSize, float spacing);

RC_IMPORT("GetGlyphIndex")
int GetGlyphIndex(Font font, int codepoint);

RC_IMPORT("GetGlyphInfo")
GlyphInfo GetGlyphInfo(Font font, int codepoint);

RC_IMPORT("GetGlyphAtlasRec")
Rectangle GetGlyphAtlasRec(Font font, int codepoint);

RC_IMPORT("LoadUTF8")
char * LoadUTF8(const int * codepoints, int length);

RC_IMPORT("UnloadUTF8")
void UnloadUTF8(char * text);

RC_IMPORT("LoadCodepoints")
int * LoadCodepoints(const char * text, int * count);

RC_IMPORT("UnloadCodepoints")
void UnloadCodepoints(int * codepoints);

RC_IMPORT("GetCodepointCount")
int GetCodepointCount(const char * text);

RC_IMPORT("GetCodepoint")
int GetCodepoint(const char * text, int * codepointSize);

RC_IMPORT("GetCodepointNext")
int GetCodepointNext(const char * text, int * codepointSize);

RC_IMPORT("GetCodepointPrevious")
int GetCodepointPrevious(const char * text, int * codepointSize);

RC_IMPORT("CodepointToUTF8")
const char * CodepointToUTF8(int codepoint, int * utf8Size);

RC_IMPORT("TextCopy")
int TextCopy(char * dst, const char * src);

RC_IMPORT("TextIsEqual")
bool TextIsEqual(const char * text1, const char * text2);

RC_IMPORT("TextLength")
unsigned int TextLength(const char * text);

RC_IMPORT("TextSubtext")
const char * TextSubtext(const char * text, int position, int length);

RC_IMPORT("TextReplace")
char * TextReplace(const char * text, const char * search, const char * replacement);

RC_IMPORT("TextInsert")
char * TextInsert(const char * text, const char * insert, int position);

RC_IMPORT("TextJoin")
char * TextJoin(char ** textList, int count, const char * delimiter);

RC_IMPORT("TextSplit")
char ** TextSplit(const char * text, char delimiter, int * count);

RC_IMPORT("TextAppend")
void TextAppend(char * text, const char * append, int * position);

RC_IMPORT("TextFindIndex")
int TextFindIndex(const char * text, const char * search);

RC_IMPORT("TextToUpper")
char * TextToUpper(const char * text);

RC_IMPORT("TextToLower")
char * TextToLower(const char * text);

RC_IMPORT("TextToPascal")
char * TextToPascal(const char * text);

RC_IMPORT("TextToSnake")
char * TextToSnake(const char * text);

RC_IMPORT("TextToCamel")
char * TextToCamel(const char * text);

RC_IMPORT("TextToInteger")
int TextToInteger(const char * text);

RC_IMPORT("TextToFloat")
float TextToFloat(const char * text);

RC_IMPORT("DrawLine3D")
void DrawLine3D(Vector3 startPos, Vector3 endPos, Color color);

RC_IMPORT("DrawPoint3D")
void DrawPoint3D(Vector3 position, Color color);

RC_IMPORT("DrawCircle3D")
void DrawCircle3D(Vector3 center, float radius, Vector3 rotationAxis, float rotationAngle, Color color);

RC_IMPORT("DrawTriangle3D")
void DrawTriangle3D(Vector3 v1, Vector3 v2, Vector3 v3, Color color);

RC_IMPORT("DrawTriangleStrip3D")
void DrawTriangleStrip3D(const Vector3 * points, int pointCount, Color color);

RC_IMPORT("DrawCube")
void DrawCube(Vector3 position, float width, float height, float length, Color color);

RC_IMPORT("DrawCubeV")
void DrawCubeV(Vector3 position, Vector3 size, Color color);

RC_IMPORT("DrawCubeWires")
void DrawCubeWires(Vector3 position, float width, float height, float length, Color color);

RC_IMPORT("DrawCubeWiresV")
void DrawCubeWiresV(Vector3 position, Vector3 size, Color color);

RC_IMPORT("DrawSphere")
void DrawSphere(Vector3 centerPos, float radius, Color color);

RC_IMPORT("DrawSphereEx")
void DrawSphereEx(Vector3 centerPos, float radius, int rings, int slices, Color color);

RC_IMPORT("DrawSphereWires")
void DrawSphereWires(Vector3 centerPos, float radius, int rings, int slices, Color color);

RC_IMPORT("DrawCylinder")
void DrawCylinder(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color);

RC_IMPORT("DrawCylinderEx")
void DrawCylinderEx(Vector3 startPos, Vector3 endPos, float startRadius, float endRadius, int sides, Color color);

RC_IMPORT("DrawCylinderWires")
void DrawCylinderWires(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color);

RC_IMPORT("DrawCylinderWiresEx")
void DrawCylinderWiresEx(Vector3 startPos, Vector3 endPos, float startRadius, float endRadius, int sides, Color color);

RC_IMPORT("DrawCapsule")
void DrawCapsule(Vector3 startPos, Vector3 endPos, float radius, int slices, int rings, Color color);

RC_IMPORT("DrawCapsuleWires")
void DrawCapsuleWires(Vector3 startPos, Vector3 endPos, float radius, int slices, int rings, Color color);

RC_IMPORT("DrawPlane")
void DrawPlane(Vector3 centerPos, Vector2 size, Color color);

RC_IMPORT("DrawRay")
void DrawRay(Ray ray, Color color);

RC_IMPORT("DrawGrid")
void DrawGrid(int slices, float spacing);

RC_IMPORT("LoadModel")
Model LoadModel(const char * fileName);

RC_IMPORT("LoadModelFromMesh")
Model LoadModelFromMesh(Mesh mesh);

RC_IMPORT("IsModelValid")
bool IsModelValid(Model model);

RC_IMPORT("UnloadModel")
void UnloadModel(Model model);

RC_IMPORT("GetModelBoundingBox")
BoundingBox GetModelBoundingBox(Model model);

RC_IMPORT("DrawModel")
void DrawModel(Model model, Vector3 position, float scale, Color tint);

RC_IMPORT("DrawModelEx")
void DrawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);

RC_IMPORT("DrawModelWires")
void DrawModelWires(Model model, Vector3 position, float scale, Color tint);

RC_IMPORT("DrawModelWiresEx")
void DrawModelWiresEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);

RC_IMPORT("DrawModelPoints")
void DrawModelPoints(Model model, Vector3 position, float scale, Color tint);

RC_IMPORT("DrawModelPointsEx")
void DrawModelPointsEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);

RC_IMPORT("DrawBoundingBox")
void DrawBoundingBox(BoundingBox box, Color color);

RC_IMPORT("DrawBillboard")
void DrawBillboard(Camera camera, Texture2D texture, Vector3 position, float scale, Color tint);

RC_IMPORT("DrawBillboardRec")
void DrawBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint);

RC_IMPORT("DrawBillboardPro")
void DrawBillboardPro(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector3 up, Vector2 size, Vector2 origin, float rotation, Color tint);

RC_IMPORT("UploadMesh")
void UploadMesh(Mesh * mesh, bool dynamic);

RC_IMPORT("UpdateMeshBuffer")
void UpdateMeshBuffer(Mesh mesh, int index, const void * data, int dataSize, int offset);

RC_IMPORT("UnloadMesh")
void UnloadMesh(Mesh mesh);

RC_IMPORT("DrawMesh")
void DrawMesh(Mesh mesh, Material material, Matrix transform);

RC_IMPORT("DrawMeshInstanced")
void DrawMeshInstanced(Mesh mesh, Material material, const Matrix * transforms, int instances);

RC_IMPORT("GetMeshBoundingBox")
BoundingBox GetMeshBoundingBox(Mesh mesh);

RC_IMPORT("GenMeshTangents")
void GenMeshTangents(Mesh * mesh);

RC_IMPORT("ExportMesh")
bool ExportMesh(Mesh mesh, const char * fileName);

RC_IMPORT("ExportMeshAsCode")
bool ExportMeshAsCode(Mesh mesh, const char * fileName);

RC_IMPORT("GenMeshPoly")
Mesh GenMeshPoly(int sides, float radius);

RC_IMPORT("GenMeshPlane")
Mesh GenMeshPlane(float width, float length, int resX, int resZ);

RC_IMPORT("GenMeshCube")
Mesh GenMeshCube(float width, float height, float length);

RC_IMPORT("GenMeshSphere")
Mesh GenMeshSphere(float radius, int rings, int slices);

RC_IMPORT("GenMeshHemiSphere")
Mesh GenMeshHemiSphere(float radius, int rings, int slices);

RC_IMPORT("GenMeshCylinder")
Mesh GenMeshCylinder(float radius, float height, int slices);

RC_IMPORT("GenMeshCone")
Mesh GenMeshCone(float radius, float height, int slices);

RC_IMPORT("GenMeshTorus")
Mesh GenMeshTorus(float radius, float size, int radSeg, int sides);

RC_IMPORT("GenMeshKnot")
Mesh GenMeshKnot(float radius, float size, int radSeg, int sides);

RC_IMPORT("GenMeshHeightmap")
Mesh GenMeshHeightmap(Image heightmap, Vector3 size);

RC_IMPORT("GenMeshCubicmap")
Mesh GenMeshCubicmap(Image cubicmap, Vector3 cubeSize);

RC_IMPORT("LoadMaterials")
Material * LoadMaterials(const char * fileName, int * materialCount);

RC_IMPORT("LoadMaterialDefault")
Material LoadMaterialDefault(void);

RC_IMPORT("IsMaterialValid")
bool IsMaterialValid(Material material);

RC_IMPORT("UnloadMaterial")
void UnloadMaterial(Material material);

RC_IMPORT("SetMaterialTexture")
void SetMaterialTexture(Material * material, int mapType, Texture2D texture);

RC_IMPORT("SetModelMeshMaterial")
void SetModelMeshMaterial(Model * model, int meshId, int materialId);

RC_IMPORT("LoadModelAnimations")
ModelAnimation * LoadModelAnimations(const char * fileName, int * animCount);

RC_IMPORT("UpdateModelAnimation")
void UpdateModelAnimation(Model model, ModelAnimation anim, int frame);

RC_IMPORT("UpdateModelAnimationBones")
void UpdateModelAnimationBones(Model model, ModelAnimation anim, int frame);

RC_IMPORT("UnloadModelAnimation")
void UnloadModelAnimation(ModelAnimation anim);

RC_IMPORT("UnloadModelAnimations")
void UnloadModelAnimations(ModelAnimation * animations, int animCount);

RC_IMPORT("IsModelAnimationValid")
bool IsModelAnimationValid(Model model, ModelAnimation anim);

RC_IMPORT("CheckCollisionSpheres")
bool CheckCollisionSpheres(Vector3 center1, float radius1, Vector3 center2, float radius2);

RC_IMPORT("CheckCollisionBoxes")
bool CheckCollisionBoxes(BoundingBox box1, BoundingBox box2);

RC_IMPORT("CheckCollisionBoxSphere")
bool CheckCollisionBoxSphere(BoundingBox box, Vector3 center, float radius);

RC_IMPORT("GetRayCollisionSphere")
RayCollision GetRayCollisionSphere(Ray ray, Vector3 center, float radius);

RC_IMPORT("GetRayCollisionBox")
RayCollision GetRayCollisionBox(Ray ray, BoundingBox box);

RC_IMPORT("GetRayCollisionMesh")
RayCollision GetRayCollisionMesh(Ray ray, Mesh mesh, Matrix transform);

RC_IMPORT("GetRayCollisionTriangle")
RayCollision GetRayCollisionTriangle(Ray ray, Vector3 p1, Vector3 p2, Vector3 p3);

RC_IMPORT("GetRayCollisionQuad")
RayCollision GetRayCollisionQuad(Ray ray, Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4);

RC_IMPORT("InitAudioDevice")
void InitAudioDevice(void);

RC_IMPORT("CloseAudioDevice")
void CloseAudioDevice(void);

RC_IMPORT("IsAudioDeviceReady")
bool IsAudioDeviceReady(void);

RC_IMPORT("SetMasterVolume")
void SetMasterVolume(float volume);

RC_IMPORT("GetMasterVolume")
float GetMasterVolume(void);

RC_IMPORT("LoadWave")
Wave LoadWave(const char * fileName);

RC_IMPORT("LoadWaveFromMemory")
Wave LoadWaveFromMemory(const char * fileType, const unsigned char * fileData, int dataSize);

RC_IMPORT("IsWaveValid")
bool IsWaveValid(Wave wave);

RC_IMPORT("LoadSound")
Sound LoadSound(const char * fileName);

RC_IMPORT("LoadSoundFromWave")
Sound LoadSoundFromWave(Wave wave);

RC_IMPORT("LoadSoundAlias")
Sound LoadSoundAlias(Sound source);

RC_IMPORT("IsSoundValid")
bool IsSoundValid(Sound sound);

RC_IMPORT("UpdateSound")
void UpdateSound(Sound sound, const void * data, int sampleCount);

RC_IMPORT("UnloadWave")
void UnloadWave(Wave wave);

RC_IMPORT("UnloadSound")
void UnloadSound(Sound sound);

RC_IMPORT("UnloadSoundAlias")
void UnloadSoundAlias(Sound alias);

RC_IMPORT("ExportWave")
bool ExportWave(Wave wave, const char * fileName);

RC_IMPORT("ExportWaveAsCode")
bool ExportWaveAsCode(Wave wave, const char * fileName);

RC_IMPORT("PlaySound")
void PlaySound(Sound sound);

RC_IMPORT("StopSound")
void StopSound(Sound sound);

RC_IMPORT("PauseSound")
void PauseSound(Sound sound);

RC_IMPORT("ResumeSound")
void ResumeSound(Sound sound);

RC_IMPORT("IsSoundPlaying")
bool IsSoundPlaying(Sound sound);

RC_IMPORT("SetSoundVolume")
void SetSoundVolume(Sound sound, float volume);

RC_IMPORT("SetSoundPitch")
void SetSoundPitch(Sound sound, float pitch);

RC_IMPORT("SetSoundPan")
void SetSoundPan(Sound sound, float pan);

RC_IMPORT("WaveCopy")
Wave WaveCopy(Wave wave);

RC_IMPORT("WaveCrop")
void WaveCrop(Wave * wave, int initFrame, int finalFrame);

RC_IMPORT("WaveFormat")
void WaveFormat(Wave * wave, int sampleRate, int sampleSize, int channels);

RC_IMPORT("LoadWaveSamples")
float * LoadWaveSamples(Wave wave);

RC_IMPORT("UnloadWaveSamples")
void UnloadWaveSamples(float * samples);

RC_IMPORT("LoadMusicStream")
Music LoadMusicStream(const char * fileName);

RC_IMPORT("LoadMusicStreamFromMemory")
Music LoadMusicStreamFromMemory(const char * fileType, const unsigned char * data, int dataSize);

RC_IMPORT("IsMusicValid")
bool IsMusicValid(Music music);

RC_IMPORT("UnloadMusicStream")
void UnloadMusicStream(Music music);

RC_IMPORT("PlayMusicStream")
void PlayMusicStream(Music music);

RC_IMPORT("IsMusicStreamPlaying")
bool IsMusicStreamPlaying(Music music);

RC_IMPORT("UpdateMusicStream")
void UpdateMusicStream(Music music);

RC_IMPORT("StopMusicStream")
void StopMusicStream(Music music);

RC_IMPORT("PauseMusicStream")
void PauseMusicStream(Music music);

RC_IMPORT("ResumeMusicStream")
void ResumeMusicStream(Music music);

RC_IMPORT("SeekMusicStream")
void SeekMusicStream(Music music, float position);

RC_IMPORT("SetMusicVolume")
void SetMusicVolume(Music music, float volume);

RC_IMPORT("SetMusicPitch")
void SetMusicPitch(Music music, float pitch);

RC_IMPORT("SetMusicPan")
void SetMusicPan(Music music, float pan);

RC_IMPORT("GetMusicTimeLength")
float GetMusicTimeLength(Music music);

RC_IMPORT("GetMusicTimePlayed")
float GetMusicTimePlayed(Music music);

RC_IMPORT("LoadAudioStream")
AudioStream LoadAudioStream(unsigned int sampleRate, unsigned int sampleSize, unsigned int channels);

RC_IMPORT("IsAudioStreamValid")
bool IsAudioStreamValid(AudioStream stream);

RC_IMPORT("UnloadAudioStream")
void UnloadAudioStream(AudioStream stream);

RC_IMPORT("UpdateAudioStream")
void UpdateAudioStream(AudioStream stream, const void * data, int frameCount);

RC_IMPORT("IsAudioStreamProcessed")
bool IsAudioStreamProcessed(AudioStream stream);

RC_IMPORT("PlayAudioStream")
void PlayAudioStream(AudioStream stream);

RC_IMPORT("PauseAudioStream")
void PauseAudioStream(AudioStream stream);

RC_IMPORT("ResumeAudioStream")
void ResumeAudioStream(AudioStream stream);

RC_IMPORT("IsAudioStreamPlaying")
bool IsAudioStreamPlaying(AudioStream stream);

RC_IMPORT("StopAudioStream")
void StopAudioStream(AudioStream stream);

RC_IMPORT("SetAudioStreamVolume")
void SetAudioStreamVolume(AudioStream stream, float volume);

RC_IMPORT("SetAudioStreamPitch")
void SetAudioStreamPitch(AudioStream stream, float pitch);

RC_IMPORT("SetAudioStreamPan")
void SetAudioStreamPan(AudioStream stream, float pan);

RC_IMPORT("SetAudioStreamBufferSizeDefault")
void SetAudioStreamBufferSizeDefault(int size);
