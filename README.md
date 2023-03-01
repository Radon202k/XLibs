# XLibs
Extra libraries for C language (In Progress)

## Dependencies ##
* stb_image.h https://github.com/nothings/stb

## Bare bones example ##

## Basic usage ##
### build.bat ###
```batch
@echo off

set warn=-wd4189 -wd4100 -wd4201 -wd4042 -wd4115

IF NOT EXIST bin mkdir bin

cls

pushd bin
cl -Z7 -FC -W4 %warn% -nologo ../main.c
popd
```
### main.c ###
```c
#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xrender.h"

global XFont font;
global XSprite white;
global Stack_T *batch;

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

XWINMAIN()
{
    XRendConfig rendconfig = {
        window_proc, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0,
        L"My window", eme4f, ini2f(0,0), ini2f(400,400),
        true, 1024, 10000, 4096, 256,
    };
    xrendinit(rendconfig); // Init XRender
    
    XWinConfig winconfig = {
        LoadCursor(NULL, IDC_ARROW),
        xrend.wh, xrend.wd, xrend.td,
    };
    xwininit(winconfig);   // Init XWindows
    
    // Load resources
    font = xfont(L"fonts/inconsolata.ttf", L"Inconsolata", 32);
    white = xspritepng(L"images/white.png", false);
    batch = xbatch(4096);
    
    // Main loop
    while (xrend.run)
    {
        // Handle Windows messages
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        
        // Beginning of frame
        
        // Do stuff ...
        
        xline(ini2f(0,0), xwin.mouse.pos, cob4f, 0);
        
        // Do stuff ...
        
        // End of frame
        
        xwinupdate();  // Update XWindows
        xrendupdate(); // Update XRender
    }
    
    xfontfree(font); // Free XFont
    xrendfini();     // Free XRender resources
}

void xrendresized(void)
{
}

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message) {
    	/* XWNDPROC macro handles the following messages: WM_SETCURSOR, WM_DESTROY, WM_CLOSE
    	   WM_CHAR, WM_MOUSEWHEEL, WM_KEYDOWN, WM_SYSKEYDOWN, WM_KEYUP, WM_SYSKEYUP, WM_LBUTTONDOWN
    	   WM_RBUTTONDOWN, WM_LBUTTONUP, WM_RBUTTONUP. Which means you can't redefine them. If you
    	   need to, look in xwindows.h to see the definition of XWNDPROC so you can copypaste that
    	   and handle the messages yourself.  */
        XWNDPROC;

        // Handle other messages
        
        default:
        {
            result = DefWindowProcW(window, message, wParam, lParam);
        } break;
    }
    return result;
}

```

## API Reference ##

### Main ###
```c
void xinitialize (void);
void xshutdown   (void);
void xconfig     (void);
void xupdate     (void);
void xresized    (void);
```
### Config ###
```c
void xwindow   (s32 x, s32 y, s32 w, s32 h, wchar_t *title);
void xwinstyle (u32 style, u32 exstyle);
void xtopdown  (bool topdown);
void xclear    (v4f color);
v2f  xmonitor  (void);
```
### Rendering ###
```c
Stack_T* xbatch       (void);
u8*      xatlasbytes  (void);
XSprite  xspritebytes (u8* bytes, s32  width, s32 height);
XSprite  xspritepng   (wchar_t* path, bool premul);
XFont    xfont        (wchar_t* path, wchar_t *name, s32 height);
XSprite  xglyphsprite (XFont font, wchar_t *c, rect2f *tightbounds, s32 *tightdescent);
s32      xfontheight  (s32 pointheight);
void     xfontfree    (XFont font);
v2f      xspritesize  (XSprite sprite);
void     xatlasupdate (u8* data);
v2f      xstringsize  (XFont font, wchar_t* str);
v2f      xglyphsize   (XFont font, u32 unicode);
```
### Drawing ###
```c
void xline     (v2f a, v2f b, v4f color, f32 sort);
void xlinerect (v2f pos, v2f dim, v4f color, f32 sort);
void xrect     (Stack_T* batch, XSprite sprite, v2f pos, v2f dim, v4f color, f32 sort);
void xsprite   (Stack_T* batch, XSprite sprite, v2f pos, v2f dim, v4f color, f32 sort);
f32  xglyph    (Stack_T* batch, XFont font, u32 unicode, v2f pos, v4f color, f32 sort);
f32  xstring   (Stack_T* batch, XFont font, wchar_t *s, v2f pos, v4f color, f32 sort, bool fixedWidth);
```
### File IO ###
```c
XFile xfileread  (wchar_t *path);
bool  xfilewrite (wchar_t *path, wchar_t *data, u32 size);
u8*   xpng       (wchar_t *path, u32 *width, u32 *height, bool premul);
```
### GUI ###
```c
bool xdraggedhandle (v2 p, f32 maxDist, void *address, bool *hover, v2 *delta);
```
### Other ###
```c
void xcbcopy  (wchar_t *text);
s32  xcbpaste (wchar_t *text, int maxLength);
void xpath    (wchar_t *path, u32 size);
void xpathabs (wchar_t *dest, u32 destSize, wchar_t *fileName);

void xpathascii    (char *path, u32 size);
void xpathabsascii (char *dest, u32 destSize, char *fileName);
```

