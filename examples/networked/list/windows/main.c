#define _CRT_SECURE_NO_WARNINGS 1

#include "plat.h"
#include "animation.h"
#include "list_linked.h"
#include "serialization.h"
#include "packet.h"
#include "client.h"
#include "linking_context.h"

#include "app.h"

global Platform plat;
global App app;
global Client client;

#include "plat.c"
#include "list_linked.c"
#include "client.c"

#include "app.c"

XWINMAIN()
{
    plat_init();
    app_construct();
    
    xd11_set_cursor(cursor_select);
    while (xd11.running) {
        /* Must be called first */
        xrender2d_pre_update();
        
        /* Update and Render */
        app_update_and_render();
        
        /* Must be called after */
        xrender2d_post_update(plat.layers, plat.layerIndex);
        
        /* Reset Batches */
        for (u32 i=0; i<plat.layerIndex; ++i)
            xrender2d_reset_batch(plat.layers + i);
    }
    
    
    app_destruct();
    xaudio_shutdown();
    
    for (u32 i=0; i<plat.fontIndex; ++i)
        xrender2d_font_free(plat.fonts[i]);
    
    for (u32 i=0; i<plat.soundIndex; ++i)
        xfree(plat.sounds[i]);
    
    xrender2d_shutdown();
    xd11_shutdown();
}

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (message) {
        XWNDPROC;
        
        default:
        {
            result = DefWindowProcA(window, message, wParam, lParam);
        } break;
    }
    return result;
}