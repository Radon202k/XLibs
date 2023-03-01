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
#include "stb_image.h"

/* =========================================================================
   DATA TYPES
   ========================================================================= */

typedef struct XContext XContext;
typedef struct XSprite  XSprite;
typedef struct XFont    XFont;

/* =========================================================================
   USER DEFINED FUNCTIONS
   ========================================================================= */

void xinitialize(void);
void xshutdown  (void);
void xconfig    (void);
void xupdate    (void);
void xresized   (void);

/* =========================================================================
   CONFIGURATION / HARDWARE SPECIFICATIONS
   ========================================================================= */

void xwinstyle(u32 style, u32 exstyle);
void xwindow  (s32 x, s32 y, s32 w, s32 h, wchar_t *title);
void xtopdown (bool topdown);
void xclear   (v4f color);
v2f  xdisplay (void);

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

XSprite  xspritepng(wchar_t *path, bool premulalpha);
u8      *xpng(wchar_t *filePath, v2i *dim, bool premulAlpha);
XSprite  xspritebytes(u8 *bytes, v2i dim);
Stack_T *xbatch(s32 size);
u8      *xatlasbytes();
void     xatlasupdate(u8 *data);

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

typedef struct
{
    bool down, pressed, released;
} XKey;

typedef union
{
    XKey all[35];
    struct {
        XKey up, left, down, right,
             backspace, alt, control, space, f1,
             a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;
    };
} XKeys;

typedef struct
{
    bool  dragging;
    f32   wheel; 
    v2f   pos, dragLastP;
    void *draggingAddress;
    XKey   left, right;
} XMouse;

struct XContext {
    s32     sgi, sas, mss, msl, gms, abi, flc;
    bool    run, mow, ice, td;
    v2f     bbs, wp, wd;
    DWORD   wexs, wcs;
    wchar_t ic, wt[256];
    f32     dt;
    v4f     cc;
    u64     pf;
    HDC     dc;
    HCURSOR ca;
    HWND    wh;
    
