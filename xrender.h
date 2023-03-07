#ifndef XLIB_RENDER
#define XLIB_RENDER

/* =========================================================================
   DEPENDENCIES
   ========================================================================= */

#include "xbase.h"
#include "xmemory.h"
#include "xmath.h"
#include "xarray.h"
#include "xlist.h"
#include "xtable.h"
#include "xwindows.h"

/* =========================================================================
   DATA TYPES INTERNAL TO THE RENDERER THAT THE USER DOESN'T CARE
   ========================================================================= */

typedef struct XVertex3D      XVertex3D;
typedef struct XLineVertex3D  XLineVertex3D;
typedef struct XVertexCBuffer XVertexCBuffer;
typedef struct XLineCommand   XLineCommand;
typedef struct XSpriteCommand XSpriteCommand;
typedef struct XMeshCommand   XMeshCommand;
typedef struct XTextureAtlas  XTextureAtlas;

struct XVertex3D
{
    v3f pos;
    v2f uv;
    v4f color;
};

struct XLineVertex3D
{
    v4f color;
    v3f pos;
};

struct XVertexCBuffer
{
    mat4f WVP;
};

struct XLineCommand
{
    v2f a, b;
    v4f col;
    f32 sort;
};

struct XSpriteCommand
{ 
    f32 rot, sort;
    v2f pos, size, pivot;
    v4f col;
    rect2f uv;
};

struct XMeshCommand
{ 
    f32 rot, sort;
    v2f pos, scale, pivot;
    v4f col;
    Array_T vertices;
};

struct XTextureAtlas
{
    s32 size, bottom;
    v2i at;
    u8 *bytes;
};

/* =========================================================================
   DATA TYPES THAT THE USER CARES ABOUT
   ========================================================================= */

typedef struct XRenderConfig XRenderConfig;
typedef struct XRender XRender;
typedef struct XSprite XSprite;
typedef struct XFont   XFont;

struct XRenderConfig
{
    WNDPROC  wndproc;
    u32 windowClassStyle;
    u32 windowClassStyleEx;
    wchar_t* windowTitle;
    v2f windowPos;
    v2f windowSize;
    v4f clearColor;
    bool topDown;
    s32 textureAtlasSize;
    s32 maxSimulSprites;
    s32 maxSimulLines;
    s32 glyphMakerSize;
};

struct XRender
{
    /* Configuration values */
    s32 maxSimulSprites;
    s32 maxSimulLines;
    s32 glyphMakerSize;
    D3D_FEATURE_LEVEL featureLevelArray[8];
    s32 featureLevelIndex;
    bool topDown;
    DWORD windowClassStyleEx;
    DWORD windowClassStyle;
    v2f backBufferSize;
    v2f windowPos;
    v2f windowSize;
    wchar_t windowTitle[256];
    v4f clearColor;
    
    /* State values */
    bool running;
    f32 dt;
    
    /* High level renderer values */
    XVertexCBuffer vertexCBufferData;
    XTextureAtlas textureAtlas;
    Array_T lineBatch;
    Array_T spriteBatchArray[32];
    s32 spriteBatchIndex;
    Array_T meshBatchArray[32];
    s32 meshBatchIndex;
    
    /* Low level renderer values */
    HDC deviceContext;
    HWND windowHandle;
    LARGE_INTEGER lastCounter;
    IDXGISwapChain *d11SwapChain;
    ID3D11Device *d11Device;
    ID3D11DeviceContext *d11DeviceContext;
    ID3D11Debug *d11Debug;
    ID3D11Texture2D *backBufferTexture2D;
    ID3D11Texture2D *depthStencilBufferTexture2D;
    ID3D11Texture2D *textureAtlasTexture2D;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11DepthStencilView *depthStencilView;
    ID3D11DepthStencilState *depthStencilState;
    ID3D11BlendState *blendState; 
    ID3D11RasterizerState *rasterizerState;
    ID3D11SamplerState *samplerState;
    ID3D11VertexShader *texturedVertexShader;
    ID3D11VertexShader *lineVertexShader;
    ID3D11PixelShader *texturedPixelShader;
    ID3D11PixelShader *linePixelShader;
    ID3D11InputLayout *texturedInputLayout;
    ID3D11InputLayout *lineInputLayout;
    ID3D11Buffer *texturedVertexBuffer;
    ID3D11Buffer *lineVertexBuffer;
    ID3D11Buffer *vertexCBuffer;
    ID3D11ShaderResourceView *textureAtlasSRV; 
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_TEXTURE2D_DESC depthStencilBufferTexDesc; 
};

struct XSprite
{
    v2f size;
    rect2f uv;
    v2f align;
};

struct XFont
{
    f32 lineadvance, charwidth, maxdescent;
    wchar_t path[MAX_PATH];
    HFONT handle;
    HBITMAP bitmap;
    TEXTMETRICW metrics;
    VOID *bytes;
    Table_T glyphs;
};

/* =========================================================================
   MAIN FUNCTIONS
   ========================================================================= */

void xrender_initialize   (XRenderConfig config);
void xrender_shutdown     (void);
void xrender_update       (void);
void xrender_resized      (void);

v2f  xrender_monitor_size (void);

/* =========================================================================
   TEXTURE / TEXTURE ATLAS / SPRITES
   ========================================================================= */

/*  A Render Batch is composed of many vertices representing triangles texture
mapped to a single gpu texture that is called the texture atlas. A Sprite is a
smaller rectangle than the whole texture atlas that represents a texture in it
self, i.e., the texture atlas is composed of many different smaller textures.
  The point of that is faster performance. xspritepng creates a sprite from a
png file located at path, i.e., it loads the png and copies the bytes of the
texture into the texture atlas and fills XSprite with the corresponding uv coo
rdinates and other info about the source image. */

XSprite  xspritepng   (wchar_t *path, bool premulalpha);
u8      *xpng         (wchar_t *filePath, v2i *dim, bool premulAlpha);
XSprite  xspritebytes (u8 *bytes, v2i dim);
Array_T *xbatch       (s32 size);
Array_T *xmeshbatch   (s32 sz);
u8      *xatlasbytes  (void);
void     xatlasupdate (u8 *data);

/* =========================================================================
   FONTS / GLYPHS
   ========================================================================= */

XFont   xfont        (wchar_t *fileName, wchar_t *fontName, int heightPoints);
void    xfontfree    (XFont font);
s32     xfontheight  (s32 pointHeight);
XSprite xglyphsprite (XFont font, wchar_t *c, rect2f *tightBounds, s32 *tightDescent);

/* =========================================================================
   DRAWING / RENDERING UTILITIES
   ========================================================================= */

void xline   (v2f a, v2f b, v4f color, f32 sort);
void xarrow  (Array_T *batch, v2f a, v2f b, v4f col, XSprite head, v2f size, f32 sort);
void xsprite (Array_T *batch, XSprite s, v2f pos, v2f dim, v4f col, v2f pivot, f32 rot, f32 sort);
f32  xglyph  (Array_T *batch, XFont f, u32 unicode, v2f pos, v4f c, v2f pivot, f32 rot, f32 sort);
f32  xstring (Array_T *batch, XFont f, wchar_t *string, v2f pos, v4f c, v2f pivot, f32 rot, f32 sort, bool fixedwidth);
void xmesh   (Array_T *batch, Array_T vertices, v2f pos, v2f scale, v4f color, v2f pivot, f32 rot, f32 sort);

void xlinerect   (v2f pos, v2f dim, v4f col, f32 sort);
void xlinecircle (v2f pos, f32 radius, s32 n, v4f col, f32 sort);
void xlinemesh   (Array_T vertices, v2f pos, v2f scale, v4f col, f32 sort);

v2f  xglyphsize  (XFont font, u32 unicode);
v2f  xstringsize (XFont font, wchar_t *s);

/* =========================================================================
   BASIC TYPES
   ========================================================================= */

bool xdraggedhandle(v2f p, f32 maxDist, void *address, bool *hover, v2f *delta);
void xpathabs      (wchar_t *dest, u32 destSize, wchar_t *fileName);
void xpathabsascii (char *dest, u32 destSize, char *fileName);

