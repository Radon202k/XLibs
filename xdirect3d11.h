#ifndef XLIB_DIRECT3D11
#define XLIB_DIRECT3D11

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


/* =========================================================================
   DATA TYPES THAT THE USER CARES ABOUT
   ========================================================================= */

typedef struct XD11             XD11;
typedef struct XD11Config       XD11Config;
typedef struct XD11Sprite       XD11Sprite;
typedef struct XD11Font         XD11Font;
typedef struct XD11Batch        XD11Batch;
typedef struct XD11Texture      XD11Texture;
typedef struct XD11TextureAtlas XD11TextureAtlas;

struct XD11Config
{
    WNDPROC  wndproc;
    u32 windowClassStyle;
    u32 windowClassStyleEx;
    wchar_t* windowTitle;
    v2f windowPos;
    v2f windowSize;
    s32 glyphMakerSize;
};

struct XD11Batch
{
    /* Core */
    ID3D11Texture2D *backBufferTexture;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11DepthStencilState *depthStencilState;
    ID3D11DepthStencilView *depthStencilView;
    ID3D11Texture2D *depthStencilTexture;
    
    /* States */
    ID3D11BlendState *stateBlend; 
    ID3D11RasterizerState *stateRasterizer;
    ID3D11SamplerState *stateSampler;
    
    /* Vertex Buffers */
    u32 vertexBufferCount;
    ID3D11Buffer **vertexBuffers; 
    u32 *vertexBufferStrides;
    u32 *vertexBufferOffsets;
    
    /* Vertex Shader Constant Buffers */
    u32 vsConstantBufferCount;
    ID3D11Buffer **vsConstantBuffers;
    
    /* Pixel Shader Resources */
    u32 psResourceCount;
    ID3D11ShaderResourceView **psResources;
    
    /* Other */
    D3D11_PRIMITIVE_TOPOLOGY topology;
    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *inputLayout;
    u32 viewportCount;
    D3D11_VIEWPORT *viewports;
    u32 scissorCount;
    D3D11_RECT *scissors;
};

struct XD11
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
    
    /* Core values */
    HDC glyphMakerDC;
    HWND windowHandle;
    LARGE_INTEGER lastCounter;
    IDXGISwapChain *swapChain;
    ID3D11Device *device;
    ID3D11DeviceContext *deviceContext;
    ID3D11Debug *debug;
    ID3D11Buffer *vertexBuffer;
};

struct XD11Texture
{
    ID3D11Texture2D *handle;
    ID3D11ShaderResourceView *shaderResourceView;
    v2i size;
    u8 *bytes;
};

struct XD11TextureAtlas
{
    XD11Texture texture;
    s32 size;
    s32 bottom;
    v2i at;
};

struct XD11Sprite
{
    v2f size;
    rect2f uv;
    v2f align;
};

struct XD11Font
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
    DIRECT3D 11 HELPER FUNCTIONS (Less verbose than D3D11 API)
   ========================================================================= */

void                      xd11_update_subresource   (void *resource, void *data);
ID3D11Texture2D          *xd11_swapchain_get_buffer (void);
ID3D11Texture2D          *xd11_texture2d (D3D11_TEXTURE2D_DESC desc, D3D11_SUBRESOURCE_DATA *data);
ID3D11Buffer             *xd11_buffer    (D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA *data);
ID3D11RenderTargetView   *xd11_render_target_view   (ID3D11Texture2D *texture);
ID3D11ShaderResourceView *xd11_shader_resource_view (ID3D11Texture2D *texture, D3D11_SHADER_RESOURCE_VIEW_DESC desc);
ID3D11DepthStencilView   *xd11_depth_stencil_view   (ID3D11Texture2D *texture, D3D11_DEPTH_STENCIL_VIEW_DESC desc);
ID3D11DepthStencilState  *xd11_depth_stencil_state  (D3D11_DEPTH_STENCIL_DESC desc);
ID3D11BlendState         *xd11_blend_state          (D3D11_BLEND_DESC desc);
ID3D11RasterizerState    *xd11_rasterizer_state     (D3D11_RASTERIZER_DESC desc);
ID3D11SamplerState       *xd11_sampler_state        (D3D11_SAMPLER_DESC desc);

