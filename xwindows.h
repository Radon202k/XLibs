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

/* ============================================================================
DATA TYPES
 ========================================================================== */

typedef struct
{
    HCURSOR cursor;
    HWND windowHandle;
} XWindowConfig;

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

typedef struct
{
    wchar_t name[512];
    bool exists;
    u32 size;
    u8 *bytes;
} XFile;

typedef struct
{
    bool input_char_entered;
    wchar_t input_char;
    
    XKeys key;
    XMouse mouse;
    HWND wh;
    HCURSOR ch;
    u64 pf;
    
} XWindows;

global XWindows xwin;

/* ============================================================================
INTERFACE
 ========================================================================== */

void xwin_initialize (XWindowConfig config);
void xwin_update (bool topdown, v2f windim);

inline LARGE_INTEGER xwin_time(void);
inline f32 xwin_seconds(LARGE_INTEGER start, LARGE_INTEGER end);

bool xwin_dragged(v2f p, f32 maxdist, void* address, bool* hover, v2f* delta);

WNDCLASSEXW xwin_wndclass(WNDPROC userwndproc);

void  xwin_clipboard_copy(wchar_t *text);
s32   xwin_clipboard_paste(wchar_t *text, int maxLength);
void  xwin_path(wchar_t *path, u32 size);
void  xwin_pathascii(char *path, u32 size);
XFile xwin_file_read(wchar_t *path);
bool  xwin_file_write(wchar_t *path, wchar_t *data, u32 size);
void  xwin_path_abs(wchar_t *dst, u32 dstsize, wchar_t *filename);
void  xwin_path_abs_ascii(char *dst, u32 dstsize, char *filename);










/* ============================================================================
IMPLEMENTATION
 ========================================================================== */

void xwin_initialize(XWindowConfig config)
{
    xwin.wh = config.windowHandle;
    xwin.ch = config.cursor;
    
    LARGE_INTEGER pf;
    QueryPerformanceFrequency(&pf);
    xwin.pf = pf.QuadPart;
}

void xwin_update(bool topdown, v2f windim)
{
    // Get mouse position
    POINT mousePoint;
    if (GetCursorPos(&mousePoint))
    {
        if (ScreenToClient(xwin.wh, &mousePoint))
        {
            xwin.mouse.pos.x = (f32)mousePoint.x;
            xwin.mouse.pos.y = (topdown ? mousePoint.y : windim.y - (f32)mousePoint.y);
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
    xwin.input_char = 0;
    xwin.input_char_entered = false;
}

#define XWINMAIN() int APIENTRY WinMain(HINSTANCE inst, \
HINSTANCE instprev, \
PSTR cmdline, \
int cmdshow)

#define XWNDPROC                        \
case WM_SETCURSOR: {    \
SetCursor(xwin.ch);  \
} break;                \
case WM_DESTROY:        \
case WM_CLOSE: {        \
xd11.running = false;    \
} break;                \
case WM_CHAR: {                 \
xwin.input_char = (wchar_t)wParam;   \
xwin.input_char_entered = true;             \
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


inline LARGE_INTEGER xwin_time(void)
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter;
}

inline f32 xwin_seconds(LARGE_INTEGER start, LARGE_INTEGER end)
{
    f32 r = ((f32)(end.QuadPart - start.QuadPart) / (f32)xwin.pf);
    return r;
}

bool xwin_dragged(v2f p, f32 maxdist, void* address, bool* hover, v2f* delta)
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

WNDCLASSEXW xwin_wndclass(WNDPROC userwndproc)
{
    WNDCLASSEXW r = {
        sizeof(r), CS_HREDRAW | CS_VREDRAW, userwndproc, 0, 0,
        GetModuleHandle(0), NULL, NULL, NULL, NULL, L"xwindow_class", NULL,
    };
    return r;
}

void xwin_clipboard_copy(wchar_t *text)
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

s32 xwin_clipboard_paste(wchar_t *text, int maxLength)
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

void xwin_path(wchar_t *path, u32 size)
{
    GetModuleFileNameW(NULL, path, size);
}

void xwin_path_ascii(char *path, u32 size)
{
    GetModuleFileNameA(NULL, path, size);
}

XFile xwin_file_read(wchar_t *path)
{
    XFile r = {0};
    
    FILE *f = 0;
    _wfopen_s(&f, path, L"rb, ccs=UTF-16LE");
    if (f != 0)
    {
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        rewind(f);
        
        r.bytes = xalloc(sz+1);
        if (r.bytes)
        {
            size_t c = fread((wchar_t *)r.bytes, sizeof(wchar_t),
                             sz/sizeof(wchar_t), f);
            if (c == sz/sizeof(wchar_t))
            {
                r.exists = true;
                r.size = sz;
            }
        }
    }
    
    return r;
}

bool xwin_file_write(wchar_t *path, wchar_t *data, u32 size)
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

/*  Copies the exe path until last slash
    c:/my/path/to/the/app/main.exe
                         ^                            */
void xwin_path_abs(wchar_t *dst, u32 dstsize, wchar_t *filename)
{
    wchar_t *slashpos, *at, exepath[MAX_PATH], dir[260];
    
    xwin_path(exepath, MAX_PATH);
    
    slashpos = 0;
    at = exepath;
    while (*at++)
        if (*at == '\\' || *at == '/')
        slashpos = at;
    
    xstrcps(dir, 260, exepath, (u32)(slashpos - exepath));
    _snwprintf_s(dst, dstsize, _TRUNCATE, L"%s\\%s", dir, filename);
}

void xwin_path_abs_ascii(char *dst, u32 dstsize, char *filename)
{
    char *slashpos, *at, exepath[MAX_PATH], dir[260];
    xwin_path_ascii(exepath, MAX_PATH);
    
    slashpos  = 0;
    at = exepath;
    while (*at++)
        if (*at == '\\' || *at == '/')
        slashpos = at;
    
    xstrcpsascii(dir, 260, exepath, (u32)(slashpos - exepath));
    _snprintf_s(dst, dstsize, _TRUNCATE, "%s\\%s", dir, filename);
}

#endif