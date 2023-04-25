#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

#include "XLibs/xbase.h"
#include "XLibs/xmemory.h"
#include "XLibs/xmath.h"

#include "camera.h"
#include "mesh.h"
#include "renderer.h"
#include "xopengl.h"
#define XOPENGL_IMPLEMENTATION
#include "x11.h"
#include "app.h"

#include "mesh.c"
#include "renderer.c"
#include "xopengl.c"
#include "x11.c"
#include "app.c"

int main() {
    x11_construct();
    xgl_construct();
    app_construct();
    while (x11.isRunning) {
        float dt = x11_begin_frame();
        
        app_update(dt);
        
        x11_end_frame(app.passes, app.passIndex);
    }
    
    xmemcheck();
    
    return 0;
}
