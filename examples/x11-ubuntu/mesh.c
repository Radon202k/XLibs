static void
mesh_push_vertex(Mesh* mesh, v3 position, v2 texCoord) {
    v3_copy(position, mesh->vertices[mesh->vertexCount].position);
    v2_copy(texCoord, mesh->vertices[mesh->vertexCount].texCoord);
    mesh->vertexCount++;
}

static void
mesh_push_index(Mesh* mesh, GLuint index) {
    mesh->indices[mesh->indexCount] = index;
    mesh->indexCount++;
}

static Mesh *
mesh_cube(float s) {
    assert(renderer.meshIndex < narray(renderer.meshes));
    Mesh *mesh = renderer.meshes + renderer.meshIndex++;
    mesh->vertexCount = 0;
    mesh->indexCount = 0;
    mesh->vertices = xalloc(24 * sizeof(MeshVertex));
    mesh->indices = xalloc(36 * sizeof(GLuint));
    
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
    GLuint indices[] = {
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