/* =========================================================================
   End of Interface
   ========================================================================= */







/* Intended white space */







/* =========================================================================
   Implementation
   ========================================================================= */

XRender xrnd;

#define RGBA(r,g,b,a) (((a)<<24) | ((r)<<16) | ((g)<<8) | (b))

/*  Copies the exe path until last slash
    c:/my/path/to/the/app/main.exe
                         ^                            */
void xpathabs(wchar_t *dst, u32 dstsize, wchar_t *filename)
{
    wchar_t *slashpos, *at, exepath[MAX_PATH], dir[260];
    
    xpath(exepath, MAX_PATH);
    
    slashpos = 0;
    at = exepath;
    while (*at++)
        if (*at == '\\' || *at == '/')
        slashpos = at;
    
    xstrcps(dir, 260, exepath, (u32)(slashpos - exepath));
    _snwprintf_s(dst, dstsize, _TRUNCATE, L"%s\\%s", dir, filename);
}

void xpathabsascii(char *dst, u32 dstsize, char *filename)
{
    char *slashpos, *at, exepath[MAX_PATH], dir[260];
    xpathascii(exepath, MAX_PATH);
    
    slashpos  = 0;
    at = exepath;
    while (*at++)
        if (*at == '\\' || *at == '/')
        slashpos = at;
    
    xstrcpsascii(dir, 260, exepath, (u32)(slashpos - exepath));
    _snprintf_s(dst, dstsize, _TRUNCATE, "%s\\%s", dir, filename);
}

u8 *xpng(wchar_t *path, v2i *dim, bool premulalpha)
{
    u8 *r, *rowDst, *rowSrc;
    int w,h, i,j, nrChannels, dataSize;
    char *asciiPath;
    u32 *pxDst, *pxSrc, cr,cg,cb,ca;
    f32 realA;
    
    r=0;
    asciiPath = xstrtoascii(path);
    unsigned char *data = stbi_load(asciiPath, &w, &h, &nrChannels, 0);
    xfree(asciiPath);
	if (data)
    {
        *dim = ini2i(w,h);
        
		dataSize = w*h*nrChannels;
		r = (u8 *)xalloc(dataSize);
        rowDst = r;
        rowSrc = data;
        
        for (j=0; j<h; ++j)
        {
            pxDst = (u32 *)rowDst;
            pxSrc = (u32 *)rowSrc;
            for (i=0; i<w; ++i)
            {
                cr = (*pxSrc >> 16) & 0xFF;
                cg = (*pxSrc >> 8) & 0xFF;
                cb = (*pxSrc >> 0) & 0xFF;
                ca = (*pxSrc >> 24) & 0xFF;
                if (premulalpha)
                {
                    realA = (f32)ca / 255.0f;
                    cr=(s32)(cr*realA);
                    cg=(s32)(cg*realA);
                    cb=(s32)(cb*realA);
                }
                *pxDst = ((cr<<16) | (cg<<8) | (cb<<0) | (ca<<24));
                pxDst++;
                pxSrc++;
            }
            rowDst += w*4;
            rowSrc += w*4;
        }
        stbi_image_free(data);
	}
	else
        *dim = ini2i(0,0);
    
	return r;
}

void blit_simple_unchecked(u8 *dst, u32 dstsz, u8 *src, v2i at, v2i dim)
{
    u8 *rowSrc, *rowDst;
    u32 *pxSrc, *pxDst;
    
    rowSrc=src;
    rowDst=dst + at.y*dstsz*4 + at.x*4;
    
    for (at.y=0; at.y<dim.y; ++at.y)
    {
        pxSrc=(u32 *)rowSrc;
        pxDst=(u32 *)rowDst;
        
        for (at.x=0; at.x<dim.x; ++at.x)
            *pxDst++ = *pxSrc++;
        
        rowSrc += 4*dim.x;
        rowDst += 4*xrnd.textureAtlas.size;
    }
}


XSprite xspritebytes(u8 *b, v2i dim)
{
    s32 m;
    u8 *dst, *src;
    XSprite r;
    XTextureAtlas *a;
    
    m = 1;
    a = &xrnd.textureAtlas;
    
    if ((a->at.x + dim.x + m) > a->size)
        a->at = ini2i(0, a->bottom);
    
    if (m+a->bottom < (a->at.y + dim.y))
        a->bottom = a->at.y + dim.y + m;
    
    assert(a->bottom <= a->size);
    
    dst = xrnd.textureAtlas.bytes;
    src = b;
    blit_simple_unchecked(dst, xrnd.textureAtlas.size, src, a->at, dim);
    
    r.uv.min = (v2f){a->at.x / (f32)a->size,
        (xrnd.topDown ? (a->at.y + dim.y) : a->at.y) / (f32)a->size};
    
    r.uv.max = ini2f((a->at.x + dim.x) / (f32)a->size, 
                     (xrnd.topDown ? a->at.y : (a->at.y + dim.y)) / (f32)a->size);
    
    r.size = ini2fs(dim.x,dim.y);
    
    a->at.x += dim.x+m;
    
    return r;
}

XSprite xspritepng(wchar_t *path, bool premulalpha)
{
    XSprite r;
    v2i dim;
    u8 *b;
    
    memset(&r, 0, sizeof(r));
    b = xpng(path, &dim, premulalpha);
    if (b)
    {
        r = xspritebytes(b, dim);
        xfree(b);
    }
    return r;
}

u8 *xatlasbytes()
{
    return xrnd.textureAtlas.bytes;
}

void xatlasupdate(u8 *updateddata)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubResource;
    ID3D11DeviceContext_Map(xrnd.d11DeviceContext, 
                            (ID3D11Resource *)xrnd.textureAtlasTexture2D, 
                            0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
    
    memcpy(mappedSubResource.pData, updateddata, 
           xrnd.textureAtlas.size*xrnd.textureAtlas.size*4);
    
    ID3D11DeviceContext_Unmap(xrnd.d11DeviceContext, 
                              (ID3D11Resource *)xrnd.textureAtlasTexture2D, 0);
}

bool cmp_glyph_unicodes(const void *a, const void *b)
{
    u32 ua, ub;
    
    ua = *(u32 *)a;
    ub = *(u32 *)b;
    return (ua == ub);
}

u32 hash_unicode(const void *k)
{
    u32 u;
    
    u = *(u32 *)k;
    u = (u & (512-1));
    return u;
}

