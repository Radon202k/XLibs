#ifndef XOPENGL_LOADER_H
#define XOPENGL_LOADER_H

#ifdef _WIN32
// TODO: Windows
#else
/* NOTE: Tested on Ubuntu 22.04.2 LTS (22.04, jammy) */
#include <EGL/egl.h>
#include <GL/glcorearb.h>
#endif

// make sure you use functions that are valid for selected GL version (specified when context is created)
#define GL_FUNCTIONS(X) \
X(PFNGLENABLEPROC,                   glEnable                   ) \
X(PFNGLDISABLEPROC,                  glDisable                  ) \
X(PFNGLBLENDFUNCPROC,                glBlendFunc                ) \
X(PFNGLVIEWPORTPROC,                 glViewport                 ) \
X(PFNGLCLEARCOLORPROC,               glClearColor               ) \
X(PFNGLCLEARPROC,                    glClear                    ) \
X(PFNGLDRAWARRAYSPROC,               glDrawArrays               ) \
X(PFNGLDRAWELEMENTSPROC,             glDrawElements             ) \
X(PFNGLCREATEBUFFERSPROC,            glCreateBuffers            ) \
X(PFNGLNAMEDBUFFERSTORAGEPROC,       glNamedBufferStorage       ) \
X(PFNGLNAMEDBUFFERSUBDATAPROC,       glNamedBufferSubData       ) \
X(PFNGLBINDVERTEXARRAYPROC,          glBindVertexArray          ) \
X(PFNGLENABLEVERTEXATTRIBARRAYPROC,  glEnableVertexAttribArray  ) \
X(PFNGLVERTEXATTRIBPOINTERPROC,      glVertexAttribPointer      ) \
X(PFNGLCREATEVERTEXARRAYSPROC,       glCreateVertexArrays       ) \
X(PFNGLVERTEXATTRIBDIVISORPROC,      glVertexAttribDivisor      ) \
X(PFNGLVERTEXARRAYBINDINGDIVISORPROC,glVertexArrayBindingDivisor) \
X(PFNGLVERTEXARRAYATTRIBBINDINGPROC, glVertexArrayAttribBinding ) \
X(PFNGLVERTEXARRAYVERTEXBUFFERPROC,  glVertexArrayVertexBuffer  ) \
X(PFNGLVERTEXARRAYATTRIBFORMATPROC,  glVertexArrayAttribFormat  ) \
X(PFNGLENABLEVERTEXARRAYATTRIBPROC,  glEnableVertexArrayAttrib  ) \
X(PFNGLCREATESHADERPROGRAMVPROC,     glCreateShaderProgramv     ) \
X(PFNGLGETPROGRAMIVPROC,             glGetProgramiv             ) \
X(PFNGLGENTEXTURESPROC,              glGenTextures              ) \
X(PFNGLGENBUFFERSPROC,               glGenBuffers               ) \
X(PFNGLGETERRORPROC,                 glGetError                 ) \
X(PFNGLBINDTEXTUREPROC,              glBindTexture              ) \
X(PFNGLTEXIMAGE2DPROC,               glTexImage2D               ) \
X(PFNGLTEXPARAMETERIPROC,            glTexParameteri            ) \
X(PFNGLGETPROGRAMINFOLOGPROC,        glGetProgramInfoLog        ) \
X(PFNGLGENPROGRAMPIPELINESPROC,      glGenProgramPipelines      ) \
X(PFNGLUSEPROGRAMSTAGESPROC,         glUseProgramStages         ) \
X(PFNGLBINDPROGRAMPIPELINEPROC,      glBindProgramPipeline      ) \
X(PFNGLBINDVERTEXBUFFERPROC,         glBindVertexBuffer         ) \
X(PFNGLBINDBUFFERPROC,               glBindBuffer               ) \
X(PFNGLPROGRAMUNIFORMMATRIX2FVPROC,  glProgramUniformMatrix2fv  ) \
X(PFNGLPROGRAMUNIFORMMATRIX4FVPROC,  glProgramUniformMatrix4fv  ) \
X(PFNGLBINDTEXTUREUNITPROC,          glBindTextureUnit          ) \
X(PFNGLCREATETEXTURESPROC,           glCreateTextures           ) \
X(PFNGLTEXTUREPARAMETERIPROC,        glTextureParameteri        ) \
X(PFNGLTEXTURESTORAGE2DPROC,         glTextureStorage2D         ) \
X(PFNGLTEXTURESUBIMAGE2DPROC,        glTextureSubImage2D        ) \
X(PFNGLBUFFERDATAPROC,               glBufferData               ) \
X(PFNGLBUFFERSUBDATAPROC,            glBufferSubData            ) \
X(PFNGLDRAWELEMENTSINSTANCEDPROC,    glDrawElementsInstanced    ) \
X(PFNGLDEBUGMESSAGECALLBACKPROC,     glDebugMessageCallback     )

#define X(type, name) static type name;
GL_FUNCTIONS(X)
#undef X

#define STR2(x) #x
#define STR(x) STR2(x)

#ifndef NDEBUG
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                   GLsizei length, const GLchar* message, const void* user) {
    fprintf(stderr, "%s\n", message);
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
        assert(!"OpenGL API usage error! Use debugger to examine call stack!");
}
#endif

#endif //XOPENGL_LOADER_H
