#ifndef XLIB_STRING
#define XLIB_STRING

#include "xmath.h"

#ifdef _WIN32
#include <tchar.h>
#endif

#define T char

T *  xstrmake (T *str);
void xstrcpy  (T *dest, T *src);
void xstrncpy (T *dest, T *src, u32 size);
bool xstrcmp  (T *stra, T *strb);
s32  xstrlen  (T *str);
void xstrcat  (T *dest, u32 destsize, T *b);
T   *xstrint  (s32 v, u32 zeroCount);
T   *xstrdbl  (double value);

/* End of Interface */






/* Intended white space */







/* Implementation */

T *xstrnew(T *str)
{
    s32 length = xstrlen(str)+1;
    T *result = xalloc(length*sizeof(T));
    
    xstrcpy(result, str);
    
    // xcopy(result, str, length*sizeof(T));
    
    result[length-1] = 0;
    return result;
}

void xstrcpy(T *dest, T *src)
{
    assert(dest);
    assert(src);
    
    xcopy(dest, src, xstrlen(src)+1);
}

void xstrncpy(T *dest, T *src, u32 copyLength)
{
    xcopy(dest, src, copyLength);
}

bool xstrcmp(T *a, T *b)
{
    return (strcmp(a, b) == 0);
}

bool xstrcmpascii(char *a, char *b)
{
    return (strcmp(a, b) == 0);
}

s32 xstrlen(T *s) {
    return (u32)strlen(s);
}

void xstrcat(T *dest, u32 destSize, T *b)
{
    strcat(dest, b);
}

T *xstrint(s32 v, u32 zeroCount)
{
    T *r;
    char s[50];
    if (zeroCount == 0)
        snprintf(s, 50, "%d", v);
    else if (zeroCount == 1)
        snprintf(s, 50, "%01d", v);
    else if (zeroCount == 2)
        snprintf(s, 50, "%02d", v);
    else if (zeroCount == 3)
        snprintf(s, 50, "%03d", v);
    r = xstrnew(s);
    return r;
}

T *xstrfromdbl(double v)
{
    T *r;
    char s[50];
    
    snprintf(s, 50, "%.6f", v);
    r = xstrnew(s);
    return r;
}

#undef T

#endif