XFont xfont(wchar_t *path, wchar_t *name, int heightpoints)
{
    XFont result;
    memset(&result, 0, sizeof(result));
    
    v2i maxGlyphSize;
    s32 maxDescent;
    
    s32 i;
    
    wchar_t c[2];
    u32 *k;
    XSprite *v;
    
    /* Add font resource to Windows */
    s32 temp = AddFontResourceW(path);
    assert(temp == 1);
    
    /* Create the font */
    result.handle = CreateFontW(xfontheight(heightpoints), 
                                0, 0, 0, FW_NORMAL, false, false, 
                                false, DEFAULT_CHARSET,
                                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                                CLEARTYPE_QUALITY, DEFAULT_PITCH, name);
    
    assert(result.handle && result.handle != INVALID_HANDLE_VALUE);
    
    /* Create a bitmap to render glyphs into */
    BITMAPINFO info = xbmpinfo(xrnd.glyphMakerSize, -xrnd.glyphMakerSize);
    result.bitmap = CreateDIBSection(xrnd.deviceContext, &info, 
                                     DIB_RGB_COLORS, &result.bytes, 0, 0);
    assert(xrnd.glyphMakerSize>0);
    memset(result.bytes, 0, xrnd.glyphMakerSize*xrnd.glyphMakerSize*4);
    
    /* Configure the bitmap drawing to use black and white */
    SelectObject(xrnd.deviceContext, result.bitmap);
    SelectObject(xrnd.deviceContext, result.handle);
    SetBkColor(xrnd.deviceContext, RGB(0,0,0));
    SetTextColor(xrnd.deviceContext, RGB(255,255,255));
    
    /* Get text metrics for font */
    TEXTMETRICW metrics = {0};
    GetTextMetricsW(xrnd.deviceContext, &metrics);
    result.metrics = metrics;
    result.lineadvance = (f32)metrics.tmHeight - metrics.tmInternalLeading;
    result.charwidth = (f32)metrics.tmAveCharWidth;
    result.glyphs = Table_new(512, cmp_glyph_unicodes, hash_unicode);
    
    /* Render some glyphs from the ASCII range */
    maxGlyphSize.x = maxGlyphSize.y = maxDescent = -10000;
    for (i=32; i<=126; ++i)
    {
        c[0]=(wchar_t)i;
        c[1]='\0';
        
        /* Allocate memory for the hash table key/value pair */
        k = xalloc(sizeof *k);
        v = xalloc(sizeof *v);
        
        /* Store key, generate the glyph sprite and store value */
        *k = (u32)i;
        
        rect2f tightBounds;
        s32 tightDescent;
        *v = xglyphsprite(result, c, &tightBounds, &tightDescent);
        
        /* Spaces wont have anything so tightBounds wont be found  */
        if (tightBounds.max.x!=0 && tightBounds.max.y!=0)
        {
            /* Calculate tight size */
            v2i tightSize = ini2i((s32)(tightBounds.max.x - tightBounds.min.x),
                                  (s32)(tightBounds.max.y - tightBounds.min.y));
            
            /* Book keep maximum glyph size and maximum descent */
            if (maxGlyphSize.x < tightSize.x) maxGlyphSize.x = tightSize.x;
            if (maxGlyphSize.y < tightSize.y) maxGlyphSize.y = tightSize.y;
            if (maxDescent < tightDescent) maxDescent = tightDescent;
        }
        
        /* Set the glyph in the hash table */
        Table_set(result.glyphs, k, v);
    }
    
    result.lineadvance = (f32)maxGlyphSize.y;
    result.maxdescent = (f32)maxDescent;
    xstrcpy(result.path, MAX_PATH, path);
    
    return result;
}

XSprite xglyphsprite(XFont font, wchar_t *c, rect2f *tBounds, s32 *tDescent)
{
    u8 *b, *dstRow, *srcRow;
    s32 charsz, i,j, x,y;
    u32 *dstPx, *srcPx, *px, color, a, ps;
    v2f d, al;
    XSprite r;
    SIZE size;
    rect2f bounds;
    
    ps=(s32)(0.3f*font.lineadvance);
    GetTextExtentPoint32W(xrnd.deviceContext, c, 1, &size);
    al=ini2f(0,0);
    d=ini2fs(size.cx,size.cy);
    charsz=(s32)wcslen(c);
    bounds = inir2f(0,0, d.x,d.y);
    tBounds->min.x=tBounds->min.y=1000000;
    tBounds->max.x=tBounds->max.y=-1000000;
    
    TextOutW(xrnd.deviceContext, ps,0, c, charsz);
    
    bool foundTBox = false;
    for (j=0; j<xrnd.glyphMakerSize; ++j)
    {
        for (i=0; i<xrnd.glyphMakerSize; ++i)
        {
            px = (u32 *)((u8 *)font.bytes + j*xrnd.glyphMakerSize*4 + i*4);
            if (*px != 0)
            {
                foundTBox = true;
                if (tBounds->min.x>i) tBounds->min.x=(f32)i;
                if (tBounds->min.y>j) tBounds->min.y=(f32)j;
                if (tBounds->max.x<i) tBounds->max.x=(f32)i;
                if (tBounds->max.y<j) tBounds->max.y=(f32)j;
            }
        }
    }
    
    if (foundTBox) {
        --tBounds->min.x;
        --tBounds->min.y;
        ++tBounds->max.x;
        ++tBounds->max.y;
        
        d.x=tBounds->max.x-tBounds->min.x+1;
        d.y=tBounds->max.y-tBounds->min.y+1;
        
        *tDescent = font.metrics.tmDescent-(font.metrics.tmHeight-(s32)tBounds->max.y);
        al.x = tBounds->min.x-ps;
        al.y = (f32)*tDescent;
    }
    
    b = (u8 *)xalloc((s32)(d.x*d.y)*4);
    
    if (foundTBox) {
        dstRow = b;
        srcRow = ((u8 *)font.bytes + (s32)tBounds->min.y*xrnd.glyphMakerSize*4 + (s32)tBounds->min.x*4);
        for (y=(s32)tBounds->min.y; y<(s32)tBounds->max.y; ++y)
        {
            srcPx = (u32 *)srcRow;
            dstPx = (u32 *)dstRow;
            for (x=(s32)tBounds->min.x; x<(s32)tBounds->max.x; ++x)
            {
                color = *srcPx++;
                a = ((color >> 16) & 0xff);
                *dstPx++ =  RGBA(255,255,255, a);
            }
            dstRow += 4*(s32)d.x;
            srcRow += 4*xrnd.glyphMakerSize;
        }
    }
    
    r = xspritebytes(b, ini2if(d.x,d.y));
    r.align = al;
    xfree(b);
    return r;
}

s32 xfontheight(s32 pointHeight)
{
    s32 result = MulDiv(pointHeight, 
                        GetDeviceCaps(xrnd.deviceContext, LOGPIXELSY), 
                        GetDeviceCaps(xrnd.deviceContext, LOGPIXELSX));
    return result;
}

void xfontfree(XFont f)
{
    s32 i;
    Table_node n;
    
    DeleteObject(f.bitmap);
    DeleteObject(f.handle);
    RemoveFontResourceW(f.path);
    
    for (i=0; i<f.glyphs->size; ++i)
        for (n=f.glyphs->storage[i]; n; n=n->next)
    {
        xfree(n->key);
        xfree(n->value);
    }
    
    Table_free(f.glyphs);
}

Array_T *xbatch(s32 size)
{
    assert(narray(xrnd.spriteBatchArray) > xrnd.spriteBatchIndex);
    
    Array_T *result = xrnd.spriteBatchArray + xrnd.spriteBatchIndex++;
    *result = Array_new(size, sizeof(XSpriteCommand));
    
    return result;
}

Array_T *xmeshbatch(s32 size)
{
    assert(narray(xrnd.meshBatchArray) > xrnd.meshBatchIndex);
    
    Array_T *result = xrnd.meshBatchArray + xrnd.meshBatchIndex++;
    *result = Array_new(size, sizeof(XMeshCommand));
    
    return result;
}

void xpush_line_command(v2f a, v2f b, v4f color, f32 sort)
{
    XLineCommand c = 
    {
        a, b, color, sort,
    };
    Array_push(&xrnd.lineBatch, &c);
}

void xpush_rect_command(Array_T *group, v2f pos, v2f dim, rect2f uvs, 
                        v4f color, v2f pivot, f32 rot, f32 sort)
{
    XSpriteCommand c = 
    {
        rot, sort,
        pos, dim, pivot,
        color, uvs
    };
    Array_push(group, &c);
}

void xpush_mesh_command(Array_T *group, Array_T vertices, v2f pos, v2f scale, 
                        v4f color, v2f pivot, f32 rot, f32 sort)
{
    XMeshCommand c = 
    {
        rot, sort,
        pos, scale, pivot,
        color,
        vertices
    };
    Array_push(group, &c);
}

