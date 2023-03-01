#ifndef ADT_STACK
#define ADT_STACK

#include <malloc.h>
#include <assert.h>

#define T Stack_T

typedef struct T T;

T Stack_new(s32 length, s32 stride);
void Stack_free(T* stk);
T Stack_copy(T stk);
void Stack_resize(T* stk, s32 newsize);


s32 Stack_length(T stk);
void Stack_push(T* stk, void *data);
void *Stack_pop(T* stk);
void *Stack_peek(T stk);

void* Stack_get(T stk, s32 i);

/* End of Interface */







/* Intentional white space */







/* Implementation */

struct T
{
	s32 size;
	s32 stride;
	u8* storage;
	s32 top;
};

T Stack_new(s32 l, s32 s)
{
	T r;

	r.top = 0;
	r.size = l * s;
	r.stride = s;
	r.storage = xalloc(r.size);
	return r;
}

void Stack_free(T *s)
{
	assert(s->storage);
	xfree(s->storage);
	s->size = 0;
	s->stride = 0;
	s->top = 0;
}

T Stack_copy(T s)
{
	T r;
	s32 i;

	r.size = s.size;
	r.stride = s.stride;
	r.storage = xalloc(r.size);
	for (i = 0; i < s.top; ++i)
		Stack_push(&r, Stack_get(s, i));
	return r;
}

void Stack_resize(T* s, s32 ns)
{
	T r;
	s32 i;

	r.top = s->top;
	r.size = ns;
	r.stride = s->stride;
	r.storage = xalloc(r.size);

	for (i = 0; i < s->top; ++i)
		xcopy(r.storage + i*s->stride, s->storage + i*s->stride, s->stride);

	Stack_free(s);

	*s = r;
}

void Stack_push(T *s, void *d)
{
	s32 i;

	i = s->top;
	if (i == Stack_length(*s)-1)
		Stack_resize(s, s->size * 2);
	xcopy(s->storage + i * s->stride, d, s->stride);
	++s->top;
}

void *Stack_pop(T *s)
{
	assert(s->top>0);
	return s->storage + (--s->top) * s->stride;
}

void *Stack_peek(T s)
{
	return s.storage + (s.top-1) * s.stride;
}

void* Stack_get(T s, s32 i)
{
	return s.storage + i * s.stride;
}

s32 Stack_length(T s)
{
	assert(s.size >= 0 && s.stride > 0 && s.stride <= s.size);
	return s.size / s.stride;
}

#undef T
#undef T_node

#endif