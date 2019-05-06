struct gfx_functions
{
    void (*Clear) (void);
    void (*Frame) (mat4);
};

#if COMPILE_GFX_OPENGL
    #include "gl/opengl.cpp"

const gfx_functions Gfx = {
    OpenGL_Clear,
    OpenGL_Frame,
};

#elif COMPILE_GFX_VULKAN
    #include "vk/vulkan.cpp"

const gfx_functions Gfx = { 
};

#elif COMPILE_GFX_SOFTWARE
#else
    #error unspecified graphics backend!
#endif
