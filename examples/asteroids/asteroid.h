#ifndef ASTEROID_H
#define ASTEROID_H

typedef struct
{
    v2f pos, vel, scale;
    Array_T mesh;
} Asteroid;

void asteroid_new(v2f pos, v2f vel, v2f scale, s32 corners);
void asteroid_generate_mesh(Array_T *mesh, int corners);

#endif