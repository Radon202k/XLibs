#ifndef PLAT_H
#define PLAT_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// #define XD11_RELEASE_BUILD

#include "XLibs/xbase.h"
#include "XLibs/xmemory.h"
#include "XLibs/xmath.h"
#include "XLibs/xstring.h"
#include "XLibs/xrect_cut.h"

/* 3D utilities / X11 & Opengl initialization */
#include "XLibs/x3d.h"
#include "XLibs/xopengl_loader.h"
#define XOPENGL_IMPLEMENTATION
#include "XLibs/x11.h"
#include "XLibs/xopengl.h"

#include "XLibs/xsocket.h"

typedef struct Platform {
    
    XGLShader shaderMesh;
    Camera orbitCamera;
    
    XGLShader shaderFont;
    Camera fontCamera;
    
    XGLPass passes[32];
    u32 passIndex;
    
    float lastMouseX;
    float lastMouseY;
    
    XGLFont *fonts[8];
    
} Platform;

#endif //PLAT_H
