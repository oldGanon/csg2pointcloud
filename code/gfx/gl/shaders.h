#define GLSL_VERT_POSITION "P"
#define GLSL_VERT_NORMAL "N"

string VertShader = 
    "#version 460\n"
    "in vec2 " GLSL_VERT_POSITION ";"
    "out vec2 ScreenPos;"
    "uniform vec2 R;"
    "void main(){"
        "ScreenPos = " GLSL_VERT_POSITION " * 2 - 1;"
        "gl_Position.xy = ScreenPos;"
    "}\0";

string SplatVertShader = 
    "#version 460\n"
    "in vec3 " GLSL_VERT_POSITION ";"
    "in vec3 " GLSL_VERT_NORMAL ";"
    "uniform mat4 WorldToCamera;"
    "out vec3 Normal;"
    "void main(){"
        "Normal =  N;"
        "gl_Position = WorldToCamera * vec4(P,1);"
    "}\0";

string SplatFragShader = 
    "#version 460\n"
    "in vec3 Normal;"
    "out vec4 Out;"
    "void main(){"
        "Out.xyz = vec3(dot(normalize(Normal), normalize(vec3(0.5,1,1))));"
        "if (Normal.z < 0) discard;"
        "Out.xyz = mix(vec3(0.8,0.5,0.35),vec3(1,0.878,0.741),Out.xyz);"
    "}\0";

static u32
OpenGL_CompileShader(u32 Type, string Shader)
{
    u32 ShaderID = glCreateShader(Type);
    const char *CShader = CString(Shader);
    glShaderSource(ShaderID, 1, &CShader, 0);
    glCompileShader(ShaderID);
    i32 ShaderCompiled = GL_FALSE;
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &ShaderCompiled);
    if (ShaderCompiled != GL_TRUE)
    {
        i32 LogLength;
        glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &LogLength);
        char *Error = (char *)Api_Talloc(LogLength);
        glGetShaderInfoLog(ShaderID, LogLength, &LogLength, Error);
        glDeleteShader(ShaderID);
        Api_Error(String(Error, LogLength));
        return 0;
    }
    return ShaderID;
}

static u32
OpenGL_LoadProgram(string Vertex, string Fragment)
{
    /* COMPILE SHADERS */
    u32 VertexShader = OpenGL_CompileShader(GL_VERTEX_SHADER, Vertex);
    if (!VertexShader) return 0;
    u32 FragmentShader = OpenGL_CompileShader(GL_FRAGMENT_SHADER, Fragment);
    if (!FragmentShader) return 0;

    /* CREATE PROGRAM */
    u32 ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShader);
    glAttachShader(ProgramID, FragmentShader);

    /* LINK PROGRAM */
    glLinkProgram(ProgramID);
    i32 ProgramSuccess = GL_TRUE;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &ProgramSuccess);
    if (ProgramSuccess != GL_TRUE)
    {
        i32 LogLength;
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &LogLength);
        char *Error = (char *)Api_Talloc(LogLength);
        glGetProgramInfoLog(ProgramID, LogLength, &LogLength, Error);
        glDeleteProgram(ProgramID);
        Api_Error(String(Error, LogLength));
        return 0;
    }

    /* LINK ATTRIBS AND SET UNIFORMS */
    glUseProgram(ProgramID);
    glBindAttribLocation(ProgramID, 0, GLSL_VERT_POSITION);
    glBindAttribLocation(ProgramID, 1, GLSL_VERT_NORMAL);
    glLinkProgram(ProgramID);
    glUseProgram(0);

    /* CLEAN UP */
    glDetachShader(ProgramID, VertexShader);
    glDetachShader(ProgramID, FragmentShader);
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    return ProgramID;
}
