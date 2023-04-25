#include "XLibs/xbase.h"
#include "XLibs/xmemory.h"
#include "XLibs/xmath.h"
#include "XLibs/xstring.h"
#include "XLibs/xsocket.h"

#include "animation.h"
#include "list_linked.h"
#include "serialization.h"
#include "packet.h"
#include "server.h"
#include "linking_context.h"

global Server server;

#include "list_linked.c"
#include "server.c"

static void
headless_server_construct(void) {
    list_linked_construct(&server.list);
    v2_copy((v2){30,20}, server.list.elDim);
    server.list.margin = 5;
    // server.list.font = plat.fonts[2];
    
}

int main() {
    server_construct();
    
    struct timespec c1;
    
    u32 tick=0;
    while (1) {
        struct timespec c2;
        clock_gettime(CLOCK_MONOTONIC, &c2);
        f32 dt = (f32)(c2.tv_sec-c1.tv_sec) + 1e-9f*(c2.tv_nsec-c1.tv_nsec);
        
        c1 = c2;
        
        server_update(dt);
        
        usleep(250 * 1000);
    }
    
    server_destruct();
    
    return 0;
}