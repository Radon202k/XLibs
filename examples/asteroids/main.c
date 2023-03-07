#include "engine.h"
#include "physics.h"
#include "asteroid.h"
#include "bullet.h"
#include "ship.h"
#include "game.h"

global GameState gs;

#include "engine.c"
#include "physics.c"
#include "asteroid.c"
#include "bullet.c"
#include "ship.c"
#include "game.c"

XWINMAIN()
{
    engine_init();
    game_init();
    
    while (xrnd.running)
    {
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        
        game_update_and_render();
        
        xsprite(engine.layer1, engine.noiseTest, 
                (v2f){0,0}, xrnd.backBufferSize, (v4f){1,1,1,1},
                (v2f){0,0}, 0, -10); 
        
        xsound_post_update();                      // Update XSound
        xwinupdate(xrnd.topDown, xrnd.windowSize); // Update XWindows
        xrender_update();                          // Update XRender
        
        xsound.flipWallClock = xwallclock();
        
        // Audio debug
        DWORD pc, wc;
        if(SUCCEEDED(IDirectSoundBuffer_GetCurrentPosition(xsound.buffer, &pc, &wc)))
        {
            xsound.pc = pc;
            xsound.wc = wc;
        }
    }
    
    game_shutdown();
    engine_shutdown();
}

