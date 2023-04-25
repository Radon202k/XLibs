#ifndef PLATFORM_H
#define PLATFORM_H

#include <X11/Xlib.h>
#include <unistd.h>

typedef struct XButton {
    bool pressed;
    bool released;
    bool down;
} XButton;

typedef struct XMouse {
    XButton left;
    XButton right;
    v2 p;
} XMouse;

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

static void x11_construct    (void);
static f32  x11_update       (void);
static void x11_swap_buffers (void);

global X11 x11;
global XMouse xmouse;














//












#define XK_Shift_L 0xffe1

// Check if the Shift key is pressed
bool isShiftKeyPressed() {
    char buffer[32];
    XQueryKeymap(x11.dpy, buffer);
    KeyCode shiftKeyCode = XKeysymToKeycode(x11.dpy, XK_Shift_L);
    return (buffer[shiftKeyCode / 8] & (1 << (shiftKeyCode % 8)));
}

static void 
x11_construct(void) {
    x11.dpy = XOpenDisplay(NULL);
    if (!x11.dpy) {
        FatalError("Cannot open X display");
    }
    
    XSetWindowAttributes attributes = {
        .event_mask = StructureNotifyMask,
    };
    
    // create window
    x11.width = 1280;
    x11.height = 720;
    x11.window = XCreateWindow(x11.dpy, DefaultRootWindow(x11.dpy),
                               0, 0, x11.width, x11.height, 
                               0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask,
                               &attributes);
    assert(x11.window && "Failed to create window");
    
    // uncomment in case you want fixed size window
    //XSizeHints* hints = XAllocSizeHints();
    //Assert(hints);
    //hints->flags |= PMinSize | PMaxSize;
    //hints->min_width  = hints->max_width  = width;
    //hints->min_height = hints->max_height = height;
    //XSetWMNormalHints(x11.dpy, window, hints);
    //XFree(hints);
    
    // set window title
    XStoreName(x11.dpy, x11.window, "OpenGL Window");
    
    // subscribe to window close notification
    x11.WM_PROTOCOLS = XInternAtom(x11.dpy, "WM_PROTOCOLS", False);
    x11.WM_DELETE_WINDOW = XInternAtom(x11.dpy , "WM_DELETE_WINDOW", False);
    XSetWMProtocols(x11.dpy, x11.window, &x11.WM_DELETE_WINDOW, 1);
    
    // initialize EGL
    {
        x11.display = eglGetDisplay((EGLNativeDisplayType)x11.dpy);
        assert(x11.display != EGL_NO_DISPLAY && "Failed to get EGL display");
        
        EGLint major, minor;
        if (!eglInitialize(x11.display, &major, &minor))
        {
            FatalError("Cannot initialize EGL display");
        }
        if (major < 1 || (major == 1 && minor < 5))
        {
            FatalError("EGL version 1.5 or higher required");
        }
    }
    
    // choose OpenGL API for EGL, by default it uses OpenGL ES
    EGLBoolean ok = eglBindAPI(EGL_OPENGL_API);
    assert(ok && "Failed to select OpenGL API for EGL");
    
    // choose EGL configuration
    EGLConfig config;
    {
        EGLint attr[] =
        {
            EGL_SURFACE_TYPE,      EGL_WINDOW_BIT,
            EGL_CONFORMANT,        EGL_OPENGL_BIT,
            EGL_RENDERABLE_TYPE,   EGL_OPENGL_BIT,
            EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
            
            EGL_RED_SIZE,      8,
            EGL_GREEN_SIZE,    8,
            EGL_BLUE_SIZE,     8,
            EGL_DEPTH_SIZE,   24,
            EGL_STENCIL_SIZE,  8,
            
            // uncomment for multisampled framebuffer
            //EGL_SAMPLE_BUFFERS, 1,
            //EGL_SAMPLES,        4, // 4x MSAA
            
            EGL_NONE,
        };
        
        EGLint count;
        if (!eglChooseConfig(x11.display, attr, &config, 1, &count) || count != 1)
        {
            FatalError("Cannot choose EGL config");
        }
    }
    
    // create EGL surface
    {
        EGLint attr[] =
        {
            EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR, // or use EGL_GL_COLORSPACE_SRGB for sRGB framebuffer
            EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
            EGL_NONE,
        };
        
        x11.surface = eglCreateWindowSurface(x11.display, config, x11.window, attr);
        if (x11.surface == EGL_NO_SURFACE)
        {
            FatalError("Cannot create EGL surface");
        }
    }
    
    // create EGL context
    EGLContext* context;
    {
        EGLint attr[] =
        {
            EGL_CONTEXT_MAJOR_VERSION, 4,
            EGL_CONTEXT_MINOR_VERSION, 5,
            EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#ifndef NDEBUG
            // ask for debug context for non "Release" builds
            // this is so we can enable debug callback
            EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
#endif
            EGL_NONE,
        };
        
        context = eglCreateContext(x11.display, config, EGL_NO_CONTEXT, attr);
        if (context == EGL_NO_CONTEXT)
        {
            FatalError("Cannot create EGL context, OpenGL 4.5 not supported?");
        }
    }
    
    ok = eglMakeCurrent(x11.display, x11.surface, x11.surface, context);
    assert(ok && "Failed to make context current");
    
    // load OpenGL functions
#define X(type, name) name = (type)eglGetProcAddress(#name); assert(name);
    GL_FUNCTIONS(X)
#undef X
    
#ifndef NDEBUG
    // enable debug callback
    glDebugMessageCallback(&DebugCallback, NULL);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
    
    // show the window
    XMapWindow(x11.dpy, x11.window);
}

