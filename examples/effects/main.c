#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xd3d11.h"
#include "w:/libs/xlibs/xrender2d.h"

global XRenderBatch layer1;

global XFont font_big;
global XFont font_small;

#include "box.h"

typedef struct
{
    v2f a;
    v2f b;
    bool positions_set;
    
    Boxes boxes;
    
    f32 timer;
} State;

global State state;

void selection_update(void);









void render_scene(void)
{
    draw_grid(&layer1, (v2f){100,100}, (v2f){1,1}, (v4f){.1f,.1f,.1f,1});
    
    boxes_update_and_render(&state.boxes);
}

void initialize(void)
{
    /* Initialize Direct3D 11 */
    xd11_initialize((XD11Config){window_proc, 0, 0, L"My Window"});
    
    /* Initialize Input */
    xwin_initialize((XWindowConfig){LoadCursor(NULL, IDC_ARROW), xd11.window_handle});
    
    /* Initialize renderer */
    xrender2d_initialize((v4f){.2f,.2f,.2f,1});
    
    random_seed(0);
    
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
    
    /* Update texture atlas */
    xd11_texture2d_update(xrender2d.texture_atlas.texture,
                          xrender2d.texture_atlas.bytes);
    
    v4f colors[] = {cri4f, gol4f, eme4f, azu4f};
    boxes_spawn(&state.boxes, colors, narray(colors));
    boxes_update_layout(&state.boxes);
}

XWINMAIN()
{
    initialize();
    
    f32 seconds = 0.001f;
    state.timer = seconds;
    
    while (xd11.running)
    {
        xrender2d_pre_update();
        
        /* Update state */
        state.timer -= xd11.dt;
        if (state.timer <= 0)
        {
            state.timer += seconds;
        }
        
        // selection_update();
        
        draw_text(&layer1, (v2f){200,200}, (v4f){1,1,1,1}, &font_big, L"This is my test");
        
        /* Render scene */
        render_scene();
        
        xrender2d_post_update(&layer1, 1);
        
        /* Reset batches */
        xrender2d_reset_batch(&layer1);
    }
    
    /* Free our resources */
    List_free(&state.boxes.storage);
    xrender2d_font_free(font_big);
    xrender2d_font_free(font_small);
    
    /* Free renderer resources */
    xrender2d_shutdown();
    
    xd11_shutdown();
}

void selection_update(void)
{
    if (xwin.mouse.left.pressed && !state.positions_set)
    {
        state.positions_set = true;
        state.a = xwin.mouse.pos;
        state.b = xwin.mouse.pos;
    }
    
    if (state.positions_set)
    {
        state.b = xwin.mouse.pos;
        
        if (state.a.x < state.b.x && state.a.y > state.b.y)
        {
            draw_rect(&layer1, 
                      (v2f){state.a.x, state.b.y}, 
                      (v2f){state.b.x-state.a.x, state.a.y-state.b.y}, 
                      cri4f);
        }
        else if (state.a.x > state.b.x && state.a.y < state.b.y)
        {
            draw_rect(&layer1, 
                      (v2f){state.a.x, state.b.y}, 
                      (v2f){state.b.x-state.a.x, state.a.y-state.b.y}, 
                      cri4f);
        }
        else
        {
            draw_rect(&layer1, state.a, sub2f(state.b, state.a), cri4f);
        }
        
        
        if (!xwin.mouse.left.down)
            state.positions_set = false;
    }
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