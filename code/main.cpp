#include "config.h"
#define ESZETT_ALL 1
#include <eszett/eszett.h>

#include "key.h"

#define ENGINE_NAME "ENGINE_NAME"
#define GAME_NAME "GAME_NAME"

#if defined(__clang__)
  #define COMPILE_CLANG 1
#elif defined(_MSC_VER)
  #define COMPILE_MSVC 1
#elif defined(__GNUC__)
  #define COMPILE_GCC 1
#endif

struct cpu
{
    b32 HasRDTSC;
    b32 HasSSE;
    b32 HasSSE2;
    b32 HasSSE3;
    b32 HasSSSE3;
    b32 HasSSE41;
    b32 HasSSE42;
    b32 HasFMA;
    b32 HasAVX;
};

global cpu CPU = { };

#if COMPILE_X86

#if COMPILE_MSVC
    #define CPUID __cpuid
#elif COMPILE_CLANG
#include <cpuid.h>
    void CPUID(int Info[4], int Id) { __cpuid_count(Id, 0, Info[0], Info[1], Info[2], Info[3]); }
#endif

static void InitCPU()
{
    i32 CPUID0[4] = { -1 };
    CPUID(CPUID0, 0);
    Swap(CPUID0 + 2, CPUID0 + 3);
    if (CPUID0[0] > 0)
    {
        i32 CPUID1[4] = { -1 };
        CPUID(CPUID1, 1);
        CPU.HasRDTSC = (CPUID1[3] & (1 <<  4)) != 0;
        CPU.HasSSE   = (CPUID1[3] & (1 << 25)) != 0;
        CPU.HasSSE2  = (CPUID1[3] & (1 << 26)) != 0;
        CPU.HasSSE3  = (CPUID1[2] & (1 <<  0)) != 0;
        CPU.HasSSSE3 = (CPUID1[2] & (1 <<  9)) != 0;
        CPU.HasSSE41 = (CPUID1[2] & (1 << 19)) != 0;
        CPU.HasSSE42 = (CPUID1[2] & (1 << 20)) != 0;
        CPU.HasFMA   = (CPUID1[2] & (1 << 12)) != 0;
        CPU.HasAVX   = (CPUID1[2] & (1 << 28)) != 0;
    }

#if COMPILE_X64
    u32 MXCSR = _mm_getcsr();
    MXCSR = _MM_ROUND_NEAREST | _MM_FLUSH_ZERO_ON | _MM_MASK_INVALID | _MM_MASK_DIV_ZERO | 
            _MM_MASK_DENORM | _MM_MASK_OVERFLOW | _MM_MASK_UNDERFLOW | _MM_MASK_INEXACT;
    _mm_setcsr(MXCSR);
#endif
}

#elif COMPILE_ARM
    void InitCPU() { }
#else
  #error unsupported cpu architecture!
#endif

/* OS LAYER */
#if COMPILE_SDL
  #include "sdl.cpp"
#elif COMPILE_WIN32
  #include "win32.cpp"
#else
  #error unsupported platform!
#endif

#if COMPILE_WINDOWS
void WinMainCRTStartup()
{
    int Result = WinMain(GetModuleHandle(0), 0, 0, 0);
    ExitProcess(Result);
}
#endif

#if COMPILE_MSVC

extern "C" int _fltused = 0;
extern "C"
{
    #pragma function(memset)
    void *memset(void *dest, int c, size_t count)
    {
        char *bytes = (char *)dest;
        while (count--) { *bytes++ = (char)c; }
        return dest;
    }

    #pragma function(memcpy)
    void *memcpy(void *dest, const void *src, size_t count)
    {
        char *dest8 = (char *)dest;
        const char *src8 = (const char *)src;
        while (count--) { *dest8++ = *src8++; }
        return dest;
    }

    #pragma function(memcmp)
    int memcmp(const void *s1, const void *s2, size_t count)
    {
        const unsigned char *p1 = (const unsigned char *)s1;
        const unsigned char *p2 = (const unsigned char *)s2;
        for (; count--; p1++, p2++){ if (*p1 != *p2) return *p1 - *p2; }
        return 0;
    }

    #pragma function(strcmp)
    int strcmp(const char* s1, const char* s2)
    {
        while(*s1 && (*s1 == *s2)) { s1++; s2++; }
        return *(const unsigned char*)s1 - *(const unsigned char*)s2;
    }
}

#endif