#ifndef XLIB_STRING
#define XLIB_STRING

#include "xmath.h"

#include <tchar.h>

#define T wchar_t

T *  xstrnew(T *str);
void xstrcpy(T *dest, u32 destsize, T *src);
void xstrcps(T *dest, u32 destsize, T *src, u32 size);
bool xstrcmp(T *stra, T *strb);
s32  xstrlen(T *str);
void xstrcat(T *dest, u32 destsize, T *b);
T   *xstrint(int v);
T   *xstrdbl(double value);

bool xstrcmpascii(char *stra, char *strb);
void xstrcpsascii(char *dest, u32 destsize, char *src, u32 size);
T   *xstrascii   (char *ascii);

/* End of Interface */






/* Intended white space */







/* Implementation */

T *xstrnew(T *str)
{
    s32 length = xstrlen(str)+1;
    T *result = xalloc(length*sizeof(T));
    
    xstrcpy(result, length, str);
    
    // xcopy(result, str, length*sizeof(T));
    
    result[length-1] = 0;
    return result;
}

void xstrcpy(T *dest, u32 destLength, T *src)
{
    assert(dest);
    assert(destLength > 0);
    assert(src);
    
    wcscpy_s(dest, destLength, src);
}

void xstrcps(T *dest, u32 destLength, T *src, u32 copyLength)
{
    wcsncpy_s(dest, destLength, src, copyLength);
}

bool xstrcmp(T *a, T *b)
{
    return (wcscmp(a, b) == 0);
}

bool xstrcmpascii(char *a, char *b)
{
    return (strcmp(a, b) == 0);
}

s32 xstrlen(T *s)
{
    return (u32)wcslen(s);
}

void xstrcat(T *dest, u32 destSize, T *b)
{
    wcscat_s(dest, destSize, b);
}

void xstrcpsascii(char *dest, u32 destSize, char *src, u32 copySize)
{
    strncpy_s(dest, destSize, src, copySize);
}

T *xstrfromascii(char *as)
{
    T *r;
    s32 l;
    
    l = (s32)strlen(as)+1;
    r = xnalloc(l, T);
    MultiByteToWideChar(CP_ACP, 0, as, -1, r, l);
    return r;
}

char *xstrtoascii(wchar_t *string)
{
    int sz;
    char *r;
    
    sz = 0;
    sz = WideCharToMultiByte(CP_ACP, 0, string, -1, NULL, 0, NULL, NULL);
    r = (char *)xalloc(sz);
    memset(r, 0, sz);
    WideCharToMultiByte(CP_ACP, 0, string, -1, r, sz, NULL, NULL);
    return r;
}


T *xstrfromint(int v)
{
    T *r;
    char s[50];
    
    sprintf_s(s, 50, "%02d", v);
    r = xstrfromascii(s);
    return r;
}

T *xstrfromdbl(double v)
{
    T *r;
    char s[50];
    
    sprintf_s(s, 50, "%.6f", v);
    r = xstrfromascii(s);
    return r;
}

#undef T

#endif