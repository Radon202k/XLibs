#include "plat.h"
#include "animation.h"
#include "list_linked.h"
#include "serialization.h"
#include "packet.h"
#include "client.h"
#include "linking_context.h"
#include "list.h"

#include "app.h"

global Platform plat;
global App app;
global Client client;

#include "plat.c"
#include "list_linked.c"
#include "client.c"
#include "list.c"
#include "app.c"

void main() {
    plat_construct();
    app_construct();
    while (x11.isRunning) {
        f32 dt = x11_update();
        
        app_update_and_render(dt);
        
        if (x11.width != 0 && x11.height != 0) {
            xgl_render_frame(plat.passes, plat.passIndex);
            x11_swap_buffers();
        }
        else {
            // window is minimized, cannot vsync - instead sleep a bit
            if (x11.enableVSync)
                usleep(10 * 1000);
        }
    }
    
    app_destruct();
}
