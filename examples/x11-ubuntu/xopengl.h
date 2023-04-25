#ifndef XOPENGL_H
#define XOPENGL_H

#ifdef _WIN32
#else
#include <EGL/egl.h>
#include <GL/glcorearb.h>
#endif

typedef enum XGLDrawType {
    XGLDrawType_null,
    XGLDrawType_normal,
    XGLDrawType_indexed,
    XGLDrawType_instanced,
} XGLDrawType;

typedef enum XGLPassType {
    XGLPassType_null,
    XGLPassType_mesh,
    XGLPassType_font,
} XGLPassType;

typedef struct XGLShader {
    GLuint pipeline;
    GLuint vshader;
    GLuint fshader;
} XGLShader;

typedef struct XGLCommandMesh {
    Mesh *mesh;
    Transform transform;
    GLuint texture;
} XGLCommandMesh;

typedef struct XGLCommandQuad {
    v2 pA, pB, pC, pD;
    v2 uvA,uvB, uvC, uvD;
    v4 color;
} XGLCommandQuad;

struct XFont;
typedef struct XGLPass {
    XGLPassType type;
    XGLDrawType drawType;
    Camera *camera;
    XGLShader *shader;
    GLuint vao;
    /* Font pass */
    struct XFont *font;
    GLuint fontPassVbo;
    GLuint fontPassEbo;
    XGLCommandMesh meshCommands[4096];
    u32 meshCommandIndex;
    XGLCommandQuad quadCommands[4096];
    u32 quadCommandIndex;
} XGLPass;

static void   xgl_construct          (void);
static GLuint xgl_texture_from_png   (char *path);
static void   xgl_shader_from_files  (XGLShader *shader, char *vsPath, char *fsPath);
static void   xgl_push_mesh          (XGLPass *pass, v3 p, v3 s, versor r, u32 texture, Mesh *m);

static void   xgl_render_frame       (XGLPass passes[32], u32 passIndex);

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

static void FatalError(const char* message) {
    fprintf(stderr, "%s\n", message);
    
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "zenity --error --no-wrap --text=\"%s\"", message);
    system(cmd);
    
    exit(0);
}

#ifndef NDEBUG
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                   GLsizei length, const GLchar* message, const void* user) {
    fprintf(stderr, "%s\n", message);
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
        assert(!"OpenGL API usage error! Use debugger to examine call stack!");
}
#endif

#endif //XOPENGL_H