static f32
x11_update(void) {
    // process all incoming X11 events
    if (XPending(x11.dpy))
    {
        XEvent event;
        XNextEvent(x11.dpy, &event);
        if (event.type == ClientMessage)
        {
            if (event.xclient.message_type == x11.WM_PROTOCOLS)
            {
                Atom protocol = event.xclient.data.l[0];
                if (protocol == x11.WM_DELETE_WINDOW)
                    x11.isRunning = 0;
                
            }
        }
    }
    
    // Update mouse buttons state
    {
        Window root, child;
        int rootX, rootY, winX, winY;
        unsigned int mask;
        XQueryPointer(x11.dpy, x11.window, &root, &child, &rootX, &rootY, &winX, &winY, &mask);
        
        bool leftDown = (mask & Button1Mask);
        if (xmouse.left.down && !leftDown)
            xmouse.left.released = true;
        if (xmouse.left.down == false && leftDown)
            xmouse.left.pressed = true;
        xmouse.left.down = leftDown;
        
        bool rightDown = (mask & Button3Mask);
        if (xmouse.right.down && !rightDown)
            xmouse.right.released = true;
        if (xmouse.right.down == false && rightDown)
            xmouse.right.pressed = true;
        xmouse.right.down = rightDown;
    }
    
    // get current window size
    XWindowAttributes attr;
    Status status = XGetWindowAttributes(x11.dpy, x11.window, &attr);
    assert(status && "Failed to get window attributes");
    
    x11.width = attr.width;
    x11.height = attr.height;
    
    struct timespec c2;
    clock_gettime(CLOCK_MONOTONIC, &c2);
    float dt = (float)(c2.tv_sec-x11.c1.tv_sec) + 
        1e-9f*(c2.tv_nsec-x11.c1.tv_nsec);
    
    x11.c1 = c2;
    
    /* Get mouse position */
    Window root_window, child_window;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;
    
    XQueryPointer(x11.dpy, x11.window, &root_window, &child_window, &root_x, &root_y, &win_x, &win_y, &mask);
    v2_copy((v2){win_x, win_y}, xmouse.p);
    
    return dt;
}

static void x11_swap_buffers(void) {
    if (!eglSwapBuffers(x11.display, x11.surface)) 
        FatalError("Failed to swap OpenGL buffers!");
    
    /* Reset Mouse buttons state */
    xmouse.left.pressed = false;
    xmouse.left.released = false;
    xmouse.right.pressed = false;
    xmouse.right.released = false;
}

#endif //PLATFORM_H
