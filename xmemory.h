#ifndef XLIB_MEMORY
#define XLIB_MEMORY

#include "xbase.h"

void xmemcheck(void);

#define xfree(ptr) safe_free(ptr, __FILE__, __LINE__)
#define xalloc(size) safe_alloc(size, __FILE__, __LINE__)
#define xnalloc(count, type) (type *)safe_alloc((count)*sizeof(type), __FILE__, __LINE__)

#define xcopy(dst, src, size) safe_copy(dst, src, size)
#define xncopy(dst, src, count, type) safe_copy(dst, src, (count)*sizeof(type))

#define XMAX_SAFE_ALLOC 2048

/* End of Interface */







/* Intentional white space */







/* Implementation */

typedef struct Alloc_entry
{
    int size;
    char *file;
    int line;
    void *address;
} Alloc_entry;

static Alloc_entry Alloc_list[XMAX_SAFE_ALLOC];
static int Alloc_count;

void Alloc_list_append(int size, char* file, int line, void *address)
{
    Alloc_entry* entry;
    
    assert(Alloc_count < XMAX_SAFE_ALLOC);
    
    entry = Alloc_list + Alloc_count++;
    entry->size = size;
    entry->file = file;
    entry->line = line;
    entry->address = address;
}

Alloc_entry *Alloc_list_find(void *ptr)
{
    int i, f;
    Alloc_entry* n;
    
    n = 0;
    f = 0;
    for (i = 0; i < Alloc_count; ++i)
    {
        n = Alloc_list + i;
        if (n->address == ptr)
        {
            f = 1;
            break;
        }
        else if (i > 10000)
            assert(!"Wrong free");
    }
    
    if (f == 1)
        return n;
    else
        return 0;
}

void Alloc_list_remove(Alloc_entry *n)
{
    Alloc_entry* le;
    
    le = Alloc_list + (--Alloc_count);
    
    *n = *le;
    
    le->size = 0;
    le->file = 0;
    le->line = 0;
    le->address = 0;
}

void xmemcheck(void)
{
    assert(Alloc_count==0);
}

void *safe_alloc(int size, char *file, int line)
{
    void *result;
    
    result = malloc(size);
    memset(result, 0, size);
    
    Alloc_list_append(size, file, line, result);
    return result;
}

void safe_free(void *ptr, char *file, int line)
{
    char out[256];
    Alloc_entry *n;
    
    assert(ptr!=0);
    assert(Alloc_count>0);
    
    n = Alloc_list_find(ptr);
    if (n == 0)
    {
        sprintf_s(out, 256, "Called Alloc_free on a non allocted pointer at %s line %d.\n", file, line);
        assert(!"Wrong free");
    }
    else
    {
        Alloc_list_remove(n);
        free(ptr);
    }
}

void safe_clear(void *ptr, int size)
{
    memset(ptr, 0, size);
}

void safe_copy(void *dst, void *src, int size)
{
    memcpy(dst, src, size);
}

#endif