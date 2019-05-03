
#include "glfuncs.h"

struct gl_state
{
    u32 SquareVBO;
    u32 SquareVAO;
    u32 SDFShader;

    u32 SplatVAO;
    u32 SplatVBO;
    u32 SplatShader;
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
OpenGL_DrawSplats()
{
    glUseProgram(GL.SplatShader);
    glBindVertexArray(GL.SplatVAO);
    glDrawArrays(GL_POINTS, 0, (GLsizei)SPLATCOUNT);
    glBindVertexArray(0);
}

static void
OpenGL_LoadSplats()
{
    glBindBuffer(GL_ARRAY_BUFFER, GL.SplatVBO);
    glBufferData(GL_ARRAY_BUFFER, SPLATCOUNT * sizeof(splat), SPLATS, GL_STATIC_DRAW);
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
    size FragShaderSize;
    char *FragShaderSource = (char *)File_ReadEntireFile("../code/gfx/gl/shader.frag", &FragShaderSize);
    string FragShader = String(FragShaderSource, FragShaderSize);
    GL.SDFShader = OpenGL_LoadProgram(VertShader, FragShader);
    
    GL.SplatShader = OpenGL_LoadProgram(SplatVertShader, SplatFragShader);
    vec3 CameraPosition = Vec3(0,0,3);
    quat CameraRotation = Quat_Id();
    mat4 Translation = Mat4_Translation(Vec3_Negate(CameraPosition));
    mat4 Rotation = Mat4_Rotation(Quat_Negate(CameraRotation));
    mat4 Perspective = Mat4_InfinitePerspective(45.0f, 9.0f/16.0f, 0.1f);
    mat4 WorldToClip = Perspective * Rotation * Translation;
    glUseProgram(GL.SplatShader);
    u32 UniformLocation = glGetUniformLocation(GL.SplatShader, "WorldToCamera");
    glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, WorldToClip.E);
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(splat), (GLvoid *)offsetof(splat, Position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(splat), (GLvoid *)offsetof(splat, Normal));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
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
OpenGL_Frame()
{
    glUseProgram(GL.SDFShader);
    // OpenGL_DrawFullscreenQuad();
    OpenGL_DrawSplats();
}
