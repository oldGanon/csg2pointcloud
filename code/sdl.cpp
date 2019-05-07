
global atomic GlobalResetGame = { };

#include "sdf.cpp"

#if COMPILE_GFX_OPENGL
  #include <gl/gl.h>
  #include <SDL/SDL_opengl_glext.h>
  #define GL_LOAD_FUNC(S) SDL_GL_GetProcAddress(S)
  global SDL_GLContext GlobalGLContext;
#elif COMPILE_GFX_VULKAN
  #include <SDL/SDL_vulkan.h>
  global SDL_vulkanInstance GlobalVKInstance;
#endif
#include "gfx/gfx.cpp"

global memory_zone *GlobalZone = 0;

global b32 GlobalRunning = true;
global u64 GlobalPerfCountFrequency = 0;
global SDL_AudioDeviceID GlobalAudioDevice = 0;

#define UPDATES_PER_SECOND 60

enum main_key_dest
{
    KEY_DEST_GAME = 0,
    KEY_DEST_CONSOLE = 1,
};

struct main_state
{
    main_key_dest KeyDest;
    SDL_GameController* Gamepad;
    i32 GamepadIndex;
    b32 VSyncState;
    b32 PreferWindowedFullscreen;

    vec2 MousePos;
    b32 MouseMoved;
    b32 LeftMouseDown;
    b32 RightMouseDown;
};

/*-------------*/
/*     API     */
/*-------------*/

inline size
Api_Tsize()
{
    memory_arena *Arena = (memory_arena *)SDL_TLSGet(MemoryArenaTLS);
    return Arena->Used - sizeof(memory_arena);
}

inline void
Api_Treset(size Size)
{
    memory_arena *Arena = (memory_arena *)SDL_TLSGet(MemoryArenaTLS);
    Arena->Used = sizeof(memory_arena) + Size;
    Arena->MarkerCount = 0;
}

inline void*
Api_Ualloc(size Size)
{
    memory_arena *Arena = (memory_arena *)SDL_TLSGet(MemoryArenaTLS);
    return Arena_PushSize(Arena, Size, 1);
}

inline void*
Api_Talloc(size Size)
{
    memory_arena *Arena = (memory_arena *)SDL_TLSGet(MemoryArenaTLS);
    return Arena_PushSize(Arena, Size, 16);
}

inline void*
Api_Malloc(size Size)
{
    return Zone_Malloc(GlobalZone, Size);
}

inline void*
Api_Realloc(void *Ptr, size Size)
{
    return Zone_Realloc(GlobalZone, Ptr, Size);
}

inline void
Api_Free(const void *Ptr)
{
    Zone_Free(GlobalZone, Ptr);
}

inline void
Api_PrintString(u32 Target, string String)
{
    // Console_Print(&GlobalConsole, String);
    switch (Target)
    {
        case 0: SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, CString(String)); break;
        case 1: SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, CString(String)); break;
        case 2: SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, CString(String)); break;
    }
}

/*--------------*/
/*     MAIN     */
/*--------------*/

static void
Main_SDLCleanUp()
{
#if COMPILE_GFX_OPENGL
    SDL_GL_DeleteContext(GlobalGLContext);
    SDL_GL_UnloadLibrary();
#endif
#if COMPILE_GFX_VULKAN
    if ((void *)Vulkan_Destroy)
        Vulkan_Destroy(GlobalVKInstance);
    SDL_Vulkan_UnloadLibrary();
#endif
    SDL_CloseAudioDevice(GlobalAudioDevice);
    SDL_Quit();
}

static i32
Main_Error(string Error)
{
    string SDL_Error = TString(SDL_GetError());
    if (SDL_Error.Length)
        Api_Error(SDL_Error);

    Api_Error(Error);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Platform Error!", CString(Error), 0);
    
    Main_SDLCleanUp();
    GlobalRunning = false;
    return 1;
}

inline u64
Main_GetWallClock()
{
    return SDL_GetPerformanceCounter();
}

