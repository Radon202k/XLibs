#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xd3d11.h"

#include "renderer.h"

XWINMAIN()
{
    /* Initialize renderer */
    renderer_initialize();
    
    while (xd11.running)
    {
        renderer_pre_update();
        
        /* Do stuff */
        
        renderer_post_update();
    }
    
    /* Free renderer resources */
    renderer_shutdown();
}
