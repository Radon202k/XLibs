#ifndef RENDERER_H
#define RENDERER_H

typedef struct Transform {
    v3 position;
    v3 scale;
    versor rotation;
} Transform;

typedef struct Renderer {
    Mesh meshes[256];
    u32 meshIndex;
} Renderer;

static Renderer renderer;

#endif //RENDERER_H