## Drawing strokes example ##
TODO: Update code (old not working)
```c
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "xlibs\xrender.h"

global XSprite white, circle128;
global XFont font;
global Stack_T *batch, strokes;
global v2f dragp;
global bool instroke;

void xconfig()
{
	xwindow(0, 0, 800, 600, L"My window");
	xclear(ini4f(.2f,.2f,.2f,1));
}

void xinitialize()
{
	HCURSOR ca;
    
	ca = LoadCursor(NULL, IDC_ARROW);
	SetCursor(ca);
	
	white = xspritepng(L"images/white.png", false);
	circle128 = xspritepng(L"images/circle128.png", false);
    
	font = xfont(L"fonts/Inconsolata-Regular.ttf", L"Inconsolata", 32);
    
	strokes = Stack_new(8, sizeof(Stack_T));
	batch = xbatch(4096);
}

void xupdate()
{
	v2f a, b;
	s32 i;
	XMouse m;
    
	m = ctx.mouse;
    
	if (!m.dragging && m.left.pressed)
		dragp = m.pos;
    
	if (!ctx.key.control.down && m.dragging) {
		if (!instroke) {
			Stack_T newstack = {0};
			Stack_push(&strokes, &newstack);
			instroke = true;
		}
		v2f diff = sub2f(m.pos, dragp);
        
		if (len2f(diff)>40) {
			Stack_T *top = Stack_peek(strokes); 
			if (top->storage == 0)
				*top = Stack_new(8, sizeof(v2f));
			Stack_push(top, &m.pos);
			dragp = m.pos;
		}
		xsprite(batch, circle128, dragp, ini2f(5,5), ini4f(1,0,0,1), ini2f(0,0), 0, 0);
        xsprite(batch, circle128, m.pos, ini2f(5,5), ini4f(0,1,0,1), ini2f(0,0), 0, 0);
	}
	else
		instroke = false;
	
	if (ctx.key.control.down && m.left.pressed) {
		Stack_T *top = Stack_peek(strokes);
		if (top->top>0)
			Stack_pop(top);
		else
		{
			Stack_T *deleted = Stack_pop(&strokes);
			assert(deleted == top);
			Stack_free(deleted);
			top = Stack_peek(strokes);
			if (top->top>0)
				Stack_pop(top);
		}
	}
	for (s32 j=0; j<strokes.top; ++j) {
		Stack_T *s = Stack_get(strokes, j);
		for (i=0; i<s->top-1; ++i) {
			a = *((v2f *)(Stack_get(*s, i)));
			b = *((v2f *)(Stack_get(*s, i+1)));
			xstroke(a, b, ini4f(1,1,1,1), 5, 0);
		}
	}
	wchar_t *s = xstrfromint(Alloc_count);
	xstring(batch, font, s, ini2f(0,0), ini4f(1,0,0,1), ini2f(0,0), 0, 0, true);
	xfree(s);
}

void xresized()
{
	// Window was resized
}

void xshutdown()
{
	for (s32 i=0; i<strokes.top; ++i)
		Stack_free(Stack_get(strokes, i));
	Stack_free(&strokes);
	xfontfree(font);
}
```

TODO: Update all the rest of the APIs to use new names (Old names were mystic inspired)
# Nexus
Memory management library for C

