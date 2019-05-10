
#include "glfuncs.h"

struct gl_state
{
    u32 SquareVBO;
    u32 SquareVAO;

    u32 SplatVAO;
    u32 SplatVBO;
    u32 SplatShader;
    u32 SplatShaderWorldToClipLoc;
    u64 SplatCountHi;
    u64 SplatCountLo;

    u32 RenderBufferMSAA;
    u32 DepthBufferMSAA;
    u32 FramebufferMSAA;

    u32 ComputeShader;
    u32 ComputeShader2;
};

global gl_state GL = { };

static b32
OpenGL_CheckError()
{
    GLenum Error = glGetError();
    if (!Error) return Error;
    Api_Error(TSPrint("Opengl Error Code: 0x%X\n", Error));
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
    glDrawArrays(GL_POINTS, 0, (GLsizei)(GL.SplatCountLo + GL.SplatCountHi));
    glBindVertexArray(0);
}

static void
OpenGL_LoadSplatsHi(sdf_splat_batch *Batch)
{
    GL.SplatCountLo = 0;
    GL.SplatCountHi = Atomic_Get(&Batch->SplatCount);
    glBindBuffer(GL_ARRAY_BUFFER, GL.SplatVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GL.SplatCountHi * sizeof(sdf_splat), Batch->Splats);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void
OpenGL_LoadSplatsLo(sdf_splat_batch *Batch)
{
    GL.SplatCountLo = Atomic_Get(&Batch->SplatCount);
    glBindBuffer(GL_ARRAY_BUFFER, GL.SplatVBO);
    glBufferSubData(GL_ARRAY_BUFFER, GL.SplatCountHi * sizeof(sdf_splat), GL.SplatCountLo * sizeof(sdf_splat), Batch->Splats);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    GLuint SSBOs[3];
    glGenBuffers(3, SSBOs);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 64, 0, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBOs[0]);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 64, 0, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBOs[1]);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[2]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 16*65, 0, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, SSBOs[2]);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    GL.ComputeShader2 = OpenGL_LoadComputeProgram(ComputeShaderSource2);
    GL.ComputeShader = OpenGL_LoadComputeProgram(ComputeShaderSource);
    glUseProgram(GL.ComputeShader);
    glDispatchCompute(1, 1, 1);
    glUseProgram(0);

    {   /* FRAMEBUFFER */
        glGenRenderbuffers(1, &GL.RenderBufferMSAA);
        glGenRenderbuffers(1, &GL.DepthBufferMSAA);

        u32 Samples = 16;
        glGetIntegerv(GL_MAX_SAMPLES, (GLint*)&Samples);
        Samples = MIN(Samples, 16);
        
        glBindRenderbuffer(GL_RENDERBUFFER, GL.RenderBufferMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_RGB, 1280, 720);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, GL.DepthBufferMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_DEPTH_COMPONENT, 1280, 720);
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(sdf_splat) * 10000000, 0, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sdf_splat), (GLvoid *)offsetof(sdf_splat, Position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sdf_splat), (GLvoid *)offsetof(sdf_splat, Normal));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(sdf_splat), (GLvoid *)offsetof(sdf_splat, Color));
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
    glScissor(0, 0, 1280, 720);
    glViewport(0, 0, 1280, 720);
    glBindFramebuffer(GL_FRAMEBUFFER, GL.FramebufferMSAA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void
OpenGL_Frame(mat4 WorldToClip)
{
    glScissor(0, 0, 1280, 720);
    glViewport(0, 0, 1280, 720);
    glBindFramebuffer(GL_FRAMEBUFFER, GL.FramebufferMSAA);

    OpenGL_DrawSplats(WorldToClip);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, GL.FramebufferMSAA);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
