/* Dependencies */
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"
/* Memory management / Math */
#include "XLibs/xbase.h"
#include "XLibs/xmemory.h"
#include "XLibs/xmath.h"
/* 3D utilities / X11 & Opengl initialization */
#include "XLibs/x3d.h"
#include "XLibs/xopengl_loader.h"
#define XOPENGL_IMPLEMENTATION
#include "XLibs/x11.h"
#include "XLibs/xopengl.h"
/* App */
#include "app.h"
#include "app.c"

int main() {
    x11_construct();
    xgl_construct();
    app_construct();
    while (x11.isRunning) {
        float dt = x11_update();
        
        app_update(dt);
        
        // Render only if window size is non-zero
        if (x11.width != 0 && x11.height != 0) {
            xgl_render_frame(app.passes, app.passIndex);
            x11_swap_buffers();
        }
        else {
            // window is minimized, cannot vsync - instead sleep a bit
            if (x11.enableVSync)
                usleep(10 * 1000);
        }
    }
    
    xmemcheck();
    
    return 0;
}
