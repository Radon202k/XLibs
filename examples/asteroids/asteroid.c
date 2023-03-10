void asteroid_new(v2f pos, v2f vel, v2f scale, s32 corners)
{
    Asteroid temp = {
        pos,
        vel,
        scale,
        Array_new(5, sizeof(XVertex3D)),
    };
    
    asteroid_generate_mesh(&temp.mesh, corners);
    Array_push(&gs.asteroids, &temp);
}

void asteroid_generate_mesh(Array_T *mesh, int corners)
{
    f32 umin = engine.white.uv.min.x;
    f32 umax = engine.white.uv.max.x;
    f32 vmin = engine.white.uv.min.y;
    f32 vmax = engine.white.uv.max.y;
    
    v2f p[256];
    for (s32 i=0; i<corners; ++i)
        p[i] = ini2f(sinf(radf(i*360.0f/corners)) + rndf(-.1f,.1f), 
                     cosf(radf(i*360.0f/corners)) + rndf(-.1f,.1f));
    
    for (s32 i=0; i<corners-2; ++i)
    {
        XVertex3D temp[3] = {
            {p[0].x,p[0].y, 0.0f, umin, vmin, 1.0f, 1.0f, 1.0f, 1.0f},
            {p[i + 2].x,p[i + 2].y, 0.0f, umax, vmax, 1.0f, 1.0f, 1.0f, 1.0f},
            {p[i + 1].x,p[i + 1].y, 0.0f, umax, vmin, 1.0f, 1.0f, 1.0f, 1.0f},
        };
        
        Array_push(mesh, &temp[0]);
        Array_push(mesh, &temp[1]);
        Array_push(mesh, &temp[2]);
    }
}

Circlef asteroid_hitbox(Asteroid *a)
{
    Circlef hitbox = {a->pos, a->scale.x-10};
    return hitbox;
}

void asteroids_draw(void)
{
    // Draw asteroids
    for (s32 i = 0; i < gs.asteroids.top; ++i)
    {
        Asteroid *a = Array_get(gs.asteroids, i);
        
        xmesh(engine.meshlayer1, a->mesh, a->pos, a->scale, 
              ini4f(1,1,1,1), ini2f(.5f,.5f), 0, 0);
        
#if 0
        xlinemesh(a->mesh, a->pos, a->scale, gol4f, 10);
        
        Circlef hitbox = asteroid_hitbox(a);
        xlinecircle(hitbox.center, hitbox.radius, 20, eme4f, 20);
#endif
        
#if 0
        // X pos
        wchar_t *s = xstrfromint((s32)a->pos.x);
        xstring(engine.layer1, engine.font, s, a->pos, gol4f, ini2f(0,0), 0, 10, true);
        xfree(s);
        
        // Y pos
        s = xstrfromint((s32)a->pos.y);
        xstring(engine.layer1, engine.font, s, 
                add2f(a->pos, ini2f(50,0)), gol4f, ini2f(0,0), 0, 10, true);
        xfree(s);
#endif
    }
}