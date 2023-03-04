void physics_wrap_pos(v2f *pos)
{
    if (pos->x < 0)
        pos->x += xrend.bbs.x;
    if (pos->x >  xrend.bbs.x)
        pos->x -= xrend.bbs.x;
    if (pos->y < 0)
        pos->y += xrend.bbs.y;
    if (pos->y >  xrend.bbs.y)
        pos->y -= xrend.bbs.y;
}

f32 physics_update_ship(void)
{
    // Turn
    f32 turn = 0;
    if (xwin.key.left.down)
        turn = -1;
    if (xwin.key.right.down)
        turn = 1;
    
    // Forward Acceleration
    f32 acc = 0;
    if (xwin.key.up.down) 
        acc = 1;
    
    v2f rotforw = rot2f(gs.ship.forw, gs.ship.rot);
    if (xwin.key.space.pressed)
    {
        Bullet b = 
        {
            gs.ship.pos, rotforw, 8000
        };
        
        Array_push(&gs.bullets, &b);
    }
    
    gs.ship.rot += xrend.dt*SPEED_SHIP_TURN*turn;
    padd2f(&gs.ship.vel, mul2f(xrend.dt*SPEED_SHIP_FORW*acc, rotforw));
    padd2f(&gs.ship.pos, mul2f(xrend.dt, gs.ship.vel));
    physics_wrap_pos(&gs.ship.pos);
    
    return acc;
}

void physics_update_bullets(void)
{
    Array_T asteroidsToBeRemoved = Array_new(8, sizeof(s32));
    Array_T bulletsToBeRemoved = Array_new(8, sizeof(s32));
    
    for (s32 i=0; i<gs.bullets.top; ++i)
    {
        Bullet *b = Array_get(gs.bullets, i);
        
        // Update physics
        v2f lastp = b->pos;
        padd2f(&b->pos, mul2f(SPEED_BULLET*xrend.dt, b->vel));
        f32 traveldistsq = lsq2f(sub2f(b->pos, lastp));
        b->maxtravelsq -= traveldistsq;
        if (b->maxtravelsq < 0)
            Array_remove(&gs.bullets, i);
        
        physics_wrap_pos(&b->pos);
        
        // Test for collisions
        for (s32 j=0; j<gs.asteroids.top; ++j)
        {
            Asteroid *a = Array_get(gs.asteroids, j);
            Circlef A = {a->pos, a->scale.x};
            Circlef B = {b->pos, 20};
            
            v4f circlesColor = eme4f;
            if (circlef_vs_circlef(A, B))
            {
                circlesColor = cri4f;
                
                Array_push(&asteroidsToBeRemoved, &j);
                Array_push(&bulletsToBeRemoved, &i);
            }
            
            v2f p1, p2;
            circlef_vs_circlef_witness(A, B, &p1, &p2);
            xline(p1, p2, circlesColor, 0);
        }
        
        // Draw
        xsprite(engine.layer1, engine.white, 
                b->pos, ini2f(10,10), eme4f,
                ini2f(.5f,.5f), 0, 0);
    }
    
    // Remove bullets and asteroids because of collision
    for (s32 i = 0; i < asteroidsToBeRemoved.top; ++i)
    {
        s32 *asteroidIndex = Array_get(asteroidsToBeRemoved, i);
        
        Asteroid *a = Array_get(gs.asteroids, *asteroidIndex);
        // Spawn little asteroids
        for (s32 k = 0; k < 2; ++k)
            asteroid_new(add2f(a->pos, rnd2f(neg2f(a->scale),a->scale)), 
                         add2f(a->vel, rnd2f(ini2f(-50,-50),ini2f(50,50))), 
                         mul2f(.5f, a->scale), rndi(3,6));
        
        // Free asteroid mesh
        Array_free(&a->mesh);
        // Remove it from array
        Array_remove(&gs.asteroids, *asteroidIndex);
    }
    
    for (s32 i = 0; i < bulletsToBeRemoved.top; ++i)
    {
        s32 *bulletIndex = Array_get(bulletsToBeRemoved, i);
        Array_remove(&gs.bullets, *bulletIndex);
    }
    
    Array_free(&asteroidsToBeRemoved);
    Array_free(&bulletsToBeRemoved);
}

void physics_update_asteroids(void)
{
    for (s32 i=0; i<gs.asteroids.top; ++i)
    {
        Asteroid *a = Array_get(gs.asteroids, i);
        
        padd2f(&a->pos, mul2f(xrend.dt, a->vel));
        physics_wrap_pos(&a->pos);
        
        ConvexPolygonf cp = {
            a->mesh.top,
            xalloc(a->mesh.top*sizeof(v2f)),
        };
        
        for (s32 j=0; j<a->mesh.top; ++j) {
            XVertex3D *v3d = Array_get(a->mesh, j);
            cp.vertices[j].x = a->pos.x + a->scale.x*v3d->pos.x;
            cp.vertices[j].y = a->pos.y + a->scale.y*v3d->pos.y;
        }
        
        Circlef c = {
            xwin.mouse.pos,
            25
        };
        
        xfree(cp.vertices);
    }
}