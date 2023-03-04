#ifndef XLIB_ARRAY
#define XLIB_ARRAY

#define T Array_T
typedef struct T T;

struct T
{
	s32 size, stride, top;
	u8* storage;
};

T     Array_new    (s32 length, s32 stride);
void  Array_free   (T *arr);
void  Array_set    (T arr, s32 i, void* data);
void* Array_get    (T arr, s32 i);
void  Array_push   (T* arr, void* data);
void* Array_pop    (T* arr);
void* Array_remove (T* arr, s32 i);
T     Array_copy   (T arr);
void  Array_resize (T* arr, s32 newsize);
s32   Array_length (T arr);
void* Array_find   (T arr, bool cmp(void *elem));
void  Array_toarray(T arr, void *dst, s32 *n);
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
	r.top = 0;
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
	xcopy((u8 *)a.storage + i*a.stride, d, a.stride);
}

void Array_push(T* arr, void* data)
{
	if (arr->top >= Array_length(*arr) - 2)
		Array_resize(arr, arr->size * 2);
	Array_set(*arr, arr->top++, data);
}

void* Array_pop(T* arr)
{
	assert(arr->top > 0);
	void* r = Array_get(*arr, --arr->top);
	return r;
}

void* Array_remove(T* arr, s32 i)
{
	assert(arr->top > 0);
	void* r = Array_get(*arr, i);
	Array_set(*arr, i, Array_get(*arr, --arr->top));
	return r;
}

void* Array_get(T a, s32 i)
{
	assert(i >= 0 && i < Array_length(a));
	return a.storage + i*a.stride;
}

T Array_copy(T a)
{
	T r;
    
	r.size = a.size;
	r.stride = a.stride;
	r.top = a.top;
	r.storage = xalloc(r.size);
	xcopy(r.storage, a.storage, r.size);
	return r;
}

void Array_resize(T* a, s32 ns)
{
	T r;
    
	r.size = ns;
	r.stride = a->stride;
	r.top = a->top;
	r.storage = xalloc(ns);
	xcopy(r.storage, a->storage, a->size);
	xfree(a->storage);
	*a = r;
}

s32 Array_length(T a)
{
	assert(a.size >= 0 && a.stride >= 0 && a.stride <= a.size);
	return a.size / a.stride;
}

void Array_toarray(T arr, void **dst, s32 *n)
{
	*dst = xalloc(arr.top * arr.stride);
	xcopy(*dst, arr.storage, arr.top * arr.stride);
	*n = arr.top;
}

#undef T

#endif