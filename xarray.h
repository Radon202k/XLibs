#ifndef XLIB_ARRAY
#define XLIB_ARRAY

#define T Array_T
typedef struct T T;

struct T
{
	s32 size;
	s32 stride;
	u8* storage;
};

T Array_new(s32 length, s32 stride);
void Array_free(T *arr);
void Array_set(T arr, s32 i, void* data);
void* Array_get(T arr, s32 i);

T Array_copy(T arr);
void Array_resize(T* arr, s32 newsize);
s32 Array_length(T arr);
void* Array_find(T arr, bool cmp(void *elem));
// T Array_slice(T arr, s32 start, s32 end);
// s32 Array_unshift(T arr, void* x, ...);
// void Array_reverse(T arr);
// void Array_reverse_range(T arr, s32 a, s32 b);

/* End of Interface */







/* Intentional white space */







/* Implementation */

T Array_new(s32 l, s32 s)
{
	T r;
    
	r.size = l * s;
	r.stride = s;
	r.storage = xalloc(r.size);
	return r;
}

void Array_free(T* a)
{
	xfree(a->storage);
	a->size = 0;
	a->stride = 0;
}

void* Array_find(T a, bool cmp(void* e))
{
	s32 i;
    
	for (i = 0; i < Array_length(a); ++i)
		if (cmp(Array_get(a, i)))
        return Array_get(a, i);
	return 0;
}

void Array_set(T a, s32 i, void* d)
{
	assert(i >= 0 && i < Array_length(a));
	xcopy(a.storage + i*a.stride, d, a.stride);
}

void* Array_get(T a, s32 i)
{
	assert(i >= 0 && i < Array_length(a));
	return a.storage + i*a.stride;
}

T Array_copy(T a)
{
	T r;
	s32 i;
    
	r.size = a.size;
	r.stride = a.stride;
	r.storage = xalloc(r.size);
	for (i = 0; i < Array_length(a); ++i)
		Array_set(r, i, Array_get(a, i));
    return r;
}

void Array_resize(T* a, s32 ns)
{
	T r;
    
	r.size = ns;
	r.stride = a->stride;
	r.storage = xalloc(r.size);
	r=Array_copy(*a);
    
	Array_free(a);
    
	*a = r;
}

s32 Array_length(T a)
{
	assert(a.size >= 0 && a.stride >= 0 && a.stride <= a.size);
	return a.size / a.stride;
}

#undef T

#endif