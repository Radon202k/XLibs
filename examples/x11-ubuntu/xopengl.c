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
xgl_texture_from_png(char *path) {
    int width, height, channels;
    unsigned char* imageData = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    
    if (imageData == NULL) {
        fprintf(stderr, "Error loading image: %s\n", stbi_failure_reason());
        exit(1);
    }
    
    GLuint result = -1;
    glCreateTextures(GL_TEXTURE_2D, 1, &result);
    
    glTextureParameteri(result, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(result, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(result, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(result, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTextureStorage2D (result, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(result, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    
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
        transform(p, scale, rotation),
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
            renderer_update_matrices_rh(viewRH, perspectiveRH, pass->camera);
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
                    mat4_from_transform(command->transform, model);
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
        else if (pass->type == XGLPassType_font) {
            // Matrix
            mat4 orthoRH;
            mat4_ortho_rh(0, x11.width, x11.height, 0,
                          -1000.0f, 1000.0f, orthoRH);
            
            glProgramUniformMatrix4fv(pass->shader->vshader, 0, 1, GL_FALSE, (f32 *)orthoRH);
            
            // Bind texture
            glBindTextureUnit(0, pass->font->ftex);
            
            if (pass->drawType == XGLDrawType_indexed) {
                
                FontVertex vertices[4096] = {0};
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
                glBindBuffer(GL_ARRAY_BUFFER, pass->fontPassVbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pass->fontPassEbo);
                
                // Update the VBO data
                glBufferSubData(GL_ARRAY_BUFFER, 0, vertexIndex*sizeof(FontVertex), vertices);
                // Update the EBO data
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexIndex*sizeof(GLuint), indices);
                
                // draw
                glDrawElements(GL_TRIANGLES, indexIndex, GL_UNSIGNED_INT, 0);
                
                // Unbind the buffers
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                
                /* Reset the count of how many meshes to draw per frame */
                pass->quadCommandIndex = 0;
            }
        }
    }
}
