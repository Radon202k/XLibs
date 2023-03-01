#ifndef XLIB_RENDER
#define XLIB_RENDER

/* =========================================================================
   DEPENDENCIES
   ========================================================================= */

#include "xbase.h"
#include "xmemory.h"
#include "xmath.h"
#include "xarray.h"
#include "xstack.h"
#include "xlist.h"
#include "xtable.h"
#include "xwindows.h"

/* =========================================================================
   DATA TYPES
   ========================================================================= */

typedef struct XRender  XRender;
typedef struct XSprite  XSprite;
typedef struct XFont    XFont;

typedef struct
{
    WNDPROC wndproc;
    u32 winClassStyle;
    u32 winClassStyleEx;
    wchar_t* winTitle;
    v4f clearColor;
    v2f winPos;
    v2f winDim;
    bool topDown;
    s32 spriteAtlasSize;
    s32 maxSimulSprites;
    s32 maxSimulLines;
    s32 glyphMakerSize;
} XRendConfig;

/* =========================================================================
   MAIN FUNCTIONS
   ========================================================================= */

void xrendinit    (XRendConfig config);
void xrendfini    (void);
void xrendupdate  (void);
void xrendresized (void);

v2f  xdisplaysize (void);

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
Stack_T *xbatch       (s32 size);
u8      *xatlasbytes  (void);
void     xatlasupdate (u8 *data);

/* =========================================================================
   FONTS / GLYPHS
   ========================================================================= */

XFont   xfont       (wchar_t *fileName, wchar_t *fontName, int heightPoints);
void    xfontfree   (XFont font);
s32     xfontheight (s32 pointHeight);
XSprite xglyphsprite(XFont font, wchar_t *c, rect2f *tightBounds, s32 *tightDescent);

/* =========================================================================
   DRAWING / RENDERING UTILITIES
   ========================================================================= */

void xline      (v2f a, v2f b, v4f color, f32 sort);
void xlinerect  (v2f pos, v2f dim, v4f col, f32 sort);
void xarrow     (Stack_T *group, v2f a, v2f b, v4f col, XSprite head, v2f size, f32 sort);
void xsprite    (Stack_T *group, XSprite s, v2f pos, v2f dim, v4f col, v2f pivot, f32 rot, f32 sort);
f32  xglyph     (Stack_T *group, XFont f, u32 unicode, v2f pos, v4f c, v2f pivot, f32 rot, f32 sort);
f32  xstring    (Stack_T *group, XFont f, wchar_t *string, v2f pos, v4f c, v2f pivot, f32 rot, f32 sort, bool fixedwidth);

v2f  xglyphsize (XFont font, u32 unicode);
v2f  xstringsize(XFont font, wchar_t *s);

/* =========================================================================
   BASIC TYPES
   ========================================================================= */

bool xdraggedhandle(v2f p, f32 maxDist, void *address, bool *hover, v2f *delta);
void xpathabs      (wchar_t *dest, u32 destSize, wchar_t *fileName);
void xpathabsascii (char *dest, u32 destSize, char *fileName);

/* End of Interface */






/* Intended white space */







/* Implementation */

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

typedef struct 
{
    v4f color;
    v2f uv;
    v3f pos;
} XVertex3D;

typedef struct
{
    v4f color;
    v3f pos;
} XLineVertex3D;

typedef struct
{
    mat4f WVP;
} XVertexCBuffer;

typedef struct
{ 
    f32 rot, sort;
    v2f pos, size, pivot;
    v4f col;
    rect2f uv;
} XSpriteCommand;

typedef struct
{
    v2f a, b;
    v4f col;
    f32 sort;
} XLineCommand;

typedef struct
{
    s32 size, bottom;
    v2i at;
    u8 *bytes;
} XSpriteAtlas;

struct XRender {
    s32     sgi, sas, mss, msl, gms, abi, flc;
    bool    run, td;
    v2f     bbs, wp, wd;
    DWORD   wexs, wcs;
    wchar_t wt[256];
    f32     dt;
    v4f     cc;
    u64     pf;
    HDC     dc;
    HWND    wh;
    LARGE_INTEGER lc;
    
    XVertexCBuffer vcbd;
    XSpriteAtlas sa;
    
    Stack_T lg, sgs[32];
    
    IDXGISwapChain *swc;
    
    ID3D11Device *dvc;
    ID3D11DeviceContext *ctx;
    ID3D11Debug *dbg;
    ID3D11Texture2D *bbt, *dsbt, *sat;
    ID3D11RenderTargetView *rtv;
    ID3D11DepthStencilView *dsv;
    ID3D11DepthStencilState *dss;
    ID3D11BlendState *bls; 
    ID3D11RasterizerState *rs;
    ID3D11SamplerState *ss;
    ID3D11VertexShader *svs, *lvs;
    ID3D11PixelShader *sps, *lps;
    ID3D11InputLayout *sil, *lil;
    ID3D11Buffer *svb, *lvb, *vcb;
    ID3D11ShaderResourceView *asrv; 
    
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    D3D11_TEXTURE2D_DESC dsbd; 
    D3D_FEATURE_LEVEL fl[8];
    
