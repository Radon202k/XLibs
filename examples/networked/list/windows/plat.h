#ifndef PLAT_H
#define PLAT_H

#define STB_IMAGE_IMPLEMENTATION
#include "w:\libs\stb_image.h"

#define XD11_RELEASE_BUILD

#include "w:/libs/xlibs/xbase.h"
#include "w:/libs/xlibs/xmemory.h"
#include "w:/libs/xlibs/xmath.h"
#include "w:/libs/xlibs/xarray.h"
#include "w:/libs/xlibs/xlist.h"
#include "w:/libs/xlibs/xtable.h"
#include "w:/libs/xlibs/xtexture_atlas.h"
#include "w:/libs/xlibs/xrect_cut.h"

#include "w:/libs/xlibs/xwindows.h"
#include "w:/libs/xlibs/xd3d11.h"
#include "w:/libs/xlibs/xrender2d.h"
#include "w:/libs/xlibs/xsocket.h"

#define MINIAUDIO_IMPLEMENTATION
#include "w:/libs/miniaudio.h"

#include "w:/libs/xlibs/xsound.h"
#include "w:/libs/xlibs/ximgui.h"

typedef struct Platform {
    /* Layers or Render batches/passes */
    XRenderBatch layers[32];
    u32 layerIndex;
    /* Images or Sprites/Textures */
    XSprite images[32];
    u32 imageIndex;
    /* Fonts (Basically maps of glyphs to sprites and align information */
    XFont *fonts[32];
    u32 fontIndex;
    /* Sound effects (Uses miniaudio.h behind the scenes) */
    XSound sounds[32];
    u32 soundIndex;
} Platform;

#endif //PLAT_H
