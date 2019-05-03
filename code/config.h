/* GRAPHICS API */
#define COMPILE_GFX_OPENGL 1
#define COMPILE_GFX_VULKAN 0
#define COMPILE_GFX_SOFTWARE 0

/* USE INTRINSICS */
#define COMPILE_INTRINSICS 1

/* PROCESSOR */
#define COMPILE_X86 1
#define COMPILE_X64 1 // SUPPORT FOR SSE, SSE2, SSE3
#define COMPILE_SSE 1 // FULL SSE SUPPORT 1 - 4.2, FMA3
#define COMPILE_ARM 0
#define COMPILE_NEON 0

/* PLARFORM LAYER */
#define COMPILE_SDL 1
#define COMPILE_WIN32 0

/* PLATFORM */
#define COMPILE_WINDOWS 1
#define COMPILE_LINUX 0

/* DEVELOPMENT DEBUG FLAG */
#define COMPILE_DEV 1

/* VERSION */
#define MAGIC_NUMBER 0x545A5345 // ESZT
#define ASSET_VERSION (1)
#define ENGINE_VERSION (1)
#define GAME_VERSION (1)