#define GLSL_VERT_POSITION "P"
#define GLSL_VERT_NORMAL "N"
#define GLSL_VERT_COLOR "C"

ez_string VertShader = 
    "#version 460\n"
    "in vec2 " GLSL_VERT_POSITION ";"
    "out vec2 ScreenPos;"
    "uniform vec2 R;"
    "void main(){"
        "ScreenPos = " GLSL_VERT_POSITION " * 2 - 1;"
        "gl_Position.xy = ScreenPos;"
    "}\0";
#if 1
ez_string SplatVertShader = 
    "#version 460\n"
    "in vec3 " GLSL_VERT_POSITION ";"
    "in vec3 " GLSL_VERT_NORMAL ";"
    "in vec3 " GLSL_VERT_COLOR ";"
    "out VS_OUT {"
        "vec3 Color;"
        "vec3 Normal;"
    "} vs_out;"
    "void main(){"
        "gl_Position = vec4(P,1);"
        "vs_out.Normal = " GLSL_VERT_NORMAL ";"
        "vs_out.Color = " GLSL_VERT_COLOR ";"
    "}\0";

ez_string SplatGeomShader = 
    "#version 460\n"
    "layout (points) in;"
    "layout (triangle_strip, max_vertices = 4) out;"
    "uniform mat4 WorldToCamera;"
    "in VS_OUT {"
        "vec3 Color;"
        "vec3 Normal;"
    "} gs_in[];"
    "out GS_OUT {"
        "vec3 Color;"
        "vec3 Normal;"
        "vec2 UV;"
    "} gs_out;"
    "void main(){"
        "vec3 UpVector = abs(gs_in[0].Normal.y) < 0.7 ? vec3(0,1,0) : vec3(1,0,0);"
        "vec3 Right = cross(UpVector, gs_in[0].Normal);"
        "vec3 Up = cross(gs_in[0].Normal, Right);"
        "Right *= 0.0025;"
        "Up *= 0.0025;"

        "gl_Position = WorldToCamera * vec4(gl_in[0].gl_Position.xyz - Right - Up,1);"
        "gs_out.Color = gs_in[0].Color;"
        "gs_out.Normal = gs_in[0].Normal;"
        "gs_out.UV = vec2(-1,-1);"
        "EmitVertex();"

        "gl_Position = WorldToCamera * vec4(gl_in[0].gl_Position.xyz + Right - Up,1);"
        "gs_out.Color = gs_in[0].Color;"
        "gs_out.Normal = gs_in[0].Normal;"
        "gs_out.UV = vec2(1,-1);"
        "EmitVertex();"

        "gl_Position = WorldToCamera * vec4(gl_in[0].gl_Position.xyz - Right + Up,1);"
        "gs_out.Color = gs_in[0].Color;"
        "gs_out.Normal = gs_in[0].Normal;"
        "gs_out.UV = vec2(-1,1);"
        "EmitVertex();"

        "gl_Position = WorldToCamera * vec4(gl_in[0].gl_Position.xyz + Right + Up,1);"
        "gs_out.Color = gs_in[0].Color;"
        "gs_out.Normal = gs_in[0].Normal;"
        "gs_out.UV = vec2(1,1);"
        "EmitVertex();"
        
        "EndPrimitive();"
    "}\0";

ez_string SplatFragShader = 
    "#version 460\n"
    "out vec4 Out;"
    "in GS_OUT {"
        "vec3 Color;"
        "vec3 Normal;"
        "vec2 UV;"
    "} fs_in;"
    "void main(){"
        "Out.xyz = vec3(dot(normalize(fs_in.Normal), normalize(vec3(0.5,1,1))));"
        "Out.xyz = fs_in.Color * max(Out.xyz,0.5);"
        "Out.a = 1.0-smoothstep(0.8, 1.0, length(fs_in.UV));"
    "}\0";
#else
string SplatVertShader = 
    "#version 460\n"
    "in vec3 " GLSL_VERT_POSITION ";"
    "in vec3 " GLSL_VERT_NORMAL ";"
    "in vec3 " GLSL_VERT_COLOR ";"
    "out VS_OUT {"
        "vec3 Color;"
        "vec3 Normal;"
    "} vs_out;"
    "void main(){"
        "gl_Position = vec4(P,1);"
        "vs_out.Normal = " GLSL_VERT_NORMAL ";"
        "vs_out.Color = " GLSL_VERT_COLOR ";"
    "}\0";
string SplatGeomShader = 
    "#version 460\n"
    "layout (points) in;"
    "layout (points, max_vertices = 1) out;"
    "uniform mat4 WorldToCamera;"
    "in VS_OUT {"
        "vec3 Color;"
        "vec3 Normal;"
    "} gs_in[];"
    "out GS_OUT {"
        "vec3 Color;"
        "vec3 Normal;"
    "} gs_out;"
    "void main(){"
        "gl_Position = WorldToCamera * vec4(gl_in[0].gl_Position.xyz,1);"
        "gs_out.Color = gs_in[0].Color;"
        "gs_out.Normal = gs_in[0].Normal;"
        "EmitVertex();"
        "EndPrimitive();"
    "}\0";
