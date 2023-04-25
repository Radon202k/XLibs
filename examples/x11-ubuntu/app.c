void app_construct(void) {
    /* Make the orbit camera */
    app.orbitCamera = x3d_camera_orbit((v3){0,0,0}, 10, 0, 0);
    app.fontCamera = x3d_camera_orbit((v3){0,0,0}, 10, 0, 0);
    
    app.passIndex = 2;
    
    /* 3D meshes */
    {
        /* Create shader from files */
        xgl_shader_from_files(&app.shaderMesh, "shaders/indexed.vs", "shaders/indexed.fs");
        /* Set up XGLPass configuration */
        XGLPass *pass = &app.passes[0];
        pass->type = XGLPassType_mesh;
        pass->drawType = XGLDrawType_indexed;
        pass->camera = &app.orbitCamera;
        pass->shader = &app.shaderMesh;
        /* Create VAO for XGLPass */
        glCreateVertexArrays(1, &pass->vao);
        /* Setup VAO's vbo 0's input attributes */
        {
            /* Set up position input attrib format */
            glVertexArrayAttribFormat (pass->vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, position));
            glVertexArrayAttribBinding(pass->vao, 0, 0);
            glEnableVertexArrayAttrib (pass->vao, 0);
            /* Set up texCoord input attrib format */
            glVertexArrayAttribFormat (pass->vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, texCoord));
            glVertexArrayAttribBinding(pass->vao, 1, 0);
            glEnableVertexArrayAttrib (pass->vao, 1);
        }
        
        /* Make a mesh for a simple cube */
        {
            /* Make the mesh vertices and indices */
            app.cubeMesh = mesh_cube(1);
            xgl_upload_mesh(app.cubeMesh, &app.passes[0]);
        }
    }
    
    /* Fonts */
    xgl_shader_from_files(&app.shaderFont, "shaders/font.vs", "shaders/font.fs");
    /* Set up XGLPass configuration */
    XGLPass *pass = &app.passes[1];
    
    {
        /* Create VAO for XGLPass */
        glCreateVertexArrays(1, &pass->vao);
        
        GLuint vbo, ebo;
        u32 vertexDataSize = 4096*sizeof(XFontVertex);
        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, vertexDataSize, 0, GL_DYNAMIC_STORAGE_BIT);
        glCreateBuffers(1, &ebo);
        glNamedBufferStorage(ebo, vertexDataSize, 0, GL_DYNAMIC_STORAGE_BIT);
        pass->fontPassVbo = vbo;
        pass->fontPassEbo = ebo;
        
        GLint vbufIndex = 0;
        glVertexArrayVertexBuffer(pass->vao, vbufIndex, vbo, 0, sizeof(XFontVertex));
        
        pass->type = XGLPassType_font;
        pass->drawType = XGLDrawType_indexed;
        pass->camera = &app.fontCamera;
        pass->shader = &app.shaderFont;
        {
            /* Set up position input attrib format */
            glVertexArrayAttribFormat (pass->vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(XFontVertex, position));
            glVertexArrayAttribBinding(pass->vao, 0, 0);
            glEnableVertexArrayAttrib (pass->vao, 0);
            /* Set up texCoord input attrib format */
            glVertexArrayAttribFormat (pass->vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(XFontVertex, texCoord));
            glVertexArrayAttribBinding(pass->vao, 1, 0);
            glEnableVertexArrayAttrib (pass->vao, 1);
            /* Set up color input attrib format */
            glVertexArrayAttribFormat (pass->vao, 2, 4, GL_FLOAT, GL_FALSE, offsetof(XFontVertex, color));
            glVertexArrayAttribBinding(pass->vao, 2, 0);
            glEnableVertexArrayAttrib (pass->vao, 2);
        }
        
        app.fonts[0] = xgl_font("fonts/test.ttf", 128);
        pass->font = app.fonts[0];
    }
    
    app.lavaTexture = xgl_texture_from_png("images/lava.png");
    app.woodTexture = xgl_texture_from_png("images/wood.png");
}

void app_update(float dt) {
    
    // Calculate the new camera rotation based on the mouse position
    float dx = xmouse.x - app.lastMouseX;
    float dy = xmouse.y - app.lastMouseY;
    
    if (isLeftButtonDown()) {
        if (isShiftKeyPressed()) {
            app.orbitCamera.distance += dy * 0.005f;
        }
        else {
            app.orbitCamera.rotationX -= dx * 0.005f;
            app.orbitCamera.rotationY += dy * 0.005f;
        }
    }
    
    
    char buf[512];
    snprintf(buf, 512, "dt: %.6f", dt);
    draw_text(&app.passes[1], (v2){0,128}, 0.25f, xcolor[Gold], buf);
    
    app.lastMouseX = xmouse.x;
    app.lastMouseY = xmouse.y;
    
    xgl_push_mesh(&app.passes[0], (v3){-2,0,-5}, (v3){1,1,1}, (versor){0,0,0,0}, app.lavaTexture, app.cubeMesh);
    xgl_push_mesh(&app.passes[0], (v3){0,0,0}, (v3){1,1,1}, (versor){0,0,0,0}, app.woodTexture, app.cubeMesh);
    xgl_push_mesh(&app.passes[0], (v3){2,0,5}, (v3){1,1,1}, (versor){0,0,0,0}, app.lavaTexture, app.cubeMesh);
}
