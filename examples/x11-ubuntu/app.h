#ifndef APP_H
#define APP_H

typedef struct FontVertex {
    v2 position;
    v2 texCoord;
    v4 color;
} FontVertex;

typedef struct App {
    Mesh *cubeMesh;
    
    u32 lavaTexture;
    u32 woodTexture;
    
    u32 fontTexture;
    
    XGLShader shaderMesh;
    Camera orbitCamera;
    
    XGLShader shaderFont;
    Camera fontCamera;
    
    XGLPass passes[32];
    u32 passIndex;
    
    float lastMouseX;
    float lastMouseY;
    
    XFont *fonts[8];
    
} App;

global App app;

#endif //APP_H