    WAVEFORMATEX wf;
    IDirectSound8 *dsound;
    IDirectSoundBuffer *abs[32];
};

/*  
    wt  : window title
    run : running
    td  : top down
    bbs : back buffer size
    dt  : delta time
    sgi : sprite groups index
    sas : sprite atlas size
    mss : max simultaneous sprites
    msl : max simultaneous lines
    gms : glyph maker size
    abi : audio buffer index
    wp  : window position
    wd  : window dimension
    dc  : device context
    ca  : cursor arrow
    wexs: window class ex style
    wcs : window class style
    wh  : window handle
    dvc : device
    ctx : context
    swc : swap chain
    dbg : debugger
    bbt : back buffer texture
    dsbt: depth stencil buf tex
    sat : sprite atlas tex
    rtv : render target view
    dsv : depth stencil view
    dss : depth stencil state
    dsvd: depth stencil view desc
    dsbd: depth stencil buffer description
    bls : blend state
    rs  : rasterizer state
    ss  : sampler state
    svs : sprites vertex shader
    lvs : lines vertex shader
    sps : sprites pixel shader
    lps : lines pixel shader
    sil : sprites input layout
    lil : lines input layout
    svb : sprites vertex buffer
    lvb : lines vertex buffer
    lg  : line group
    sgs : sprite groups
    vcb : vertex constant buffer
    vcbd: vertex constant buffer data
    asrv: atlas shader resource view
    sa  : sprite atlas
    wf  : wave format
    abs : audio buffers
    pf  : perf frequency
    cc  : clear color
    fl  : feature levels
    flc : feature level count
*/

/* =========================================================================
   GLOBALS
   ========================================================================= */

XRender xrend;

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
        rowDst += 4*xrend.sa.size;
    }
}

XSprite xspritebytes(u8 *b, v2i dim)
{
    s32 m;
    u8 *dst, *src;
    XSprite r;
    XSpriteAtlas *a;
    
    m = 1;
    a = &xrend.sa;
    
    if ((a->at.x + dim.x + m) > a->size)
        a->at = ini2i(0, a->bottom);
    
    if (m+a->bottom < (a->at.y + dim.y))
        a->bottom = a->at.y + dim.y + m;
    
    assert(a->bottom <= a->size);
    
    dst = xrend.sa.bytes;
    src = b;
    blit_simple_unchecked(dst, xrend.sa.size, src, a->at, dim);
    
    r.uv.min = ini2f(
                     a->at.x / (f32)a->size,
                     (xrend.td ? (a->at.y + dim.y) : a->at.y) / (f32)a->size
                     );
    
    r.uv.max = ini2f(
                     (a->at.x + dim.x) / (f32)a->size, 
                     (xrend.td ? a->at.y : (a->at.y + dim.y)) / (f32)a->size
                     );
    
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
    return xrend.sa.bytes;
}

