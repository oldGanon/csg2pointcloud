#define GL_FUNC(NAME, name) static PFN##NAME##PROC name

/* 1.2 */
GL_FUNC(GLTEXIMAGE3D,               glTexImage3D);
GL_FUNC(GLTEXSUBIMAGE3D,            glTexSubImage3D);

/* 2.0 */
GL_FUNC(GLGENBUFFERS,               glGenBuffers);
GL_FUNC(GLBINDBUFFER,               glBindBuffer);
GL_FUNC(GLDELETEBUFFERS,            glDeleteBuffers);
GL_FUNC(GLBUFFERDATA,               glBufferData);
GL_FUNC(GLBUFFERSUBDATA,            glBufferSubData);
GL_FUNC(GLCREATESHADER,             glCreateShader);
GL_FUNC(GLCOMPILESHADER,            glCompileShader);
GL_FUNC(GLDELETESHADER,             glDeleteShader);
GL_FUNC(GLSHADERSOURCE,             glShaderSource);
GL_FUNC(GLGETSHADERIV,              glGetShaderiv);
GL_FUNC(GLATTACHSHADER,             glAttachShader);
GL_FUNC(GLGETSHADERINFOLOG,         glGetShaderInfoLog);
GL_FUNC(GLDETACHSHADER,             glDetachShader);
GL_FUNC(GLCREATEPROGRAM,            glCreateProgram);
GL_FUNC(GLLINKPROGRAM,              glLinkProgram);
GL_FUNC(GLUSEPROGRAM,               glUseProgram);
GL_FUNC(GLGETPROGRAMIV,             glGetProgramiv);
GL_FUNC(GLGETPROGRAMINFOLOG,        glGetProgramInfoLog);
GL_FUNC(GLDELETEPROGRAM,            glDeleteProgram);
GL_FUNC(GLENABLEVERTEXATTRIBARRAY,  glEnableVertexAttribArray);
GL_FUNC(GLDISABLEVERTEXATTRIBARRAY, glDisableVertexAttribArray);
GL_FUNC(GLVERTEXATTRIBPOINTER,      glVertexAttribPointer);
GL_FUNC(GLBINDATTRIBLOCATION,       glBindAttribLocation);
GL_FUNC(GLACTIVETEXTURE,            glActiveTexture);
GL_FUNC(GLBLENDFUNCSEPARATE,        glBlendFuncSeparate);
GL_FUNC(GLGETUNIFORMLOCATION,       glGetUniformLocation);
GL_FUNC(GLUNIFORM1F,                glUniform1f);
GL_FUNC(GLUNIFORM2F,                glUniform2f);
GL_FUNC(GLUNIFORM3F,                glUniform3f);
GL_FUNC(GLUNIFORM4F,                glUniform4f);
GL_FUNC(GLUNIFORM1I,                glUniform1i);
GL_FUNC(GLUNIFORM2I,                glUniform2i);
GL_FUNC(GLUNIFORM3I,                glUniform3i);
GL_FUNC(GLUNIFORM4I,                glUniform4i);
GL_FUNC(GLUNIFORM1FV,               glUniform1fv);
GL_FUNC(GLUNIFORM2FV,               glUniform2fv);
GL_FUNC(GLUNIFORM3FV,               glUniform3fv);
GL_FUNC(GLUNIFORM2IV,               glUniform2iv);
GL_FUNC(GLUNIFORMMATRIX4FV,         glUniformMatrix4fv);
GL_FUNC(GLVERTEXATTRIB2F,           glVertexAttrib2f);
GL_FUNC(GLVERTEXATTRIB3F,           glVertexAttrib3f);
GL_FUNC(GLVERTEXATTRIB4F,           glVertexAttrib4f);
GL_FUNC(GLVERTEXATTRIB3FV,          glVertexAttrib3fv);
GL_FUNC(GLGETBUFFERSUBDATA,         glGetBufferSubData);

/* 3.0 */
GL_FUNC(GLGENFRAMEBUFFERS,                glGenFramebuffers);
GL_FUNC(GLBINDFRAMEBUFFER,                glBindFramebuffer);
GL_FUNC(GLFRAMEBUFFERTEXTURE2D,           glFramebufferTexture2D);
GL_FUNC(GLCHECKFRAMEBUFFERSTATUS,         glCheckFramebufferStatus);
GL_FUNC(GLGENRENDERBUFFERS,               glGenRenderbuffers);
GL_FUNC(GLBINDRENDERBUFFER,               glBindRenderbuffer);
GL_FUNC(GLRENDERBUFFERSTORAGE,            glRenderbufferStorage);
GL_FUNC(GLFRAMEBUFFERRENDERBUFFER,        glFramebufferRenderbuffer);
GL_FUNC(GLGENERATEMIPMAP,                 glGenerateMipmap);
GL_FUNC(GLVERTEXATTRIBIPOINTER,           glVertexAttribIPointer);
GL_FUNC(GLMAPBUFFERRANGE,                 glMapBufferRange);
GL_FUNC(GLBINDVERTEXARRAY,                glBindVertexArray);
GL_FUNC(GLGENVERTEXARRAYS,                glGenVertexArrays);
GL_FUNC(GLBLITFRAMEBUFFER,                glBlitFramebuffer);
GL_FUNC(GLRENDERBUFFERSTORAGEMULTISAMPLE, glRenderbufferStorageMultisample);
GL_FUNC(GLGETINTEGERI_V,                  glGetIntegeri_v);
GL_FUNC(GLBINDBUFFERBASE,                 glBindBufferBase);