## Dependencies ##
* Arcane (https://github.com/Radon202k/Arcane)

## API Reference ##
```c
void Nex_check(void);

void Alloc_free(void *ptr);
void *Alloc_size(u32 size);
void *Alloc_type(<type>);
void *Alloc_array(u32 count, <type>);

void Clear_size(void *ptr, u32 size);
void Clear_type(void *ptr, <type>);
void Clear_array(void *ptr, u32 count, <type>);

void Copy_size(void *dst, void *src, u32 size);
void Copy_struct(void *dst, void *src, <type>);
void Copy_array(void *dst, void *src, u32 count, <type>);
```

# Arcane
Math library for C (In progress)

## API Reference ##
```c
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint32_t bool;
typedef float f32;
#define false 0
#define true 1
#define global static
#define function static
```
### Array ###
```c
#define Arc_narray(a) (sizeof(a) / sizeof((a)[0]))
```
### Float ###
```c
f32 Arc_lerp(f32 a, f32 t, f32 b);
```
### Vector 2D float ###
```c
typedef struct
{
  f32 x, y;
} v2;

v2 V2(f32 x, f32 y);
v2 V2f(s32 x, s32 y);

v2 v2_add(v2 a, v2 b);
v2 v2_sub(v2 a, v2 b);
v2 v2_mul(f32 k, v2 a);

f32 v2_length(v2 a);
f32 v2_inner(v2 a, v2 b);

v2 v2_lerp(v2 a, f32 t, v2 b);
v2 v2_bezier(f32 x1, f32 y1, f32 x2, f32 y2, f32 t);

f32 v2_length2(v2 a);
```
### Vector 3D float ###
```c
typedef struct
{
    f32 x, y, z;
} v3;
```
### Vector 4D float ###
```c
typedef struct
{
    f32 x, y, z, w;
} v4;
```
### Vector 2D int ###
```c
typedef struct
{
    s32 x, y;
} v2i;
```
### Color float ###
```c
typedef union
{
  f32 data[4];
  struct
  {
    f32 r, g, b, a;
  };
} Color;

Color Color_rgba(f32 r, f32 g, f32 b, f32 a);
Color Color_lerp(Color a, f32 t, Color b);
```
### Rect 2D float ###
```c
typedef struct
{
  v2 min;
  v2 max;
} Rect2;

Rect2 rect2(f32 minX, f32 minY, f32 maxX, f32 maxY);
```
### Matrix 4x4 float ###
```c
typedef struct
{
    f32 data[4][4];
} m4x4;
```

# Sylphic
String library for C (In progress)

# Dependencies #
* Arcane (https://github.com/Radon202k/Arcane)

# API Reference #
```c
void String_copy(T *dest, u32 destSize, T *src);
void String_copy_size(T *dest, u32 destSize, T *src, u32 copySize);
bool String_equal(T *a, T *b);
bool String_equal_ascii(char *a, char *b);
s32 String_length(T *s);
void String_append(T *dest, u32 destSize, T *b);
void String_copy_size_ascii(char *dest, u32 destSize, char *src, u32 copySize);
T *String_from_ascii(char *asciiString);
T *String_from_int(int v);
T *String_from_double(double value);
```


# ADT
ADTs for C (In progress)

# Dependencies #
* Arcane - Math (https://github.com/Radon202k/Arcane)
* Nexus - Memory management (https://github.com/Radon202k/Nexus)
* Sylphic - String (https://github.com/Radon202k/Sylphic)

# API Reference #

## List ##
```c
typedef struct List_node List_node;
struct List_node {
	void *data;
	List_node *next;
};
struct List_T {
	List_node *head;
	List_node *tail;
	s32 count;
};

List_T List_empty ();
List_T List_list (void *data, ...);
void *List_i (List_T list, s32 index);
void **List_ia (List_T list, s32 index);
void List_push_front (List_T *list, void *data);
void *List_pop_front (List_T *list);
void List_push_back (List_T *list, void *data);
void List_reverse (List_T *list);
```
## Table ##
```c
typedef struct Table_T * Table_T;
typedef struct Table_node * Table_node;
struct Table_node {
	void *key;
	void *value;
	Table_node next;
};
struct Table_T {
	s32 size;
	Table_node *storage;
	bool (*cmp)(void *a, void *b);
	u32 (*hash)(void *k);
};

Table_T Table_new (s32 size, bool cmp(void *a, void *b), u32 hash(void *key));
void Table_free (Table_T table);

void  Table_set (Table_T table, void *key, void *value);
void *Table_get (Table_T table, void *key);
void  Table_del (Table_T table, void *key);
```
## Stack ##
```c
typedef struct Stack_T * Stack_T;
typedef struct Stack_node Stack_node;
struct Stack_node {
	void *data;
	Stack_node *next;	
};
struct Stack_T {
	int count;
	Stack_node *top;
};

Stack_T Stack_new();
void Stack_free(Stack_T stack);
void Stack_push(Stack_T stack, void *data);
void *Stack_pop(Stack_T stack);
void *Stack_peek(Stack_T stack);
```