inline f32
Main_GetSecondsElapsed(u64 Start, u64 End = Main_GetWallClock())
{
    f32 Result = ((f32)(End - Start) / (f32)GlobalPerfCountFrequency);
    return Result;
}

inline u64
Main_GetMillisecondsElapsed(u64 Start, u64 End)
{
    u64 Result = ((End - Start) * 1000) / GlobalPerfCountFrequency;
    return Result;
}

static ivec2
Main_GetWindowSize(SDL_Window *Window)
{
    i32 Width, Height;
#if COMPILE_GFX_OPENGL
    SDL_GL_GetDrawableSize(Window, &Width, &Height);
#elif COMPILE_GFX_VULKAN
    SDL_Vulkan_GetDrawableSize(Window, &Width, &Height);
#endif
    ivec2 WindowDimension = { Width, Height };
    return WindowDimension;
}

static irect
Main_GetDrawArea(SDL_Window *Window)
{
    return iRect(iVec2(), Main_GetWindowSize(Window));
}

static b32
Main_SetExclusiveFullscreen(SDL_Window *Window)
{
    i32 DiplayIndex = SDL_GetWindowDisplayIndex(Window);
    if (DiplayIndex < 0) return false;

    SDL_DisplayMode DisplayMode = { };
    if (SDL_GetCurrentDisplayMode(DiplayIndex, &DisplayMode))
        return false;

    if (SDL_SetWindowDisplayMode(Window, &DisplayMode))
        return false;

    if (SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN))
        return false;

    return true;
}

