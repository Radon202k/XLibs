static void plat_construct()
{
    /* Initialization must happen in this order */
    x11_construct();
    xgl_construct();
    
    xsocket_initialize();
    
    /* Set up XGLPass configuration */
    {
        printf("Created second XGLPass\n");
        XGLPass *pass = &plat.passes[plat.passIndex++];
        /* Create VAO for XGLPass */
        glCreateVertexArrays(1, &pass->vao);
        /* Create VBO and EBO for dynamic drawing */
        GLuint vbo, ebo;
        u32 vertexDataSize = 4096*sizeof(XGLQuadVertex);
        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, vertexDataSize, 0, GL_DYNAMIC_STORAGE_BIT);
        glCreateBuffers(1, &ebo);
        glNamedBufferStorage(ebo, vertexDataSize, 0, GL_DYNAMIC_STORAGE_BIT);
        pass->quadVbo = vbo;
        pass->quadEbo = ebo;
        /* Create white texture */
        u8 *whiteBytes = xalloc(512*512*4);
        memset(whiteBytes, 0xFF, 512*512*4);
        pass->quadTex = xgl_texture_from_bytes(512, 512, whiteBytes);
        xfree(whiteBytes);
        /* Bind vbo to vao */
        GLint vbufIndex = 0;
        glVertexArrayVertexBuffer(pass->vao, vbufIndex, vbo, 0, sizeof(XGLQuadVertex));
        /* Setup render pass as a font pass */
        pass->type = XGLPassType_quad;
        /* Draw type is indexed */
        pass->drawType = XGLDrawType_indexed;
        /* Camera is a null one */
        pass->camera = &plat.fontCamera;
        /* Shader we loaded just now */
        pass->shader = &plat.shaderFont;
        /* Set up input layout */
        {
            /* Position v2 */
            glVertexArrayAttribFormat (pass->vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(XGLQuadVertex, position));
            glVertexArrayAttribBinding(pass->vao, 0, 0);
            glEnableVertexArrayAttrib (pass->vao, 0);
            /* TexCoord v2 */
            glVertexArrayAttribFormat (pass->vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(XGLQuadVertex, texCoord));
            glVertexArrayAttribBinding(pass->vao, 1, 0);
            glEnableVertexArrayAttrib (pass->vao, 1);
            /* Color v4 */
            glVertexArrayAttribFormat (pass->vao, 2, 4, GL_FLOAT, GL_FALSE, offsetof(XGLQuadVertex, color));
            glVertexArrayAttribBinding(pass->vao, 2, 0);
            glEnableVertexArrayAttrib (pass->vao, 2);
        }
    }
    
    /* Font pass */
    xgl_shader_from_files(&plat.shaderFont, "shaders/font.vs", "shaders/font.fs");
    /* Set up XGLPass configuration */
    {
        printf("Created first XGLPass\n");
        XGLPass *pass = &plat.passes[plat.passIndex++];
        /* Create VAO for XGLPass */
        glCreateVertexArrays(1, &pass->vao);
        /* Create VBO and EBO for dynamic drawing */
        GLuint vbo, ebo;
        u32 vertexDataSize = 4096*sizeof(XGLQuadVertex);
        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, vertexDataSize, 0, GL_DYNAMIC_STORAGE_BIT);
        glCreateBuffers(1, &ebo);
        glNamedBufferStorage(ebo, vertexDataSize, 0, GL_DYNAMIC_STORAGE_BIT);
        pass->quadVbo = vbo;
        pass->quadEbo = ebo;
        /* Bind vbo to vao */
        GLint vbufIndex = 0;
        glVertexArrayVertexBuffer(pass->vao, vbufIndex, vbo, 0, sizeof(XGLQuadVertex));
        /* Setup render pass as a font pass */
        pass->type = XGLPassType_font;
        /* Draw type is indexed */
        pass->drawType = XGLDrawType_indexed;
        /* Camera is a null one */
        pass->camera = &plat.fontCamera;
        /* Shader we loaded just now */
        pass->shader = &plat.shaderFont;
        /* Set up input layout */
        {
            /* Position v2 */
            glVertexArrayAttribFormat (pass->vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(XGLQuadVertex, position));
            glVertexArrayAttribBinding(pass->vao, 0, 0);
            glEnableVertexArrayAttrib (pass->vao, 0);
            /* TexCoord v2 */
            glVertexArrayAttribFormat (pass->vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(XGLQuadVertex, texCoord));
            glVertexArrayAttribBinding(pass->vao, 1, 0);
            glEnableVertexArrayAttrib (pass->vao, 1);
            /* Color v4 */
            glVertexArrayAttribFormat (pass->vao, 2, 4, GL_FLOAT, GL_FALSE, offsetof(XGLQuadVertex, color));
            glVertexArrayAttribBinding(pass->vao, 2, 0);
            glEnableVertexArrayAttrib (pass->vao, 2);
        }
        /* Load font */
        plat.fonts[0] = xgl_font("fonts/Inconsolata.ttf", 64);
        pass->font = plat.fonts[0];
    }
    
    /* Create another quad pass */
    XGLPass *pass = &plat.passes[plat.passIndex++];
    *pass = plat.passes[0];
    
    
    glDisable(GL_DEPTH_TEST);
}