v2f xmonitor()
{
    v2f r;
    
    r=ini2fs(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    
    return r;
}

void xline(v2f a, v2f b, v4f c, f32 s)
{
    xpush_line_command(a, b, c, s);
}

void xstroke(v2f a, v2f b, v4f c, f32 w, f32 s)
{
    v2f dir, o1, o2;
    f32 angle;
    
    dir = nrm2f(sub2f(b,a));
    angle = atan2f(dir.y, dir.x);
    
    o1=ini2f(a.x+w/2*cosf(angle+PIf/2), a.y+w/2*sinf(angle+PIf/2));
    o2=ini2f(a.x+w/2*cosf(angle-PIf/2), a.y+w/2*sinf(angle-PIf/2));
    
    xline(o1, o2, ini4f(1,1,1,1), 0);
    xline(o1,  b, ini4f(1,1,1,1), 0);
    xline( b, o2, ini4f(1,1,1,1), 0);
    xline( a,  b, ini4f(1,1,1,1), 0);
}

void xarrow(Array_T *g, v2f a, v2f b, v4f c, XSprite head, v2f headsize, f32 s)
{
    v2f dir;
    
    dir=nrm2f(sub2f(b,a));
    xline(a, b, c, s);
    f32 arrowAngle = atan2f(dir.y,dir.x);
    xsprite(g, head, b, headsize, c, ini2f(1,.5f), degf(arrowAngle), s);
}

void xsprite(Array_T *g, XSprite sp, v2f p, v2f s, v4f cl, v2f pv, f32 r, f32 st)
{
    xpush_rect_command(g, p, s, sp.uv, cl, pv, r, st);
}

void xmesh(Array_T *b, Array_T v, v2f p, v2f s, v4f c, v2f pv, f32 r, f32 st)
{
    xpush_mesh_command(b, v, p, s, c, pv, r, st);
}

f32 xglyph(Array_T *g, XFont f, u32 u, v2f p, v4f c, v2f pv, f32 r, f32 s)
{
    XSprite *sp = (XSprite *)Table_get(f.glyphs, &u);
    if (sp) {
        xsprite(g, *sp, p, sp->size, c, pv, r, s);
        return sp->size.x;
    }
    return 0;
}

f32 xstring(Array_T *g, XFont f, wchar_t *s, v2f p, v4f c, v2f pv, f32 r, f32 st, bool fw)
{
    f32 startX, w;
    XSprite *sp;
    u32 u;
    
    startX = p.x;
    wchar_t *at = s;
    while (*at)
    {
        w=0;
        u=*at;
        sp = (XSprite *)Table_get(f.glyphs, &u);
        if (sp) {
            if (fw)
                p.x += .5f*(f.charwidth-sp->size.x);
            
            p.x -= sp->align.x;
            p.y -= sp->align.y;
            
            if (fw)
                w=f.charwidth;
            else
                w=sp->size.x;
            //            Draw_outline_rect(g, p, sp->size, c, st);
            xsprite(g, *sp, p, sp->size, c, pv, r, st+1);
            if (fw)
                p.x -= .5f*(f.charwidth-sp->size.x);
            p.y += sp->align.y;
        }
        
        p.x += w;
        at++;
    }
    return p.x-startX;
}

void xlinerect(v2f p, v2f wh, v4f cl, f32 st)
{
    xpush_line_command(p, ini2f(p.x + wh.x, p.y), cl, st); // bottom
    xpush_line_command(ini2f(p.x, p.y + wh.y), ini2f(p.x + wh.x, p.y + wh.y), cl, st); // top
    xpush_line_command(p, ini2f(p.x, p.y + wh.y + 1), cl, st); // left
    xpush_line_command(ini2f(p.x + wh.x, p.y), ini2f(p.x + wh.x, p.y + wh.y), cl, st); // right
}

void xlinecircle(v2f pos, f32 radius, s32 n, v4f col, f32 sort)
{
    for (s32 i = 0; i < n; ++i) {
        f32 angle1 = radf(i * 360.0f / n);
        f32 angle2 = radf((i + 1) * 360.0f / n);
        
        v2f p1 = ini2f(pos.x + sinf(angle1) * radius, pos.y + cosf(angle1) * radius);
        v2f p2 = ini2f(pos.x + sinf(angle2) * radius, pos.y + cosf(angle2) * radius);
        xpush_line_command(p1, p2, col, sort);
    }
}

void xlinemesh(Array_T vertices, v2f pos, v2f scale, v4f col, f32 sort)
{
    for (s32 i = 0; i < vertices.top-1; ++i)
    {
        XVertex3D *v3d1 = Array_get(vertices, i);
        XVertex3D* v3d2 = Array_get(vertices, i + 1);
        
        v2f p1 = add2f(pos, ini2f(scale.x*v3d1->pos.x, scale.y*v3d1->pos.y));
        v2f p2 = add2f(pos, ini2f(scale.x*v3d2->pos.x, scale.y*v3d2->pos.y));
        
        xpush_line_command(p1, p2, col, sort);
    }
}

v2f xglyphsize(XFont f, u32 u)
{
    XSprite *sp;
    
    sp=(XSprite *)Table_get(f.glyphs, &u);
    if (sp) {
        return sp->size;
    }
    return ini2f(0,0);
}

v2f xstringsize(XFont f, wchar_t *s)
{
    f32 w, mh;
    wchar_t *c;
    v2f temp;
    
    w=0;
    mh=-10000;
    temp=ini2f(0,0);
    for (c=s; *c; ++c)
    {
        temp=xglyphsize(f, *c);
        if (mh<temp.y)
            mh=temp.y;
        w+=temp.x;
    }
    
    return ini2f(w,temp.y);
}

function void
create_sprites_shaders()
{
    // First we will declare the vertex shader code
    char *vertexShaderSource = 
        "struct vs_in"
        "{"
        "  float3 position_local : POS;"
        "  float2 uv             : TEX;"
        "  float4 color          : COL;"
        "};"
        
        "struct vs_out"
        "{"
        "  float4 position_clip : SV_POSITION;"
        "  float2 uv            : TEXCOORD;"
        "  float4 color         : COLOR;"
        "};"
        
        "cbuffer cbPerObject"
        "{"
        "  float4x4 WVP;"
        "};"
        
        "vs_out vs_main(vs_in input)"
        "{"
        "  vs_out output = (vs_out)0;"
        "  float4 pos = float4(input.position_local, 1.0);"
        "  pos.z += 500;"
        
        "  output.position_clip = mul(pos, WVP);"
        "  output.uv = input.uv;"
        "  output.color = input.color;"
        "  return output;"
        "}";
    
    // Then we will compile the vertex shader
    ID3DBlob *compiledVS;
    ID3DBlob *errorVertexMessages;
    if (FAILED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource) + 1,
                          0, 0, 0, "vs_main", "vs_5_0",
                          0, 0, &compiledVS, &errorVertexMessages)))
    {
        if (errorVertexMessages)
        {
            wchar_t *msg = (wchar_t *)
            (ID3D10Blob_GetBufferPointer(errorVertexMessages));
            OutputDebugStringW(msg);
            exit(1);
        }
        else
        {
            exit(1);
        }
    }
    
    // And create the vertex shader if compilation went ok
    ID3D11Device_CreateVertexShader(xrnd.d11Device,
                                    ID3D10Blob_GetBufferPointer(compiledVS),
                                    ID3D10Blob_GetBufferSize(compiledVS),
                                    NULL, &xrnd.texturedVertexShader);
    
    // Then the pixel shader code
    char *pixelShaderSource = 
        "struct ps_in"
        "{"
        "  float4 position_clip : SV_POSITION;"
        "  float2 uv            : TEXCOORD;"
        "  float4 color         : COLOR;"
        "};"
        
        "Texture2D tex;"
        "SamplerState samp;"
        
        "float4 ps_main(ps_in input) : SV_TARGET"
        "{"
        "  return tex.Sample(samp, input.uv) * input.color;"
        "}";
    
    // And compilation of the pixel shader
    ID3DBlob *compiledPS;
    ID3DBlob *errorPixelMessages;
    if (FAILED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource) + 1,
                          0, 0, 0, "ps_main", "ps_5_0",
                          0, 0, &compiledPS, &errorPixelMessages)))
    {
        if (errorPixelMessages)
        {
            char *msg = (char *)
            (ID3D10Blob_GetBufferPointer(errorPixelMessages));
            OutputDebugStringA(msg);
            exit(1);
        }
        else
        {
            exit(1);
        }
    }
    // And create the pixel shader if compilation went ok
    ID3D11Device_CreatePixelShader(xrnd.d11Device,
                                   ID3D10Blob_GetBufferPointer(compiledPS),
                                   ID3D10Blob_GetBufferSize(compiledPS),
                                   NULL, &xrnd.texturedPixelShader);
    
    // Create the input layout
    
    // Make the input element desc array that describes the vertex input 
    // layout we will be using
    D3D11_INPUT_ELEMENT_DESC ied [] =
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
    // And create the input layout
    void *vsPointer = ID3D10Blob_GetBufferPointer(compiledVS);
    u32 vsSize = (u32)ID3D10Blob_GetBufferSize(compiledVS);
    
    if (FAILED(ID3D11Device_CreateInputLayout(xrnd.d11Device, ied, narray(ied),
                                              vsPointer, vsSize, &xrnd.texturedInputLayout)))
        exit(1);
}

