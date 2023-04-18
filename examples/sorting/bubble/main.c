#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xd3d11.h"
#include "w:/libs/xlibs/xrender2d.h"

global XRenderBatch layer1;

typedef struct
{
    f32 bar_heights[100];
    s32 bubble_sort_i;
    s32 bubble_sort_j;
    f32 timer;
} State;

global State state;

void initialize(void);
void render_scene(void);


bool heights_greater(u32 index_a, u32 index_b);
void heights_swap(u32 index_a, u32 index_b);
void bubble_sort_iterate(void);

XWINMAIN()
{
    initialize();
    
    f32 seconds = 0.001f;
    state.timer = seconds;
    state.bubble_sort_i = 0;
    state.bubble_sort_j = narray(state.bar_heights)-2;
    
    while (xd11.running)
    {
        xrender2d_pre_update();
        
        state.timer -= xd11.dt;
        if (state.timer <= 0)
        {
            state.timer += seconds;
            bubble_sort_iterate();
        }
        
        /* Render Scene */
        render_scene();
        
        xrender2d_post_update(&layer1, 1);
        
        /* Reset Batches */
        xrender2d_reset_batch(&layer1);
    }
    
    /* Free renderer resources */
    xrender2d_shutdown();
    
    xd11_shutdown();
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
    
    /* Update texture atlas */
    xd11_texture2d_update(xrender2d.texture_atlas.texture,
                          xrender2d.texture_atlas.bytes);
    
    /* Initialize bar heights */
    
    f32 bar_width = xd11.back_buffer_size.x / (narray(state.bar_heights)+1);
    for (s32 i=0; i<narray(state.bar_heights); ++i)
        state.bar_heights[i] = rndf(0,1);
}

void render_scene(void)
{
    draw_grid(&layer1, (v2f){100,100}, (v2f){1,1}, (v4f){.3f,.3f,.3f,1});
    
    u32 bar_count = narray(state.bar_heights);
    
    f32 margin = 2;
    f32 bar_width = (xd11.back_buffer_size.x - margin*bar_count)
        / (bar_count);
    
    /* Draw vertical bars */
    for (s32 x=0; x<narray(state.bar_heights); ++x)
    {
        f32 bar_height = state.bar_heights[x] * xd11.back_buffer_size.y;
        v2f bar_pos = {x*(bar_width + margin), xd11.back_buffer_size.y-bar_height};
        v4f bar_color = (v4f){.2f,.1f,.1f,1};
        if (x==state.bubble_sort_i)
            bar_color = (v4f){.8f,.5f,.5f,1};
        else if (x-1>state.bubble_sort_j)
            bar_color = (v4f){.5f,.9f,.5f,1};
        draw_rect(&layer1, bar_pos, (v2f){bar_width, bar_height}, bar_color);
    }
}

bool heights_greater(u32 index_a, u32 index_b)
{
    return (state.bar_heights[index_a] > state.bar_heights[index_b]);
}

void heights_swap(u32 index_a, u32 index_b)
{
    f32 temp = state.bar_heights[index_a];
    state.bar_heights[index_a] = state.bar_heights[index_b];
    state.bar_heights[index_b] = temp;
}

void bubble_sort_iterate(void)
{
    if (heights_greater(state.bubble_sort_i, state.bubble_sort_i+1))
        heights_swap(state.bubble_sort_i, state.bubble_sort_i+1);
    
    if (state.bubble_sort_i++ >= state.bubble_sort_j)
    {
        --state.bubble_sort_j;
        state.bubble_sort_i=0;
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