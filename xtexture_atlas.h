#ifndef XTEXTURE_ATLAS_H
#define XTEXTURE_ATLAS_H

typedef struct {
    v2i at;
    rect3f uvs;
} XTextureAtlasCoords;

typedef struct {
    v2i dim;
    /* position to blit next */
    v2i at;
    /* y position to go to when moving to next row */
    s32 nextTop;
    s32 uvZ;
} XTextureAtlas;

function void                xtexture_atlas_init (XTextureAtlas *a, v2i dim, s32 uvZ);
function XTextureAtlasCoords xtexture_atlas_put  (XTextureAtlas *a, v2i dim);

/* End of interface










Implementation interface */

/* End of implementation interface







 

*/

function void xtexture_atlas_init(XTextureAtlas *a, v2i dim, s32 uvZ) {
    // Initialize other fields of XTextureAtlas, such as texture, bytes, bottom, and at
    a->dim = dim;
    a->uvZ = uvZ;
}

function XTextureAtlasCoords xtexture_atlas_put(XTextureAtlas *a, v2i dim) {
    XTextureAtlasCoords result = {0};
    
    /* If the rect wont fit in the row */
    if (a->at.x + dim.x > a->dim.x) { 
        a->at.y = a->nextTop;
        a->at.x = 0;
    }
    /* If it wont fit at all */
    if (a->at.y + dim.y > a->dim.y) {
        // TODO: Handle
        assert(!"Next image wont fit in the atlas!");
    }
    
    /* Advance next top if this image is taller than current next top allows for */
    if (a->at.y + dim.y > a->nextTop) 
        a->nextTop = a->at.y + dim.y;
    
    /* Calculate uvs */
    result.uvs = (rect3f){
        (v3f){a->at.x/(f32)a->dim.x, a->at.y/(f32)a->dim.y, (f32)a->uvZ},
        (v3f){(a->at.x+dim.x)/(f32)a->dim.x, (a->at.y+dim.y)/(f32)a->dim.y, (f32)a->uvZ}
    };
    
    result.at = a->at;
    
    /* Advance at.x */
    a->at.x += dim.x;
    
    return result;
}



#endif //XTEXTURE_ATLAS_H
