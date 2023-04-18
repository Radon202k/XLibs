#define XD11_RELEASE_BUILD

#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"
#include "w:/libs/xlibs/xd3d11.h"
#include "w:/libs/xlibs/xrender2d.h"

#include "imgui.h"
#include "xsocket.h"
#include "xstream_mem_out.h"
#include "xstream_mem_in.h"

#include "string.h"
#include "string_queue.h"

#include "div.h"


global XRenderBatch layer1;
global XFont font16;
global XFont font32;
global bool isRunningServer;


#include "chatroom_server.h"
#include "chatroom_client.h"

XWINMAIN()
{
    xd11_initialize((XD11Config){window_proc, 0, 0, L"ChatRoom"});
    xwin_initialize((XWindowConfig){LoadCursor(NULL, IDC_ARROW), xd11.window_handle});
    xrender2d_initialize((v4f){.9f,.9f,.9f,1});
    
    wchar_t fontFullPath[260];
    xwin_path_abs(fontFullPath, 260, L"fonts\\Inconsolata.ttf");
    
    font16 = xrender2d_font(&xrender2d.texture_atlas,
                            fontFullPath, L"Inconsolata", 16);
    
    font32 = xrender2d_font(&xrender2d.texture_atlas,
                            fontFullPath, L"Inconsolata", 32);
    
    /* Load sprites */
    
    /* Update texture atlas */
    xd11_texture2d_update(xrender2d.texture_atlas.texture,
                          xrender2d.texture_atlas.bytes);
    
    /* Initialize the socket api */
    xsocket_initialize();
    
    /* Initialize the client */
    chatroom_client_initialize();
    
    while (xd11.running)
    {
        xrender2d_pre_update();
        
        if (isRunningServer) {
            /* Update the server */
            chatroom_server_update();
        }
        
        /* Update the client */
        chatroom_client_update();
        
        /* Update Imgui after everything */
        imgui_update(&layer1, &font32);
        
        xrender2d_post_update(&layer1, 1);
        xrender2d_reset_batch(&layer1);
    }
    
    if (isRunningServer) {
        chatroom_server_shutdown();
    }
    
    chatroom_client_shutdown();
    
    xrender2d_font_free(&font16);
    xrender2d_font_free(&font32);
    
    WSACleanup();
    
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
