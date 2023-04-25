#ifndef XLIB_WINDOWS_H
#define XLIB_WINDOWS_H

#define COBJMACROS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include "xstring.h"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
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
        tab, enter, backspace, alt, control, space, f1,
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z;
    };
} XKeys;

typedef struct {
    bool  dragging;
    f32   wheel;
    v2   p, dragLastP;
    void* draggingAddress;
    XKey   left, right;
} XMouse;

typedef struct
{
    char name[512];
    u32 fileCount;
    XFile *files;
} XDirectory;

typedef struct
{
    bool inputCharEntered;
    char inputChar;
    
    HCURSOR lastCursorSet;
    HCURSOR cursorSet;
    
    XKeys key;
    HWND wh;
    u64 pf;
    
} XWindows;

global XWindows xwin;
global XMouse xmouse;

/* ============================================================================
INTERFACE
 ========================================================================== */

void xwin_initialize (XWindowConfig config);
void xwin_update (bool topdown, v2 windim);

inline LARGE_INTEGER xwin_time(void);
inline f32 xwin_seconds(LARGE_INTEGER start, LARGE_INTEGER end);

bool xwin_dragged(v2 p, f32 maxdist, void *address, bool *hover, v2 delta);

static WNDCLASSEXA xwin_wndclass(WNDPROC userwndproc);

void  xwin_clipboard_copy(char *text);
s32   xwin_clipboard_paste(char *text, int maxLength);
void  xwin_path(char *path, u32 size);
void  xwin_pathascii(char *path, u32 size);
XFile xwin_file_read(char *path);
bool  xwin_file_write(char *path, u8 *data, u32 size);
void  xwin_path_abs(char *dst, u32 dstsize, char *filename);
void  xwin_path_abs_ascii(char *dst, u32 dstsize, char *filename);










/* ============================================================================
IMPLEMENTATION
 ========================================================================== */

void xwin_initialize(XWindowConfig config)
{
    xwin.wh = config.windowHandle;
    xwin.cursorSet = config.cursor;
    
    LARGE_INTEGER pf;
    QueryPerformanceFrequency(&pf);
    xwin.pf = pf.QuadPart;
}

void xwin_update(bool topdown, v2 windim)
{
    // Get mouse position
    POINT mousePoint;
    if (GetCursorPos(&mousePoint)) {
        if (ScreenToClient(xwin.wh, &mousePoint)) {
            xmouse.p[0] = (f32)mousePoint.x;
            xmouse.p[1] = (topdown ? mousePoint.y : windim[1] - (f32)mousePoint.y);
        }
    }
    
    // Mouse dragging
    if (xmouse.left.down && !xmouse.dragging) {
        xmouse.dragging = true;
    }
    
    if (xmouse.dragging && !xmouse.left.down) {
        xmouse.dragging = false;
    }
    
    // Clear keyboard pressed state from last frame
    for (u32 i = 0; i < narray(xwin.key.all); ++i) {
        xwin.key.all[i].pressed = false;
    }
    
    // Clear mouse pressed state
    xmouse.left.pressed = false;
    xmouse.right.pressed = false;
    
    xmouse.left.released = false;
    xmouse.right.released = false;
    
    xmouse.wheel = 0;
    
    // Clear the input char
    xwin.inputChar = 0;
    xwin.inputCharEntered = false;
}

#define XWINMAIN() int APIENTRY WinMain(HINSTANCE inst, \
HINSTANCE instprev, \
PSTR cmdline, \
int cmdshow)

