#ifndef X3D_H
#define X3D_H

typedef struct Transform {
    v3 position;
    v3 scale;
    versor rotation;
} Transform;

typedef struct MeshVertex {
    v3 position;
    v2 texCoord;
} MeshVertex;

typedef struct Mesh {
    MeshVertex *vertices;
    u32 *indices;
    u32 vertexCount;
    u32 indexCount;
    u32 vbo;
    u32 ebo;
} Mesh;

typedef struct Camera {
    /* Orbit camera variables */
    v3 targetPos;
    f32 distance;
    f32 rotationX;
    f32 rotationY;
    /* Other camera types */
    // TODO: Union maybe?
} Camera;

typedef struct X3D {
    Mesh meshes[256];
    u32 meshIndex;
} X3D;


static void      x3d_update_matrices_rh (mat4 v, mat4 p, Camera *c);
static void      x3d_object_transform   (Transform t, mat4 d);
static Transform x3d_transform          (v3 p, v3 s, versor r);
static Camera    x3d_camera_orbit       (v3 t, f32 d, f32 rx, f32 ry);

static void   mesh_push_vertex (Mesh *mesh, v3 position, v2 texCoord);
static void   mesh_push_index  (Mesh *mesh, u32 index);
static Mesh * mesh_cube        (float s);







static X3D x3d;





static void
renderer_update_matrices_rh(Camera *camera, f32 aspectRatio, 
                            mat4 destView, mat4 destProj) {
    v3 cameraP;
    cameraP[0] = camera->targetPos[0] + camera->distance * sin(camera->rotationX) * cos(camera->rotationY);
    cameraP[1] = camera->targetPos[1] + camera->distance * sin(camera->rotationY);
    cameraP[2] = camera->targetPos[2] + camera->distance * cos(camera->rotationX) * cos(camera->rotationY);
    
    // Calculate the camera direction as the vector pointing from the camera position towards the target point
    v3 cameraDir;
    v3_sub(camera->targetPos, cameraP, cameraDir);
    v3_normalize(cameraDir);
    
    mat4_lookat_rh(cameraP, camera->targetPos, (v3){0,1,0}, destView);
    mat4_perspective_rh(f32_d2r(45), aspectRatio, 0.1f, 1000.0f, destProj);
}

static void
x3d_object_transform(Transform transform, mat4 destObjModel) {
    mat4 translation, scale, rotation;
    mat4_translation(transform.position, translation);
    mat4_scale(transform.scale, scale);
    quat_mat4(transform.rotation, rotation);
    mat4 identity = {
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
    };
    mat4_mul(identity, translation, destObjModel);
    mat4_mul(destObjModel, scale, destObjModel);
    mat4_mul(destObjModel, rotation, destObjModel);
}

static Transform
x3d_transform(v3 p, v3 scale, versor rotation) {
    Transform r;
    v3_copy(p, r.position);
    v3_copy(scale, r.scale);
    v4_copy(rotation, r.rotation);
    return r;
}

static Camera
x3d_camera_orbit(v3 targetP, f32 dist, f32 rotX, f32 rotY) {
    Camera camera;
    v3_copy(targetP, camera.targetPos);
    camera.distance = dist;
    camera.rotationX = rotX;
    camera.rotationY = rotY;
    return camera;
}

static void
mesh_push_vertex(Mesh* mesh, v3 position, v2 texCoord) {
    v3_copy(position, mesh->vertices[mesh->vertexCount].position);
    v2_copy(texCoord, mesh->vertices[mesh->vertexCount].texCoord);
    mesh->vertexCount++;
}

static void
mesh_push_index(Mesh* mesh, u32 index) {
    mesh->indices[mesh->indexCount] = index;
    mesh->indexCount++;
}

static Mesh *
mesh_cube(float s) {
    Mesh *mesh = xalloc(sizeof *mesh);
    mesh->vertexCount = 0;
    mesh->indexCount = 0;
    mesh->vertices = xalloc(24 * sizeof(MeshVertex));
    mesh->indices = xalloc(36 * sizeof(u32));
    
    float hs = s * 0.5f; // Half-size
    
    // Define positions and texCoords for each face
    v3 positions[] = {
        // Front face
        {-hs, -hs, hs}, {hs, -hs, hs}, {hs, hs, hs}, {-hs, hs, hs},
        // Back face
        {hs, -hs, -hs}, {-hs, -hs, -hs}, {-hs, hs, -hs}, {hs, hs, -hs},
        // Left face
        {-hs, -hs, -hs}, {-hs, -hs, hs}, {-hs, hs, hs}, {-hs, hs, -hs},
        // Right face
        {hs, -hs, hs}, {hs, -hs, -hs}, {hs, hs, -hs}, {hs, hs, hs},
        // Top face
        {-hs, hs, hs}, {hs, hs, hs}, {hs, hs, -hs}, {-hs, hs, -hs},
        // Bottom face
        {-hs, -hs, -hs}, {hs, -hs, -hs}, {hs, -hs, hs}, {-hs, -hs, hs}
    };
    
    v2 texCoords[] = {
        {0, 1}, {1, 1}, {1, 0}, {0, 0}
    };
    
    // Push vertices for each face
    for (int i = 0; i < 24; ++i) {
        mesh_push_vertex(mesh, positions[i], texCoords[i % 4]);
    }
    
    // Push indices for each face
    u32 indices[] = {
        0, 1, 2, 2, 3, 0,       // Front face
        4, 5, 6, 6, 7, 4,       // Back face
        8, 9, 10, 10, 11, 8,    // Left face
        12, 13, 14, 14, 15, 12, // Right face
        16, 17, 18, 18, 19, 16, // Top face
        20, 21, 22, 22, 23, 20  // Bottom face
    };
    
    for (int i = 0; i < 36; ++i) {
        mesh_push_index(mesh, indices[i]);
    }
    
    return mesh;
}


#endif //X3D_H