static void
Main_ToggleFullscreen(main_state *MainState, SDL_Window *Window)
{
    
    u32 Fullscreen = (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (SDL_GetWindowFlags(Window) & Fullscreen)
    {
        SDL_SetWindowFullscreen(Window, 0);
    }
    else
    {
        if(MainState->PreferWindowedFullscreen)
        {
            SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        else
        {
            if (!Main_SetExclusiveFullscreen(Window))
            {
                MainState->PreferWindowedFullscreen = true;
                SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
        }
    }
}

static string
Main_GetClipboardText()
{
    if (!SDL_HasClipboardText())
        return String();

    char *Clipboard = SDL_GetClipboardText();
    string Result = TString(Clipboard);
    SDL_free(Clipboard);
    return Result;
}

static void
Main_SetClipboardText(string Text)
{
    SDL_SetClipboardText(CString(Text));
}

static void 
Main_Audiocallback(void* Data, u8* Buffer, i32 Buffersize)
{
#if 0
    game_sound_output SoundBuffer = 
        { 48000, Buffersize/(sizeof(i16)*2), (i16 *)Buffer };
    GlobalGameCode.Game_GetAudio(&SoundBuffer, (game_memory *)Data);
#endif
}

static void
Main_SetVSync(main_state *MainState, b32 VSync)
{
    if (MainState->VSyncState == VSync)
        return;

    MainState->VSyncState = VSync;
    if (VSync) SDL_GL_SetSwapInterval(1);
    else       SDL_GL_SetSwapInterval(0);
}

#if COMPILE_GFX_OPENGL
static SDL_Window *
Main_CreateOpenGLWindow()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    u32 WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
    SDL_Window *Window = SDL_CreateWindow(GAME_NAME,
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED,
                                          1280, 
                                          720,
                                          WindowFlags);
    if (!Window)
    {
        Main_Error("Couldn't create Window!");
        return 0;
    }

    SDL_SetWindowMinimumSize(Window, 320, 180);

    GlobalGLContext = SDL_GL_CreateContext(Window);
    if (!GlobalGLContext)
    {
        Main_Error("Couldn't create OpenGL Context!");
        return 0;
    }
    if (!GL_LoadFunctions())
    {
        Main_Error("Couldn't load OpenGL functions!");
        return 0;
    }
    if (OpenGL_Init())
    {
        Main_Error("Couldn't set up OpenGL state!");
        return 0;
    }

    return Window;
}
#endif

#if COMPILE_GFX_VULKAN
static SDL_Window *
Main_CreateVulkanWindow(iv2 Resolution)
{
    iv2 WindowDim = Resolution * iV2(3);
    u32 WindowFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
    SDL_Window *Window = SDL_CreateWindow(GAME_NAME,
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED,
                                          WindowDim.x, 
                                          WindowDim.y,
                                          WindowFlags);
    if (!Window)
    {
        Main_Error("Couldn't create Window!");
        return 0;
    }

    SDL_SetWindowMinimumSize(Window, 320, 180);

    vkGetInstanceProcAddr = 
        (PFN_vkGetInstanceProcAddr)SDL_Vulkan_GetVkGetInstanceProcAddr();
    u32 ExtCount;
    if (!SDL_Vulkan_GetInstanceExtensions(Window, &ExtCount, 0))
    {
        Main_Error("Couldn't query Vulkan required extensions!");
        return 0;
    }
    const char** ExtNames = (const char **)Api_Malloc(sizeof(char *) * (ExtCount + 1));
    if (!SDL_Vulkan_GetInstanceExtensions(Window, &ExtCount, ExtNames))
    {
        Api_Free(ExtNames);
        Main_Error("Couldn't query Vulkan extension names!");
        return 0;
    }
#if COMPILE_DEV
    ExtNames[ExtCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
#endif
    b32 Success = Vulkan_Instance(&GlobalVKInstance, ExtCount, ExtNames);
    Api_Free(ExtNames);
    if (!Success)
    {
        Main_Error("Couldn't create Vulkan instance!");
        return 0;
    }

    VkSurfaceKHR Surface;
    if (!SDL_Vulkan_CreateSurface(Window, GlobalVKInstance, &Surface))
    {
        Main_Error("Couldn't create Vulkan surface!");
        return 0;
    }

    if (!Vulkan_Init(GlobalVKInstance, Surface))
    {
        Main_Error("Couldn't init Vulkan!");
        return 0;
    }

    return Window;
}
#endif

static void
Main_CollectEvents(SDL_Window *Window, main_state *MainState)
{
    b32 StartTextInput = false;

    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        /* PROCESS EVENTS */
        switch (Event.type)
        {
            /* QUIT EVENT */
            case SDL_QUIT:
            {
                GlobalRunning = false;
            } break;

            /* KEY EVENTS */
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                b32 IsDown = (Event.key.state == SDL_PRESSED);
                u32 ScanCode = Event.key.keysym.scancode;
                u32 KeyCode = Event.key.keysym.sym;
                u16 Mod = Event.key.keysym.mod;

                if (IsDown)
                {
                    if (ScanCode == SDL_SCANCODE_F1)
                        Atomic_Set(&GlobalResetGame,1);

                    if (Event.key.repeat)
                        break;

                    b32 AltKeyDown = !!(Mod & KMOD_ALT);
                    if (AltKeyDown)
                    {
                        if (ScanCode == SDL_SCANCODE_F4)
                            GlobalRunning = false;

                        if (ScanCode == SDL_SCANCODE_RETURN)
                            Main_ToggleFullscreen(MainState, Window);
                    }
                }

                if (Event.key.repeat)
                    break;
            } break;

            /* TEXT INPUT EVENTS */
            case SDL_TEXTINPUT:
            {

            } break;

            case SDL_TEXTEDITING:
            {
                if (MainState->KeyDest != KEY_DEST_CONSOLE)
                    break;

                char *Text = Event.edit.text;
                i32 Start = Event.edit.start;
                i32 Length = Event.edit.length;
            } break;

            /* MOUSE EVENTS */
            case SDL_MOUSEWHEEL:
            {
            } break;

            case SDL_MOUSEMOTION:
            {
                ivec2 D = Main_GetWindowSize(Window);
                f32 X = (Event.motion.x / (f32)D.x) * -2.0f + 1.0f;
                f32 Y = (Event.motion.y / (f32)D.y) * 2.0f - 1.0f;
                MainState->MousePos = Vec2(X, Y);
                MainState->MouseMoved = true;
            } break;

            case SDL_MOUSEBUTTONUP: break;
            case SDL_MOUSEBUTTONDOWN:
            {
                if (Event.button.button == SDL_BUTTON_LEFT)
                    MainState->LeftMouseDown = true;
                if (Event.button.button == SDL_BUTTON_RIGHT)
                    MainState->RightMouseDown = true;
            } break;

            /* WINDOW EVENTS */
            case SDL_WINDOWEVENT:
            {
                switch (Event.window.event)
                {
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                    {
                        SDL_PauseAudioDevice(GlobalAudioDevice, 0);
                    } break;

                    case SDL_WINDOWEVENT_FOCUS_LOST:
                    {
                        SDL_PauseAudioDevice(GlobalAudioDevice, 1);
                    } break;

                    case SDL_WINDOWEVENT_RESIZED:
                    {
                    } break;
                }
            } break;

            /* GAMEPAD EVENTS */
            case SDL_CONTROLLERDEVICEADDED:
            {
                MainState->GamepadIndex = Event.cdevice.which;
                MainState->Gamepad = SDL_GameControllerOpen(MainState->GamepadIndex);
                SDL_GameControllerEventState(SDL_ENABLE);
            } break;

            case SDL_CONTROLLERDEVICEREMOVED:
            {
                if (MainState->GamepadIndex == Event.jdevice.which)
                {
                    SDL_GameControllerClose(MainState->Gamepad);
                    SDL_GameControllerEventState(SDL_DISABLE);
                }
            } break;

            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN:
            {
                u8 State = Event.cbutton.state;
                u8 Button = Event.cbutton.button;
            } break;

            case SDL_CONTROLLERAXISMOTION:
            {
                u8 Axis = Event.caxis.axis;
                f32 State = Event.caxis.value;
                if (State < 0.0f) State /= 32768.0f;
                else              State /= 32767.0f;
            } break;
        }
    }

    if (StartTextInput)
        SDL_StartTextInput();
}

#define CHECK_CPU_FEATURE(F) \
    if (!CPU.Has##F) { \
        Api_Error(#F " is not supported on this CPU!"); \
        AllFeatures = false; } \

static b32
Main_CheckCPUFeatures()
{
    b32 AllFeatures = true;

#if COMPILE_DEV
    CHECK_CPU_FEATURE(RDTSC);
#endif
    
#if COMPILE_X64
    CHECK_CPU_FEATURE(SSE);
    CHECK_CPU_FEATURE(SSE2);
    CHECK_CPU_FEATURE(SSE3);

    #if COMPILE_SSE
        CHECK_CPU_FEATURE(SSSE3);
        CHECK_CPU_FEATURE(SSE41);
        CHECK_CPU_FEATURE(SSE42);
        CHECK_CPU_FEATURE(FMA);
    #endif
#endif

#if COMPILE_NEON
    CHECK_CPU_FEATURE(NEON);
#endif
    
    return AllFeatures;
}

int SDL_main(int argc, char **argv)
{
    InitCPU();

    main_state MainState = { };

    /* INIT */
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER))
        return Main_Error("Couldn't initialize SDL!");

    /* GLOBALS */
    GlobalPerfCountFrequency = SDL_GetPerformanceFrequency();
    GlobalZone = Zone_Init((memory_zone *)SDL_malloc(Megabytes(256)), Megabytes(256));

    /* THREAD LOCALS */
    MemoryArenaTLS = SDL_TLSCreate();

    memory_arena *Arena = Arena_Clear((memory_arena *)SDL_malloc(Megabytes(1)), Megabytes(1));
    SDL_TLSSet(MemoryArenaTLS, Arena, SDL_free);
    memory_arena_marker InitMarker = Arena_PlaceArenaMarker(Arena);

    /* CHECK CPU FEATURES */
    if (!Main_CheckCPUFeatures())
        return Main_Error("Missing CPU features!");
    
    /* START TIMING */
    u64 StartTime = Main_GetWallClock();

    /* RNG */
    RNG_Init(0xDEAD);

    /* ASYNC */
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
    Async_Init(8);

    /* AUDIO */
    {
#if 0
        //NOTE: Use directsound on windows maybe
        if (SDL_AudioInit("directsound")) 
            return Main_Error("Couldn't initialize Directsound!");
#endif
        SDL_AudioSpec OpenedSpec, DesiredSpec = { };
        DesiredSpec.freq = 48000;
        DesiredSpec.format = AUDIO_S16LSB;
        DesiredSpec.channels = 2;
        DesiredSpec.samples = 4096;
        DesiredSpec.callback = Main_Audiocallback;
        DesiredSpec.userdata = 0;

        GlobalAudioDevice = SDL_OpenAudioDevice(0, 0, &DesiredSpec, &OpenedSpec, 0);
        if (!GlobalAudioDevice)
            Api_Error("Couldn't open audio device!");
        SDL_PauseAudioDevice(GlobalAudioDevice, 0);
    }

    /* GRAPHICS */
#if COMPILE_GFX_OPENGL
    SDL_Window *Window = Main_CreateOpenGLWindow();
#elif COMPILE_GFX_VULKAN
    SDL_Window *Window = Main_CreateVulkanWindow();
#endif
    if (!Window) return Main_Error("Couldn't create window!");

    SDL_ShowWindow(Window);
    SDL_RaiseWindow(Window);

    /* INPUT */
    SDL_GameControllerAddMapping("gamecontrollerdb.txt");
    SDL_StopTextInput();

    /* PERF */
    // SDL_GL_SetSwapInterval(0);

    Api_Print(TSPrint("Startup Time: %.2fs!", Main_GetSecondsElapsed(StartTime)));

    sdf_splat_batch *Splats = (sdf_splat_batch *)SDL_malloc(sizeof(sdf_splat) * 10000000);

    sdf SDF;
    SDF.EditCount = 0;

    {
        vec3 Color = Vec3(1.0f,0.878f,0.741f);
        vec3 Red = Vec3(1,0,0);
        vec3 Green = Vec3(0,1,0);
        vec3 Blue = Vec3(0,0,1);

        SDF_Add(&SDF,       SDF_Cuboid(Vec3( 0.0f,0.0f,0.0f), Quat(), Color, Vec3(0.5f,0.1f,0.01f)));
        SDF_Add(&SDF,       SDF_Cuboid(Vec3( 0.0f,0.0f,0.0f), Quat(), Color, Vec3(0.1f,0.5f,0.01f)));

        SDF_AddSmooth(&SDF, SDF_Cuboid(Vec3( 0.0f,0.5f,0.0f), Quat(Vec3(1,0,0), PI/3.0f), Red, Vec3(0.5f,0.1f,0.1f)), 0.02f);
        SDF_AddSmooth(&SDF, SDF_Sphere(Vec3(-0.5f,0.0f,0.0f), Blue, 0.25f), 0.05f);
        SDF_AddSmooth(&SDF, SDF_Sphere(Vec3( 0.5f,0.0f,0.0f), Green, 0.25f), 0.05f);
        SDF_AddSmooth(&SDF, SDF_Sphere(Vec3(-0.8f,0.0f,0.0f), Red, 0.1f), 0.05f);
        SDF_SubSmooth(&SDF, SDF_Sphere(Vec3( 0.5f,0.0f,0.25), Red, 0.1f), 0.05f);
        SDF_AddSmooth(&SDF, SDF_Sphere(Vec3(-0.95f,0,0), Red, 0.025f), 0.05f);
        SDF_AddSmooth(&SDF, SDF_Sphere(Vec3( 0.95f,0,0), Red, 0.025f), 0.05f);
        SDF_AddSmooth(&SDF, SDF_Cylinder(Vec3( 0.75f,0,0), Quat(Vec3(1,0,0), PI/1.5f), Blue, 0.45f, 0.25f), 0.05f);

        // SDF_Add(&SDF, SDF_Ellipsoid(Vec3_Set1(0), Quat(Vec3(0,0,1), PI/3.0f), Color, Vec3(0.5f, 0.75f, 0.5f)));
        // SDF_Sub(&SDF, SDF_Cuboid(Vec3(0.0f,-0.5,0.0f), Quat(), Color, Vec3(0.6f, 0.5f, 0.6f)));
        // SDF_AddSmooth(&SDF, SDF_Sphere(Vec3_Set1(0), Color, 0.5f), 0.1f);

        SDF_Gen(&SDF, Vec3_Zero(), 1.0f, 8, Splats);

        Api_PrintF("Splat Count: %llu!", Splats->SplatCount);

        OpenGL_LoadSplatsHi(Splats);
    }

    vec3 CameraPosition = Vec3(0,0,3);
    quat CameraRotation = Quat_Id();

    mat4 Translation = Mat4_Translation(Vec3_Negate(CameraPosition));
    mat4 Rotation = Mat4_Rotation(Quat_Negate(CameraRotation));
    mat4 Perspective = Mat4_Perspective(45.0f, 9.0f/16.0f, 0.1f, 1000.0f);
    mat4 WorldToClip = Perspective * Rotation * Translation;

    mat4 InvTranslation = Mat4_Translation(CameraPosition);
    mat4 InvRotation = Mat4_Rotation(CameraRotation);
    mat4 InvPerspective = Mat4_Perspective(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    mat4 ClipToWorld = InvPerspective * InvRotation * InvTranslation;

    sdf_shape EditShape = SDF_Sphere(Vec3(0,0,0), Vec3(1,0,0), 0.1f);
    EditShape = SDF_Cube(Vec3(0,0,0), Quat(), Vec3(1,0,0), 0.1f);
    EditShape = SDF_Cylinder(Vec3(0,0,0), Quat(), Vec3(1,0,1), 0.1f, 0.1f);
    sdf_op EditOp = { SDF_OP_ADD_SMOOTH, 0.025f };
    f32 EditDim = 0.125f;

    /* GAME INIT */
    Api_Print(TSPrint("Startup Time: %.2fs!", Main_GetSecondsElapsed(StartTime)));
    Arena_RevertToArenaMarker(InitMarker);

    /* GAME LOOP */
    u64 LastLastTime = Main_GetWallClock();
    u64 LastTime = Main_GetWallClock();
    while (GlobalRunning)
    {
        ARENA_STACK_MARKER((memory_arena *)SDL_TLSGet(MemoryArenaTLS));

        Main_CollectEvents(Window, &MainState);
        
        if (MainState.RightMouseDown)
        {
            MainState.RightMouseDown = false;
            if (EditOp.Type == SDF_OP_ADD_SMOOTH)
                EditOp.Type = SDF_OP_SUB_SMOOTH;
            else
                EditOp.Type = SDF_OP_ADD_SMOOTH;
        }

        if (MainState.LeftMouseDown)
        {
            MainState.LeftMouseDown = false;
            SDF_AddEdit(&SDF, EditShape, EditOp);
            SDF_Gen(&SDF, Vec3_Zero(), 1.0f, 8, Splats);
            OpenGL_LoadSplatsHi(Splats);
        }

        if (MainState.MouseMoved)
        {
            MainState.MouseMoved = false;
            vec4 MousePos3D = Vec4(MainState.MousePos, Vec2(0,1));
            MousePos3D = ClipToWorld * MousePos3D;
            MousePos3D = MousePos3D / MousePos3D.w;
            vec3 MouseDir = Vec3_Normalize(MousePos3D-CameraPosition);
            f32 Depth = SDF_Dist(&SDF, CameraPosition, MouseDir);

            vec3 EditPos;
            if (Depth < 100.0f)
                EditPos = CameraPosition + MouseDir * Depth;
            else
                EditPos = CameraPosition + MouseDir * 3.0f;

            EditShape.Position = EditPos;
            SDF_AddEdit(&SDF, EditShape, EditOp);
            SDF_Gen(&SDF, EditPos, EditDim, 5, Splats);
            OpenGL_LoadSplatsLo(Splats);
            SDF_Undo(&SDF);
        }

        Gfx.Clear();
        Gfx.Frame(WorldToClip);
        SDL_GL_SwapWindow(Window);

        /* FRAME TIMING */
        u64 EndTime = Main_GetWallClock();
        LastLastTime = LastTime;
        LastTime = EndTime;
    }

    Main_SDLCleanUp();

    return 0;
}
