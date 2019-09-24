#include "config.h"
#define ESZETT_ALL 1
#include <eszett/eszett.h>

#include "key.h"

#define ENGINE_NAME "ENGINE_NAME"
#define GAME_NAME "GAME_NAME"

/* OS LAYER */
#if COMPILE_SDL
  #include "sdl.cpp"
#elif COMPILE_WIN32
  #include "win32.cpp"
#else
  #error unsupported platform!
#endif

#if ESZETT_WINDOWS
void WinMainCRTStartup()
{
    int Result = WinMain(GetModuleHandle(0), 0, 0, 0);
    ExitProcess(Result);
}
#endif

#if ESZETT_MSVC

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
