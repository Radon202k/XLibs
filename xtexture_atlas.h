#ifndef XTEXTURE_ATLAS_H
#define XTEXTURE_ATLAS_H

typedef struct {
    v2i at;
    Rect3 uvs;
} XTextureAtlasCoords;

typedef struct {
    v2i dim;
    /* position to blit next */
    v2i at;
    /* y position to go to when moving to next row */
    s32 nextTop;
    s32 uvZ;
} XTextureAtlas;

static void                xtexture_atlas_init (XTextureAtlas *a, v2i dim, s32 uvZ);
static XTextureAtlasCoords xtexture_atlas_put  (XTextureAtlas *a, v2i dim);

/* End of interface










Implementation interface */

/* End of implementation interface







 

*/

static void xtexture_atlas_init(XTextureAtlas *a, v2i dim, s32 uvZ) {
    // Initialize other fields of XTextureAtlas, such as texture, bytes, bottom, and at
    v2i_copy(dim, a->dim);
    a->uvZ = uvZ;
}

static XTextureAtlasCoords 
xtexture_atlas_put(XTextureAtlas *a, v2i dim) {
    XTextureAtlasCoords result = {0};
    
    /* If the rect wont fit in the row */
    if (a->at[0] + dim[0] > a->dim[0]) { 
        a->at[1] = a->nextTop;
        a->at[0] = 0;
    }
    /* If it wont fit at all */
    if (a->at[1] + dim[1] > a->dim[1]) {
        // TODO: Handle
        assert(!"Next image wont fit in the atlas!");
    }
    
    /* Advance next top if this image is taller than current next top allows for */
    if (a->at[1] + dim[1] > a->nextTop) 
        a->nextTop = a->at[1] + dim[1];
    
    /* Calculate uvs */
    v3 min = {
        (f32)a->at[0]/(f32)a->dim[0], 
        (f32)a->at[1]/(f32)a->dim[1], 
        (f32)a->uvZ
    };
    v3 max = {
        (a->at[0]+dim[0])/(f32)a->dim[0], 
        (a->at[1]+dim[1])/(f32)a->dim[1], 
        (f32)a->uvZ
    };
    result.uvs = rect3_min_max(min, max);
    
    v2i_copy(a->at, result.at);
    
    /* Advance at[0] */
    a->at[0] += dim[0];
    
    return result;
}



#endif //XTEXTURE_ATLAS_H
