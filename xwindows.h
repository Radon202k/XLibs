#ifndef XLIB_WINDOWS
#define XLIB_WINDOWS

#define COBJMACROS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <mmeapi.h>
#include <dsound.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include "xstring.h"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

typedef struct XFile* XFile;

typedef struct
{
    HCURSOR cursor;
    HWND winhandle;
    v2f windim;
    bool topdown;

} XWinConfig;

void xwininit(XWinConfig config);

DXGI_RATIONAL dxgiratio(DWORD numerator, DWORD denominator);
BITMAPINFO bmpinfo(int width, int height);

void xcbcopy(wchar_t *text);
s32 xcbpaste(wchar_t *text, int maxLength);

void xpath(wchar_t *path, u32 size);
void xpathascii(char *path, u32 size);

XFile xfileread(wchar_t *filePath);
bool xfilewrite(wchar_t *filePath, wchar_t *data, u32 size);

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
            a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z;
    };
} XKeys;

typedef struct
{
    bool  dragging;
    f32   wheel;
    v2f   pos, dragLastP;
    void* draggingAddress;
    XKey   left, right;
} XMouse;

/*

    ic  : input char
    ice : input char entered


*/

typedef struct
{
    s32 ice;
    XKeys key;
    XMouse mouse;
    wchar_t ic;
    HWND wh;
    v2f wd;
    bool td;
    HCURSOR ch;
} XWindows;

global XWindows xwin;

void xwininit(XWinConfig config)
{
    xwin.wh = config.winhandle;
    xwin.wd = config.windim;
    xwin.td = config.topdown;
    xwin.ch = config.cursor;
}

#define XWINMAIN() int APIENTRY WinMain(HINSTANCE inst, HINSTANCE instprev, PSTR cmdline, int cmdshow)

