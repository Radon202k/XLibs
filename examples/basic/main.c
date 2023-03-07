#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xrender.h"

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

XWINMAIN()
{
    xrender_init((XRenderConfig){window_proc, .topDown=true});
    xwindow_init((XWindowConfig){LoadCursor(NULL, IDC_ARROW), xrnd.windowHandle});
    
    /* Create resources */
    
    while (xrnd.running)
    {
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        
        /* Do stuff */
        
        xrender_update();
        xwindow_update(xrnd.topDown, xrnd.windowSize);
    }
    
    /* Free resources */
    
    xrender_shutdown();
}

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message) {
        XWNDPROC;
        
        default:
        {
            result = DefWindowProcW(window, message, wParam, lParam);
        } break;
    }
    return result;
}

void xrender_resized(void)
{
}
