#ifndef XOPENGL_H
#define XOPENGL_H

typedef enum XGLDrawType {
    XGLDrawType_null,
    XGLDrawType_normal,
    XGLDrawType_indexed,
    XGLDrawType_instanced,
} XGLDrawType;

typedef enum XGLPassType {
    XGLPassType_null,
    XGLPassType_mesh,
    XGLPassType_quad,
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

typedef struct XGLQuadVertex {
    v2 position;
    v2 texCoord;
    v4 color;
} XGLQuadVertex;

typedef struct XFont {
    s32 w, h;
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    GLuint ftex;
    f32 ascent;
    f32 descent;
    f32 lineGap;
} XGLFont;

typedef struct XGLPass {
    XGLPassType type;
    XGLDrawType drawType;
    Camera *camera;
    XGLShader *shader;
    GLuint vao;
    /* Quad/Font pass */
    struct XFont *font;
    GLuint quadVbo;
    GLuint quadEbo;
    GLuint quadTex;
    XGLCommandQuad quadCommands[4096];
    u32 quadCommandIndex;
    /* Mesh pass */
    XGLCommandMesh meshCommands[4096];
    u32 meshCommandIndex;
} XGLPass;

static void   xgl_construct          (void);
static GLuint xgl_texture_from_png   (char *path);
static void   xgl_shader_from_files  (XGLShader *shader, char *vsPath, char *fsPath);
static void   xgl_render_frame       (XGLPass passes[32], u32 passIndex);

/* Internal drawing functions */
static void   xgl_push_mesh          (XGLPass *pass, v3 p, v3 s, versor r, u32 texture, Mesh *m);
static void   xgl_push_quad          (XGLPass *pass, 
                                      v2  pA, v2  pB, v2  pC, v2  pD,
                                      v2 uvA, v2 uvB, v2 uvC, v2 uvD,
                                      v4 color);
static void   xgl_push_text          (XGLPass *pass, v2 p, f32 scale, v4 color, char *text);


static void   draw_text (XGLPass *pass, v2 p, f32 scale, v4 color, char *text);
static void   draw_rect (XGLPass *pass, v2 p, v2 dim, v4 color);





















//



















static void
xgl_construct(void) {
    // setup global GL state
    {
        // enable alpha blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glEnable(GL_DEPTH_TEST);
        
        // disable culling
        glEnable(GL_CULL_FACE);
    }
    
    // use 0 to disable vsync
    x11.enableVSync = true;
    EGLBoolean ok = eglSwapInterval(x11.display, x11.enableVSync);
    assert(ok && "Failed to set vsync for EGL");
    
    x11.isRunning = 1;
    clock_gettime(CLOCK_MONOTONIC, &x11.c1);
}

static void
xgl_upload_mesh(Mesh *mesh, XGLPass *pass) {
    // vertex buffer containing triangle vertices
    glCreateBuffers(1, &mesh->vbo);
    glNamedBufferStorage(mesh->vbo, mesh->vertexCount*sizeof(MeshVertex), mesh->vertices, 0);
    
    glCreateBuffers(1, &mesh->ebo);
    glNamedBufferStorage(mesh->ebo, mesh->indexCount*sizeof(GLuint), mesh->indices, 0);
    
    GLint vbufIndex = 0;
    glVertexArrayVertexBuffer(pass->vao, vbufIndex, mesh->vbo, 0, sizeof(MeshVertex));
}

static GLuint
xgl_texture_from_bytes(u32 width, u32 height, u8 *bytes) {
    GLuint result = -1;
    glCreateTextures(GL_TEXTURE_2D, 1, &result);
    /* Set up filtering */
    glTextureParameteri(result, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(result, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(result, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(result, GL_TEXTURE_WRAP_T, GL_REPEAT);
    /* Allocate/upload data to gpu */
    glTextureStorage2D (result, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(result, 0, 0, 0, width, height, 
                        GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    return result;
}

static GLuint
xgl_texture_from_png(char *path) {
    int width, height, channels;
    unsigned char* imageData = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    
    if (imageData == NULL) {
        fprintf(stderr, "Error loading image: %s\n", stbi_failure_reason());
        exit(1);
    }
    
    GLuint result = xgl_texture_from_bytes(width, height, imageData) ;
    
    // Free the image data
    stbi_image_free(imageData);
    
    return result;
}

static void
xgl_shader_from_files(XGLShader *shader, char *vsPath, char *fsPath) {
    // fragment & vertex shaders for drawing triangle
    XFile vsFile = xfile_read(vsPath);
    XFile fsFile = xfile_read(fsPath);
    if (!vsFile.exists || !fsFile.exists)
        FatalError("Couldn't load shaders");
    
    const char* vshaders[1] = { vsFile.bytes };
    const char* fshaders[1] = { fsFile.bytes };
    
    shader->vshader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, vshaders);
    shader->fshader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, fshaders);
    
    GLint linked;
    glGetProgramiv(shader->vshader, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        char message[1024];
        glGetProgramInfoLog(shader->vshader, sizeof(message), NULL, message);
        fprintf(stderr, "%s\n", message);
        assert(!"Failed to create vertex shader!");
    }
    
    glGetProgramiv(shader->fshader, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        char message[1024];
        glGetProgramInfoLog(shader->fshader, sizeof(message), NULL, message);
        fprintf(stderr, "%s\n", message);
        assert(!"Failed to create fragment shader!");
    }
    
    glGenProgramPipelines(1, &shader->pipeline);
    glUseProgramStages(shader->pipeline, GL_VERTEX_SHADER_BIT, shader->vshader);
    glUseProgramStages(shader->pipeline, GL_FRAGMENT_SHADER_BIT, shader->fshader);
}

static void
xgl_push_mesh(XGLPass *pass, v3 p, v3 scale, versor rotation, u32 texture,
              Mesh *mesh) {
    XGLCommandMesh c = {
        mesh,
        x3d_transform(p, scale, rotation),
        texture,
    };
    
    pass->meshCommands[pass->meshCommandIndex++] = c;
}

static void
xgl_push_quad(XGLPass *pass,
              v2  pA, v2  pB, v2  pC, v2  pD,
              v2 uvA, v2 uvB, v2 uvC, v2 uvD,
              v4 color) {
    XGLCommandQuad c;
    v2_copy(pA, c.pA);
    v2_copy(pB, c.pB);
    v2_copy(pC, c.pC);
    v2_copy(pD, c.pD);
    v2_copy(uvA, c.uvA);
    v2_copy(uvB, c.uvB);
    v2_copy(uvC, c.uvC);
    v2_copy(uvD, c.uvD);
    v4_copy(color, c.color);
    
    pass->quadCommands[pass->quadCommandIndex++] = c;
}

static void 
xgl_render_frame(XGLPass passes[32], u32 passCount) {
    // setup output size covering all client area of window
    glViewport(0, 0, x11.width, x11.height);
    
    // clear screen
    glClearColor(0, 0, 0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    for (u32 passIndex=0; passIndex<passCount; ++passIndex) {
        XGLPass *pass = passes + passIndex;
        // vao
        glBindVertexArray(pass->vao);
        // shader
        glBindProgramPipeline(pass->shader->pipeline);
        if (pass->type == XGLPassType_mesh) {
            // matrices
            mat4 viewRH;
            mat4 perspectiveRH;
            renderer_update_matrices_rh(pass->camera, (f32)x11.width/x11.height, 
                                        viewRH, perspectiveRH);
            glProgramUniformMatrix4fv(pass->shader->vshader, 0, 1, GL_FALSE, (f32 *)perspectiveRH);
            glProgramUniformMatrix4fv(pass->shader->vshader, 1, 1, GL_FALSE, (f32 *)viewRH);
            if (pass->drawType == XGLDrawType_indexed) {
                /* For every mesh */
                for (u32 commandIndex=0; commandIndex<pass->meshCommandIndex; ++commandIndex) {
                    XGLCommandMesh *command = pass->meshCommands + commandIndex;
                    Mesh *mesh = command->mesh;
                    // Bind texture
                    glBindTextureUnit(0, command->texture);
                    // Model matrix
                    mat4 model;
                    x3d_object_transform(command->transform, model);
                    glProgramUniformMatrix4fv(pass->shader->vshader, 2, 1, GL_FALSE, (f32 *)model);
                    // vbo
                    glBindVertexBuffer(0, mesh->vbo, 0, sizeof(MeshVertex));
                    // ebo
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
                    // draw
                    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
                    
                    // Unbind the buffers
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                }
                /* Reset the count of how many meshes to draw per frame */
                pass->meshCommandIndex = 0;
            }
        }
        else if (pass->type == XGLPassType_font ||
                 pass->type == XGLPassType_quad) {
            // Matrix
            mat4 orthoRH;
            mat4_ortho_rh(0, x11.width, x11.height, 0,
                          -1000.0f, 1000.0f, orthoRH);
            
            glProgramUniformMatrix4fv(pass->shader->vshader, 0, 1, GL_FALSE, (f32 *)orthoRH);
            
            // Bind texture
            if (pass->type == XGLPassType_quad)
                glBindTextureUnit(0, pass->quadTex);
            else
                glBindTextureUnit(0, pass->font->ftex);
            
            if (pass->drawType == XGLDrawType_indexed) {
                
                XGLQuadVertex vertices[4096] = {0};
                u32 vertexIndex = 0;
                
                GLuint indices[4096] = {0};
                u32 indexIndex = 0;
                
                /* For every quad */
                for (u32 commandIndex=0; commandIndex<pass->quadCommandIndex; ++commandIndex) {
                    XGLCommandQuad *command = pass->quadCommands + commandIndex;
                    /* indexIndex start at first vertex */
                    u32 istart = vertexIndex;
                    /* A */
                    v2_copy(command->pA, vertices[vertexIndex].position);
                    v2_copy(command->uvA, vertices[vertexIndex].texCoord);
                    v4_copy(command->color, vertices[vertexIndex].color);
                    ++vertexIndex;
                    /* B */
                    v2_copy(command->pB, vertices[vertexIndex].position);
                    v2_copy(command->uvB, vertices[vertexIndex].texCoord);
                    v4_copy(command->color, vertices[vertexIndex].color);
                    ++vertexIndex;
                    /* C */
                    v2_copy(command->pC, vertices[vertexIndex].position);
                    v2_copy(command->uvC, vertices[vertexIndex].texCoord);
                    v4_copy(command->color, vertices[vertexIndex].color);
                    ++vertexIndex;
                    /* D */
                    v2_copy(command->pD, vertices[vertexIndex].position);
                    v2_copy(command->uvD, vertices[vertexIndex].texCoord);
                    v4_copy(command->color, vertices[vertexIndex].color);
                    ++vertexIndex;
                    
                    indices[indexIndex++] = istart + 0;
                    indices[indexIndex++] = istart + 2;
                    indices[indexIndex++] = istart + 1;
                    indices[indexIndex++] = istart + 3;
                    indices[indexIndex++] = istart + 2;
                    indices[indexIndex++] = istart + 0;
                }
                
                // Bind the buffers
                glBindBuffer(GL_ARRAY_BUFFER, pass->quadVbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pass->quadEbo);
                
                // Update the VBO data
                glBufferSubData(GL_ARRAY_BUFFER, 0, vertexIndex*sizeof(XGLQuadVertex), vertices);
                // Update the EBO data
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexIndex*sizeof(GLuint), indices);
                
                // draw
                glDrawElements(GL_TRIANGLES, indexIndex, GL_UNSIGNED_INT, 0);
                
                // Unbind the buffers
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                
                /* Reset the count of how many quads to draw per frame */
                pass->quadCommandIndex = 0;
            }
        }
    }
}

static XGLFont *
xgl_font(char *path, f32 height) {
    XGLFont *result = xalloc(sizeof *result);
    u32 w = 2048;
    u32 h = 2048;
    result->w = w;
    result->h = h;
    /* Temp buffers */
    u8 *tempBitmap = xalloc(w*h);
    /* Open file */
    XFile font = xfile_read(path);
    if (!font.exists)
        assert(!"Could not open font");
    
    stbtt_GetScaledFontVMetrics(font.bytes, 0, height, 
                                &result->ascent, &result->descent, &result->lineGap);
    
    // int line_height = (int)ceil((ascent - descent + line_gap) * scale);
    
    
    /* Bake glyph bitmaps */
    stbtt_BakeFontBitmap(font.bytes,0, height, tempBitmap,w,h, 32,96, result->cdata); // no guarantee this fits!
    /* Alloc full RGBA bytes to make things easier */ 
    u8 *bytes = xalloc(w*h*4);
    /* Convert Alpha-only bytes to RGBA bytes */
    u8 *destRow = bytes;
    u8 *srcRow = tempBitmap;
    for (u32 y=0; y<h; ++y) {
        u32 *destPixel = (u32 *)destRow;
        u8 *srcPixel = srcRow;
        for (u32 x=0; x<w; ++x) {
            u8 alpha = *srcPixel;
            *destPixel++ = (alpha << 24 | alpha << 16 | alpha << 8 | alpha);
            srcPixel++;
        }
        destRow += 4*w;
        srcRow += 1*w;
    }
    /* Create texture from full RGBA bytes */
    glCreateTextures(GL_TEXTURE_2D, 1, &result->ftex);
    glTextureParameteri(result->ftex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(result->ftex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(result->ftex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(result->ftex, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureStorage2D (result->ftex, 1, GL_RGBA8, w, h);
    glTextureSubImage2D(result->ftex, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    /* Free temp bytes */
    xfree(bytes);
    xfree(tempBitmap);
    
    return result;
}

static void 
xgl_push_text(XGLPass *pass, v2 p, f32 scale, v4 color, char *text) {
    f32 x = 0;
    f32 y = 0;
    f32 h = (f32)pass->font->ascent;
    //f32 h = 0;
    while (*text) {
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(pass->font->cdata, pass->font->w,pass->font->h, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
        
        xgl_push_quad(pass,
                      (v2){p[0]+scale*q.x0, p[1]+scale*(h+q.y0)}, 
                      (v2){p[0]+scale*q.x1, p[1]+scale*(h+q.y0)}, 
                      (v2){p[0]+scale*q.x1, p[1]+scale*(h+q.y1)}, 
                      (v2){p[0]+scale*q.x0, p[1]+scale*(h+q.y1)},
                      (v2){q.s0,q.t0}, (v2){q.s1,q.t0}, (v2){q.s1,q.t1}, (v2){q.s0,q.t1},
                      color);
        ++text;
    }
}

static void
draw_text(XGLPass *pass, v2 p, f32 scale, v4 color, char *text) {
    xgl_push_text(pass, p, scale, color, text);
}

static void
draw_rect(XGLPass *pass, v2 p, v2 dim, v4 color) {
    v2 pA = {p[0], p[1]};
    v2 pB = {p[0] + dim[0], p[1]};
    v2 pC = {p[0] + dim[0], p[1] + dim[1]};
    v2 pD = {p[0], p[1] + dim[1]};
    
    v2 uvA = {0, 0};
    v2 uvB = {1, 0};
    v2 uvC = {1, 1};
    v2 uvD = {0, 1};
    
    xgl_push_quad(pass,
                  pA, pB, pC, pD,
                  uvA, uvB, uvC, uvD,
                  color);
}



#endif //XOPENGL_H