#define XWNDPROC                        \
case WM_SETCURSOR: {    \
SetCursor(xwin.ch);  \
} break;                \
case WM_DESTROY:        \
case WM_CLOSE: {        \
xrend.run = false;    \
} break;                \
case WM_CHAR: {                 \
xwin.ic = (wchar_t)wParam;   \
xwin.ice = true;             \
} break;                        \
case WM_MOUSEWHEEL: {                                                               \
xwin.mouse.wheel = ((f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA);     \
} break;                                                                            \
case WM_KEYDOWN:                                                    \
case WM_SYSKEYDOWN:                                                 \
case WM_KEYUP:                                                      \
case WM_SYSKEYUP: {                                                 \
XKey* key = 0;                                                  \
if (wParam == VK_UP)              key = &xwin.key.up;            \
else if (wParam == VK_LEFT)       key = &xwin.key.left;          \
else if (wParam == VK_DOWN)       key = &xwin.key.down;          \
else if (wParam == VK_RIGHT)      key = &xwin.key.right;         \
else if (wParam == VK_BACK)       key = &xwin.key.backspace;     \
else if (wParam == VK_MENU)       key = &xwin.key.alt;           \
else if (wParam == VK_F1)         key = &xwin.key.f1;            \
else if (wParam == 'A')           key = &xwin.key.a;             \
else if (wParam == 'B')           key = &xwin.key.b;             \
else if (wParam == 'C')           key = &xwin.key.c;             \
else if (wParam == 'D')           key = &xwin.key.d;             \
else if (wParam == 'E')           key = &xwin.key.e;             \
else if (wParam == 'F')           key = &xwin.key.f;             \
else if (wParam == 'G')           key = &xwin.key.g;             \
else if (wParam == 'H')           key = &xwin.key.h;             \
else if (wParam == 'I')           key = &xwin.key.i;             \
else if (wParam == 'J')           key = &xwin.key.j;             \
else if (wParam == 'K')           key = &xwin.key.k;             \
else if (wParam == 'L')           key = &xwin.key.l;             \
else if (wParam == 'M')           key = &xwin.key.m;             \
else if (wParam == 'N')           key = &xwin.key.n;             \
else if (wParam == 'O')           key = &xwin.key.o;             \
else if (wParam == 'P')           key = &xwin.key.p;             \
else if (wParam == 'Q')           key = &xwin.key.q;             \
else if (wParam == 'R')           key = &xwin.key.r;             \
else if (wParam == 'S')           key = &xwin.key.s;             \
else if (wParam == 'T')           key = &xwin.key.t;             \
else if (wParam == 'U')           key = &xwin.key.u;             \
else if (wParam == 'V')           key = &xwin.key.v;             \
else if (wParam == 'W')           key = &xwin.key.w;             \
else if (wParam == 'X')           key = &xwin.key.x;             \
else if (wParam == 'Y')           key = &xwin.key.y;             \
else if (wParam == 'Z')           key = &xwin.key.z;             \
else if (wParam == VK_CONTROL)    key = &xwin.key.control;       \
else if (wParam == VK_SPACE)      key = &xwin.key.space;         \
if (key) {                                                      \
key->down = (message == WM_KEYDOWN);                        \
key->pressed = (message == WM_KEYDOWN);                     \
key->released = (message == WM_KEYUP);                      \
}                                                               \
} break;                                                            \
case WM_LBUTTONDOWN: {                  \
xwin.mouse.left.down = true;         \
xwin.mouse.left.pressed = true;      \
} break;                                \
case WM_RBUTTONDOWN: {                  \
xwin.mouse.right.down = true;        \
xwin.mouse.right.pressed = true;     \
} break;                                \
case WM_LBUTTONUP: {                    \
xwin.mouse.left.down = false;        \
xwin.mouse.left.released = true;     \
} break;                                \
case WM_RBUTTONUP: {                    \
xwin.mouse.right.down = false;       \
xwin.mouse.right.released = true;    \
} break                                 \


struct XFile
{
    wchar_t name[512];
    bool exists;
    u32 size;
    u8 *bytes;
};

void xwinupdate()
{
    // Get mouse position
    POINT mousePoint;
    if (GetCursorPos(&mousePoint))
    {
        if (ScreenToClient(xwin.wh, &mousePoint))
        {
            xwin.mouse.pos.x = (f32)mousePoint.x;
            xwin.mouse.pos.y = (xwin.td ? mousePoint.y : xwin.wd.y - (f32)mousePoint.y);
        }
    }

    // Mouse dragging
    if (xwin.mouse.left.down && !xwin.mouse.dragging)
    {
        xwin.mouse.dragging = true;
    }

    if (xwin.mouse.dragging && !xwin.mouse.left.down)
    {
        xwin.mouse.dragging = false;
    }

    // Clear keyboard pressed state from last frame
    for (u32 i = 0; i < narray(xwin.key.all); ++i)
    {
        xwin.key.all[i].pressed = false;
    }

    // Clear mouse pressed state
    xwin.mouse.left.pressed = false;
    xwin.mouse.right.pressed = false;

    xwin.mouse.left.released = false;
    xwin.mouse.right.released = false;

    xwin.mouse.wheel = 0;

    // Clear the input char
    xwin.ic = 0;
    xwin.ice = false;
}

bool xdraggedhandle(v2f p, f32 maxdist, void* address, bool* hover, v2f* delta)
{
    bool dragged = false;

    if (len2f(sub2f(p, xwin.mouse.pos)) < maxdist * maxdist)
    {
        *hover = true;

        if (xwin.mouse.dragging)
        {
            xwin.mouse.dragLastP = xwin.mouse.pos;
            xwin.mouse.draggingAddress = address;
        }
    }
    else
    {
        *hover = false;
    }

    if (xwin.mouse.dragging && xwin.mouse.draggingAddress == address)
    {
        v2f deltaP = sub2f(xwin.mouse.pos, xwin.mouse.dragLastP);
        xwin.mouse.dragLastP = xwin.mouse.pos;
        *delta = deltaP;
        dragged = true;
    }

    return dragged;
}

WNDCLASSEXW xwndclass(WNDPROC userwndproc)
{
    WNDCLASSEXW r = {
        sizeof(r), CS_HREDRAW | CS_VREDRAW, userwndproc, 0, 0,
        GetModuleHandle(0), NULL, NULL, NULL, NULL, L"xwindow_class", NULL,
    };
    return r;
}

void xfeatureleves(D3D_FEATURE_LEVEL* levels, s32 *count)
{
    D3D_FEATURE_LEVEL r[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };
    xncopy(levels, &r, narray(r), D3D_FEATURE_LEVEL);
    *count = narray(r);
}

D3D11_RENDER_TARGET_BLEND_DESC xtargetblenddesc()
{
    D3D11_RENDER_TARGET_BLEND_DESC r = {
        true,
        D3D11_BLEND_SRC_ALPHA,
        D3D11_BLEND_INV_SRC_ALPHA,
        D3D11_BLEND_OP_ADD,
        D3D11_BLEND_ONE,
        D3D11_BLEND_ZERO,
        D3D11_BLEND_OP_ADD,
        D3D11_COLOR_WRITE_ENABLE_ALL,
    };
    return r;
}

D3D11_BLEND_DESC xblenddesc()
{
    D3D11_BLEND_DESC r = {
        false,
        false,
    };
    
    r.RenderTarget[0] = xtargetblenddesc();
    
    return r;   
}

D3D11_BUFFER_DESC xcbufferdesc(s32 size)
{
    D3D11_BUFFER_DESC r = {
        size, D3D11_USAGE_DEFAULT, 
        D3D11_BIND_CONSTANT_BUFFER,
        0, 0, 0,
    };
    return r;
}

D3D11_RASTERIZER_DESC xrasterstate()
{
    D3D11_RASTERIZER_DESC r =
    {
        D3D11_FILL_SOLID, D3D11_CULL_BACK, false,
        0, 0, 0, true, true, false, false,
    };
    return r;
}


D3D11_SAMPLER_DESC xsamplerdesc()
{
    D3D11_SAMPLER_DESC r =
    {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_COMPARISON_NEVER,
        0,
        D3D11_FLOAT32_MAX,
    };
    return r;
}


DXGI_RATIONAL xrational(DWORD n, DWORD d)
{
	DXGI_RATIONAL r = {
        n, d
    };
	return r;
}

BITMAPINFO xbmpinfo(int width, int height)
{
    BITMAPINFOHEADER h = {
        sizeof(h), width, -height, 1, 32
    };
    
    BITMAPINFO r = {
        h
    };
    
    return r;
}

D3D11_DEPTH_STENCIL_DESC xdepthstencildesc()
{
    D3D11_DEPTH_STENCIL_DESC r = 
    {
        .DepthEnable = true,
        D3D11_DEPTH_WRITE_MASK_ALL,
        D3D11_COMPARISON_GREATER_EQUAL,
        .StencilEnable = false,
        .StencilReadMask = 0,
        .StencilWriteMask = 0,
        .FrontFace = {0},
        .BackFace = {0},
    };
    return r;
}


D3D11_DEPTH_STENCIL_VIEW_DESC xdsviewdesc(DXGI_FORMAT format, D3D11_DSV_DIMENSION dim) {
    D3D11_TEX2D_DSV t = {
        0
    };
    
    D3D11_DEPTH_STENCIL_VIEW_DESC r = {
        format, dim,
    };
    
    if (dim == D3D11_DSV_DIMENSION_TEXTURE2D) {
        r.Texture2D = t;
    } 
    
    return r;
}

D3D11_SHADER_RESOURCE_VIEW_DESC xshadresview(DXGI_FORMAT format, D3D11_SRV_DIMENSION dim)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC r =
    {
        format,
        dim,
    };
    
    if (dim == D3D_SRV_DIMENSION_TEXTURE2D) {
        r.Texture2D.MostDetailedMip = 0;
        r.Texture2D.MipLevels = 1;
    }
    
    return r;
}

