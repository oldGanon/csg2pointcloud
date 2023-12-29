
#include "glfuncs.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct gl_state
{
    u32 SquareVBO;
    u32 SquareVAO;

    u32 SplatVAO;
    u32 SplatVBO;
    u32 SplatShader;
    u32 SplatShaderWorldToClipLoc;
    u64 SplatCount;

    u32 RenderBufferMSAA;
    u32 DepthBufferMSAA;
    u32 FramebufferMSAA;

    u32 VoxelComputeShader;
    u32 SplatComputeShader;

    u32 SdfSSBO;
    u32 BlocksSSBO;
    u32 DispatchSSBO;
    u32 SplatSSBO;

    u32 DispatchBuffer;
};

global gl_state GL = { };

static b32
OpenGL_CheckError()
{
    GLenum Error = glGetError();
    if (!Error) return Error;
    Console_ErrorF("OpenGL Error Code: 0x%X\n", Error);
    Assert(!Error);
    return Error;
}

static void
OpenGL_DrawFullscreenQuad()
{
    glBindVertexArray(GL.SquareVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static void
OpenGL_DrawSplats(mat4 WorldToClip)
{
    glUseProgram(GL.SplatShader);
    glBindVertexArray(GL.SplatVAO);
    glUniformMatrix4fv(GL.SplatShaderWorldToClipLoc, 1, GL_FALSE, WorldToClip.E);
    glDrawArrays(GL_POINTS, 0, (GLsizei)(GL.SplatCount));
    glBindVertexArray(0);
}

#include "glsdf.cpp"

struct glsdf_block
{
    u32 ReadIndex;
    u32 WriteIndex;
    u32 Padding[2];
    vec4 Blocks[1];
};

static void
OpenGL_GPUSplats()
{
    glsdf_block Blocks = { 0,1,0,0,Vec4(0,0,0,1) };
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL.BlocksSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 32, &Blocks);
    
    u32 Dispatches[3] = { 1, 1, 1 };
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL.DispatchSSBO);
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, GL.DispatchBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 12, &Dispatches);

    glUseProgram(GL.VoxelComputeShader);
    for (u32 i = 0; i < 5; ++i)
    {
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_DISPATCH_INDIRECT_BUFFER, 0, 0, 12);
        glDispatchComputeIndirect(0);
    }

    u32 SplatCount;
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &SplatCount);
    
    glUseProgram(GL.SplatComputeShader);
    glDispatchCompute(SplatCount / 32 + 1, 1, 1);

    glBindBuffer(GL_ARRAY_BUFFER, GL.SplatVBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL.SplatSSBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glsdf_splat) * SplatCount, 0, GL_STATIC_DRAW);
    glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(glsdf_splat) * SplatCount);

    GL.SplatCount = SplatCount;

    Console_LogF("Splatcount: %d!", SplatCount);

    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

static void
OpenGL_GPU(glsdf *SDF)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL.SdfSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glsdf), SDF);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

#include "shaders.h"
#include "compute.h"

static b32
OpenGL_Init()
{
    // glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    /* SHADERS */
    GL.SplatShader = OpenGL_LoadProgram(SplatVertShader, SplatGeomShader, SplatFragShader);
    glUseProgram(GL.SplatShader);
    GL.SplatShaderWorldToClipLoc = glGetUniformLocation(GL.SplatShader, "WorldToCamera");
    glUseProgram(0);

    glGenBuffers(1, &GL.SdfSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL.SdfSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glsdf), 0, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, GL.SdfSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &GL.BlocksSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL.BlocksSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 128*1024*1024, 0, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, GL.BlocksSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &GL.DispatchSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL.DispatchSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 64, 0, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, GL.DispatchSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &GL.SplatSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL.SplatSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 128*1024*1024, 0, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, GL.SplatSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    
    glGenBuffers(1, &GL.DispatchBuffer);
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, GL.DispatchBuffer);
    glBufferData(GL_DISPATCH_INDIRECT_BUFFER, 64, 0, GL_DYNAMIC_READ);
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);

    const char *VoxelShader[3] = { GLSL_Version, SDFShaderLib, VoxelComputeShaderSource };
    const char *SplatShader[3] = { GLSL_Version, SDFShaderLib, SplatComputeShaderSource };

    GL.VoxelComputeShader = OpenGL_LoadComputeProgram(VoxelShader, 3);
    GL.SplatComputeShader = OpenGL_LoadComputeProgram(SplatShader, 3);

    {   /* FRAMEBUFFER */
        glGenRenderbuffers(1, &GL.RenderBufferMSAA);
        glGenRenderbuffers(1, &GL.DepthBufferMSAA);

        u32 Samples = 16;
        glGetIntegerv(GL_MAX_SAMPLES, (GLint*)&Samples);
        Samples = EZ_MIN(Samples, 16);
        
        glBindRenderbuffer(GL_RENDERBUFFER, GL.RenderBufferMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, GL.DepthBufferMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glGenFramebuffers(1, &GL.FramebufferMSAA);
        glBindFramebuffer(GL_FRAMEBUFFER, GL.FramebufferMSAA);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, GL.RenderBufferMSAA);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GL.DepthBufferMSAA);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    {   /* SQUARE VAO */
        const u8 Vs[] = { 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0 };
        glGenVertexArrays(1, &GL.SquareVAO);
        glBindVertexArray(GL.SquareVAO);
        glGenBuffers(1, &GL.SquareVBO);
        glBindBuffer(GL_ARRAY_BUFFER, GL.SquareVBO);
        glVertexAttribPointer(0, 2, GL_UNSIGNED_BYTE, GL_FALSE, 2, 0);
        glEnableVertexAttribArray(0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vs), Vs, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    {   /* SPLAT VAO */
        glGenVertexArrays(1, &GL.SplatVAO);
        glBindVertexArray(GL.SplatVAO);
        glGenBuffers(1, &GL.SplatVBO);
        glBindBuffer(GL_ARRAY_BUFFER, GL.SplatVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glsdf_splat), (GLvoid *)offsetof(glsdf_splat, Position));
        glVertexAttribPointer(1, 4, GL_INT_2_10_10_10_REV, GL_TRUE, sizeof(glsdf_splat), (GLvoid *)offsetof(glsdf_splat, Normal));
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glsdf_splat), (GLvoid *)offsetof(glsdf_splat, Color));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    return OpenGL_CheckError();
}

static void
OpenGL_Clear()
{
    glClearColor(1,1,1,1);
    glScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, GL.FramebufferMSAA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void
OpenGL_Frame(mat4 WorldToClip)
{
    glScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, GL.FramebufferMSAA);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OpenGL_DrawSplats(WorldToClip);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, GL.FramebufferMSAA);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
