#ifndef CAMERA_H
#define CAMERA_H

typedef struct Camera {
    /* Orbit camera variables */
    v3 targetPos;
    f32 distance;
    f32 rotationX;
    f32 rotationY;
    /* Other camera types */
    // TODO: Union maybe?
} Camera;

#endif //CAMERA_H