function void
create_lines_shaders()
{
    // First we will declare the vertex shader code
    char *vertexShaderSource = 
        "struct vs_in"
        "{"
        "  float3 position_local : POS;"
        "  float4 color          : COL;"
        "};"
        
        "struct vs_out"
        "{"
        "  float4 position_clip : SV_POSITION;"
        "  float4 color         : COLOR;"
        "};"
        
        "cbuffer cbPerObject"
        "{"
        "  float4x4 WVP;"
        "};"
        
        "vs_out vs_main(vs_in input)"
        "{"
        "  vs_out output = (vs_out)0;"
        "  float4 pos = float4(input.position_local, 1.0);"
        "  pos.z += 500;"
        
        "  output.position_clip = mul(pos, WVP);"
        "  output.color = input.color;"
        "  return output;"
        "}";
    
    // Then we will compile the vertex shader
    ID3DBlob *compiledVS;
    ID3DBlob *errorVertexMessages;
    if (FAILED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource) + 1,
                          0, 0, 0, "vs_main", "vs_5_0",
                          0, 0, &compiledVS, &errorVertexMessages)))
    {
        if (errorVertexMessages)
        {
            wchar_t *msg = (wchar_t *)
            (ID3D10Blob_GetBufferPointer(errorVertexMessages));
            OutputDebugStringW(msg);
            exit(1);
        }
        else
        {
            exit(1);
        }
    }
    
    // And create the vertex shader if compilation went ok
    ID3D11Device_CreateVertexShader(xrnd.d11Device,
                                    ID3D10Blob_GetBufferPointer(compiledVS),
                                    ID3D10Blob_GetBufferSize(compiledVS),
                                    NULL, &xrnd.lineVertexShader);
    
    // Then the pixel shader code
    char *pixelShaderSource = 
        "struct ps_in"
        "{"
        "  float4 position_clip : SV_POSITION;"
        "  float4 color         : COLOR;"
        "};"
        
        "float4 ps_main(ps_in input) : SV_TARGET"
        "{"
        "  return input.color;"
        "}";
    
    // And compilation of the pixel shader
    ID3DBlob *compiledPS;
    ID3DBlob *errorPixelMessages;
    if (FAILED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource) + 1,
                          0, 0, 0, "ps_main", "ps_5_0",
                          0, 0, &compiledPS, &errorPixelMessages)))
    {
        if (errorPixelMessages)
        {
            wchar_t *msg = (wchar_t *)
            (ID3D10Blob_GetBufferPointer(errorPixelMessages));
            OutputDebugStringW(msg);
            exit(1);
        }
        else
        {
            exit(1);
        }
    }
    // And create the pixel shader if compilation went ok
    ID3D11Device_CreatePixelShader(xrnd.d11Device,
                                   ID3D10Blob_GetBufferPointer(compiledPS),
                                   ID3D10Blob_GetBufferSize(compiledPS),
                                   NULL, &xrnd.linePixelShader);
    
    //  Create the input layout
    
    // Make the input element desc array that describes the vertex input 
    // layout we will be using
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
    // And create the input layout
    void *vsPointer = ID3D10Blob_GetBufferPointer(compiledVS);
    u32 vsSize = (u32)ID3D10Blob_GetBufferSize(compiledVS);
    
    if (FAILED(ID3D11Device_CreateInputLayout(xrnd.d11Device, ied, narray(ied),
                                              vsPointer, vsSize, &xrnd.lineInputLayout)))
        exit(1);
}

void create_sprites_vertex_buffer()
{
    assert(xrnd.maxSimulSprites > 0);
    
    u32 size = xrnd.maxSimulSprites * 6 * sizeof(XVertex3D);
    D3D11_BUFFER_DESC desc =
    {
        size, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE, 0, sizeof(XVertex3D)
    };
    
    if (FAILED(ID3D11Device_CreateBuffer(xrnd.d11Device, &desc, 0, 
                                         &xrnd.texturedVertexBuffer)))
        exit(1);
}

void create_lines_vertex_buffer()
{
    u32 m, sz;
    
    m = 10000;
    sz = m*2*sizeof(XLineVertex3D);
    
    D3D11_BUFFER_DESC d = { sz, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE, 0, sizeof(XLineVertex3D), };
    
    if (FAILED(ID3D11Device_CreateBuffer(xrnd.d11Device, &d, 0, 
                                         &xrnd.lineVertexBuffer))) {
        exit(1);
    }
}

void free_sprite_and_mesh_groups()
{
    s32 i;
    
    for (i=0; i < xrnd.spriteBatchIndex; ++i)
        Array_free(xrnd.spriteBatchArray + i);
    
    for (i=0; i<xrnd.meshBatchIndex; ++i)
        Array_free(xrnd.meshBatchArray + i);
}

void reset_sprite_groups()
{
    s32 i;
    
    for (i = 0; i < xrnd.spriteBatchIndex; ++i)
        xrnd.spriteBatchArray[i].top = 0;
}

void reset_mesh_groups()
{
    s32 i;
    
    for (i = 0; i < xrnd.meshBatchIndex; ++i)
        xrnd.meshBatchArray[i].top = 0;
}

