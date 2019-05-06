
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

static b32
OpenGL_Init()
{
    // glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    /* SHADERS */
    GL.SplatShader = OpenGL_LoadProgram(SplatVertShader, SplatFragShader);
    glUseProgram(GL.SplatShader);
    GL.SplatShaderWorldToClipLoc = glGetUniformLocation(GL.SplatShader, "WorldToCamera");
    glUseProgram(0);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void
OpenGL_Frame(mat4 WorldToClip)
{
    OpenGL_DrawSplats(WorldToClip);
}
