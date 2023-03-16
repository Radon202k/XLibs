#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xd3d11.h"

#include "renderer.h"

RenderBatch layer1;

Sprite sprite_naruto;

XWINMAIN()
{
    /* Initialize renderer */
    renderer_initialize();
    
    /* Load textures */
    sprite_naruto = renderer_sprite_from_png(&renderer.texture_atlas, L"images/naruto.png", false);
    
    /* Update texture atlas */
    xd11_texture2d_update(renderer.texture_atlas.texture, renderer.texture_atlas.bytes);
    
    while (xd11.running)
    {
        renderer_pre_update();
        
        draw_line(&layer1,         // Batch
                  (v2f){0,0},      // A Pos
                  xwin.mouse.pos,  // B Pos
                  (v4f){1,1,1,1}); // Color
        
        {
            v2f s = sprite_naruto.size;
            v2f p = add2f(xwin.mouse.pos, mul2f(-.5f,s));
            draw_sprite(&layer1, p, s, (v4f){1,1,1,1}, sprite_naruto);
        }
        
        renderer_post_update(&layer1, 1);
        
        /* Reset Batches */
        renderer_reset_batch(&layer1);
    }
    
    /* Free renderer resources */
    renderer_shutdown();
}
