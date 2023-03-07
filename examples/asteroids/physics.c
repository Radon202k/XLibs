void physics_wrap_pos(v2f *pos)
{
    if (pos->x < 0)
        pos->x += xrnd.backBufferSize.x;
    if (pos->x >  xrnd.backBufferSize.x)
        pos->x -= xrnd.backBufferSize.x;
    if (pos->y < 0)
        pos->y += xrnd.backBufferSize.y;
    if (pos->y >  xrnd.backBufferSize.y)
        pos->y -= xrnd.backBufferSize.y;
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
    
    gs.ship.rot += xrnd.dt*SPEED_SHIP_TURN*turn;
    padd2f(&gs.ship.vel, mul2f(xrnd.dt*SPEED_SHIP_FORW*acc, rotforw));
    padd2f(&gs.ship.pos, mul2f(xrnd.dt, gs.ship.vel));
    physics_wrap_pos(&gs.ship.pos);
    
    return acc;
}

void physics_update_bullets(PhysicsGrid grid)
{
    Array_T asteroidsToBeRemoved = Array_new(8, sizeof(s32));
    Array_T bulletsToBeRemoved = Array_new(8, sizeof(s32));
    
    for (s32 i=0; i<gs.bullets.top; ++i)
    {
        Bullet *b = Array_get(gs.bullets, i);
        
        // Update physics
        v2f lastp = b->pos;
        padd2f(&b->pos, mul2f(SPEED_BULLET*xrnd.dt, b->vel));
        f32 traveldistsq = lsq2f(sub2f(b->pos, lastp));
        b->maxtravelsq -= traveldistsq;
        if (b->maxtravelsq < 0)
            Array_remove(&gs.bullets, i);
        
        physics_wrap_pos(&b->pos);
        
        // Test for collisions
        v2i bulletGridCell = physics_grid_cell(b->pos, grid.cellSize);
        RigidBody *body = grid.storage[bulletGridCell.x + grid.ncell.x*bulletGridCell.y];
        while (body)
        {
            Asteroid *a = Array_get(gs.asteroids, body->arrayIndex);
            Circlef A = {a->pos, a->scale.x};
            Circlef B = {b->pos, 20};
            
            v4f circlesColor = eme4f;
            if (circlef_vs_circlef(A, B))
            {
                circlesColor = cri4f;
                
                Array_push(&asteroidsToBeRemoved, &body->arrayIndex);
                Array_push(&bulletsToBeRemoved, &i);
            }
            
            v2f p1, p2;
            circlef_vs_circlef_witness(A, B, &p1, &p2);
            xline(p1, p2, circlesColor, 0);
            
            body = body->nextInCell;
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
        for (s32 k = 0; k < rndi(2, 5); ++k)
            asteroid_new(add2f(a->pos, rnd2f(neg2f(a->scale),a->scale)), 
                         add2f(a->vel, rnd2f(ini2f(-100,-100),ini2f(100,100))), 
                         mul2f(rndf(.5f,.75f), a->scale), rndi(3,6));
        
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

typedef struct 
{
    u32 arrayIndexA;
    u32 arrayIndexB;
    
} CollisionPair;

void physics_update_asteroids(PhysicsGrid grid)
{
    if (xrnd.dt == 0) return;
    
    Array_T collisionsTested = Array_new(256, sizeof(CollisionPair));
    
    for (u32 i=0; i<(u32)gs.asteroids.top; ++i)
    {
        Asteroid *a = Array_get(gs.asteroids, i);
        physics_wrap_pos(&a->pos);
        
        v2f motion = mul2f(xrnd.dt, a->vel);
        padd2f(&a->pos, motion);
        
        /* Test for collisions. */
        
        /* Calculate the grid cell in which the asteroid being tested against
           _  falls into after being moved by physics just now. */
        v2i aGridCell = physics_grid_cell(a->pos, grid.cellSize);
        
        Circlef A = asteroid_hitbox(a);
        
        /* Traverse RigidBody linked list stored at the grid cell a is in. */
        RigidBody *body = grid.storage[aGridCell.x + grid.ncell.x*aGridCell.y];
        while (body)
        {
            switch (body->type)
            {
                case RigidBodyType_asteroid:
                {
                    Asteroid *b = Array_get(gs.asteroids, body->arrayIndex);
                    
                    /* Avoid testing the same body against itself. */
                    if (body->arrayIndex != i)
                    {
                        Circlef B = {body->center, body->radius};
                        
                        bool alreadyTested = false;
                        /* Check in the collisionsTested array to see if we tested
         '  B vs A and now are about to test A vs B to avoid that. */
                        for (s32 pairIndex=0; !alreadyTested && pairIndex<collisionsTested.top; ++pairIndex)
                        {
                            CollisionPair *pair = Array_get(collisionsTested, pairIndex);
                            if (pair->arrayIndexA == body->arrayIndex &&
                                pair->arrayIndexB == i)
                            {
                                alreadyTested = true;
                            }
                        }
                        
                        /* Do the collision test. */
                        if (!alreadyTested && circlef_vs_circlef(A, B))
                        {
                            /* Book keep the collision to assert that we never test
        '  A vs B then test again B vs A in the same frame. */
                            CollisionPair pair = 
                            {
                                i, body->arrayIndex
                            };
                            Array_push(&collisionsTested, &pair);
                            
                            f32 dist = circlef_vs_circlef_dist(A, B);
                            
                            v2f axis  = nrm2f(sub2f(B.center, A.center));
                            
                            v2f naxis = neg2f(axis);
                            
                            // Move position
                            padd2f(&a->pos, mul2f((dist+1), naxis));
                            
                            // Make em bounce
                            padd2f(&a->vel, mul2f(-1.98f*dot2f(a->vel, naxis), naxis));
                            padd2f(&b->vel, mul2f(-1.98f*dot2f(b->vel, axis), axis));
                        }
                        
                        v2f p1, p2;
                        circlef_vs_circlef_witness(A, B, &p1, &p2);
                        xline(p1, p2, cob4f, 0);
                    }
                } break;
                
                case RigidBodyType_bullet:
                {
                    int y = 3;
                } break;
                
                default:
                {
                } break;
            }
            
            body = body->nextInCell;
        }
        
    }
    
    // DEBUG assert to make sure we are only testing A vs B and not B vs A
    if (collisionsTested.top > 1)
    {
        for (s32 pairIndex=0; pairIndex<collisionsTested.top; ++pairIndex)
        {
            CollisionPair *a = Array_get(collisionsTested, pairIndex);
            
            for (s32 otherPair=0; otherPair<collisionsTested.top; ++otherPair)
            {
                CollisionPair *b = Array_get(collisionsTested, pairIndex);
                
                if (a->arrayIndexA == b->arrayIndexB &&
                    a->arrayIndexB == b->arrayIndexA)
                {
                    assert(!"We should not test A vs B and B vs A !!");
                }
            }
        }
    }
    
    Array_free(&collisionsTested);
}

v2i physics_grid_cell(v2f pos, v2i cellSize)
{
    v2i r = 
    {
        (s32)((f32)pos.x / (f32)cellSize.x),
        (s32)((f32)pos.y / (f32)cellSize.y),
    };
    return r;
}

PhysicsGrid physics_grid_generate(v2i cellSize)
{
    v2i cellCount = 
    {
        (s32)ceilf(xrnd.backBufferSize.x / cellSize.x),
        (s32)ceilf(xrnd.backBufferSize.y / cellSize.y),
    };
    
    /* Construct grid */
    PhysicsGrid grid = 
    {
        cellSize, cellCount,
        xalloc(cellCount.x*cellCount.y*sizeof(RigidBody *)),
    };
    
    // Put asteroids into grid
    for (s32 i=0; i<gs.asteroids.top; ++i)
    {
        Asteroid *a = Array_get(gs.asteroids, i);
        
        Circlef hitbox = asteroid_hitbox(a);
        
        v2i cell = physics_grid_cell(hitbox.center, cellSize);
        
        f32 offsetX = hitbox.center.x - cell.x*cellSize.x;
        f32 offsetY = hitbox.center.y - cell.y*cellSize.y;
        
        // Add to cell that the center is in
        v4f c = cob4f;
        c.a = .2f;
        physics_grid_set(&grid, cell.x, cell.y, RigidBodyType_asteroid, i,
                         hitbox.center, hitbox.radius);
        
        xsprite(engine.layer1, engine.white,
                ini2f((f32)cell.x*cellSize.x, (f32)cell.y*cellSize.y),
                ini2fs(cellSize.x, cellSize.y),
                c, ini2f(0,0), 0, 0);
        
        bool overlapsLeftCell, overlapsRightCell, 
        overlapsTopCell, overlapsBotCell,
        overlapsTopLeftCell, overlapsBotLeftCell,
        overlapsTopRightCell, overlapsBotRightCell;
        
        overlapsLeftCell = overlapsRightCell = overlapsTopCell =
            overlapsBotCell = overlapsTopLeftCell = overlapsBotLeftCell =
            overlapsTopRightCell = overlapsBotRightCell = false;
        
        if ((offsetX - hitbox.radius) < 0) overlapsLeftCell = true;
        if ((offsetY - hitbox.radius) < 0) overlapsTopCell = true;
        if ((offsetX + hitbox.radius) >= cellSize.x) overlapsRightCell = true;
        if ((offsetY + hitbox.radius) >= cellSize.y) overlapsBotCell = true;
        if (overlapsLeftCell && overlapsTopCell)  overlapsTopLeftCell = true;
        if (overlapsLeftCell && overlapsBotCell)  overlapsBotLeftCell = true;
        if (overlapsRightCell && overlapsTopCell) overlapsTopRightCell = true;
        if (overlapsRightCell && overlapsBotCell) overlapsBotRightCell = true;
        
        // Add to the cell if it exists
        
        c = rub4f;
        c.a = .2f;
        
        // Left
        if (overlapsLeftCell && (cell.x > 0))
        {
            physics_grid_set(&grid, cell.x-1, cell.y, 
                             RigidBodyType_asteroid, i,
                             hitbox.center, hitbox.radius);
            
            xsprite(engine.layer1, engine.white,
                    ini2f((f32)(cell.x-1)*cellSize.x, (f32)cell.y*cellSize.y),
                    ini2fs(cellSize.x, cellSize.y),
                    c, ini2f(0,0), 0, 0);
        }
        
        // Top
        if (overlapsTopCell && (cell.y > 0))
        {
            physics_grid_set(&grid, cell.x, cell.y-1, 
                             RigidBodyType_asteroid, i,
                             hitbox.center, hitbox.radius);
            
            xsprite(engine.layer1, engine.white,
                    ini2f((f32)cell.x*cellSize.x, (f32)(cell.y-1)*cellSize.y),
                    ini2fs(cellSize.x, cellSize.y),
                    c, ini2f(0,0), 0, 0);
        }
        
        // Right
        if (overlapsRightCell && (cell.x < (cellCount.x-1)))
        {
            physics_grid_set(&grid, cell.x+1, cell.y, 
                             RigidBodyType_asteroid, i,
                             hitbox.center, hitbox.radius);
            
            xsprite(engine.layer1, engine.white,
                    ini2f((f32)(cell.x+1)*cellSize.x, (f32)cell.y*cellSize.y),
                    ini2fs(cellSize.x, cellSize.y),
                    c, ini2f(0,0), 0, 0);
        }
        
        // Bot
        if (overlapsBotCell && (cell.y < (cellCount.y-1)))
        {
            physics_grid_set(&grid, cell.x, cell.y+1, 
                             RigidBodyType_asteroid, i,
                             hitbox.center, hitbox.radius);
            
            xsprite(engine.layer1, engine.white,
                    ini2f((f32)cell.x*cellSize.x, (f32)(cell.y+1)*cellSize.y),
                    ini2fs(cellSize.x, cellSize.y),
                    c, ini2f(0,0), 0, 0);
        }
        
        // Top-Left
        if (overlapsTopLeftCell && (cell.x > 0 && cell.y > 0))
        {
            physics_grid_set(&grid, cell.x-1, cell.y-1, 
                             RigidBodyType_asteroid, i,
                             hitbox.center, hitbox.radius);
            
            xsprite(engine.layer1, engine.white,
                    ini2f((f32)(cell.x-1)*cellSize.x, (f32)(cell.y-1)*cellSize.y),
                    ini2fs(cellSize.x, cellSize.y),
                    c, ini2f(0,0), 0, 0);
        }
        
        // Top-Right
        if (overlapsTopRightCell && (cell.x < (cellCount.x-1) && cell.y > 0))
        {
            physics_grid_set(&grid, cell.x+1, cell.y-1, 
                             RigidBodyType_asteroid, i,
                             hitbox.center, hitbox.radius);
            
            xsprite(engine.layer1, engine.white,
                    ini2f((f32)(cell.x+1)*cellSize.x, (f32)(cell.y-1)*cellSize.y),
                    ini2fs(cellSize.x, cellSize.y),
                    c, ini2f(0,0), 0, 0);
        }
        
        // Bot-Left
        if (overlapsBotLeftCell && (cell.x > 0 && cell.y < (cellCount.y-1)))
        {
            physics_grid_set(&grid, cell.x-1, cell.y+1, 
                             RigidBodyType_asteroid, i,
                             hitbox.center, hitbox.radius);
            
            xsprite(engine.layer1, engine.white,
                    ini2f((f32)(cell.x-1)*cellSize.x, (f32)(cell.y+1)*cellSize.y),
                    ini2fs(cellSize.x, cellSize.y),
                    c, ini2f(0,0), 0, 0);
        }
        
        // Bot-Right
        if (overlapsBotRightCell && (cell.x < (cellCount.x-1) && cell.y < (cellCount.y-1)))
        {
            physics_grid_set(&grid, cell.x+1, cell.y+1, 
                             RigidBodyType_asteroid, i,
                             hitbox.center, hitbox.radius);
            
            xsprite(engine.layer1, engine.white,
                    ini2f((f32)(cell.x+1)*cellSize.x, (f32)(cell.y+1)*cellSize.y),
                    ini2fs(cellSize.x, cellSize.y),
                    c, ini2f(0,0), 0, 0);
        }
    }
    
    return grid;
}

void physics_grid_free(PhysicsGrid grid)
{
    /* Free grid */
    for (s32 i=0; i<(grid.ncell.x*grid.ncell.y); ++i)
    {
        RigidBody *body = grid.storage[i];
        while (body)
        {
            RigidBody *next = body->nextInCell;
            xfree(body);
            
            body = next;
        }
    }
    
    xfree(grid.storage);
}

void physics_grid_draw(v2i cellSize, v2i cellCount)
{
    // Horizontal lines
    for (s32 i=0; i<=cellCount.y; ++i)
    {
        f32 y = (f32)(i * cellSize.y);
        xline(ini2f(0, y),
              ini2f(xrnd.backBufferSize.x, y),
              gol4f, 0);
    }
    
    // Vertical lines
    for (s32 i=0; i<=cellCount.x; ++i)
    {
        f32 x = (f32)(i * cellSize.x);
        xline(ini2f(x, 0),
              ini2f(x, xrnd.backBufferSize.y),
              gol4f, 0);
    }
}

void physics_grid_set(PhysicsGrid *grid, s32 x, s32 y, 
                      RigidBodyType type, s32 arrayIndex,
                      v2f center, f32 radius)
{
    assert(x >= 0 && y >= 0);
    assert(x < grid->ncell.x && y < grid->ncell.y);
    
    RigidBody *body = xalloc(sizeof(RigidBody));
    body->type = type;
    body->arrayIndex = arrayIndex;
    body->center = center;
    body->radius = radius;
    
    s32 index = x + y*grid->ncell.x;
    assert(index < grid->ncell.x*grid->ncell.y);
    
    body->nextInCell = grid->storage[index];
    grid->storage[index] = body;
}