string SplatFragShader = 
    "#version 460\n"
    "out vec4 Out;"
    "in GS_OUT {"
        "vec3 Color;"
        "vec3 Normal;"
    "} fs_in;"
    "void main(){"
        "Out.xyz = vec3(dot(normalize(fs_in.Normal), normalize(vec3(0.5,1,1))));"
        "Out.xyz = fs_in.Color * max(Out.xyz,0.5);"
    "}\0";
#endif

static u32
OpenGL_CompileShader(u32 Type, ez_string Shader)
{
    u32 ShaderID = glCreateShader(Type);
    glShaderSource(ShaderID, 1, &Shader.Data, (i32 *)&Shader.Length);
    glCompileShader(ShaderID);
    i32 ShaderCompiled = GL_FALSE;
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &ShaderCompiled);
    if (ShaderCompiled != GL_TRUE)
    {
        i32 LogLength;
        glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &LogLength);
        char *Error = (char *)ez_StackAlloc(0, LogLength, 1);
        glGetShaderInfoLog(ShaderID, LogLength, &LogLength, Error);
        glDeleteShader(ShaderID);
        ez_ConsoleError(ez_UTF8FromString(ez_String(Error, LogLength)));
        return 0;
    }
    return ShaderID;
}

static u32
OpenGL_LoadProgram(ez_string Vertex, ez_string Geometry, ez_string Fragment)
{
    /* CREATE PROGRAM */
    u32 ProgramID = glCreateProgram();

    /* COMPILE SHADERS */
    u32 VertexShader = OpenGL_CompileShader(GL_VERTEX_SHADER, Vertex);
    if (VertexShader) glAttachShader(ProgramID, VertexShader);
    u32 GeometryShader = OpenGL_CompileShader(GL_GEOMETRY_SHADER, Geometry);
    if (GeometryShader) glAttachShader(ProgramID, GeometryShader);
    u32 FragmentShader = OpenGL_CompileShader(GL_FRAGMENT_SHADER, Fragment);
    if (FragmentShader) glAttachShader(ProgramID, FragmentShader);

    /* LINK PROGRAM */
    glLinkProgram(ProgramID);
    i32 ProgramSuccess = GL_TRUE;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &ProgramSuccess);
    if (ProgramSuccess != GL_TRUE)
    {
        i32 LogLength;
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &LogLength);
        char *Error = (char *)ez_StackAlloc(0, LogLength, 1);
        glGetProgramInfoLog(ProgramID, LogLength, &LogLength, Error);
        glDeleteProgram(ProgramID);
        ez_ConsoleError(ez_UTF8FromString(ez_String(Error, LogLength)));
        return 0;
    }

    /* LINK ATTRIBS AND SET UNIFORMS */
    glUseProgram(ProgramID);
    glBindAttribLocation(ProgramID, 0, GLSL_VERT_POSITION);
    glBindAttribLocation(ProgramID, 1, GLSL_VERT_NORMAL);
    glBindAttribLocation(ProgramID, 2, GLSL_VERT_COLOR);
    glLinkProgram(ProgramID);
    glUseProgram(0);

    /* CLEAN UP */
    glDetachShader(ProgramID, VertexShader);
    glDetachShader(ProgramID, GeometryShader);
    glDetachShader(ProgramID, FragmentShader);
    glDeleteShader(VertexShader);
    glDeleteShader(GeometryShader);
    glDeleteShader(FragmentShader);

    return ProgramID;
}

static u32
OpenGL_CompileShader(u32 Type, const char **ShaderParts, u32 PartCount)
{
    u32 ShaderID = glCreateShader(Type);
    glShaderSource(ShaderID, PartCount, ShaderParts, 0);
    glCompileShader(ShaderID);
    i32 ShaderCompiled = GL_FALSE;
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &ShaderCompiled);
    if (ShaderCompiled != GL_TRUE)
    {
        i32 LogLength;
        glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &LogLength);
        char *Error = (char *)ez_StackAlloc(0, LogLength, 1);
        glGetShaderInfoLog(ShaderID, LogLength, &LogLength, Error);
        glDeleteShader(ShaderID);
        ez_ConsoleError(ez_UTF8FromString(ez_String(Error, LogLength)));
        return 0;
    }
    return ShaderID;
}

static u32
OpenGL_LoadComputeProgram(const char **ShaderParts, u32 PartCount)
{
    /* CREATE PROGRAM */
    u32 ProgramID = glCreateProgram();

    /* COMPILE SHADERS */
    u32 ComputeShader = OpenGL_CompileShader(GL_COMPUTE_SHADER, ShaderParts, PartCount);
    if (ComputeShader) glAttachShader(ProgramID, ComputeShader);

    /* LINK PROGRAM */
    glLinkProgram(ProgramID);
    i32 ProgramSuccess = GL_TRUE;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &ProgramSuccess);
    if (ProgramSuccess != GL_TRUE)
    {
        i32 LogLength;
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &LogLength);
        char *Error = (char *)ez_StackAlloc(0, LogLength, 1);
        glGetProgramInfoLog(ProgramID, LogLength, &LogLength, Error);
        glDeleteProgram(ProgramID);
        ez_ConsoleError(ez_UTF8FromString(ez_String(Error, LogLength)));
        return 0;
    }

    /* CLEAN UP */
    glDetachShader(ProgramID, ComputeShader);
    glDeleteShader(ComputeShader);

    return ProgramID;
}
