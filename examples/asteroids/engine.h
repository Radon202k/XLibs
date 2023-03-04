#ifndef ENGINE_H
#define ENGINE_H

#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xrender.h"
#include "w:/libs/xlibs/xsound.h"

typedef struct
{
    XFont font;
    XSprite white, arrow, circle;
    
    Array_T *layer1, *layer2, *meshlayer1;
    
} Engine;

global Engine engine;

void engine_init(void);
void engine_shutdown(void);

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
void xrendresized(void);


#endif //ENGINE_H
