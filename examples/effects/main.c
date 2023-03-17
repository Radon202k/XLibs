#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xd3d11.h"
#include "w:/libs/xlibs/xrender2d.h"

XRenderBatch layer1;

XFont font_big;
XFont font_small;
XSprite sprite_naruto;

XWINMAIN()
{
    /* Initialize Direct3D 11 */
    xd11_initialize((XD11Config){window_proc, 0, 0, L"My Window"});
    
    /* Initialize Input */
    xwin_initialize((XWindowConfig){LoadCursor(NULL, IDC_ARROW), xd11.window_handle});
    
    /* Initialize renderer */
    xrender2d_initialize();
    
    /* Load fonts */
    /* Big */
    {
        wchar_t full_path[512];
        xwin_path_abs(full_path, 512, L"fonts\\Inconsolata.ttf");
        font_big = xrender2d_font(&xrender2d.texture_atlas,
                                  full_path, L"Inconsolata", 64);
    }
    /* Small */
    {
        wchar_t full_path[512];
        xwin_path_abs(full_path, 512, L"fonts\\Inconsolata.ttf");
        font_small = xrender2d_font(&xrender2d.texture_atlas,
                                    full_path, L"Inconsolata", 32);
    }
    
    
    /* Load textures */
    {
        wchar_t full_path[512];
        xwin_path_abs(full_path, 512, L"images\\naruto.png");
        sprite_naruto = xrender2d_sprite_from_png(&xrender2d.texture_atlas,
                                                  full_path, false);
    }
    
    /* Update texture atlas */
    xd11_texture2d_update(xrender2d.texture_atlas.texture,
                          xrender2d.texture_atlas.bytes);
    
    while (xd11.running)
    {
        xrender2d_pre_update();
        
        draw_text(&layer1, (v2f){0,0}, (v4f){1,1,1,1}, &font_small, L"Effects example");
        
        draw_text(&layer1, (v2f){0,xd11.back_buffer_size.y-32}, (v4f){0,0,0,1}, &font_big, L"XLibraries");
        
        draw_line(&layer1, mul2f(.5f, xd11.back_buffer_size), xwin.mouse.pos, gol4f);

        draw_sprite(&layer1, sub2f(xwin.mouse.pos, (v2f) { 50, 50 }), 
            (v2f) { 100, 100 }, (v4f) { 1, 1, 1, 1 }, sprite_naruto);

        xrender2d_post_update(&layer1, 1);
        
        /* Reset Batches */
        xrender2d_reset_batch(&layer1);
    }
    
    /* Free our resources */
    xrender2d_font_free(font_big);
    xrender2d_font_free(font_small);
    
    /* Free renderer resources */
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