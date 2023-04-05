#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"
#include "w:/libs/xlibs/xd3d11.h"
#include "w:/libs/xlibs/xrender2d.h"

global XRenderBatch layer1;

XWINMAIN()
{
    xd11_initialize((XD11Config){window_proc, 0, 0, L"My Window"});
    xwin_initialize((XWindowConfig){LoadCursor(NULL, IDC_ARROW), xd11.window_handle});
    xrender2d_initialize((v4f){.2f,.2f,.2f,1});
    
    while (xd11.running)
    {
        xrender2d_pre_update();
        draw_rect(&layer1, xwin.mouse.pos, (v2f){50,50}, gol4f);
        xrender2d_post_update(&layer1, 1);
        xrender2d_reset_batch(&layer1);
    }
    xrender2d_shutdown();
    xd11_shutdown();
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
