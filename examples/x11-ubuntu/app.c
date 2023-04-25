
static XFont *
x11_font(char *path, f32 height) {
    XFont *result = xalloc(sizeof *result);
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
draw_text(XGLPass *pass, v2 p, f32 scale, v4 color, char *text) {
    f32 x = p[0];
    f32 y = p[1];
    while (*text) {
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(pass->font->cdata, pass->font->w,pass->font->h, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
        
        xgl_push_quad(pass,
                      (v2){scale*q.x0,scale*q.y0}, 
                      (v2){scale*q.x1,scale*q.y0}, 
                      (v2){scale*q.x1,scale*q.y1}, 
                      (v2){scale*q.x0,scale*q.y1},
                      (v2){q.s0,q.t0}, (v2){q.s1,q.t0}, (v2){q.s1,q.t1}, (v2){q.s0,q.t1},
                      color);
        ++text;
    }
}

void app_construct(void) {
    /* Make the orbit camera */
    app.orbitCamera = camera_orbit((v3){0,0,0}, 10, 0, 0);
    app.fontCamera = camera_orbit((v3){0,0,0}, 10, 0, 0);
    
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
        u32 vertexDataSize = 4096*sizeof(FontVertex);
        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, vertexDataSize, 0, GL_DYNAMIC_STORAGE_BIT);
        glCreateBuffers(1, &ebo);
        glNamedBufferStorage(ebo, vertexDataSize, 0, GL_DYNAMIC_STORAGE_BIT);
        pass->fontPassVbo = vbo;
        pass->fontPassEbo = ebo;
        
        GLint vbufIndex = 0;
        glVertexArrayVertexBuffer(pass->vao, vbufIndex, vbo, 0, sizeof(FontVertex));
        
        pass->type = XGLPassType_font;
        pass->drawType = XGLDrawType_indexed;
        pass->camera = &app.fontCamera;
        pass->shader = &app.shaderFont;
        {
            /* Set up position input attrib format */
            glVertexArrayAttribFormat (pass->vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(FontVertex, position));
            glVertexArrayAttribBinding(pass->vao, 0, 0);
            glEnableVertexArrayAttrib (pass->vao, 0);
            /* Set up texCoord input attrib format */
            glVertexArrayAttribFormat (pass->vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(FontVertex, texCoord));
            glVertexArrayAttribBinding(pass->vao, 1, 0);
            glEnableVertexArrayAttrib (pass->vao, 1);
            /* Set up color input attrib format */
            glVertexArrayAttribFormat (pass->vao, 2, 4, GL_FLOAT, GL_FALSE, offsetof(FontVertex, color));
            glVertexArrayAttribBinding(pass->vao, 2, 0);
            glEnableVertexArrayAttrib (pass->vao, 2);
        }
        
        app.fonts[0] = x11_font("fonts/test.ttf", 128);
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
