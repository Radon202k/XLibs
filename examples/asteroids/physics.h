#ifndef PHYSICS_H
#define PHYSICS_H

#define SPEED_SHIP_FORW   500
#define SPEED_SHIP_TURN   200
#define SPEED_BULLET      900

typedef enum
{
    RigidBodyType_null,
    RigidBodyType_ship,
    RigidBodyType_bullet,
    RigidBodyType_asteroid,
    
} RigidBodyType;

/*  arrayIndex represents the index of the rigidbody object in its
'   corresponding array, e.g., if the body is of type RigidBodyType_bullet
'   and arrayIndex is 2, it means that the bullet is at gs.bullets[2]. */

typedef struct RigidBody RigidBody;

struct RigidBody
{
    RigidBodyType type;
    u32 arrayIndex;
    
    v2f center;
    f32 radius;
    
    v2i *cellChain; // Linked list of cells
    
    RigidBody *nextInCell; // Linked list
};

/*  If the grid has 6 columns and 3 rows, ncell is v2i(6,3).
  '   storage holds exactly ncell.x*ncell.y rigid bodies.  */

typedef struct
{
    v2i cellSize;
    v2i ncell;
    RigidBody **storage;
    
} PhysicsGrid;

void physics_wrap_pos        (v2f *pos);
f32  physics_update_ship     (void);
void physics_update_bullets  (PhysicsGrid grid);
void physics_update_asteroids(PhysicsGrid grid);

PhysicsGrid physics_grid_generate(v2i cellSize);
void        physics_grid_free(PhysicsGrid grid);
void        physics_grid_set (PhysicsGrid *grid, s32 x, s32 y, RigidBodyType type,
                              s32 arrayIndex, v2f center, f32 radius);
void        physics_grid_draw(v2i cellSize, v2i cellCount);
v2i         physics_grid_cell(v2f pos, v2i cellSize);

#endif //PHYSICS_H