void Clipboard_copy(wchar_t *text)
{
    HGLOBAL globalMemory = GlobalAlloc(GMEM_MOVEABLE, (xstrlen(text)+1) * sizeof(wchar_t));
    
    wchar_t *data = (wchar_t *)GlobalLock(globalMemory);
    
    u32 copiedLength = xstrlen(text)+1;
    
    xstrcpy(data, copiedLength, text);
    
    assert(copiedLength < 1000);
    
    GlobalUnlock(globalMemory);
    
    if (OpenClipboard(NULL))
    {
        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, globalMemory);
        CloseClipboard();
    }
    
    GlobalFree(globalMemory);
}

s32 Clipboard_paste(wchar_t *text, int maxLength)
{
    s32 pastedLength = 0;
    
    if (OpenClipboard(NULL))
    {
        HGLOBAL globalMemory = GetClipboardData(CF_UNICODETEXT);
        if (globalMemory != NULL)
        {
            wchar_t *data = (wchar_t *)GlobalLock(globalMemory);
            pastedLength = xstrlen(data);
            
            assert(pastedLength < 1000);
            
            xstrcps(text, maxLength*sizeof(wchar_t), data, pastedLength);
            GlobalUnlock(globalMemory);
        }
        
        CloseClipboard();
    }
    
    return pastedLength;
}

void xpath(wchar_t *path, u32 size)
{
    GetModuleFileNameW(NULL, path, size);
}

void xpathascii(char *path, u32 size)
{
    GetModuleFileNameA(NULL, path, size);
}


XFile xfileread(wchar_t *path)
{
    XFile r = xalloc(sizeof *r);
    
    FILE *f = 0;
    _wfopen_s(&f, path, L"rb, ccs=UTF-16LE");
    if (f != 0)
    {
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        rewind(f);
        
        r->bytes = xalloc(sz+1);
        if (r->bytes)
        {
            size_t c = fread((wchar_t *)r->bytes, sizeof(wchar_t),
                             sz/sizeof(wchar_t), f);
            if (c == sz/sizeof(wchar_t))
            {
                r->exists = true;
                r->size = sz;
            }
        }
    }
    
    return r;
}

bool File_write(wchar_t *path, wchar_t *data, u32 size)
{
    bool r = false;
    HANDLE h;
    DWORD sz;
    
    h = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
    sz = 0;
    if (!WriteFile(h, data, size, &sz, 0))
    {
        CloseHandle(h);
        return r;
    }
    
    assert(size == sz);
    r = true;
    CloseHandle(h);
    return r;
}


#endif