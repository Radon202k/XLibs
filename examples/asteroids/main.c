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
    
    while (xrend.run)
    {
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        
        game_update_and_render();
        
        xsound_post_update();           // Update XSound
        xwinupdate(xrend.td, xrend.wd); // Update XWindows
        xrendupdate();                  // Update XRender
        
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