void xatlasupdate(u8 *updateddata)
{
    D3D11_MAPPED_SUBRESOURCE mr;
    ID3D11DeviceContext_Map(xrend.ctx, (ID3D11Resource *)xrend.sat, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
    memcpy(mr.pData, updateddata, xrend.sa.size*xrend.sa.size*4);
    ID3D11DeviceContext_Unmap(xrend.ctx, (ID3D11Resource *)xrend.sat, 0);
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
    XFont r;
    s32 tDescent, tW, tH, temp, maxGlyphW, maxGlyphH, maxDescent, i;
    rect2f tBounds;
    wchar_t c[2];
    u32 *k;
    XSprite *v;
    
    memset(&r, 0, sizeof(r));
    temp = AddFontResourceW(path);
    assert(temp == 1);
    
    r.handle = CreateFontW(xfontheight(heightpoints), 
                           0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET,
                           OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                           DEFAULT_PITCH, name);
    assert(r.handle && r.handle != INVALID_HANDLE_VALUE);
    
    BITMAPINFO info = xbmpinfo(xrend.gms, -xrend.gms);
    r.bitmap = CreateDIBSection(xrend.dc, &info, DIB_RGB_COLORS, &r.bytes, 0, 0);
    assert(xrend.gms>0);
    memset(r.bytes, 0, xrend.gms*xrend.gms*4);
    
    SelectObject(xrend.dc, r.bitmap);
    SelectObject(xrend.dc, r.handle);
    SetBkColor(xrend.dc, RGB(0,0,0));
    SetTextColor(xrend.dc, RGB(255,255,255));
    
    TEXTMETRICW metrics = {0};
    GetTextMetricsW(xrend.dc, &metrics);
    r.metrics = metrics;
    
    r.lineadvance = (f32)metrics.tmHeight - metrics.tmInternalLeading;
    r.charwidth = (f32)metrics.tmAveCharWidth;
    r.glyphs = Table_new(512, cmp_glyph_unicodes, hash_unicode);
    maxGlyphW = maxGlyphH=maxDescent=-10000;
    for (i=32; i<=126; ++i)
    {
        if ((wchar_t)i == L'S')
        {
            int y = 3;
        }
        c[0]=(wchar_t)i; c[1]='\0';
        
        k = xalloc(sizeof *k);
        *k = (u32)i;
        
        v = xalloc(sizeof *v);
        *v = xglyphsprite(r, c, &tBounds, &tDescent);
        if (tBounds.max.x!=0 && tBounds.max.y!=0)
        {
            tW=(s32)(tBounds.max.x-tBounds.min.x);
            tH=(s32)(tBounds.max.y-tBounds.min.y);
            if (maxGlyphW<tW) maxGlyphW=tW;
            if (maxGlyphH<tH) maxGlyphH=tH;
            if (maxDescent<tDescent) maxDescent=tDescent;
        }
        
        Table_set(r.glyphs, k, v);
    }
    
    r.lineadvance = (f32)maxGlyphH;
    r.maxdescent = (f32)maxDescent;
    xstrcpy(r.path, MAX_PATH, path);
    
    return r;
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
    GetTextExtentPoint32W(xrend.dc, c, 1, &size);
    al=ini2f(0,0);
    d=ini2fs(size.cx,size.cy);
    charsz=(s32)wcslen(c);
    bounds = inir2f(0,0, d.x,d.y);
    tBounds->min.x=tBounds->min.y=1000000;
    tBounds->max.x=tBounds->max.y=-1000000;
    
    TextOutW(xrend.dc, ps,0, c, charsz);
    
    bool foundTBox = false;
    for (j=0; j<xrend.gms; ++j)
    {
        for (i=0; i<xrend.gms; ++i)
        {
            px = (u32 *)((u8 *)font.bytes + j*xrend.gms*4 + i*4);
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
        srcRow = ((u8 *)font.bytes + (s32)tBounds->min.y*xrend.gms*4 + (s32)tBounds->min.x*4);
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
            srcRow += 4*xrend.gms;
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
                        GetDeviceCaps(xrend.dc, LOGPIXELSY), 
                        GetDeviceCaps(xrend.dc, LOGPIXELSX));
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

Stack_T *xbatch(s32 sz)
{
    Stack_T *r;
    
    assert(narray(xrend.sgs)>xrend.sgi);
    
    r = xrend.sgs + xrend.sgi++;
    *r = Stack_new(sz, sizeof(XSpriteCommand));
    
    return r;
}

void xpush_line_command(v2f a, v2f b, v4f color, f32 sort)
{
    XLineCommand c = 
    {
        a, b, color, sort,
    };
    Stack_push(&xrend.lg, &c);
}

void xpush_rect_command(Stack_T *group, v2f pos, v2f dim, rect2f uvs, 
                        v4f color, v2f pivot, f32 rot, f32 sort)
{
    XSpriteCommand c = 
    {
        rot, sort,
        pos, dim, pivot,
        color, uvs
    };
    Stack_push(group, &c);
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

void xarrow2d(Stack_T *g, v2f a, v2f b, v4f c, XSprite head, v2f headsize, f32 s)
{
    v2f dir;
    
    dir=nrm2f(sub2f(b,a));
    xline(a, b, c, s);
    f32 arrowAngle = atan2f(dir.y,dir.x);
    xsprite(g, head, b, headsize, c, ini2f(1,.5f), degf(arrowAngle), s);
}

void xlinerect(v2f p, v2f wh, v4f cl, f32 st)
{
    xpush_line_command(p, ini2f(p.x+wh.x, p.y), cl, st); // bottom
    xpush_line_command(ini2f(p.x,p.y+wh.y), ini2f(p.x+wh.x, p.y+wh.y), cl, st); // top
    xpush_line_command(p, ini2f(p.x,p.y+wh.y+1), cl, st); // left
    xpush_line_command(ini2f(p.x+wh.x,p.y), ini2f(p.x+wh.x,p.y+wh.y), cl, st); // right
}

void xsprite(Stack_T *g, XSprite sp, v2f p, v2f s, v4f cl, v2f pv, f32 r, f32 st)
{
    xpush_rect_command(g, p, s, sp.uv, cl, pv, r, st);
}

f32 xglyph(Stack_T *g, XFont f, u32 u, v2f p, v4f c, v2f pv, f32 r, f32 s)
{
    XSprite *sp = (XSprite *)Table_get(f.glyphs, &u);
    if (sp) {
        xsprite(g, *sp, p, sp->size, c, pv, r, s);
        return sp->size.x;
    }
    return 0;
}

f32 xstring(Stack_T *g, XFont f, wchar_t *s, v2f p, v4f c, v2f pv, f32 r, f32 st, bool fw)
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
    ID3D11Device_CreateVertexShader(xrend.dvc,
                                    ID3D10Blob_GetBufferPointer(compiledVS),
                                    ID3D10Blob_GetBufferSize(compiledVS),
                                    NULL, &xrend.svs);
    
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
    ID3D11Device_CreatePixelShader(xrend.dvc,
                                   ID3D10Blob_GetBufferPointer(compiledPS),
                                   ID3D10Blob_GetBufferSize(compiledPS),
                                   NULL, &xrend.sps);
    
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
    
    if (FAILED(ID3D11Device_CreateInputLayout(xrend.dvc, ied, narray(ied),
                                              vsPointer, vsSize, &xrend.sil))) {
        exit(1);
    }
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
    ID3D11Device_CreateVertexShader(xrend.dvc,
                                    ID3D10Blob_GetBufferPointer(compiledVS),
                                    ID3D10Blob_GetBufferSize(compiledVS),
                                    NULL, &xrend.lvs);
    
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
    ID3D11Device_CreatePixelShader(xrend.dvc,
                                   ID3D10Blob_GetBufferPointer(compiledPS),
                                   ID3D10Blob_GetBufferSize(compiledPS),
                                   NULL, &xrend.lps);
    
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
    
    if (FAILED(ID3D11Device_CreateInputLayout(xrend.dvc, ied, narray(ied),
                                              vsPointer, vsSize, &xrend.lil))) {
        exit(1);
    }
}

void create_sprites_vertex_buffer()
{
    u32 sz;
    
    assert(xrend.mss>0);
    
    sz = xrend.mss*6*sizeof(XVertex3D);
    D3D11_BUFFER_DESC d = { sz, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE, 0, sizeof(XVertex3D), };
    if (FAILED(ID3D11Device_CreateBuffer(xrend.dvc, &d, 0, &xrend.svb))) {
        exit(1);
    }
}

void create_lines_vertex_buffer()
{
    u32 m, sz;
    
    m = 10000;
    sz = m*2*sizeof(XLineVertex3D);
    
    D3D11_BUFFER_DESC d = { sz, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE, 0, sizeof(XLineVertex3D), };
    
    if (FAILED(ID3D11Device_CreateBuffer(xrend.dvc, &d, 0, &xrend.lvb))) {
        exit(1);
    }
}

void free_sprite_groups()
{
    s32 i;
    
    for (i=0; i<xrend.sgi; ++i)
        Stack_free(xrend.sgs + i);
}

void reset_sprite_groups()
{
    s32 i;
    
    for (i = 0; i < xrend.sgi; ++i)
        xrend.sgs[i].top = 0;
}

u32 produce_vertices_from_sprite_groups()
{
    f32 rotRad, pvx, pvy;
    v2f p,pa,pb,pc,pd,xaxis,yaxis;
    s32 i, j, vi, vc;
    XVertex3D *v;
    Stack_T* sg;
    XSpriteCommand* cmd;
    D3D11_MAPPED_SUBRESOURCE mr;
    
    for (i=0, vc=0; i<xrend.sgi; ++i)
        vc+=6*(xrend.sgs[i].top);
    
    if (vc>0) {
        v = xnalloc(vc, XVertex3D);
        vi = 0;
        for (i=0; i<xrend.sgi; ++i)
        {
            sg = xrend.sgs + i;
            if (sg->top>0)
            {
                for (j=0; j<sg->top; ++j)
                {
                    cmd = Stack_get(*sg, j);
                    
                    rotRad = -radf(cmd->rot);
                    
                    pvx=cmd->pivot.x;
                    pvy=cmd->pivot.y;
                    
                    xaxis=mul2f(cmd->size.x, ini2f(cosf(rotRad),-sinf(rotRad)));
                    yaxis=mul2f(cmd->size.y, ini2f(sinf(rotRad),cosf(rotRad)));
                    
                    p=cmd->pos;
                    pa=sub2f(sub2f(p, mul2f(  pvx, xaxis)), mul2f(  pvy, yaxis));
                    pb=sub2f(sub2f(p, mul2f(pvx-1, xaxis)), mul2f(  pvy, yaxis));
                    pc=sub2f(sub2f(p, mul2f(pvx-1, xaxis)), mul2f(pvy-1, yaxis));
                    pd=sub2f(sub2f(p, mul2f(  pvx, xaxis)), mul2f(pvy-1, yaxis));
                    
                    XVertex3D a =
                    {
                        pa.x, pa.y, cmd->sort,
                        cmd->uv.min.x, cmd->uv.min.y,
                        cmd->col.r, cmd->col.g, cmd->col.b, cmd->col.a
                    };
                    
                    XVertex3D b =
                    {
                        pb.x, pb.y, cmd->sort,
                        cmd->uv.max.x, cmd->uv.min.y,
                        cmd->col.r, cmd->col.g, cmd->col.b, cmd->col.a
                    };
                    
                    XVertex3D c =
                    {
                        pc.x, pc.y, cmd->sort,
                        cmd->uv.max.x, cmd->uv.max.y,
                        cmd->col.r, cmd->col.g, cmd->col.b, cmd->col.a
                    };
                    
                    XVertex3D d =
                    {
                        pd.x, pd.y, cmd->sort,
                        cmd->uv.min.x, cmd->uv.max.y,
                        cmd->col.r, cmd->col.g, cmd->col.b, cmd->col.a
                    };
                    
                    v[vi++] = a;
                    v[vi++] = (xrend.td ? b : c);
                    v[vi++] = (xrend.td ? c : b);
                    v[vi++] = a;
                    v[vi++] = (xrend.td ? c : d);
                    v[vi++] = (xrend.td ? d : c);
                }
            }
        }
        
        ID3D11DeviceContext_Map(xrend.ctx, (ID3D11Resource *)xrend.svb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
        memcpy(mr.pData, v, vc*sizeof(XVertex3D)); 
        ID3D11DeviceContext_Unmap(xrend.ctx, (ID3D11Resource *)xrend.svb, 0);
        xfree(v);
    }
    
    reset_sprite_groups();
    
    return vc;
}

void free_line_group()
{
    Stack_free(&xrend.lg);
}

void reset_line_group()
{
    xrend.lg.top = 0;
}

u32 produce_vertices_from_line_group()
{
    s32 vc, vi, i;
    XLineVertex3D *v;
    XLineCommand *cmd;
    D3D11_MAPPED_SUBRESOURCE mr;
    
    vc = 0;
    vc += 2 * xrend.lg.top;
    
    if (vc>0) {
        v = xnalloc(vc, XLineVertex3D);
        vi = 0;
        
        for (i = 0; i < xrend.lg.top; ++i) {
            cmd = Stack_get(xrend.lg, i);
            
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
        
        ID3D11DeviceContext_Map(xrend.ctx, (ID3D11Resource *)xrend.lvb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
        memcpy(mr.pData, v, vc*sizeof(XLineVertex3D));
        ID3D11DeviceContext_Unmap(xrend.ctx, (ID3D11Resource *)xrend.lvb, 0);
        xfree(v);
        
        reset_line_group();
    }
    
    return vc;
}

void render_pass(D3D11_PRIMITIVE_TOPOLOGY tp, ID3D11InputLayout *il, u32 vc, ID3D11Buffer *vb, u32 vbst,
                 ID3D11VertexShader *vs, ID3D11PixelShader *ps)
{
    ID3D11DeviceContext_IASetPrimitiveTopology(xrend.ctx, tp);
    ID3D11DeviceContext_IASetInputLayout(xrend.ctx, il);
    ID3D11Buffer *vbs[1] = { vb };
    UINT vbss[1] = { vbst };
    UINT vbos[1] = { 0 };
    ID3D11DeviceContext_IASetVertexBuffers(xrend.ctx, 0, 1, vbs, vbss, vbos);
    ID3D11DeviceContext_VSSetShader(xrend.ctx, vs, NULL, 0);
    ID3D11DeviceContext_VSSetConstantBuffers(xrend.ctx, 0, 1, &xrend.vcb);
    ID3D11DeviceContext_PSSetShader(xrend.ctx, ps, NULL, 0);
    ID3D11DeviceContext_PSSetSamplers(xrend.ctx, 0, 1, &xrend.ss);
    ID3D11DeviceContext_PSSetShaderResources(xrend.ctx, 0, 1, &xrend.asrv);
    D3D11_VIEWPORT vps[1] =  { { 0, 0, xrend.bbs.x, xrend.bbs.y, 0.0f, 1.0f } };
    ID3D11DeviceContext_RSSetViewports(xrend.ctx, 1, vps);
    D3D11_RECT scs[1] = { { 0, 0, (LONG)xrend.bbs.x, (LONG)xrend.bbs.y } };
    ID3D11DeviceContext_RSSetScissorRects(xrend.ctx, 1, scs);
    ID3D11DeviceContext_RSSetState(xrend.ctx, xrend.rs);
    ID3D11DeviceContext_OMSetDepthStencilState(xrend.ctx, xrend.dss, 0);
    ID3D11DeviceContext_OMSetBlendState(xrend.ctx, xrend.bls, NULL, 0xffffffff);
    ID3D11DeviceContext_Draw(xrend.ctx, vc, 0);
}

function void
swap_chain_resize()
{
    RECT rect;
    if (!GetClientRect(xrend.wh, &rect)) {
        exit(1);
    }
    
    v2f bbs = {
        (f32)(rect.right - rect.left),
        (f32)(rect.bottom - rect.top),
    };
    
    if (((UINT)bbs.x != 0 && (UINT)bbs.y != 0) &&
        (((UINT)bbs.x != xrend.bbs.x) || ((UINT)bbs.y != xrend.bbs.y)))
    {
        xrend.bbs = bbs;
        
        ID3D11RenderTargetView_Release(xrend.rtv);
        ID3D11DepthStencilView_Release(xrend.dsv);
        ID3D11Texture2D_Release(xrend.bbt);
        ID3D11Texture2D_Release(xrend.dsbt);
        
        IDXGISwapChain_ResizeBuffers(xrend.swc, 2, (UINT)bbs.x,(UINT)bbs.y, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        
        if (FAILED(IDXGISwapChain_GetBuffer(xrend.swc, 0, &IID_ID3D11Texture2D, (void **)&xrend.bbt)))
            exit(1);
        
        if (FAILED(ID3D11Device_CreateRenderTargetView(xrend.dvc, (ID3D11Resource *)xrend.bbt, 0, &xrend.rtv)))
            exit(1);
        
        xrend.dsbd.Width = (UINT)xrend.bbs.x;
        xrend.dsbd.Height = (UINT)xrend.bbs.y;
        
        if (FAILED(ID3D11Device_CreateTexture2D(xrend.dvc, &xrend.dsbd, 0, &xrend.dsbt)))
            exit(1);
        
        if (FAILED(ID3D11Device_CreateDepthStencilView(xrend.dvc, (ID3D11Resource *)xrend.dsbt, 
                                                       &xrend.dsvd, &xrend.dsv)))
            exit(1);
    }
    
    xrendresized();
}

typedef struct
{
    char chunkID[4];
    u32 chunkSize;
    char format[4];
} WavRIFFHeader;

typedef struct
{
    char subChunkID[4];
    u32 subChunkSize;
    u16 audioFormat;
    u16 numChannels;
    u32 sampleRate;
    u32 byteRate;
    u16 blockAlign;
    u16 bitsPerSample;
} WavFMTChunk;

typedef struct
{
    char subChunkID[4];
    u32 subChunkSize;
} WavDataChunk;

typedef struct
{
    WavRIFFHeader riffHeader;
    WavFMTChunk fmtChunk;
    WavDataChunk dataChunk;
    
    wchar_t fileName[260];
    s16 *sampleData;
    u32 sampleCount;
} Sound;

function Sound
load_wav(wchar_t *filePath, wchar_t *fileName)
{
    Sound result = {0};
    
    // Open file
    FILE *file;
    _wfopen_s(&file, filePath, L"rb");
    
    // If the file exists
    if (file != 0)
    {
        // Read the RIFF
        fread(&result.riffHeader, sizeof(WavRIFFHeader), 1, file);
        
        // Read the FMT chunk
        fread(&result.fmtChunk, sizeof(WavFMTChunk), 1, file);
        
        // Read the Data chunk
        fread(&result.dataChunk, sizeof(WavDataChunk), 1, file);
        
        // Calculate sample count
        result.sampleCount = (result.dataChunk.subChunkSize
                              / (result.fmtChunk.numChannels *
                                 (result.fmtChunk.bitsPerSample/8)));
        
        // If the audio format is PCM
        if (result.fmtChunk.audioFormat == 1)
        {
            // Allocate space for audio bytes
            result.sampleData = xalloc(result.dataChunk.subChunkSize);
            if (result.sampleData != 0)
            {
                // Read the audio bytes
                fread(result.sampleData, result.dataChunk.subChunkSize, 1, file);
                
                // Copy the file name
                xstrcpy(result.fileName, 260, fileName);
            }
            
        }
        
        // Close the file handle
        fclose(file);
    }
    
    return result;
}

// Direct Sound 8
// Initialize DirectSound
function bool
dsound8_init(void)
{
    // Create DirectSound object
    if (FAILED(DirectSoundCreate8(0, &xrend.dsound, 0))) return false;
    
    // Set the wav format
    xrend.wf.wFormatTag = WAVE_FORMAT_PCM;
    xrend.wf.nChannels = 2;
    xrend.wf.nSamplesPerSec = 44100;
    xrend.wf.wBitsPerSample = 16;
    xrend.wf.nBlockAlign = (xrend.wf.nChannels*xrend.wf.wBitsPerSample)/8;
    xrend.wf.nAvgBytesPerSec = xrend.wf.nSamplesPerSec*xrend.wf.nBlockAlign;
    
    // Set cooperative level
    if (FAILED(IDirectSound8_SetCooperativeLevel(xrend.dsound, 
                                                 xrend.wh, 
                                                 DSSCL_PRIORITY)))
        return false;
    
    // Primary buffer is only used to set the format
    IDirectSoundBuffer *primaryBuffer;
    
    DSBUFFERDESC primaryBufferDesc = {0};
    primaryBufferDesc.dwSize = sizeof(primaryBufferDesc);
    primaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    
    if (FAILED(IDirectSound8_CreateSoundBuffer(xrend.dsound, 
                                               &primaryBufferDesc, 
                                               &primaryBuffer, 
                                               0)))
        return false;
    
    if (FAILED(IDirectSoundBuffer_SetFormat(primaryBuffer, &xrend.wf)))
        return false;
    
    return true;
}

IDirectSoundBuffer *dsound8_create_buffer(wchar_t *filePath, wchar_t *fileName)
{
    Sound sound = load_wav(filePath, fileName);
    
    IDirectSoundBuffer *result = 0;
    if (xrend.abi == narray(xrend.abs))
        return false;
    
    // The actual audio buffer description
    DSBUFFERDESC d = {0};
    d.dwSize = sizeof(d);
    d.dwFlags = 0;
    d.dwBufferBytes = sound.sampleCount*2*sizeof(s16);
    d.lpwfxFormat = &xrend.wf;
    
    // Create the actual audio buffer
    if (FAILED(IDirectSound8_CreateSoundBuffer(xrend.dsound,
                                               &d, 
                                               &xrend.abs[xrend.abi],
                                               0)))
        return false;
    
    result = xrend.abs[xrend.abi];
    
    xrend.abi++;
    
    return result;
}


void xrendinit(XRendConfig config)
{
    ID3D11InfoQueue* iq;
    DXGI_FORMAT dsbf, atf; // depth stencil buffer format, atlas tex format
    TRACKMOUSEEVENT tme;
    
    xrend.dc = CreateCompatibleDC(GetDC(0));
    
    WNDCLASSEXW wc = xwndclass(config.wndproc);
    if (RegisterClassExW(&wc) == 0) exit(1);
    
    xrend.wexs = config.winClassStyleEx;
    xrend.wcs = config.winClassStyle;
    xrend.wp = config.winPos;
    xrend.wd = config.winDim;
    xstrcpy(xrend.wt, 256, config.winTitle);
    xrend.td = config.topDown;
    xrend.sas = config.spriteAtlasSize;
    xrend.mss = config.maxSimulSprites;
    xrend.msl = config.maxSimulLines;
    xrend.gms = config.glyphMakerSize;
    xrend.cc = config.clearColor;
    
    xrend.bbs = xrend.wd;
    
    RECT size = { (u32)xrend.wp.x, (u32)xrend.wp.y,
        (u32)(xrend.wp.x + xrend.wd.x), (u32)(xrend.wp.y + xrend.wd.y) };
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
    xrend.wd = ini2f((f32)(size.right - size.left), (f32)(size.bottom - size.top));
    
    xrend.wh = CreateWindowExW(xrend.wexs, L"xwindow_class", xrend.wt, xrend.wcs,
                             (s32)xrend.wp.x, (s32)xrend.wp.y, (s32)xrend.wd.x, (s32)xrend.wd.y,
                             NULL, NULL, GetModuleHandle(0), NULL);
    if (xrend.wh == NULL)
        exit(1);
    
    // Request notification when the mouse leaves the non-client area
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_NONCLIENT | TME_LEAVE;
    tme.hwndTrack = xrend.wh;
    TrackMouseEvent(&tme);
    
    DXGI_MODE_DESC bbd = { (s32)xrend.bbs.x, (s32)xrend.bbs.y, xrational(60,1), DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_CENTERED, };
    DXGI_SAMPLE_DESC bbsd = { .Count = 1,  .Quality = 0, };
    DXGI_SWAP_CHAIN_DESC scd = { bbd, bbsd, DXGI_USAGE_RENDER_TARGET_OUTPUT,
        2, xrend.wh, true, DXGI_SWAP_EFFECT_FLIP_DISCARD, 0, };
    
    xfeatureleves(xrend.fl, &xrend.flc);
    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
                                             D3D11_CREATE_DEVICE_DEBUG, xrend.fl, xrend.flc, D3D11_SDK_VERSION, &scd, &xrend.swc,
                                             &xrend.dvc, NULL, &xrend.ctx)))
        exit(1);
    
    if (FAILED(ID3D11Device_QueryInterface(xrend.dvc, &IID_ID3D11Debug, (void**)&xrend.dbg)))
        exit(1);
    
    ID3D11Device_QueryInterface(xrend.dvc, &IID_ID3D11InfoQueue, (void**)&iq);
    ID3D11InfoQueue_SetBreakOnSeverity(iq, D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
    ID3D11InfoQueue_Release(iq);
    
    if (FAILED(IDXGISwapChain_GetBuffer(xrend.swc, 0, &IID_ID3D11Texture2D, (void**)&xrend.bbt)))
        exit(1);
    
    dsbf = DXGI_FORMAT_D24_UNORM_S8_UINT;
    D3D11_TEXTURE2D_DESC dsbtd = { (s32)xrend.bbs.x, (s32)xrend.bbs.y, 0, 1, dsbf, bbsd,
        D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL, 0, 0, };
    xrend.dsbd = dsbtd;
    
    if (FAILED(ID3D11Device_CreateTexture2D(xrend.dvc, &dsbtd, 0, &xrend.dsbt)))
        exit(1);
    
    xrend.sa.size = xrend.sas;
    xrend.sa.at.x = xrend.sa.at.y = xrend.sa.bottom = 0;
    
    atf = DXGI_FORMAT_R8G8B8A8_UNORM;
    D3D11_TEXTURE2D_DESC atd = { xrend.sa.size, xrend.sa.size, 1, 1, atf, {1,0},
        D3D11_USAGE_DYNAMIC, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, 0, };
    
    u32 ams = xrend.sa.size * xrend.sa.size * 4;
    xrend.sa.bytes = (u8*)xalloc(ams);
    
    LARGE_INTEGER pf;
    QueryPerformanceFrequency(&pf);
    xrend.pf = pf.QuadPart;
    
    dsound8_init();
    
    D3D11_SUBRESOURCE_DATA ad = { xrend.sa.bytes, (UINT)(xrend.sa.size * 4), 0 };
    if (FAILED(ID3D11Device_CreateTexture2D(xrend.dvc, &atd, &ad, &xrend.sat)))
        exit(1);
    
    if (FAILED(ID3D11Device_CreateRenderTargetView(xrend.dvc, (ID3D11Resource*)xrend.bbt, 0, &xrend.rtv)))
        exit(1);
    
    xrend.dsvd = xdsviewdesc(dsbf, D3D11_DSV_DIMENSION_TEXTURE2D);
    if (FAILED(ID3D11Device_CreateDepthStencilView(xrend.dvc, (ID3D11Resource*)xrend.dsbt, &xrend.dsvd, &xrend.dsv)))
        exit(1);
    
    D3D11_SHADER_RESOURCE_VIEW_DESC asrvd = xshadresview(atf, D3D_SRV_DIMENSION_TEXTURE2D);
    if (FAILED(ID3D11Device_CreateShaderResourceView(xrend.dvc, (ID3D11Resource*)xrend.sat, &asrvd, &xrend.asrv)))
        exit(1);
    
    D3D11_DEPTH_STENCIL_DESC dsd = xdepthstencildesc();
    
    if (FAILED(ID3D11Device_CreateDepthStencilState(xrend.dvc, &dsd, &xrend.dss)))
        exit(1);
    
    D3D11_BLEND_DESC bd = xblenddesc();
    
    if (FAILED(ID3D11Device_CreateBlendState(xrend.dvc, &bd, &xrend.bls)))
        exit(1);
    
    D3D11_RASTERIZER_DESC rd = xrasterstate();
    if (FAILED(ID3D11Device_CreateRasterizerState(xrend.dvc, &rd, &xrend.rs)))
        exit(1);
    
    D3D11_SAMPLER_DESC sd = xsamplerdesc();
    if (FAILED(ID3D11Device_CreateSamplerState(xrend.dvc, &sd, &xrend.ss)))
        exit(1);
    
    create_sprites_shaders();
    create_lines_shaders();
    
    create_sprites_vertex_buffer();
    create_lines_vertex_buffer();
    
    D3D11_BUFFER_DESC vcbd = xcbufferdesc(sizeof(XVertexCBuffer));
    if (FAILED(ID3D11Device_CreateBuffer(xrend.dvc, &vcbd, 0, &xrend.vcb)))
        exit(1);
    
    xrend.lg = Stack_new(256, sizeof(XLineCommand));
    
    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);
    
    xrend.run = true;
}

void xrendupdate(void)
{
    // Measure fps
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    
    // Calculate dt
    xrend.dt = ((f32)(counter.QuadPart - xrend.lc.QuadPart) / (f32)xrend.pf);
    
    // Save counter
    xrend.lc = counter;
    
    swap_chain_resize();
    
    u32 svc = produce_vertices_from_sprite_groups();
    u32 lvc = produce_vertices_from_line_group();
    
    f32 scaleX = 2.0f / xrend.bbs.x;
    f32 scaleY = (xrend.td ? -2.0f : 2.0f) / xrend.bbs.y;
    mat4f wvpMatrix =
    {
        scaleX, 0, 0, -1,
        0, scaleY, 0, (xrend.td ? 1.f : -1.f),
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    xrend.vcbd.WVP = wvpMatrix;
    ID3D11DeviceContext_UpdateSubresource(xrend.ctx, (ID3D11Resource*)xrend.vcb, 0, NULL, &xrend.vcbd, 0, 0);
    
    ID3D11RenderTargetView* views[1] = { xrend.rtv };
    ID3D11DeviceContext_OMSetRenderTargets(xrend.ctx, 1, views, xrend.dsv);
    ID3D11DeviceContext_ClearRenderTargetView(xrend.ctx, xrend.rtv, xrend.cc.e);
    ID3D11DeviceContext_ClearDepthStencilView(xrend.ctx, xrend.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
    
    // Lines
    render_pass(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, xrend.lil, lvc, xrend.lvb, sizeof(XLineVertex3D), xrend.lvs, xrend.lps);
    // Sprites
    render_pass(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, xrend.sil, svc, xrend.svb, sizeof(XVertex3D), xrend.svs, xrend.sps);
    
    IDXGISwapChain_Present(xrend.swc, 1, 0);
}

void xrendfini(void)
{
    xfree(xrend.sa.bytes);
    
    free_sprite_groups();
    free_line_group();
    
    xmemcheck();
    
    if (xrend.asrv) ID3D11ShaderResourceView_Release(xrend.asrv);
    if (xrend.vcb) ID3D11Buffer_Release(xrend.vcb);
    if (xrend.lvb) ID3D11Buffer_Release(xrend.lvb);
    if (xrend.svb) ID3D11Buffer_Release(xrend.svb);
    if (xrend.lil) ID3D11InputLayout_Release(xrend.lil);
    if (xrend.sil) ID3D11InputLayout_Release(xrend.sil);
    if (xrend.lps) ID3D11PixelShader_Release(xrend.lps);
    if (xrend.sps) ID3D11PixelShader_Release(xrend.sps);
    if (xrend.lvs) ID3D11VertexShader_Release(xrend.lvs);
    if (xrend.svs) ID3D11VertexShader_Release(xrend.svs);
    if (xrend.ss) ID3D11SamplerState_Release(xrend.ss);
    if (xrend.rs) ID3D11RasterizerState_Release(xrend.rs);
    if (xrend.bls) ID3D11BlendState_Release(xrend.bls);
    if (xrend.dss) ID3D11DepthStencilState_Release(xrend.dss);
    if (xrend.dsv) ID3D11DepthStencilView_Release(xrend.dsv);
    if (xrend.rtv) ID3D11RenderTargetView_Release(xrend.rtv);
    if (xrend.sat) ID3D11Texture2D_Release(xrend.sat);
    if (xrend.dsbt) ID3D11Texture2D_Release(xrend.dsbt);
    if (xrend.bbt) ID3D11Texture2D_Release(xrend.bbt);
    if (xrend.dbg) ID3D11Debug_Release(xrend.dbg);
    if (xrend.swc) IDXGISwapChain_Release(xrend.swc);
    if (xrend.ctx) ID3D11DeviceContext_Release(xrend.ctx);
    if (xrend.dvc) ID3D11Device_Release(xrend.dvc);
}

#undef T

#endif