/* =========================================================================
   MAIN FUNCTIONS
   ========================================================================= */

void xd11_initialize   (XD11Config config);
void xd11_shutdown     (void);
void xd11_update       (void);
void xd11_resized      (void);

v2f  xd11_monitor_size (void);

/* =========================================================================
   TEXTURE / TEXTURE ATLAS / SPRITES
   ========================================================================= */

/*  A D11 Batch is composed of many vertices representing triangles texture
mapped to a single gpu texture that is called the texture atlas. A Sprite is a
smaller rectangle than the whole texture atlas that represents a texture in it
self, i.e., the texture atlas is composed of many different smaller textures.
  The point of that is faster performance. xspritepng creates a sprite from a
png file located at path, i.e., it loads the png and copies the bytes of the
texture into the texture atlas and fills XSprite with the corresponding uv coo
rdinates and other info about the source image. */


u8 *     xd11_load_png(wchar_t *path, v2i *dim, bool premulAlpha);
void     xd11_texture_update(XD11Texture texture, u8 *bytes);

XD11Sprite  xd11_sprite_from_png  (XD11TextureAtlas *atlas, wchar_t *path, bool premulalpha);
XD11Sprite  xd11_sprite_from_bytes(XD11TextureAtlas *atlas, u8 *b, v2i dim);

u32      xd11_hash_unicode(const void *k);
bool     xd11_cmp_glyph_unicodes(const void *a, const void *b);

s32      xd11_font_height(s32 pointHeight);
void     xd11_font_free(XD11Font f);

void     xd11_blit_simple_unchecked(XD11Texture *dest, u8 *src, v2i at, v2i dim);


/* =========================================================================
   FONTS / GLYPHS
   ========================================================================= */

XD11Font    xd11_font(XD11TextureAtlas *a, wchar_t *path, wchar_t *name, int heightpoints);
XD11Sprite  xd11_glyph_from_char(XD11TextureAtlas *a, XD11Font font, wchar_t *c, rect2f *tBounds, s32 *tDescent);
void    xd11_font_free    (XD11Font font);
s32     xd11_font_height  (s32 pointHeight);


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

XD11 xd11;

#define RGBA(r,g,b,a) (((a)<<24) | ((r)<<16) | ((g)<<8) | (b))

void xd11_update_subresource(void *resource, void *data)
{
    ID3D11DeviceContext_UpdateSubresource(xd11.deviceContext, 
                                          (ID3D11Resource *)resource, 
                                          0, 
                                          NULL, 
                                          data, 
                                          0, 
                                          0);
}

ID3D11Texture2D *xd11_swapchain_get_buffer(void)
{
    ID3D11Texture2D *result = 0;
    if (FAILED(IDXGISwapChain_GetBuffer(xd11.swapChain, 0, &IID_ID3D11Texture2D, (void**)&result)))
        assert(!"Can't fail");
    return result;
}

