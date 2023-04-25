#ifndef MESH_H
#define MESH_H

typedef struct MeshVertex {
    v3 position;
    v2 texCoord;
} MeshVertex;

struct glBuffer;
typedef struct Mesh {
    MeshVertex *vertices;
    u32 *indices;
    u32 vertexCount;
    u32 indexCount;
    u32 vbo;
    u32 ebo;
} Mesh;

static void   mesh_push_vertex (Mesh *mesh, v3 position, v2 texCoord);
static void   mesh_push_index  (Mesh *mesh, u32 index);
static Mesh * mesh_cube        (float s);

#endif //MESH_H