u32 produce_vertices_from_sprite_and_mesh_groups()
{
    f32 rotRad, pvx, pvy;
    v2f p, pa, pb, pc, pd, xaxis, yaxis;
    s32 i, j, vi, spritesVertexCount, meshsVertexCount;
    XVertex3D *v;
    Array_T *batch;
    XSpriteCommand *scmd;
    XMeshCommand *mcmd;
    D3D11_MAPPED_SUBRESOURCE mr;
    
    /*  Count number of vertices from all sprite groups                    */
    
    for (i=0, spritesVertexCount=0; i < xrnd.spriteBatchIndex; ++i)
        spritesVertexCount += 6*(xrnd.spriteBatchArray[i].top);
    
    /*  Count number of vertices from all mesh groups                      */
    
    for (i=0, meshsVertexCount=0; i < xrnd.meshBatchIndex; ++i)
        for (j=0; j < xrnd.meshBatchArray[i].top; ++j)
        meshsVertexCount += ((XMeshCommand*)Array_get(xrnd.meshBatchArray[i], j))->vertices.top;
    
    v = 0;
    vi = 0;
    if (spritesVertexCount+meshsVertexCount) {
        v = xnalloc(spritesVertexCount+meshsVertexCount, XVertex3D);
    }
    
    /*  Loop through all sprite groups and produces the vertices.          */
    if (spritesVertexCount>0) {
        for (i=0; i<xrnd.spriteBatchIndex; ++i)
        {
            batch = xrnd.spriteBatchArray + i;
            if (batch->top > 0)
            {
                for (j=0; j < batch->top; ++j)
                {
                    scmd = Array_get(*batch, j);
                    
                    rotRad = -radf(scmd->rot);
                    
                    pvx = scmd->pivot.x;
                    pvy = scmd->pivot.y;
                    
                    xaxis = mul2f(scmd->size.x, ini2f(cosf(rotRad), -sinf(rotRad)));
                    yaxis = mul2f(scmd->size.y, ini2f(sinf(rotRad), cosf(rotRad)));
                    
                    p = scmd->pos;
                    pa = sub2f(sub2f(p, mul2f(  pvx, xaxis)), mul2f(  pvy, yaxis));
                    pb = sub2f(sub2f(p, mul2f(pvx-1, xaxis)), mul2f(  pvy, yaxis));
                    pc = sub2f(sub2f(p, mul2f(pvx-1, xaxis)), mul2f(pvy-1, yaxis));
                    pd = sub2f(sub2f(p, mul2f(  pvx, xaxis)), mul2f(pvy-1, yaxis));
                    
                    v4f c = scmd->col;
                    f32 umin = scmd->uv.min.x;
                    f32 umax = scmd->uv.max.x;
                    f32 vmin = scmd->uv.min.y;
                    f32 vmax = scmd->uv.max.y;
                    f32 sort = scmd->sort;
                    
                    XVertex3D va = { pa.x, pa.y, sort, umin, (xrnd.topDown ? vmax : vmin), c.r,c.g,c.b,c.a };
                    XVertex3D vb = { pb.x, pb.y, sort, umax, (xrnd.topDown ? vmax : vmin), c.r,c.g,c.b,c.a };
                    XVertex3D vc = { pc.x, pc.y, sort, umax, (xrnd.topDown ? vmin : vmax), c.r,c.g,c.b,c.a };
                    XVertex3D vd = { pd.x, pd.y, sort, umin, (xrnd.topDown ? vmin : vmax), c.r,c.g,c.b,c.a };
                    
                    v[vi++] = va;
                    v[vi++] = (xrnd.topDown ? vb : vc);
                    v[vi++] = (xrnd.topDown ? vc : vb);
                    v[vi++] = va;
                    v[vi++] = (xrnd.topDown ? vc : vd);
                    v[vi++] = (xrnd.topDown ? vd : vc);
                }
            }
        }
    }
    
    /*  Loop through all mesh groups and produces the vertices.          */
    
    if (meshsVertexCount>0) {
        for (i=0; i < xrnd.meshBatchIndex; ++i)
        {
            batch = xrnd.meshBatchArray + i;
            if (batch->top > 0)
            {
                for (j=0; j < batch->top; ++j)
                {
                    mcmd = Array_get(*batch, j);
                    
                    p = mcmd->pos;
                    
                    for (s32 k = 0; k < mcmd->vertices.top; ++k)
                    {
                        XVertex3D *v3d = (XVertex3D *)Array_get(mcmd->vertices, k);
                        
                        XVertex3D temp = 
                        {
                            mcmd->pos.x + v3d->pos.x*mcmd->scale.x, mcmd->pos.y + v3d->pos.y*mcmd->scale.y, v3d->pos.z,
                            v3d->uv.x, v3d->uv.y,
                            v3d->color.r*mcmd->col.r, v3d->color.g*mcmd->col.g, v3d->color.b*mcmd->col.b, v3d->color.a*mcmd->col.a,
                        };
                        
                        v[vi++] = temp;
                    }
                }
            }
        }
    }
    
    if (spritesVertexCount+meshsVertexCount > 0)
    {
        ID3D11DeviceContext_Map(xrnd.d11DeviceContext, 
                                (ID3D11Resource *)xrnd.texturedVertexBuffer, 0, 
                                D3D11_MAP_WRITE_DISCARD, 0, &mr);
        memcpy(mr.pData, v, vi*sizeof(XVertex3D)); 
        ID3D11DeviceContext_Unmap(xrnd.d11DeviceContext, 
                                  (ID3D11Resource *)xrnd.texturedVertexBuffer, 0);
        
        xfree(v);
    }
    
    reset_sprite_groups();
    reset_mesh_groups();
    
    return vi;
}

void free_line_group()
{
    Array_free(&xrnd.lineBatch);
}

void reset_line_group()
{
    xrnd.lineBatch.top = 0;
}