/* 3.1 */
GL_FUNC(GLCOPYBUFFERSUBDATA,        glCopyBufferSubData);

/* 3.2 */
GL_FUNC(GLTEXIMAGE2DMULTISAMPLE,    glTexImage2DMultisample);
GL_FUNC(GLFENCESYNC,                glFenceSync);
GL_FUNC(GLCLIENTWAITSYNC,           glClientWaitSync);
GL_FUNC(GLDELETESYNC,               glDeleteSync);

/* 4.2 */
GL_FUNC(GLMEMORYBARRIER,            glMemoryBarrier);
    
/* 4.3 */
GL_FUNC(GLDISPATCHCOMPUTE,          glDispatchCompute);
GL_FUNC(GLDISPATCHCOMPUTEINDIRECT,  glDispatchComputeIndirect);
/* 4.4 */
GL_FUNC(GLBUFFERSTORAGE,            glBufferStorage);

/* ES 2.0 */
GL_FUNC(GLCLEARDEPTHF, glClearDepthf);
GL_FUNC(GLDEPTHRANGEF, glDepthRangef);

#define GL_LOAD_AND_CHECK(NAME, name) name = (PFN##NAME##PROC)GL_LOAD_FUNC(#name); if(!name) return false;

static b32
GL_LoadFunctions()
{
    /* 1.2 */
    GL_LOAD_AND_CHECK(GLTEXIMAGE3D,               glTexImage3D);
    GL_LOAD_AND_CHECK(GLTEXSUBIMAGE3D,            glTexSubImage3D);

    /* 2.0 */
    GL_LOAD_AND_CHECK(GLGENBUFFERS,               glGenBuffers);
    GL_LOAD_AND_CHECK(GLBINDBUFFER,               glBindBuffer);
    GL_LOAD_AND_CHECK(GLDELETEBUFFERS,            glDeleteBuffers);
    GL_LOAD_AND_CHECK(GLBUFFERDATA,               glBufferData);
    GL_LOAD_AND_CHECK(GLCREATEPROGRAM,            glCreateProgram);
    GL_LOAD_AND_CHECK(GLCREATESHADER,             glCreateShader);
    GL_LOAD_AND_CHECK(GLDELETESHADER,             glDeleteShader);
    GL_LOAD_AND_CHECK(GLSHADERSOURCE,             glShaderSource);
    GL_LOAD_AND_CHECK(GLCOMPILESHADER,            glCompileShader);
    GL_LOAD_AND_CHECK(GLGETSHADERIV,              glGetShaderiv);
    GL_LOAD_AND_CHECK(GLATTACHSHADER,             glAttachShader);
    GL_LOAD_AND_CHECK(GLGETSHADERINFOLOG,         glGetShaderInfoLog);
    GL_LOAD_AND_CHECK(GLDETACHSHADER,             glDetachShader);
    GL_LOAD_AND_CHECK(GLLINKPROGRAM,              glLinkProgram);
    GL_LOAD_AND_CHECK(GLGETPROGRAMIV,             glGetProgramiv);
    GL_LOAD_AND_CHECK(GLUSEPROGRAM,               glUseProgram);
    GL_LOAD_AND_CHECK(GLGETPROGRAMINFOLOG,        glGetProgramInfoLog);
    GL_LOAD_AND_CHECK(GLDELETEPROGRAM,            glDeleteProgram);
    GL_LOAD_AND_CHECK(GLENABLEVERTEXATTRIBARRAY,  glEnableVertexAttribArray);
    GL_LOAD_AND_CHECK(GLDISABLEVERTEXATTRIBARRAY, glDisableVertexAttribArray);
    GL_LOAD_AND_CHECK(GLVERTEXATTRIBPOINTER,      glVertexAttribPointer);
    GL_LOAD_AND_CHECK(GLGETUNIFORMLOCATION,       glGetUniformLocation);
    GL_LOAD_AND_CHECK(GLACTIVETEXTURE,            glActiveTexture);
    GL_LOAD_AND_CHECK(GLBUFFERSUBDATA,            glBufferSubData);
    GL_LOAD_AND_CHECK(GLBINDATTRIBLOCATION,       glBindAttribLocation);
    GL_LOAD_AND_CHECK(GLBLENDFUNCSEPARATE,        glBlendFuncSeparate);
    GL_LOAD_AND_CHECK(GLUNIFORM1F,                glUniform1f);
    GL_LOAD_AND_CHECK(GLUNIFORM2F,                glUniform2f);
    GL_LOAD_AND_CHECK(GLUNIFORM3F,                glUniform3f);
    GL_LOAD_AND_CHECK(GLUNIFORM4F,                glUniform4f);
    GL_LOAD_AND_CHECK(GLUNIFORM1I,                glUniform1i);
    GL_LOAD_AND_CHECK(GLUNIFORM2I,                glUniform2i);
    GL_LOAD_AND_CHECK(GLUNIFORM3I,                glUniform3i);
    GL_LOAD_AND_CHECK(GLUNIFORM4I,                glUniform4i);
    GL_LOAD_AND_CHECK(GLUNIFORM1FV,               glUniform1fv);
    GL_LOAD_AND_CHECK(GLUNIFORM2FV,               glUniform2fv);
    GL_LOAD_AND_CHECK(GLUNIFORM3FV,               glUniform3fv);
    GL_LOAD_AND_CHECK(GLUNIFORM2IV,               glUniform2iv);
    GL_LOAD_AND_CHECK(GLUNIFORMMATRIX4FV,         glUniformMatrix4fv);
    GL_LOAD_AND_CHECK(GLVERTEXATTRIB2F,           glVertexAttrib2f);
    GL_LOAD_AND_CHECK(GLVERTEXATTRIB3F,           glVertexAttrib3f);
    GL_LOAD_AND_CHECK(GLVERTEXATTRIB4F,           glVertexAttrib4f);
    GL_LOAD_AND_CHECK(GLVERTEXATTRIB3FV,          glVertexAttrib3fv);
    GL_LOAD_AND_CHECK(GLGETBUFFERSUBDATA,         glGetBufferSubData);

    /* 3.0 */
    GL_LOAD_AND_CHECK(GLGENFRAMEBUFFERS,                glGenFramebuffers);
    GL_LOAD_AND_CHECK(GLBINDFRAMEBUFFER,                glBindFramebuffer);
    GL_LOAD_AND_CHECK(GLFRAMEBUFFERTEXTURE2D,           glFramebufferTexture2D);
    GL_LOAD_AND_CHECK(GLCHECKFRAMEBUFFERSTATUS,         glCheckFramebufferStatus);
    GL_LOAD_AND_CHECK(GLGENRENDERBUFFERS,               glGenRenderbuffers);
    GL_LOAD_AND_CHECK(GLBINDRENDERBUFFER,               glBindRenderbuffer);
    GL_LOAD_AND_CHECK(GLRENDERBUFFERSTORAGE,            glRenderbufferStorage);
    GL_LOAD_AND_CHECK(GLFRAMEBUFFERRENDERBUFFER,        glFramebufferRenderbuffer);
    GL_LOAD_AND_CHECK(GLGENERATEMIPMAP,                 glGenerateMipmap);
    GL_LOAD_AND_CHECK(GLVERTEXATTRIBIPOINTER,           glVertexAttribIPointer);
    GL_LOAD_AND_CHECK(GLMAPBUFFERRANGE,                 glMapBufferRange);
    GL_LOAD_AND_CHECK(GLBINDVERTEXARRAY,                glBindVertexArray);
    GL_LOAD_AND_CHECK(GLGENVERTEXARRAYS,                glGenVertexArrays);
    GL_LOAD_AND_CHECK(GLBLITFRAMEBUFFER,                glBlitFramebuffer);
    GL_LOAD_AND_CHECK(GLRENDERBUFFERSTORAGEMULTISAMPLE, glRenderbufferStorageMultisample);
    GL_LOAD_AND_CHECK(GLGETINTEGERI_V,                  glGetIntegeri_v);
    GL_LOAD_AND_CHECK(GLBINDBUFFERBASE,                 glBindBufferBase);

    /* 3.1 */
    GL_LOAD_AND_CHECK(GLCOPYBUFFERSUBDATA,        glCopyBufferSubData);

    /* 3.2 */
    GL_LOAD_AND_CHECK(GLTEXIMAGE2DMULTISAMPLE,    glTexImage2DMultisample);
    GL_LOAD_AND_CHECK(GLFENCESYNC,                glFenceSync);
    GL_LOAD_AND_CHECK(GLCLIENTWAITSYNC,           glClientWaitSync);
    GL_LOAD_AND_CHECK(GLDELETESYNC,               glDeleteSync);

    /* 4.2 */
    GL_LOAD_AND_CHECK(GLMEMORYBARRIER,            glMemoryBarrier);

    /* 4.3 */
    GL_LOAD_AND_CHECK(GLDISPATCHCOMPUTE,          glDispatchCompute);
    GL_LOAD_AND_CHECK(GLDISPATCHCOMPUTEINDIRECT,  glDispatchComputeIndirect);

    /* 4.4 */
    GL_LOAD_AND_CHECK(GLBUFFERSTORAGE,            glBufferStorage);

    /* ES 2.0 */
    GL_LOAD_AND_CHECK(GLCLEARDEPTHF, glClearDepthf);
    GL_LOAD_AND_CHECK(GLDEPTHRANGEF, glDepthRangef);

    return true;
}