ID3D11Buffer *xd11_buffer(D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA *data)
{
    ID3D11Buffer *result = 0;
    if (FAILED(ID3D11Device_CreateBuffer(xd11.device, 
                                         &desc, 
                                         0, 
                                         &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11Texture2D *xd11_texture2d(D3D11_TEXTURE2D_DESC desc, D3D11_SUBRESOURCE_DATA *data)
{
    ID3D11Texture2D *result = 0;
    if (FAILED(ID3D11Device_CreateTexture2D(xd11.device, &desc, data, &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11RenderTargetView *xd11_render_target_view(ID3D11Texture2D *texture)
{
    ID3D11RenderTargetView *result = 0;
    if (FAILED(ID3D11Device_CreateRenderTargetView(xd11.device, 
                                                   (ID3D11Resource* )texture, 
                                                   0, &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11DepthStencilView *xd11_depth_stencil_view(ID3D11Texture2D *texture,
                                                D3D11_DEPTH_STENCIL_VIEW_DESC desc)
{
    ID3D11DepthStencilView *result = 0;
    if (FAILED(ID3D11Device_CreateDepthStencilView(xd11.device, 
                                                   (ID3D11Resource*)texture, 
                                                   &desc,
                                                   &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11ShaderResourceView *xd11_shader_resource_view(ID3D11Texture2D *texture,
                                                    D3D11_SHADER_RESOURCE_VIEW_DESC desc)
{
    ID3D11ShaderResourceView *result = 0;
    if (FAILED(ID3D11Device_CreateShaderResourceView(xd11.device, 
                                                     (ID3D11Resource*)texture, 
                                                     &desc,
                                                     &result)))
        assert(!"Can't fail");
    
    return result;
}

ID3D11DepthStencilState *xd11_depth_stencil_state(D3D11_DEPTH_STENCIL_DESC desc)
{
    ID3D11DepthStencilState *result = 0;
    if (FAILED(ID3D11Device_CreateDepthStencilState(xd11.device, 
                                                    &desc, 
                                                    &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11BlendState *xd11_blend_state(D3D11_BLEND_DESC desc)
{
    ID3D11BlendState *result = 0;
    if (FAILED(ID3D11Device_CreateBlendState(xd11.device, 
                                             &desc, 
                                             &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11RasterizerState *xd11_rasterizer_state(D3D11_RASTERIZER_DESC desc)
{
    ID3D11RasterizerState *result = 0;
    if (FAILED(ID3D11Device_CreateRasterizerState(xd11.device,
                                                  &desc,
                                                  &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11SamplerState *xd11_sampler_state(D3D11_SAMPLER_DESC desc)
{
    ID3D11SamplerState *result = 0;
    if (FAILED(ID3D11Device_CreateSamplerState(xd11.device, 
                                               &desc, 
                                               &result)))
        assert(!"Can't fail");
    return result;
}

u8 *xd11_load_png(wchar_t *path, v2i *dim, bool premulalpha)
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

void xd11_blit_simple_unchecked(XD11Texture *dest, u8 *src, v2i at, v2i dim)
{
    u8 *rowSrc, *rowDst;
    u32 *pxSrc, *pxDst;
    
    u8 *dst = dest->bytes;
    u32 dstStride = dest->size.x*4;
    
    rowSrc=src;
    rowDst=dst + at.y*dstStride + at.x*4;
    
    for (at.y=0; at.y<dim.y; ++at.y)
    {
        pxSrc=(u32 *)rowSrc;
        pxDst=(u32 *)rowDst;
        
        for (at.x=0; at.x<dim.x; ++at.x)
            *pxDst++ = *pxSrc++;
        
        rowSrc += 4*dim.x;
        rowDst += dstStride;
    }
}

XD11Sprite xd11_sprite_from_bytes(XD11TextureAtlas *a, u8 *b, v2i dim)
{
    s32 m;
    u8 *dst, *src;
    XD11Sprite r;
    
    m = 1;
    
    if ((a->at.x + dim.x + m) > a->size)
        a->at = ini2i(0, a->bottom);
    
    if (m+a->bottom < (a->at.y + dim.y))
        a->bottom = a->at.y + dim.y + m;
    
    assert(a->bottom <= a->size);
    
    dst = a->texture.bytes;
    src = b;
    xd11_blit_simple_unchecked(&a->texture, src, a->at, dim);
    
    r.uv.min = (v2f){a->at.x / (f32)a->size,
        ( (a->at.y + dim.y) ) / (f32)a->size};
    
    r.uv.max = ini2f((a->at.x + dim.x) / (f32)a->size, 
                     ( a->at.y ) / (f32)a->size);
    
    r.size = ini2fs(dim.x,dim.y);
    
    a->at.x += dim.x+m;
    
    return r;
}

XD11Sprite xd11_sprite_from_png(XD11TextureAtlas *a, wchar_t *path, bool premulalpha)
{
    XD11Sprite r;
    v2i dim;
    u8 *b;
    
    memset(&r, 0, sizeof(r));
    b = xd11_load_png(path, &dim, premulalpha);
    if (b)
    {
        r = xd11_sprite_from_bytes(a, b, dim);
        xfree(b);
    }
    return r;
}

void xd11_texture_update(XD11Texture texture, u8 *bytes)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubResource;
    ID3D11DeviceContext_Map(xd11.deviceContext, (ID3D11Resource *)texture.handle, 
                            0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
    
    memcpy(mappedSubResource.pData, bytes, texture.size.x*texture.size.y*4);
    
    ID3D11DeviceContext_Unmap(xd11.deviceContext, (ID3D11Resource *)texture.handle, 0);
}

bool xd11_cmp_glyph_unicodes(const void *a, const void *b)
{
    u32 ua, ub;
    
    ua = *(u32 *)a;
    ub = *(u32 *)b;
    return (ua == ub);
}

u32 xd11_hash_unicode(const void *k)
{
    u32 u;
    
    u = *(u32 *)k;
    u = (u & (512-1));
    return u;
}

XD11Font xd11_font(XD11TextureAtlas *atlas, wchar_t *path, wchar_t *name, int heightpoints)
{
    XD11Font result;
    memset(&result, 0, sizeof(result));
    
    v2i maxGlyphSize;
    s32 maxDescent;
    
    s32 i;
    
    wchar_t c[2];
    u32 *k;
    XD11Sprite *v;
    
    /* Add font resource to Windows */
    s32 temp = AddFontResourceW(path);
    assert(temp == 1);
    
    /* Create the font */
    result.handle = CreateFontW(xd11_font_height(heightpoints), 
                                0, 0, 0, FW_NORMAL, false, false, 
                                false, DEFAULT_CHARSET,
                                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                                CLEARTYPE_QUALITY, DEFAULT_PITCH, name);
    
    assert(result.handle && result.handle != INVALID_HANDLE_VALUE);
    
    /* Create a bitmap to d11 glyphs into */
    BITMAPINFO info = xbmpinfo(xd11.glyphMakerSize, -xd11.glyphMakerSize);
    result.bitmap = CreateDIBSection(xd11.glyphMakerDC, &info, 
                                     DIB_RGB_COLORS, &result.bytes, 0, 0);
    assert(xd11.glyphMakerSize>0);
    memset(result.bytes, 0, xd11.glyphMakerSize*xd11.glyphMakerSize*4);
    
    /* Configure the bitmap drawing to use black and white */
    SelectObject(xd11.glyphMakerDC, result.bitmap);
    SelectObject(xd11.glyphMakerDC, result.handle);
    SetBkColor(xd11.glyphMakerDC, RGB(0,0,0));
    SetTextColor(xd11.glyphMakerDC, RGB(255,255,255));
    
    /* Get text metrics for font */
    TEXTMETRICW metrics = {0};
    GetTextMetricsW(xd11.glyphMakerDC, &metrics);
    result.metrics = metrics;
    result.lineadvance = (f32)metrics.tmHeight - metrics.tmInternalLeading;
    result.charwidth = (f32)metrics.tmAveCharWidth;
    result.glyphs = Table_new(512, xd11_cmp_glyph_unicodes, xd11_hash_unicode);
    
    /* D11 some glyphs from the ASCII range */
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
        *v = xd11_glyph_from_char(atlas, result, c, &tightBounds, &tightDescent);
        
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

XD11Sprite xd11_glyph_from_char(XD11TextureAtlas *atlas, XD11Font font, wchar_t *c, 
                                rect2f *tBounds, s32 *tDescent)
{
    u8 *b, *dstRow, *srcRow;
    s32 charsz, i,j, x,y;
    u32 *dstPx, *srcPx, *px, color, a, ps;
    v2f d, al;
    XD11Sprite r;
    SIZE size;
    rect2f bounds;
    
    ps=(s32)(0.3f*font.lineadvance);
    GetTextExtentPoint32W(xd11.glyphMakerDC, c, 1, &size);
    al=ini2f(0,0);
    d=ini2fs(size.cx,size.cy);
    charsz=(s32)wcslen(c);
    bounds = inir2f(0,0, d.x,d.y);
    tBounds->min.x=tBounds->min.y=1000000;
    tBounds->max.x=tBounds->max.y=-1000000;
    
    TextOutW(xd11.glyphMakerDC, ps,0, c, charsz);
    
    bool foundTBox = false;
    for (j=0; j<xd11.glyphMakerSize; ++j)
    {
        for (i=0; i<xd11.glyphMakerSize; ++i)
        {
            px = (u32 *)((u8 *)font.bytes + j*xd11.glyphMakerSize*4 + i*4);
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
        srcRow = ((u8 *)font.bytes + (s32)tBounds->min.y*xd11.glyphMakerSize*4 + (s32)tBounds->min.x*4);
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
            srcRow += 4*xd11.glyphMakerSize;
        }
    }
    
    r = xd11_sprite_from_bytes(atlas, b, ini2if(d.x,d.y));
    r.align = al;
    xfree(b);
    return r;
}

s32 xd11_font_height(s32 pointHeight)
{
    s32 result = MulDiv(pointHeight, 
                        GetDeviceCaps(xd11.glyphMakerDC, LOGPIXELSY), 
                        GetDeviceCaps(xd11.glyphMakerDC, LOGPIXELSX));
    return result;
}

void xd11_font_free(XD11Font f)
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

v2f xd11_monitor_size(void)
{
    v2f r;
    
    r=ini2fs(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    
    return r;
}

function ID3D11VertexShader *xd11_compile_vertex_shader(char *source, ID3DBlob **compiledVS)
{
    ID3D11VertexShader *result = 0;
    
    // Compile the shader
    ID3DBlob *errorVertexMessages;
    if (FAILED(D3DCompile(source, strlen(source) + 1,
                          0, 0, 0, "vs_main", "vs_5_0",
                          0, 0, compiledVS, &errorVertexMessages)))
    {
        if (errorVertexMessages)
        {
            wchar_t *msg = (wchar_t *)
            (ID3D10Blob_GetBufferPointer(errorVertexMessages));
            OutputDebugStringW(msg);
            assert(!"Failed to compile shader");
        }
        else
        {
            assert(!"Failed to compile shader");
        }
    }
    
    // Create the vertex shader if compilation went ok
    ID3D11Device_CreateVertexShader(xd11.device,
                                    ID3D10Blob_GetBufferPointer(*compiledVS),
                                    ID3D10Blob_GetBufferSize(*compiledVS),
                                    NULL, &result);
    
    return result;
}

function ID3D11PixelShader *xd11_compile_pixel_shader(char *source, ID3DBlob **compiledPS)
{
    ID3D11PixelShader *result = 0;
    
    // And compilation of the pixel shader
    ID3DBlob *errorPixelMessages;
    if (FAILED(D3DCompile(source, strlen(source) + 1,
                          0, 0, 0, "ps_main", "ps_5_0",
                          0, 0, compiledPS, &errorPixelMessages)))
    {
        if (errorPixelMessages)
        {
            char *msg = (char *)
            (ID3D10Blob_GetBufferPointer(errorPixelMessages));
            OutputDebugStringA(msg);
            assert(!"Failed to compile shader");
        }
        else
        {
            assert(!"Failed to compile shader");
        }
    }
    
    // Create the pixel shader if compilation went ok
    ID3D11Device_CreatePixelShader(xd11.device,
                                   ID3D10Blob_GetBufferPointer(*compiledPS),
                                   ID3D10Blob_GetBufferSize(*compiledPS),
                                   NULL, &result);
    
    return result;
}

function ID3D11InputLayout *xd11_input_layout(ID3DBlob *compiledVS,
                                              D3D11_INPUT_ELEMENT_DESC *inputElementDescs, u32 count)
{    
    ID3D11InputLayout *result = 0;
    // And create the input layout
    void *vsPointer = ID3D10Blob_GetBufferPointer(compiledVS);
    u32 vsSize = (u32)ID3D10Blob_GetBufferSize(compiledVS);
    
    if (FAILED(ID3D11Device_CreateInputLayout(xd11.device, inputElementDescs, count,
                                              vsPointer, vsSize, &result)))
        exit(1);
    
    return result;
}

void xd11_buffer_update(ID3D11Buffer *buffer, void *data, u32 size)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubResource;
    ID3D11DeviceContext_Map(xd11.deviceContext, 
                            (ID3D11Resource *)buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
    
    memcpy(mappedSubResource.pData, data, size); 
    
    ID3D11DeviceContext_Unmap(xd11.deviceContext, (ID3D11Resource *)buffer, 0);
}

void xd11_batch_clear(ID3D11RenderTargetView *d11TargetView, 
                      ID3D11DepthStencilView *depthStencilView, v4f clearColor)
{
    ID3D11DeviceContext_ClearRenderTargetView(xd11.deviceContext, d11TargetView, clearColor.e);
    ID3D11DeviceContext_ClearDepthStencilView(xd11.deviceContext, depthStencilView, 
                                              D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
}

void xd11_batch_draw(XD11Batch *batch, u32 vertexCount)
{
    ID3D11DeviceContext_IASetPrimitiveTopology(xd11.deviceContext, batch->topology);
    
    ID3D11DeviceContext_IASetInputLayout(xd11.deviceContext, batch->inputLayout);
    
    /*
    ID3D11Buffer *vbs[1] = { batch->vertexBuffer };
    UINT vbss[1] = { batch->vertexBufferStride };
    UINT vbos[1] = { 0 };
    D3D11_VIEWPORT vps[1] =  { { batch->viewport.min.x, batch->viewport.min.y, batch->viewport.max.x, batch->viewport.max.y, 0.0f, 1.0f } };
    D3D11_RECT scs[1] = { { 0, 0, (LONG)xd11.backBufferSize.x, (LONG)xd11.backBufferSize.y } };

    */
    
    ID3D11DeviceContext_IASetVertexBuffers(xd11.deviceContext, 0, batch->vertexBufferCount, batch->vertexBuffers, 
                                           batch->vertexBufferStrides, batch->vertexBufferOffsets);
    
    ID3D11DeviceContext_VSSetShader(xd11.deviceContext, batch->vertexShader, NULL, 0);
    
    ID3D11DeviceContext_VSSetConstantBuffers(xd11.deviceContext, 0, batch->vsConstantBufferCount, batch->vsConstantBuffers);
    
    ID3D11DeviceContext_PSSetShader(xd11.deviceContext, batch->pixelShader, NULL, 0);
    ID3D11DeviceContext_PSSetSamplers(xd11.deviceContext, 0, 1, &batch->stateSampler);
    
    ID3D11DeviceContext_PSSetShaderResources(xd11.deviceContext, 0, batch->psResourceCount, batch->psResources);
    
    ID3D11DeviceContext_RSSetViewports(xd11.deviceContext, batch->viewportCount, batch->viewports);
    ID3D11DeviceContext_RSSetScissorRects(xd11.deviceContext, batch->scissorCount, batch->scissors);
    
    ID3D11DeviceContext_RSSetState(xd11.deviceContext, batch->stateRasterizer);
    ID3D11DeviceContext_OMSetDepthStencilState(xd11.deviceContext, batch->depthStencilState, 0);
    
    ID3D11DeviceContext_OMSetBlendState(xd11.deviceContext, batch->stateBlend, NULL, 0xffffffff);
    ID3D11DeviceContext_Draw(xd11.deviceContext, vertexCount, 0);
}

function void xd11_swap_chain_resize()
{
    RECT rect;
    if (!GetClientRect(xd11.windowHandle, &rect)) {
        exit(1);
    }
    
    v2f backBufferSize = {
        (f32)(rect.right - rect.left),
        (f32)(rect.bottom - rect.top),
    };
    
    if (((UINT)backBufferSize.x != 0 && (UINT)backBufferSize.y != 0) &&
        (((UINT)backBufferSize.x != xd11.backBufferSize.x) || 
         ((UINT)backBufferSize.y != xd11.backBufferSize.y)))
    {
        xd11.backBufferSize = backBufferSize;
        
        xd11_resized();
    }
}

void xd11_initialize(XD11Config config)
{
    /* Create a DC to d11 glyphs with  */
    xd11.glyphMakerDC = CreateCompatibleDC(GetDC(0));
    
    /* Register a window class */
    WNDCLASSEXW windowClass = xwndclass(config.wndproc);
    if (RegisterClassExW(&windowClass) == 0) exit(1);
    
    /* Window style based on config */
    xd11.windowClassStyleEx =  config.windowClassStyleEx;
    xd11.windowClassStyle   = (config.windowClassStyle == 0) ? 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE : config.windowClassStyle;
    
    /* Window pos and size */
    xd11.windowPos  = (config.windowPos.x==0 && config.windowPos.y==0) ? 
        xd11.windowPos = ini2f(0,0) : config.windowPos;
    
    xd11.windowSize = (config.windowSize.x==0 && config.windowSize.y==0) ? 
        xd11.windowSize = ini2f(800,600) : config.windowSize;
    
    xd11.backBufferSize = xd11.windowSize;
    
    RECT size = 
    { 
        (u32)xd11.windowPos.x, (u32)xd11.windowPos.y,
        (u32)(xd11.windowPos.x + xd11.windowSize.x), (u32)(xd11.windowPos.y + xd11.windowSize.y)
    };
    
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
    xd11.windowSize = ini2f((f32)(size.right - size.left), (f32)(size.bottom - size.top));
    
    /* Window title */
    xstrcpy(xd11.windowTitle, 256, (config.windowTitle == 0) ? 
            L"XLib's xd11er" : config.windowTitle);
    
    /* More configuration */
    xd11.glyphMakerSize  = (config.glyphMakerSize  == 0) ? xd11.glyphMakerSize = 256   : config.glyphMakerSize;
    
    
    /* Create the window */
    xd11.windowHandle = CreateWindowExW(xd11.windowClassStyleEx, L"xwindow_class", xd11.windowTitle, 
                                        xd11.windowClassStyle, (s32)xd11.windowPos.x, (s32)xd11.windowPos.y, 
                                        (s32)xd11.windowSize.x, (s32)xd11.windowSize.y, NULL, NULL, GetModuleHandle(0), NULL);
    
    if (xd11.windowHandle == NULL)
        exit(1);
    
    /* Request notification when the mouse leaves the non-client area */
    TRACKMOUSEEVENT trackMouseEvent;
    trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
    trackMouseEvent.dwFlags = TME_NONCLIENT | TME_LEAVE;
    trackMouseEvent.hwndTrack = xd11.windowHandle;
    TrackMouseEvent(&trackMouseEvent);
    
    /* Direct3D 11 backbuffer size, refresh rate, scale, swap chain, feature levels...  */
    DXGI_MODE_DESC modeDesc = 
    {
        (s32)xd11.backBufferSize.x, (s32)xd11.backBufferSize.y,
        xrational(60,1), DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_CENTERED
    };
    
    DXGI_SAMPLE_DESC backBufferSize = { .Count = 1,  .Quality = 0 };
    
    DXGI_SWAP_CHAIN_DESC swapChainDesc =
    {
        modeDesc, backBufferSize, 
        DXGI_USAGE_RENDER_TARGET_OUTPUT,
        2, xd11.windowHandle, true, 
        DXGI_SWAP_EFFECT_FLIP_DISCARD, 0
    };
    
    xfeatureleves(xd11.featureLevelArray, &xd11.featureLevelIndex);
    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
                                             D3D11_CREATE_DEVICE_DEBUG, 
                                             xd11.featureLevelArray, xd11.featureLevelIndex, 
                                             D3D11_SDK_VERSION, &swapChainDesc, &xd11.swapChain,
                                             &xd11.device, NULL, &xd11.deviceContext)))
        exit(1);
    
    /* Direct3D 11 Debug interface */
    if (FAILED(ID3D11Device_QueryInterface(xd11.device, &IID_ID3D11Debug, 
                                           (void**)&xd11.debug)))
        exit(1);
    
    ID3D11InfoQueue* infoQueue;
    ID3D11Device_QueryInterface(xd11.device, &IID_ID3D11InfoQueue, (void**)&infoQueue);
    ID3D11InfoQueue_SetBreakOnSeverity(infoQueue, D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
    ID3D11InfoQueue_Release(infoQueue);
    
    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);
    
    xd11.running = true;
}

void xd11_update(void)
{
    // Measure fps
    LARGE_INTEGER counter = xwallclock();
    
    // Calculate dt
    xd11.dt = xseconds(xd11.lastCounter, counter);
    if (xd11.dt > 1000)
        xd11.dt = 0;
    
    // Save counter
    xd11.lastCounter = counter;
    
    xd11_swap_chain_resize();
    
    IDXGISwapChain_Present(xd11.swapChain, 1, 0);
}

void xd11_shutdown(void)
{
    xmemcheck();
    
    if (xd11.debug) 
        ID3D11Debug_Release(xd11.debug);
    
    if (xd11.swapChain) 
        IDXGISwapChain_Release(xd11.swapChain);
    
    if (xd11.deviceContext) 
        ID3D11DeviceContext_Release(xd11.deviceContext);
    
    if (xd11.device) 
        ID3D11Device_Release(xd11.device);
}

#endif