u32 produce_vertices_from_line_group()
{
    s32 vc, vi, i;
    XLineVertex3D *v;
    XLineCommand *cmd;
    D3D11_MAPPED_SUBRESOURCE mr;
    
    vc = 0;
    vc += 2 * xrnd.lineBatch.top;
    
    if (vc>0) {
        v = xnalloc(vc, XLineVertex3D);
        vi = 0;
        
        for (i = 0; i < xrnd.lineBatch.top; ++i) {
            cmd = Array_get(xrnd.lineBatch, i);
            
            XLineVertex3D a =
            {
                cmd->a.x, cmd->a.y, cmd->sort,
                cmd->col.r, cmd->col.g, cmd->col.b, cmd->col.a
            };
            
            XLineVertex3D b =
            {
                cmd->b.x, cmd->b.y, cmd->sort,
                cmd->col.r, cmd->col.g, cmd->col.b, cmd->col.a
            };
            
            v[vi++] = a;
            v[vi++] = b;
        }
        
        ID3D11DeviceContext_Map(xrnd.d11DeviceContext, 
                                (ID3D11Resource *)xrnd.lineVertexBuffer, 
                                0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
        memcpy(mr.pData, v, vc*sizeof(XLineVertex3D));
        ID3D11DeviceContext_Unmap(xrnd.d11DeviceContext, 
                                  (ID3D11Resource *)xrnd.lineVertexBuffer, 0);
        xfree(v);
        
        reset_line_group();
    }
    
    return vc;
}

void render_pass(D3D11_PRIMITIVE_TOPOLOGY tp, ID3D11InputLayout *il, u32 vc, ID3D11Buffer *vb, u32 vbst,
                 ID3D11VertexShader *vs, ID3D11PixelShader *ps)
{
    ID3D11DeviceContext_IASetPrimitiveTopology(xrnd.d11DeviceContext, tp);
    ID3D11DeviceContext_IASetInputLayout(xrnd.d11DeviceContext, il);
    ID3D11Buffer *vbs[1] = { vb };
    UINT vbss[1] = { vbst };
    UINT vbos[1] = { 0 };
    ID3D11DeviceContext_IASetVertexBuffers(xrnd.d11DeviceContext, 0, 1, vbs, vbss, vbos);
    ID3D11DeviceContext_VSSetShader(xrnd.d11DeviceContext, vs, NULL, 0);
    ID3D11DeviceContext_VSSetConstantBuffers(xrnd.d11DeviceContext, 0, 1, &xrnd.vertexCBuffer);
    ID3D11DeviceContext_PSSetShader(xrnd.d11DeviceContext, ps, NULL, 0);
    ID3D11DeviceContext_PSSetSamplers(xrnd.d11DeviceContext, 0, 1, &xrnd.samplerState);
    ID3D11DeviceContext_PSSetShaderResources(xrnd.d11DeviceContext, 0, 1, &xrnd.textureAtlasSRV);
    D3D11_VIEWPORT vps[1] =  { { 0, 0, xrnd.backBufferSize.x, xrnd.backBufferSize.y, 0.0f, 1.0f } };
    ID3D11DeviceContext_RSSetViewports(xrnd.d11DeviceContext, 1, vps);
    D3D11_RECT scs[1] = { { 0, 0, (LONG)xrnd.backBufferSize.x, (LONG)xrnd.backBufferSize.y } };
    ID3D11DeviceContext_RSSetScissorRects(xrnd.d11DeviceContext, 1, scs);
    ID3D11DeviceContext_RSSetState(xrnd.d11DeviceContext, xrnd.rasterizerState);
    ID3D11DeviceContext_OMSetDepthStencilState(xrnd.d11DeviceContext, xrnd.depthStencilState, 0);
    ID3D11DeviceContext_OMSetBlendState(xrnd.d11DeviceContext, xrnd.blendState, NULL, 0xffffffff);
    ID3D11DeviceContext_Draw(xrnd.d11DeviceContext, vc, 0);
}

function void
swap_chain_resize()
{
    RECT rect;
    if (!GetClientRect(xrnd.windowHandle, &rect)) {
        exit(1);
    }
    
    v2f backBufferSize = {
        (f32)(rect.right - rect.left),
        (f32)(rect.bottom - rect.top),
    };
    
    if (((UINT)backBufferSize.x != 0 && (UINT)backBufferSize.y != 0) &&
        (((UINT)backBufferSize.x != xrnd.backBufferSize.x) || 
         ((UINT)backBufferSize.y != xrnd.backBufferSize.y)))
    {
        xrnd.backBufferSize = backBufferSize;
        
        ID3D11RenderTargetView_Release(xrnd.renderTargetView);
        ID3D11DepthStencilView_Release(xrnd.depthStencilView);
        ID3D11Texture2D_Release(xrnd.backBufferTexture2D);
        ID3D11Texture2D_Release(xrnd.depthStencilBufferTexture2D);
        
        IDXGISwapChain_ResizeBuffers(xrnd.d11SwapChain, 2, 
                                     (UINT)backBufferSize.x,(UINT)backBufferSize.y, 
                                     DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        
        if (FAILED(IDXGISwapChain_GetBuffer(xrnd.d11SwapChain, 0, &IID_ID3D11Texture2D, 
                                            (void **)&xrnd.backBufferTexture2D)))
            exit(1);
        
        if (FAILED(ID3D11Device_CreateRenderTargetView(xrnd.d11Device, 
                                                       (ID3D11Resource *)xrnd.backBufferTexture2D, 
                                                       0, &xrnd.renderTargetView)))
            exit(1);
        
        xrnd.depthStencilBufferTexDesc.Width = (UINT)xrnd.backBufferSize.x;
        xrnd.depthStencilBufferTexDesc.Height = (UINT)xrnd.backBufferSize.y;
        
        if (FAILED(ID3D11Device_CreateTexture2D(xrnd.d11Device, &xrnd.depthStencilBufferTexDesc, 0, 
                                                &xrnd.depthStencilBufferTexture2D)))
            exit(1);
        
        if (FAILED(ID3D11Device_CreateDepthStencilView(xrnd.d11Device, 
                                                       (ID3D11Resource *)xrnd.depthStencilBufferTexture2D, 
                                                       &xrnd.depthStencilViewDesc,
                                                       &xrnd.depthStencilView)))
            exit(1);
    }
    
    xrender_resized();
}

void xrender_init(XRenderConfig config)
{
    DXGI_FORMAT atf; // depth stencil buffer format, atlas tex format
    
    /* Create a DC to render glyphs with  */
    xrnd.deviceContext = CreateCompatibleDC(GetDC(0));
    
    /* Register a window class */
    WNDCLASSEXW windowClass = xwndclass(config.wndproc);
    if (RegisterClassExW(&windowClass) == 0) exit(1);
    
    /* Window style based on config */
    xrnd.windowClassStyleEx =  config.windowClassStyleEx;
    xrnd.windowClassStyle   = (config.windowClassStyle == 0) ? 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE : config.windowClassStyle;
    
    /* Window pos and size */
    xrnd.windowPos  = (config.windowPos.x==0 && config.windowPos.y==0) ? 
        xrnd.windowPos = ini2f(0,0) : config.windowPos;
    
    xrnd.windowSize = (config.windowSize.x==0 && config.windowSize.y==0) ? 
        xrnd.windowSize = ini2f(800,600) : config.windowSize;
    
    RECT size = 
    { 
        (u32)xrnd.windowPos.x, (u32)xrnd.windowPos.y,
        (u32)(xrnd.windowPos.x + xrnd.windowSize.x), (u32)(xrnd.windowPos.y + xrnd.windowSize.y)
    };
    
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
    xrnd.windowSize = ini2f((f32)(size.right - size.left), (f32)(size.bottom - size.top));
    
    /* Window title */
    xstrcpy(xrnd.windowTitle, 256, (config.windowTitle == 0) ? 
            L"XLib's xrnder" : config.windowTitle);
    
    /* More renderer configuration */
    xrnd.topDown = config.topDown;
    xrnd.backBufferSize = xrnd.windowSize;
    xrnd.maxSimulSprites = (config.maxSimulSprites == 0) ? 10000 : config.maxSimulSprites;
    xrnd.maxSimulLines   = (config.maxSimulLines   == 0) ? 4096  : config.maxSimulLines;
    xrnd.glyphMakerSize  = (config.glyphMakerSize  == 0) ? xrnd.glyphMakerSize = 256   : config.glyphMakerSize;
    xrnd.clearColor      = (config.clearColor.a == 0) ? ini4f(.08f,.05f,.02f,1) : config.clearColor;
    
    
    /* Create the window */
    xrnd.windowHandle = CreateWindowExW(xrnd.windowClassStyleEx, L"xwindow_class", xrnd.windowTitle, 
                                        xrnd.windowClassStyle, (s32)xrnd.windowPos.x, (s32)xrnd.windowPos.y, 
                                        (s32)xrnd.windowSize.x, (s32)xrnd.windowSize.y, NULL, NULL, GetModuleHandle(0), NULL);
    
    if (xrnd.windowHandle == NULL)
        exit(1);
    
    /* Request notification when the mouse leaves the non-client area */
    TRACKMOUSEEVENT trackMouseEvent;
    trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
    trackMouseEvent.dwFlags = TME_NONCLIENT | TME_LEAVE;
    trackMouseEvent.hwndTrack = xrnd.windowHandle;
    TrackMouseEvent(&trackMouseEvent);
    
    /* Direct3D 11 backbuffer size, refresh rate, scale, swap chain, feature levels...  */
    DXGI_MODE_DESC modeDesc = 
    {
        (s32)xrnd.backBufferSize.x, (s32)xrnd.backBufferSize.y,
        xrational(60,1), DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_CENTERED
    };
    
    DXGI_SAMPLE_DESC backBufferSize = { .Count = 1,  .Quality = 0 };
    
    DXGI_SWAP_CHAIN_DESC swapChainDesc =
    {
        modeDesc, backBufferSize, 
        DXGI_USAGE_RENDER_TARGET_OUTPUT,
        2, xrnd.windowHandle, true, 
        DXGI_SWAP_EFFECT_FLIP_DISCARD, 0
    };
    
    xfeatureleves(xrnd.featureLevelArray, &xrnd.featureLevelIndex);
    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
                                             D3D11_CREATE_DEVICE_DEBUG, 
                                             xrnd.featureLevelArray, xrnd.featureLevelIndex, 
                                             D3D11_SDK_VERSION, &swapChainDesc, &xrnd.d11SwapChain,
                                             &xrnd.d11Device, NULL, &xrnd.d11DeviceContext)))
        exit(1);
    
    if (FAILED(IDXGISwapChain_GetBuffer(xrnd.d11SwapChain, 0, &IID_ID3D11Texture2D, 
                                        (void**)&xrnd.backBufferTexture2D)))
        exit(1);
    
    /* Direct3D 11 Debug interface */
    if (FAILED(ID3D11Device_QueryInterface(xrnd.d11Device, &IID_ID3D11Debug, 
                                           (void**)&xrnd.d11Debug)))
        exit(1);
    
    ID3D11InfoQueue* infoQueue;
    ID3D11Device_QueryInterface(xrnd.d11Device, &IID_ID3D11InfoQueue, (void**)&infoQueue);
    ID3D11InfoQueue_SetBreakOnSeverity(infoQueue, D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
    ID3D11InfoQueue_Release(infoQueue);
    
    
    DXGI_FORMAT depthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    D3D11_TEXTURE2D_DESC depthStencilBufferTexDesc =
    {
        (s32)xrnd.backBufferSize.x, (s32)xrnd.backBufferSize.y,
        0, 1, depthStencilBufferFormat, backBufferSize, 
        D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL, 0, 0
    };
    
    xrnd.depthStencilBufferTexDesc = depthStencilBufferTexDesc;
    
    if (FAILED(ID3D11Device_CreateTexture2D(xrnd.d11Device, &depthStencilBufferTexDesc, 0, 
                                            &xrnd.depthStencilBufferTexture2D)))
        exit(1);
    
    xrnd.textureAtlas.size = (config.textureAtlasSize == 0) ? 
        4096 : config.textureAtlasSize;
    
    xrnd.textureAtlas.at.x = xrnd.textureAtlas.at.y = xrnd.textureAtlas.bottom = 0;
    
    atf = DXGI_FORMAT_R8G8B8A8_UNORM;
    D3D11_TEXTURE2D_DESC atd = { xrnd.textureAtlas.size, xrnd.textureAtlas.size, 1, 1, atf, {1,0},
        D3D11_USAGE_DYNAMIC, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, 0, };
    
    u32 ams = xrnd.textureAtlas.size * xrnd.textureAtlas.size * 4;
    xrnd.textureAtlas.bytes = (u8*)xalloc(ams);
    
    D3D11_SUBRESOURCE_DATA ad = { xrnd.textureAtlas.bytes, (UINT)(xrnd.textureAtlas.size * 4), 0 };
    if (FAILED(ID3D11Device_CreateTexture2D(xrnd.d11Device, &atd, &ad, 
                                            &xrnd.textureAtlasTexture2D)))
        exit(1);
    
    if (FAILED(ID3D11Device_CreateRenderTargetView(xrnd.d11Device, 
                                                   (ID3D11Resource*)xrnd.backBufferTexture2D, 
                                                   0, &xrnd.renderTargetView)))
        exit(1);
    
    xrnd.depthStencilViewDesc = xdsviewdesc(depthStencilBufferFormat, 
                                            D3D11_DSV_DIMENSION_TEXTURE2D);
    if (FAILED(ID3D11Device_CreateDepthStencilView(xrnd.d11Device, 
                                                   (ID3D11Resource*)xrnd.depthStencilBufferTexture2D, 
                                                   &xrnd.depthStencilViewDesc,
                                                   &xrnd.depthStencilView)))
        exit(1);
    
    D3D11_SHADER_RESOURCE_VIEW_DESC asrvd = xshadresview(atf, D3D_SRV_DIMENSION_TEXTURE2D);
    if (FAILED(ID3D11Device_CreateShaderResourceView(xrnd.d11Device, 
                                                     (ID3D11Resource*)xrnd.textureAtlasTexture2D, 
                                                     &asrvd, &xrnd.textureAtlasSRV)))
        exit(1);
    
    D3D11_DEPTH_STENCIL_DESC dsd = xdepthstencildesc();
    
    if (FAILED(ID3D11Device_CreateDepthStencilState(xrnd.d11Device, &dsd, &xrnd.depthStencilState)))
        exit(1);
    
    D3D11_BLEND_DESC bd = xblenddesc();
    
    if (FAILED(ID3D11Device_CreateBlendState(xrnd.d11Device, &bd, &xrnd.blendState)))
        exit(1);
    
    D3D11_RASTERIZER_DESC rasterizerDesc = xrasterstate();
    if (FAILED(ID3D11Device_CreateRasterizerState(xrnd.d11Device, &rasterizerDesc, 
                                                  &xrnd.rasterizerState)))
        exit(1);
    
    D3D11_SAMPLER_DESC sd = xsamplerdesc();
    if (FAILED(ID3D11Device_CreateSamplerState(xrnd.d11Device, &sd, &xrnd.samplerState)))
        exit(1);
    
    create_sprites_shaders();
    create_lines_shaders();
    
    create_sprites_vertex_buffer();
    create_lines_vertex_buffer();
    
    D3D11_BUFFER_DESC vcbd = xcbufferdesc(sizeof(XVertexCBuffer));
    if (FAILED(ID3D11Device_CreateBuffer(xrnd.d11Device, &vcbd, 0, &xrnd.vertexCBuffer)))
        exit(1);
    
    xrnd.lineBatch = Array_new(256, sizeof(XLineCommand));
    
    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);
    
    xrnd.running = true;
}

