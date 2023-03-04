#ifndef PHYSICS_H
#define PHYSICS_H

#define SPEED_SHIP_FORW   500
#define SPEED_SHIP_TURN   200
#define SPEED_BULLET      900

void physics_wrap_pos(v2f *pos);
f32 physics_update_ship(void);
void physics_update_bullets(void);
void physics_update_asteroids(void);


#endif //PHYSICS_H
