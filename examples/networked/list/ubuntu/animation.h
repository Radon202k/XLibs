#ifndef ANIMATION_H
#define ANIMATION_H

typedef enum AnimationType {
    AnimationType_null,
    AnimationType_v2,
    AnimationType_v4,
} AnimationType;

typedef struct Animation {
    bool isAnimating;
    f32 tPlay;
    f32 tTotal;
    void *a;
    void *b;
    void *target;
    AnimationType type;
    Rect2 curve;
} Animation;

static void animation_construct (Animation *anim,
                                 AnimationType type,
                                 Rect2 curve,
                                 void *a, void *b, void *target);
static void animation_destruct  (Animation *anim);
static void animation_update    (Animation *anim, f32 dt);



static void
animation_construct(Animation *anim, 
                    AnimationType type,
                    Rect2 curve,
                    void *a, void *b, void *target) {
    u32 size = 0;
    switch(type) {
        case AnimationType_v2: {
            size = sizeof(v2);
        } break;
        
        case AnimationType_v4: {
            size = sizeof(v4);
        } break;
        
        default: {
            size = 0;
        } break;
    }
    
    anim->a = xalloc(size);
    memcpy(anim->a, a, size);
    
    anim->b = xalloc(size);
    memcpy(anim->b, b, size);
    
    anim->isAnimating = false;
    anim->tPlay = 0;
    anim->tTotal = 0;
    anim->type = type;
    anim->curve = curve;
    anim->target = target;
}

static void
animation_destruct(Animation *anim) {
    xfree(anim->a);
    xfree(anim->b);
    anim->a = 0;
    anim->b = 0;
}

static void
animation_update(Animation *anim, f32 dt) {
    anim->tPlay += dt;
    if (anim->tPlay >= anim->tTotal) {
        anim->isAnimating = false;
    }
    
    v2 easedT;
    switch(anim->type) {
        case AnimationType_v2: {
            v2 a, b;
            v2_copy(anim->a, a);
            v2_copy(anim->b, b);
            v2_bezier(anim->curve.min[0], anim->curve.max[0],
                      anim->curve.min[1], anim->curve.max[1],
                      anim->tPlay, easedT);
            
            v2_lerp(a, easedT[1], b, anim->target);
        } break;
        
        case AnimationType_v4: {
            v4 a, b;
            v4_copy(anim->a, a);
            v4_copy(anim->b, b);
            v2_bezier(anim->curve.min[0], anim->curve.min[1],
                      anim->curve.max[0], anim->curve.max[1],
                      anim->tPlay, easedT);
            v4_lerp(a, easedT[1], b, anim->target);
        } break;
        
        default: {
        } break;
    }
}

#endif //ANIMATION_H