void xrender_update(void)
{
    // Measure fps
    LARGE_INTEGER counter = xwallclock();
    
    // Calculate dt
    xrnd.dt = xseconds(xrnd.lastCounter, counter);
    if (xrnd.dt > 1000)
        xrnd.dt = 0;
    
    // Save counter
    xrnd.lastCounter = counter;
    
    swap_chain_resize();
    
    u32 svc = produce_vertices_from_sprite_and_mesh_groups();
    u32 lvc = produce_vertices_from_line_group();
    
    f32 scaleX = 2.0f / xrnd.backBufferSize.x;
    f32 scaleY = (xrnd.topDown ? -2.0f : 2.0f) / xrnd.backBufferSize.y;
    mat4f wvpMatrix =
    {
        scaleX, 0, 0, -1,
        0, scaleY, 0, (xrnd.topDown ? 1.f : -1.f),
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    xrnd.vertexCBufferData.WVP = wvpMatrix;
    ID3D11DeviceContext_UpdateSubresource(xrnd.d11DeviceContext, 
                                          (ID3D11Resource*)xrnd.vertexCBuffer, 0, NULL, 
                                          &xrnd.vertexCBufferData, 0, 0);
    
    ID3D11RenderTargetView* views[1] = { xrnd.renderTargetView };
    ID3D11DeviceContext_OMSetRenderTargets(xrnd.d11DeviceContext, 1, views, xrnd.depthStencilView);
    ID3D11DeviceContext_ClearRenderTargetView(xrnd.d11DeviceContext, xrnd.renderTargetView, xrnd.clearColor.e);
    ID3D11DeviceContext_ClearDepthStencilView(xrnd.d11DeviceContext, xrnd.depthStencilView, 
                                              D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
    
    // Lines
    render_pass(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, 
                xrnd.lineInputLayout, lvc, 
                xrnd.lineVertexBuffer, sizeof(XLineVertex3D), 
                xrnd.lineVertexShader, xrnd.linePixelShader);
    // Sprites
    render_pass(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 
                xrnd.texturedInputLayout, svc, 
                xrnd.texturedVertexBuffer, sizeof(XVertex3D), 
                xrnd.texturedVertexShader, xrnd.texturedPixelShader);
    
    IDXGISwapChain_Present(xrnd.d11SwapChain, 1, 0);
}

void xrender_shutdown(void)
{
    xfree(xrnd.textureAtlas.bytes);
    
    free_sprite_and_mesh_groups();
    free_line_group();
    
    xmemcheck();
    
    if (xrnd.textureAtlasSRV) 
        ID3D11ShaderResourceView_Release(xrnd.textureAtlasSRV);
    
    if (xrnd.vertexCBuffer) 
        ID3D11Buffer_Release(xrnd.vertexCBuffer);
    
    if (xrnd.lineVertexBuffer) 
        ID3D11Buffer_Release(xrnd.lineVertexBuffer);
    
    if (xrnd.texturedVertexBuffer) 
        ID3D11Buffer_Release(xrnd.texturedVertexBuffer);
    
    if (xrnd.lineInputLayout) 
        ID3D11InputLayout_Release(xrnd.lineInputLayout);
    
    if (xrnd.texturedInputLayout) 
        ID3D11InputLayout_Release(xrnd.texturedInputLayout);
    
    if (xrnd.linePixelShader)
        ID3D11PixelShader_Release(xrnd.linePixelShader);
    
    if (xrnd.texturedPixelShader) 
        ID3D11PixelShader_Release(xrnd.texturedPixelShader);
    
    if (xrnd.lineVertexShader) 
        ID3D11VertexShader_Release(xrnd.lineVertexShader);
    
    if (xrnd.texturedVertexShader)
        ID3D11VertexShader_Release(xrnd.texturedVertexShader);
    
    if (xrnd.samplerState)
        ID3D11SamplerState_Release(xrnd.samplerState);
    
    if (xrnd.rasterizerState) 
        ID3D11RasterizerState_Release(xrnd.rasterizerState);
    
    if (xrnd.blendState) 
        ID3D11BlendState_Release(xrnd.blendState);
    
    if (xrnd.depthStencilState)
        ID3D11DepthStencilState_Release(xrnd.depthStencilState);
    
    if (xrnd.depthStencilView) 
        ID3D11DepthStencilView_Release(xrnd.depthStencilView);
    
    if (xrnd.renderTargetView) 
        ID3D11RenderTargetView_Release(xrnd.renderTargetView);
    
    if (xrnd.textureAtlasTexture2D) 
        ID3D11Texture2D_Release(xrnd.textureAtlasTexture2D);
    
    if (xrnd.depthStencilBufferTexture2D) 
        ID3D11Texture2D_Release(xrnd.depthStencilBufferTexture2D);
    
    if (xrnd.backBufferTexture2D) 
        ID3D11Texture2D_Release(xrnd.backBufferTexture2D);
    
    if (xrnd.d11Debug) 
        ID3D11Debug_Release(xrnd.d11Debug);
    
    if (xrnd.d11SwapChain) 
        IDXGISwapChain_Release(xrnd.d11SwapChain);
    
    if (xrnd.d11DeviceContext) 
        ID3D11DeviceContext_Release(xrnd.d11DeviceContext);
    
    if (xrnd.d11Device) 
        ID3D11Device_Release(xrnd.d11Device);
}

#undef T

#endif