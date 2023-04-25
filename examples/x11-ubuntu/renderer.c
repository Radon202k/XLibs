static void
renderer_update_matrices_rh(mat4 view, mat4 perspective, Camera *camera) {
    v3 cameraP;
    cameraP[0] = camera->targetPos[0] + camera->distance * sin(camera->rotationX) * cos(camera->rotationY);
    cameraP[1] = camera->targetPos[1] + camera->distance * sin(camera->rotationY);
    cameraP[2] = camera->targetPos[2] + camera->distance * cos(camera->rotationX) * cos(camera->rotationY);
    
    // Calculate the camera direction as the vector pointing from the camera position towards the target point
    v3 cameraDir;
    v3_sub(camera->targetPos, cameraP, cameraDir);
    v3_normalize(cameraDir);
    
    mat4_lookat_rh(cameraP, camera->targetPos, (v3){0,1,0}, view);
    mat4_perspective_rh(f32_d2r(45), (f32)x11.width/x11.height, 0.1f, 1000.0f, perspective);
}

static void 
mat4_from_transform(Transform transform, mat4 model) {
    mat4 translation;
    mat4_translation(transform.position, translation);
    
    mat4 scale;
    mat4_scale(transform.scale, scale);
    
    mat4 rotation;
    quat_mat4(transform.rotation, rotation);
    
    mat4 identity = {
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
    };
    mat4_mul(identity, translation, model);
    mat4_mul(model, scale, model);
    mat4_mul(model, rotation, model);
}

static Transform
transform(v3 p, v3 scale, versor rotation) {
    Transform r;
    v3_copy(p, r.position);
    v3_copy(scale, r.scale);
    v4_copy(rotation, r.rotation);
    return r;
}

static Camera
camera_orbit(v3 targetP, f32 dist, f32 rotX, f32 rotY) {
    Camera camera;
    v3_copy(targetP, camera.targetPos);
    camera.distance = dist;
    camera.rotationX = rotX;
    camera.rotationY = rotY;
    return camera;
}
