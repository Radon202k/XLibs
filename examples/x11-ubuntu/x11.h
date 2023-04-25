#ifndef PLATFORM_H
#define PLATFORM_H

#include <X11/Xlib.h>

#include <unistd.h>

typedef struct XMouse {
    f32 x;
    f32 y;
} XMouse;

typedef struct XFont {
    s32 w, h;
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    GLuint ftex;
} XFont;

typedef struct X11 {
    bool isRunning;
    bool enableVSync;
    struct timespec c1;
    
#ifdef XOPENGL_IMPLEMENTATION
    EGLDisplay *display;
    EGLSurface* surface;
#else
    // TODO: Vulkan?
#endif
    
    Display *dpy;
    Window window;
    
    Atom WM_PROTOCOLS;
    Atom WM_DELETE_WINDOW;
    int width;
    int height;
    
} X11;

static void x11_construct     (void);
static f32  x11_begin_frame   (void);
static void x11_present_frame (void);

#ifdef XOPENGL_IMPLEMENTATION
static void x11_end_frame     (XGLPass passes[32], u32 passIndex);
#else
// TODO: Vulkan?
#endif

global X11 x11;
global XMouse xmouse;

#endif //PLATFORM_H