    XVertexCBuffer vcbd;
    XSpriteAtlas sa;
    XKeys key;
    XMouse mouse;

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

/*  ic  : input char
    wt  : window title
    run : running
    mow : mouse outside window
    ice : input char entered
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

XContext ctx;

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

bool xdraggedhandle(v2f p, f32 maxdist, void *address, bool *hover, v2f *delta)
{
    bool dragged = false;
    
    if (len2f(sub2f(p, ctx.mouse.pos)) < maxdist*maxdist)
    {
        *hover = true;
        
        if (ctx.mouse.dragging)
        {
            ctx.mouse.dragLastP = ctx.mouse.pos;
            ctx.mouse.draggingAddress = address;
        }
    }
    else
    {
        *hover = false;
    }
    
    if (ctx.mouse.dragging && ctx.mouse.draggingAddress == address)
    {
        v2f deltaP = sub2f(ctx.mouse.pos, ctx.mouse.dragLastP);
        ctx.mouse.dragLastP = ctx.mouse.pos;
        *delta = deltaP;
        dragged = true;
    }
    
    return dragged;
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
        rowDst += 4*ctx.sa.size;
    }
}

XSprite xspritebytes(u8 *b, v2i dim)
{
    s32 m;
    u8 *dst, *src;
    XSprite r;
    XSpriteAtlas *a;
    
    m = 1;
    a = &ctx.sa;

    if ((a->at.x + dim.x + m) > a->size)
        a->at = ini2i(0, a->bottom);
    
    if (m+a->bottom < (a->at.y + dim.y))
        a->bottom = a->at.y + dim.y + m;
    
    assert(a->bottom <= a->size);
    
    dst = ctx.sa.bytes;
    src = b;
    blit_simple_unchecked(dst, ctx.sa.size, src, a->at, dim);
    
    r.uv.min = ini2f((f32)a->at.x/(f32)a->size, (f32)a->at.y/(f32)a->size);
    r.uv.max = ini2f(((f32)a->at.x+dim.x)/(f32)a->size, ((f32)a->at.y+dim.y)/(f32)a->size);
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
    return ctx.sa.bytes;
}

void xatlasupdate(u8 *updateddata)
{
    D3D11_MAPPED_SUBRESOURCE mr;
    ID3D11DeviceContext_Map(ctx.ctx, (ID3D11Resource *)ctx.sat, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
    memcpy(mr.pData, updateddata, ctx.sa.size*ctx.sa.size*4);
    ID3D11DeviceContext_Unmap(ctx.ctx, (ID3D11Resource *)ctx.sat, 0);
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
    
    BITMAPINFO info = xbmpinfo(ctx.gms, -ctx.gms);
    r.bitmap = CreateDIBSection(ctx.dc, &info, DIB_RGB_COLORS, &r.bytes, 0, 0);
    assert(ctx.gms>0);
    memset(r.bytes, 0, ctx.gms*ctx.gms*4);
    
    SelectObject(ctx.dc, r.bitmap);
    SelectObject(ctx.dc, r.handle);
    SetBkColor(ctx.dc, RGB(0,0,0));
    SetTextColor(ctx.dc, RGB(255,255,255));
    
    TEXTMETRICW metrics = {0};
    GetTextMetricsW(ctx.dc, &metrics);
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
    GetTextExtentPoint32W(ctx.dc, c, 1, &size);
    al=ini2f(0,0);
    d=ini2fs(size.cx,size.cy);
    charsz=(s32)wcslen(c);
    bounds = inir2f(0,0, d.x,d.y);
    tBounds->min.x=tBounds->min.y=1000000;
    tBounds->max.x=tBounds->max.y=-1000000;
    
    TextOutW(ctx.dc, ps,0, c, charsz);
    
    bool foundTBox = false;
    for (j=0; j<ctx.gms; ++j)
    {
        for (i=0; i<ctx.gms; ++i)
        {
            px = (u32 *)((u8 *)font.bytes + j*ctx.gms*4 + i*4);
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
        srcRow = ((u8 *)font.bytes + (s32)tBounds->min.y*ctx.gms*4 + (s32)tBounds->min.x*4);
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
            srcRow += 4*ctx.gms;
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
                        GetDeviceCaps(ctx.dc, LOGPIXELSY), 
                        GetDeviceCaps(ctx.dc, LOGPIXELSX));
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
    
    assert(narray(ctx.sgs)>ctx.sgi);
    
    r = ctx.sgs + ctx.sgi++;
    *r = Stack_new(sz, sizeof(XSpriteCommand));
    
    return r;
}

void xpush_line_command(v2f a, v2f b, v4f color, f32 sort)
{
    XLineCommand c = 
    {
        a, b, color, sort,
    };
    Stack_push(&ctx.lg, &c);
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

void xwindow(s32 x, s32 y, s32 w, s32 h, wchar_t *t)
{
    ctx.wp = ini2fs(x,y);
    ctx.wd = ini2fs(w,h);
}

void xwinstyle(u32 wcs, u32 wexs)
{
    ctx.wcs = wcs;
    ctx.wexs = wexs;
}

void xtopdown(bool td)
{
    ctx.td = td;
}

void xclear(v4f c)
{
    ctx.cc = c;
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

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message) {
#if 0
        case WM_MOUSELEAVE: {
            ctx.mow=true;
            Syn_mouseleave();
        } break;
        
        case WM_NCMOUSEMOVE: {
            ctx.mow=false;
            Syn_ncmousemove();
        }
#endif
        case WM_SETCURSOR: {
            SetCursor(ctx.ca);
        } break;
        
        case WM_NCHITTEST: {
            result = DefWindowProc(window, message, wParam, lParam);
            POINT point = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            ScreenToClient(window, &point);
            if (result == HTCLIENT && 
                point.x < ctx.bbs.x-50 &&
                point.y < 40) {
                return HTCAPTION;
            } else {
                return result;
            }
        };
        
        case WM_DESTROY:
        case WM_CLOSE:
        {
            ctx.run = false;
        } break;
        
        case WM_SIZE:
        {
            // TODO: Paint the window black or something
        } break;
        
        case WM_CHAR:
        {
            ctx.ic = (wchar_t)wParam;
            ctx.ice = true;
        } break;
        
        case WM_MOUSEWHEEL:
        {
            ctx.mouse.wheel = ((f32)GET_WHEEL_DELTA_WPARAM(wParam) / 
                (f32)WHEEL_DELTA);
        } break;
        
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            XKey *key = 0;
            
            if (wParam == VK_UP)              key = &ctx.key.up;
            else if (wParam == VK_LEFT)       key = &ctx.key.left;
            else if (wParam == VK_DOWN)       key = &ctx.key.down;
            else if (wParam == VK_RIGHT)      key = &ctx.key.right;
            else if (wParam == VK_BACK)       key = &ctx.key.backspace;
            else if (wParam == VK_MENU)       key = &ctx.key.alt;
            else if (wParam == VK_F1)         key = &ctx.key.f1;
            
            else if (wParam == 'A')           key = &ctx.key.a;
            else if (wParam == 'B')           key = &ctx.key.b;
            else if (wParam == 'C')           key = &ctx.key.c;
            else if (wParam == 'D')           key = &ctx.key.d;
            else if (wParam == 'E')           key = &ctx.key.e;
            else if (wParam == 'F')           key = &ctx.key.f;
            else if (wParam == 'G')           key = &ctx.key.g;
            else if (wParam == 'H')           key = &ctx.key.h;
            else if (wParam == 'I')           key = &ctx.key.i;
            else if (wParam == 'J')           key = &ctx.key.j;
            else if (wParam == 'K')           key = &ctx.key.k;
            else if (wParam == 'L')           key = &ctx.key.l;
            else if (wParam == 'M')           key = &ctx.key.m;
            else if (wParam == 'N')           key = &ctx.key.n;
            else if (wParam == 'O')           key = &ctx.key.o;
            else if (wParam == 'P')           key = &ctx.key.p;
            else if (wParam == 'Q')           key = &ctx.key.q;
            else if (wParam == 'R')           key = &ctx.key.r;
            else if (wParam == 'S')           key = &ctx.key.s;
            else if (wParam == 'T')           key = &ctx.key.t;
            else if (wParam == 'U')           key = &ctx.key.u;
            else if (wParam == 'V')           key = &ctx.key.v;
            else if (wParam == 'W')           key = &ctx.key.w;
            else if (wParam == 'X')           key = &ctx.key.x;
            else if (wParam == 'Y')           key = &ctx.key.y;
            else if (wParam == 'Z')           key = &ctx.key.z;
            
            else if (wParam == VK_CONTROL)    key = &ctx.key.control;
            else if (wParam == VK_SPACE)      key = &ctx.key.space;
            
            if (key)
            {
                key->down = (message == WM_KEYDOWN);
                key->pressed = (message == WM_KEYDOWN);
                key->released = (message == WM_KEYUP);
            }
            
        } break;
        
        case WM_LBUTTONDOWN:
        {
            ctx.mouse.left.down = true;
            ctx.mouse.left.pressed = true;
        } break;
        
        case WM_RBUTTONDOWN:
        {
            ctx.mouse.right.down = true;
            ctx.mouse.right.pressed = true;
        } break;
        
        case WM_LBUTTONUP:
        {
            ctx.mouse.left.down = false;
            ctx.mouse.left.released = true;
        } break;
        
        case WM_RBUTTONUP:
        {
            ctx.mouse.right.down = false;
            ctx.mouse.right.released = true;
        } break;
        
        // [W3] . Call DefWindowProcA for every message we don't handle
        default:
        {
            result = DefWindowProcW(window, message, wParam, lParam);
        } break;
    }
    return result;
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
    ID3D11Device_CreateVertexShader(ctx.dvc,
                                    ID3D10Blob_GetBufferPointer(compiledVS),
                                    ID3D10Blob_GetBufferSize(compiledVS),
                                    NULL, &ctx.svs);
    
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
    ID3D11Device_CreatePixelShader(ctx.dvc,
                                   ID3D10Blob_GetBufferPointer(compiledPS),
                                   ID3D10Blob_GetBufferSize(compiledPS),
                                   NULL, &ctx.sps);
    
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
    
    if (FAILED(ID3D11Device_CreateInputLayout(ctx.dvc, ied, narray(ied),
                                              vsPointer, vsSize, &ctx.sil))) {
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
    ID3D11Device_CreateVertexShader(ctx.dvc,
                                    ID3D10Blob_GetBufferPointer(compiledVS),
                                    ID3D10Blob_GetBufferSize(compiledVS),
                                    NULL, &ctx.lvs);
    
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
    ID3D11Device_CreatePixelShader(ctx.dvc,
                                   ID3D10Blob_GetBufferPointer(compiledPS),
                                   ID3D10Blob_GetBufferSize(compiledPS),
                                   NULL, &ctx.lps);
    
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
    
    if (FAILED(ID3D11Device_CreateInputLayout(ctx.dvc, ied, narray(ied),
                                              vsPointer, vsSize, &ctx.lil))) {
        exit(1);
    }
}

void create_sprites_vertex_buffer()
{
    u32 sz;
    
    assert(ctx.mss>0);
    
    sz = ctx.mss*6*sizeof(XVertex3D);
    D3D11_BUFFER_DESC d = { sz, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE, 0, sizeof(XVertex3D), };
    if (FAILED(ID3D11Device_CreateBuffer(ctx.dvc, &d, 0, &ctx.svb))) {
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
    
    if (FAILED(ID3D11Device_CreateBuffer(ctx.dvc, &d, 0, &ctx.lvb))) {
        exit(1);
    }
}

void free_sprite_groups()
{
    s32 i;
    
    for (i=0; i<ctx.sgi; ++i)
        Stack_free(ctx.sgs + i);
}

void reset_sprite_groups()
{
    s32 i;
    
    for (i = 0; i < ctx.sgi; ++i)
        ctx.sgs[i].top = 0;
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
    
    for (i=0, vc=0; i<ctx.sgi; ++i)
        vc+=6*(ctx.sgs[i].top);
    
    if (vc>0) {
        v = xnalloc(vc, XVertex3D);
        vi = 0;
        for (i=0; i<ctx.sgi; ++i)
        {
            sg = ctx.sgs + i;
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
                    v[vi++] = (ctx.td ? b : c);
                    v[vi++] = (ctx.td ? c : b);
                    v[vi++] = a;
                    v[vi++] = (ctx.td ? c : d);
                    v[vi++] = (ctx.td ? d : c);
                }
            }
        }
        
        ID3D11DeviceContext_Map(ctx.ctx, (ID3D11Resource *)ctx.svb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
        memcpy(mr.pData, v, vc*sizeof(XVertex3D)); 
        ID3D11DeviceContext_Unmap(ctx.ctx, (ID3D11Resource *)ctx.svb, 0);
        xfree(v);
    }
    
    reset_sprite_groups();
    
    return vc;
}

void free_line_group()
{
    Stack_free(&ctx.lg);
}

void reset_line_group()
{
    ctx.lg.top = 0;
}

u32 produce_vertices_from_line_group()
{
    s32 vc, vi, i;
    XLineVertex3D *v;
    XLineCommand *cmd;
    D3D11_MAPPED_SUBRESOURCE mr;
    
    vc = 0;
    vc += 2 * ctx.lg.top;
    
    if (vc>0) {
        v = xnalloc(vc, XLineVertex3D);
        vi = 0;
        
        for (i = 0; i < ctx.lg.top; ++i) {
            cmd = Stack_get(ctx.lg, i);
            
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
        
        ID3D11DeviceContext_Map(ctx.ctx, (ID3D11Resource *)ctx.lvb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
        memcpy(mr.pData, v, vc*sizeof(XLineVertex3D));
        ID3D11DeviceContext_Unmap(ctx.ctx, (ID3D11Resource *)ctx.lvb, 0);
        xfree(v);
        
        reset_line_group();
    }
    
    return vc;
}

void render_pass(D3D11_PRIMITIVE_TOPOLOGY tp, ID3D11InputLayout *il, u32 vc, ID3D11Buffer *vb, u32 vbst,
                 ID3D11VertexShader *vs, ID3D11PixelShader *ps)
{
    ID3D11DeviceContext_IASetPrimitiveTopology(ctx.ctx, tp);
    ID3D11DeviceContext_IASetInputLayout(ctx.ctx, il);
    ID3D11Buffer *vbs[1] = { vb };
    UINT vbss[1] = { vbst };
    UINT vbos[1] = { 0 };
    ID3D11DeviceContext_IASetVertexBuffers(ctx.ctx, 0, 1, vbs, vbss, vbos);
    ID3D11DeviceContext_VSSetShader(ctx.ctx, vs, NULL, 0);
    ID3D11DeviceContext_VSSetConstantBuffers(ctx.ctx, 0, 1, &ctx.vcb);
    ID3D11DeviceContext_PSSetShader(ctx.ctx, ps, NULL, 0);
    ID3D11DeviceContext_PSSetSamplers(ctx.ctx, 0, 1, &ctx.ss);
    ID3D11DeviceContext_PSSetShaderResources(ctx.ctx, 0, 1, &ctx.asrv);
    D3D11_VIEWPORT vps[1] =  { { 0, 0, ctx.bbs.x, ctx.bbs.y, 0.0f, 1.0f } };
    ID3D11DeviceContext_RSSetViewports(ctx.ctx, 1, vps);
    D3D11_RECT scs[1] = { { 0, 0, (LONG)ctx.bbs.x, (LONG)ctx.bbs.y } };
    ID3D11DeviceContext_RSSetScissorRects(ctx.ctx, 1, scs);
    ID3D11DeviceContext_RSSetState(ctx.ctx, ctx.rs);
    ID3D11DeviceContext_OMSetDepthStencilState(ctx.ctx, ctx.dss, 0);
    ID3D11DeviceContext_OMSetBlendState(ctx.ctx, ctx.bls, NULL, 0xffffffff);
    ID3D11DeviceContext_Draw(ctx.ctx, vc, 0);
}

function void
swap_chain_resize()
{
    RECT rect;
    if (!GetClientRect(ctx.wh, &rect)) {
        exit(1);
    }
    
    v2f bbs = {
        (f32)(rect.right - rect.left),
        (f32)(rect.bottom - rect.top),
    };
    
    if (((UINT)bbs.x != 0 && (UINT)bbs.y != 0) &&
        (((UINT)bbs.x != ctx.bbs.x) || ((UINT)bbs.y != ctx.bbs.y)))
    {
        ctx.bbs = bbs;
        
        ID3D11RenderTargetView_Release(ctx.rtv);
        ID3D11DepthStencilView_Release(ctx.dsv);
        ID3D11Texture2D_Release(ctx.bbt);
        ID3D11Texture2D_Release(ctx.dsbt);
        
        IDXGISwapChain_ResizeBuffers(ctx.swc, 2, (UINT)bbs.x,(UINT)bbs.y, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        
        if (FAILED(IDXGISwapChain_GetBuffer(ctx.swc, 0, &IID_ID3D11Texture2D, (void **)&ctx.bbt)))
            exit(1);
        
        if (FAILED(ID3D11Device_CreateRenderTargetView(ctx.dvc, (ID3D11Resource *)ctx.bbt, 0, &ctx.rtv)))
            exit(1);
        
        ctx.dsbd.Width = (UINT)ctx.bbs.x;
        ctx.dsbd.Height = (UINT)ctx.bbs.y;
        
        if (FAILED(ID3D11Device_CreateTexture2D(ctx.dvc, &ctx.dsbd, 0, &ctx.dsbt)))
            exit(1);
        
        if (FAILED(ID3D11Device_CreateDepthStencilView(ctx.dvc, (ID3D11Resource *)ctx.dsbt, 
                                                       &ctx.dsvd, &ctx.dsv)))
            exit(1);
    }
    
    xresized();
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
    if (FAILED(DirectSoundCreate8(0, &ctx.dsound, 0))) return false;
    
    // Set the wav format
    ctx.wf.wFormatTag = WAVE_FORMAT_PCM;
    ctx.wf.nChannels = 2;
    ctx.wf.nSamplesPerSec = 44100;
    ctx.wf.wBitsPerSample = 16;
    ctx.wf.nBlockAlign = (ctx.wf.nChannels*ctx.wf.wBitsPerSample)/8;
    ctx.wf.nAvgBytesPerSec = ctx.wf.nSamplesPerSec*ctx.wf.nBlockAlign;
    
    // Set cooperative level
    if (FAILED(IDirectSound8_SetCooperativeLevel(ctx.dsound, 
                                                 ctx.wh, 
                                                 DSSCL_PRIORITY)))
        return false;
    
    // Primary buffer is only used to set the format
    IDirectSoundBuffer *primaryBuffer;
    
    DSBUFFERDESC primaryBufferDesc = {0};
    primaryBufferDesc.dwSize = sizeof(primaryBufferDesc);
    primaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    
    if (FAILED(IDirectSound8_CreateSoundBuffer(ctx.dsound, 
                                               &primaryBufferDesc, 
                                               &primaryBuffer, 
                                               0)))
        return false;
    
    if (FAILED(IDirectSoundBuffer_SetFormat(primaryBuffer, &ctx.wf)))
        return false;
    
    return true;
}

IDirectSoundBuffer *dsound8_create_buffer(wchar_t *filePath, wchar_t *fileName)
{
    Sound sound = load_wav(filePath, fileName);
    
    IDirectSoundBuffer *result = 0;
    if (ctx.abi == narray(ctx.abs))
        return false;
    
    // The actual audio buffer description
    DSBUFFERDESC d = {0};
    d.dwSize = sizeof(d);
    d.dwFlags = 0;
    d.dwBufferBytes = sound.sampleCount*2*sizeof(s16);
    d.lpwfxFormat = &ctx.wf;
    
    // Create the actual audio buffer
    if (FAILED(IDirectSound8_CreateSoundBuffer(ctx.dsound,
                                               &d, 
                                               &ctx.abs[ctx.abi],
                                               0)))
        return false;
    
    result = ctx.abs[ctx.abi];
    
    ctx.abi++;
    
    return result;
}

/******************************************************************************
*** [ENTRYPOINT]
******************************************************************************/

// Here is the entry point of our application that uses Windows API 
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, 
                     PSTR cmdline, int cmdshow)
{
    ID3D11InfoQueue *iq;
    DXGI_FORMAT dsbf, atf; // depth stencil buffer format, atlas tex format
    TRACKMOUSEEVENT tme;
    
    ctx.dc = CreateCompatibleDC(GetDC(0));
    
    WNDCLASSEXW wc = { sizeof(wc), CS_HREDRAW|CS_VREDRAW, window_proc, 0, 0, hInst, 
        NULL, NULL, NULL, NULL, L"synergy_window_class", NULL, };
    if (RegisterClassExW(&wc) == 0) exit(1);
    
    ctx.wexs=0;
    ctx.wcs=WS_OVERLAPPEDWINDOW|WS_VISIBLE;
    ctx.wp=ini2f(0,0);
    ctx.wd=ini2f(800,600);
    xstrcpy(ctx.wt, 256, L"XRender realtime engine");
    ctx.td=false;
    ctx.sas=4096;
    ctx.mss=10000;
    ctx.msl=10000;
    ctx.gms=256;
    ctx.cc=ini4f(1,0,1,1);
    xconfig();
    
    ctx.bbs=ctx.wd;
    
    RECT size = {(u32)ctx.wp.x, (u32)ctx.wp.y, 
        (u32)(ctx.wp.x+ctx.wd.x), (u32)(ctx.wp.y+ctx.wd.y)};
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
    ctx.wd=ini2f((f32)(size.right-size.left), (f32)(size.bottom-size.top));
    
    ctx.wh = CreateWindowExW(ctx.wexs, L"synergy_window_class", ctx.wt, ctx.wcs, 
                              (s32)ctx.wp.x, (s32)ctx.wp.y, (s32)ctx.wd.x, (s32)ctx.wd.y, NULL, NULL, hInst, NULL);
    if (ctx.wh == NULL)
        exit(1);
    
    ctx.ca = LoadCursor(NULL, IDC_ARROW);
    SetCursor(ctx.ca);
    
    // Request notification when the mouse leaves the non-client area
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_NONCLIENT | TME_LEAVE;
    tme.hwndTrack = ctx.wh;
    TrackMouseEvent(&tme);
    
    DXGI_MODE_DESC bbd = { (s32)ctx.bbs.x, (s32)ctx.bbs.y, xrational(60,1), DXGI_FORMAT_R8G8B8A8_UNORM, 
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_CENTERED, };
    DXGI_SAMPLE_DESC bbsd = { .Count = 1,  .Quality = 0, };
    DXGI_SWAP_CHAIN_DESC scd = { bbd, bbsd, DXGI_USAGE_RENDER_TARGET_OUTPUT,
        2, ctx.wh, true, DXGI_SWAP_EFFECT_FLIP_DISCARD, 0, };
    
    
    xfeatureleves(ctx.fl, &ctx.flc);
    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 
        D3D11_CREATE_DEVICE_DEBUG, ctx.fl, ctx.flc, D3D11_SDK_VERSION, &scd, &ctx.swc,
        &ctx.dvc, NULL, &ctx.ctx)))
        exit(1);
    
    if (FAILED(ID3D11Device_QueryInterface(ctx.dvc, &IID_ID3D11Debug, (void **)&ctx.dbg)))
        exit(1);
    
    ID3D11Device_QueryInterface(ctx.dvc, &IID_ID3D11InfoQueue, (void **)&iq);
    ID3D11InfoQueue_SetBreakOnSeverity(iq, D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
    ID3D11InfoQueue_Release(iq);
    
    if (FAILED(IDXGISwapChain_GetBuffer(ctx.swc, 0, &IID_ID3D11Texture2D, (void **)&ctx.bbt)))
        exit(1);
    
    dsbf = DXGI_FORMAT_D24_UNORM_S8_UINT;
    D3D11_TEXTURE2D_DESC dsbtd = { (s32)ctx.bbs.x, (s32)ctx.bbs.y, 0, 1, dsbf, bbsd, 
        D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL, 0, 0, };
    ctx.dsbd = dsbtd;
    
    if (FAILED(ID3D11Device_CreateTexture2D(ctx.dvc, &dsbtd, 0, &ctx.dsbt)))
        exit(1);
    
    ctx.sa.size=ctx.sas;
    ctx.sa.at.x=ctx.sa.at.y=ctx.sa.bottom=0;
    
    atf = DXGI_FORMAT_R8G8B8A8_UNORM;
    D3D11_TEXTURE2D_DESC atd = { ctx.sa.size, ctx.sa.size, 1, 1, atf, {1,0}, 
        D3D11_USAGE_DYNAMIC, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, 0, };
    
    u32 ams = ctx.sa.size*ctx.sa.size*4;
    ctx.sa.bytes = (u8 *)xalloc(ams);
    
    LARGE_INTEGER pf;
    QueryPerformanceFrequency(&pf);
    ctx.pf = pf.QuadPart;
    
    dsound8_init();
    
    xinitialize();
    
    D3D11_SUBRESOURCE_DATA ad = { ctx.sa.bytes, (UINT)(ctx.sa.size*4), 0 };
    if (FAILED(ID3D11Device_CreateTexture2D(ctx.dvc, &atd, &ad, &ctx.sat)))
        exit(1);
    
    if (FAILED(ID3D11Device_CreateRenderTargetView(ctx.dvc, (ID3D11Resource *) ctx.bbt, 0, &ctx.rtv)))
        exit(1);
    
    ctx.dsvd = xdsviewdesc(dsbf, D3D11_DSV_DIMENSION_TEXTURE2D);
    if (FAILED(ID3D11Device_CreateDepthStencilView(ctx.dvc, (ID3D11Resource *)ctx.dsbt, &ctx.dsvd, &ctx.dsv)))
        exit(1);
    
    D3D11_SHADER_RESOURCE_VIEW_DESC asrvd = xshadresview(atf, D3D_SRV_DIMENSION_TEXTURE2D);
    if (FAILED(ID3D11Device_CreateShaderResourceView(ctx.dvc, (ID3D11Resource *)ctx.sat, &asrvd, &ctx.asrv)))
        exit(1);
    
    D3D11_DEPTH_STENCIL_DESC dsd = xdepthstencildesc();
    
    if (FAILED(ID3D11Device_CreateDepthStencilState(ctx.dvc, &dsd, &ctx.dss)))
        exit(1);
    
    D3D11_BLEND_DESC bd = xblenddesc();
    
    if (FAILED(ID3D11Device_CreateBlendState(ctx.dvc, &bd, &ctx.bls)))
        exit(1);
    
    D3D11_RASTERIZER_DESC rd = xrasterstate();
    if (FAILED(ID3D11Device_CreateRasterizerState(ctx.dvc, &rd, &ctx.rs)))
        exit(1);
    
    D3D11_SAMPLER_DESC sd = xsamplerdesc();    
    if (FAILED(ID3D11Device_CreateSamplerState(ctx.dvc, &sd, &ctx.ss)))
        exit(1);
    
    create_sprites_shaders();
    create_lines_shaders();
    
    create_sprites_vertex_buffer();
    create_lines_vertex_buffer();
    
    D3D11_BUFFER_DESC vcbd = xcbufferdesc(sizeof(XVertexCBuffer));
    if (FAILED(ID3D11Device_CreateBuffer(ctx.dvc, &vcbd, 0, &ctx.vcb)))
        exit(1);
    
    ctx.lg = Stack_new(256, sizeof(XLineCommand));
    
    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);
    
    ctx.run = true;
    while (ctx.run)
    {
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        
        // Get mouse position
        POINT mousePoint;
        if (GetCursorPos(&mousePoint))
        {
            if (ScreenToClient(ctx.wh, &mousePoint))
            {
                ctx.mouse.pos.x = (f32)mousePoint.x; 
                ctx.mouse.pos.y = ctx.bbs.y - (f32)mousePoint.y; 
            }
        }
        
        // Mouse dragging
        if (ctx.mouse.left.down && !ctx.mouse.dragging)
        {
            ctx.mouse.dragging = true;
        }
        
        if (ctx.mouse.dragging && !ctx.mouse.left.down)
        {
            ctx.mouse.dragging = false;
        }
        
        // Measure fps
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        
        // Calculater fps
        ctx.dt = ((f32)(counter.QuadPart - lastCounter.QuadPart) / (f32)ctx.pf);
        
        // Save counter
        lastCounter = counter;
        
        
        // Call update for the app
        xupdate();
        
        swap_chain_resize();
        
        u32 svc = produce_vertices_from_sprite_groups();
        u32 lvc = produce_vertices_from_line_group();
        
        f32 scaleX = 2.0f / ctx.bbs.x;
        f32 scaleY = (ctx.td ? -2.0f : 2.0f) / ctx.bbs.y;
        mat4f wvpMatrix = 
        {
            scaleX, 0, 0, -1,
            0, scaleY, 0, (ctx.td ? 1.f : -1.f),
            0, 0, .001f, 0,
            0, 0, 0, 1,
        };
        
        ctx.vcbd.WVP = wvpMatrix;
        ID3D11DeviceContext_UpdateSubresource(ctx.ctx, (ID3D11Resource *)ctx.vcb, 0, NULL, &ctx.vcbd, 0, 0);
        
        ID3D11RenderTargetView *views[1] = { ctx.rtv };
        ID3D11DeviceContext_OMSetRenderTargets(ctx.ctx, 1, views, ctx.dsv);
        ID3D11DeviceContext_ClearRenderTargetView(ctx.ctx, ctx.rtv, ctx.cc.e);
        ID3D11DeviceContext_ClearDepthStencilView(ctx.ctx, ctx.dsv, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 0, 0);
        
        // Lines
        render_pass(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, ctx.lil, lvc, ctx.lvb, sizeof(XLineVertex3D), ctx.lvs, ctx.lps);
        // Sprites
        render_pass(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, ctx.sil, svc, ctx.svb, sizeof(XVertex3D), ctx.svs, ctx.sps);
        
        IDXGISwapChain_Present(ctx.swc, 1, 0);
        
        // Clear keyboard pressed state from last frame
        for (u32 i = 0; i < narray(ctx.key.all); ++i)
        {
            ctx.key.all[i].pressed = false;
        }
        
        // Clear mouse pressed state
        ctx.mouse.left.pressed = false;
        ctx.mouse.right.pressed = false;
        
        ctx.mouse.left.released = false;
        ctx.mouse.right.released = false;
        
        ctx.mouse.wheel = 0;
        
        // Clear the input char
        ctx.ic = 0;
        ctx.ice = false;
    }
    
    xshutdown();
    
    xfree(ctx.sa.bytes);
    
    free_sprite_groups();
    free_line_group();
    
    xmemcheck();
    
    if (ctx.asrv) ID3D11ShaderResourceView_Release(ctx.asrv);
    if (ctx.vcb) ID3D11Buffer_Release(ctx.vcb);
    if (ctx.lvb) ID3D11Buffer_Release(ctx.lvb);
    if (ctx.svb) ID3D11Buffer_Release(ctx.svb);
    if (ctx.lil) ID3D11InputLayout_Release(ctx.lil);
    if (ctx.sil) ID3D11InputLayout_Release(ctx.sil);
    if (ctx.lps) ID3D11PixelShader_Release(ctx.lps);
    if (ctx.sps) ID3D11PixelShader_Release(ctx.sps);
    if (ctx.lvs) ID3D11VertexShader_Release(ctx.lvs);
    if (ctx.svs) ID3D11VertexShader_Release(ctx.svs);
    if (ctx.ss) ID3D11SamplerState_Release(ctx.ss);
    if (ctx.rs) ID3D11RasterizerState_Release(ctx.rs);
    if (ctx.bls) ID3D11BlendState_Release(ctx.bls);
    if (ctx.dss) ID3D11DepthStencilState_Release(ctx.dss);
    if (ctx.dsv) ID3D11DepthStencilView_Release(ctx.dsv);
    if (ctx.rtv) ID3D11RenderTargetView_Release(ctx.rtv);
    if (ctx.sat) ID3D11Texture2D_Release(ctx.sat);
    if (ctx.dsbt) ID3D11Texture2D_Release(ctx.dsbt);
    if (ctx.bbt) ID3D11Texture2D_Release(ctx.bbt);
    if (ctx.dbg) ID3D11Debug_Release(ctx.dbg);
    if (ctx.swc) IDXGISwapChain_Release(ctx.swc);
    if (ctx.ctx) ID3D11DeviceContext_Release(ctx.ctx);
    if (ctx.dvc) ID3D11Device_Release(ctx.dvc);
    
    return 0;
}

#undef T

#endif