#define XWNDPROC                                \
case WM_DESTROY:                            \
case WM_CLOSE: {                            \
xd11.running = false;                   \
} break;                                    \
case WM_CHAR: {                             \
xwin.inputChar = (char)wParam;       \
xwin.inputCharEntered = true;           \
} break;                                    \
case WM_MOUSEWHEEL: {                                                               \
xmouse.wheel = ((f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA);     \
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
else if (wParam == VK_TAB)        key = &xwin.key.tab;     \
else if (wParam == VK_RETURN)     key = &xwin.key.enter;     \
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
xmouse.left.down = true;         \
xmouse.left.pressed = true;      \
} break;                                \
case WM_RBUTTONDOWN: {                  \
xmouse.right.down = true;        \
xmouse.right.pressed = true;     \
} break;                                \
case WM_LBUTTONUP: {                    \
xmouse.left.down = false;        \
xmouse.left.released = true;     \
} break;                                \
case WM_RBUTTONUP: {                    \
xmouse.right.down = false;       \
xmouse.right.released = true;    \
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

bool xwin_dragged(v2 p, f32 maxdist, void *address, bool *hover, v2 delta)
{
    bool dragged = false;
    v2 pMinusMouseP;
    v2_sub(p, xmouse.p, pMinusMouseP);
    if (v2_length2(pMinusMouseP) < maxdist * maxdist)
    {
        *hover = true;
        
        if (xmouse.dragging)
        {
            v2_copy(xmouse.p, xmouse.dragLastP);
            xmouse.draggingAddress = address;
        }
    }
    else
    {
        *hover = false;
    }
    
    if (xmouse.dragging && xmouse.draggingAddress == address)
    {
        v2 deltaP;
        v2_sub(xmouse.p, xmouse.dragLastP, deltaP);
        v2_copy(xmouse.p, xmouse.dragLastP);
        v2_copy(deltaP, delta);
        dragged = true;
    }
    
    return dragged;
}

static WNDCLASSEXA
xwin_wndclass(WNDPROC userwndproc)
{
    WNDCLASSEXA r = {
        sizeof(r), CS_HREDRAW | CS_VREDRAW, userwndproc, 0, 0,
        GetModuleHandle(0), NULL, NULL, NULL, NULL, "xwindow_class", NULL,
    };
    return r;
}

void xwin_clipboard_copy(char *text) {
    HGLOBAL globalMemory = GlobalAlloc(GMEM_MOVEABLE, (xstrlen(text)+1) * sizeof(char));
    
    char *data = (char *)GlobalLock(globalMemory);
    
    u32 copiedLength = xstrlen(text)+1;
    
    xstrcpy(data, text);
    
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

s32 xwin_clipboard_paste(char *text, int maxLength)
{
    s32 pastedLength = 0;
    
    if (OpenClipboard(NULL))
    {
        HGLOBAL globalMemory = GetClipboardData(CF_UNICODETEXT);
        if (globalMemory != NULL)
        {
            char *data = (char *)GlobalLock(globalMemory);
            pastedLength = xstrlen(data);
            
            assert(pastedLength < 1000);
            
            xstrncpy(text, data, pastedLength);
            GlobalUnlock(globalMemory);
        }
        
        CloseClipboard();
    }
    
    return pastedLength;
}

void xwin_path(char *path, u32 size)
{
    GetModuleFileNameA(NULL, path, size);
}

XFile xwin_file_read(char *path)
{
    XFile r = {0};
    
    FILE *f = fopen(path, "rb, ccs=UTF-8");
    if (f != 0)
    {
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        rewind(f);
        
        r.bytes = xalloc(sz+1);
        if (r.bytes)
        {
            size_t c = fread((char *)r.bytes, sizeof(char),
                             sz/sizeof(char), f);
            if (c == sz/sizeof(char))
            {
                r.exists = true;
                r.size = sz;
            }
        }
        fclose(f);
    }
    
    return r;
}

bool xwin_file_write(char *path, u8 *data, u32 size)
{
    bool r = false;
    HANDLE h;
    DWORD sz;
    
    h = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
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

void xwin_directory_open(XDirectory *dir, char *directoryPath) {
    WIN32_FIND_DATA findData;
    HANDLE findHandle;
    
    char searchPath[MAX_PATH];
    xstrcpy(searchPath, directoryPath);
    xstrcat(searchPath, "\\*");
    
    findHandle = FindFirstFileA(searchPath, &findData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        printf("Error: %lu\n", GetLastError());
        return;
    }
    
    u32 fileCount = 0;
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            fileCount++;
        }
    } while (FindNextFileA(findHandle, &findData));
    
    FindClose(findHandle);
    
    // Allocate memory for files
    dir->files = xnalloc(fileCount, XFile);
    dir->fileCount = fileCount;
    
    // Reset findHandle
    findHandle = FindFirstFileA(searchPath, &findData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        printf("Error: %lu\n", GetLastError());
        return;
    }
    
    u32 fileIndex = 0;
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            // xstrcpy(dir->files[fileIndex].name, findData.cFileName);
            dir->files[fileIndex].exists = true;
            dir->files[fileIndex].size = findData.nFileSizeLow;
            dir->files[fileIndex].bytes = NULL; // You can load the file bytes later using xwin_file_read()
            fileIndex++;
        }
    } while (FindNextFileA(findHandle, &findData));
    
    FindClose(findHandle);
}

void xwin_directory_close(XDirectory *dir) {
    if (dir->files != NULL) {
        free(dir->files);
        dir->files = NULL;
    }
    dir->fileCount = 0;
}

/*  Copies the exe path until last slash
    c:/my/path/to/the/app/main.exe
                         ^                            */
void xwin_path_abs(char *dst, u32 dstsize, char *filename)
{
    char *slashpos, *at, exepath[MAX_PATH]={0}, dir[260]={0};
    
    xwin_path(exepath, MAX_PATH);
    
    slashpos = 0;
    at = exepath;
    while (*at++)
        if (*at == '\\' || *at == '/')
        slashpos = at;
    
    xstrncpy(dir, exepath, (u32)(slashpos - exepath));
    snprintf(dst, _TRUNCATE, "%s\\%s", dir, filename);
}

#endif