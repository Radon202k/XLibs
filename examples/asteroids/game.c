void game_init(void)
{
    gs.ship.pos = mul2f(.5f, xrend.bbs);
    gs.ship.dim = ini2f(50,50);
    gs.ship.forw = ini2f(1,0);
    
    gs.bullets = Array_new(16, sizeof(Bullet));
    gs.asteroids = Array_new(16, sizeof(Asteroid));
    
    for (s32 i=0; i<1; ++i)
        asteroid_new(rnd2f(ini2f(0,0), xrend.bbs), 
                     rnd2f(ini2f(-50,-50), ini2f(50,50)), 
                     rnd2f(ini2f(50,50), ini2f(100,100)), 
                     rndi(5,7));
    
    xsoundinit(xrend.wh);
}

void game_update_and_render(void)
{
    f32 acc = physics_update_ship();
    physics_update_bullets();
    physics_update_asteroids();
    
    // Draw fire behind ship
    if (acc>0)
    {
        for (s32 i = 0; i < 3; ++i)
        {
            v2f r = rnd2f(ini2f(-10,-10), ini2f(10,10));
            
            v2f rotforw = rot2f(gs.ship.forw, gs.ship.rot);
            xline(gs.ship.pos, 
                  sub2f(add2f(gs.ship.pos, r), mul2f(50,rotforw)), 
                  red4f, 0);
        }
    }
    
    asteroids_draw(); // Draw asteroids
    ship_draw();      // Draw ship
    
#if 0
    // Test draw sound buffer
    f32 soundBufwidth = 1000;
    
    xsprite(gs.layer1, gs.white, 
            ini2f(0,0), 
            ini2f(soundBufwidth,50), gol4f, ini2f(0,0), 0, 0);
    
    for (u32 i=0; i<=narray(xsound.pcs); ++i)
    {
        xsprite(gs.layer1, gs.white, 
                ini2f((f32)xsound.pcs[i] / xsound.bufSize * soundBufwidth,0), 
                ini2f(2, 20.0f), cri4f, ini2f(0,0), 0, 0);
        
        xsprite(gs.layer1, gs.white, 
                ini2f((f32)xsound.wcs[i] / xsound.bufSize * soundBufwidth,0), 
                ini2f(2, 20.0f), eme4f, ini2f(0,0), 0, 0);
    }
    
    xsprite(gs.layer1, gs.white, 
            ini2f((f32)xsound.pc / xsound.bufSize * soundBufwidth,0), 
            ini2f(6, 50.0f), cri4f, ini2f(0,0), 0, 0);
    
    xsprite(gs.layer1, gs.white, 
            ini2f((f32)xsound.wc / xsound.bufSize * soundBufwidth,0), 
            ini2f(6, 50.0f), eme4f, ini2f(0,0), 0, 0);
#endif
    
    // End of frame
}

void game_shutdown(void)
{
    for (s32 i=0; i<gs.asteroids.top; ++i)
    {
        Asteroid *a = Array_get(gs.asteroids, i);
        Array_free(&a->mesh);
    }
    
    Array_free(&gs.asteroids);
    Array_free(&gs.bullets);
}
