#ifndef XLIB_LIST
#define XLIB_LIST

#include <stdarg.h>

#include "xmemory.h"

#define T List_T
#define T_node List_node

typedef struct T T;
typedef struct T_node T_node;

T List_empty();
T List_list(void *data, ...);
void *List_i(T list, s32 index);
void **List_ia(T list, s32 index);
void List_push_front(T *list, void *data);
void *List_pop_front(T *list);
void List_push_back(T *list, void *data);
void List_reverse(T *list);

/* End of Interface */







/* Intentional white space */







/* Implementation */

struct T_node 
{
	void *data;
	T_node *next;
};

struct T
{
	T_node *head;
	T_node *tail;
    s32 count;
};

T List_empty()
{
	T r = {0};
	r.tail = r.head = 0;
	r.count = 0;
	return r;
}

void List_free(T l)
{
	T_node *i, *n;
    
	if (l.count>0)
		for (i=l.head; i; )
    {
        n=i->next;
        xfree(i);
        i=n;
    }
	l.count=0;
	l.head=l.tail=0;
}

T List_list(void *data, ...)
{
	T r = List_empty();
    
	va_list ap;
    
	va_start(ap, data);
	for ( ; data; data=va_arg(ap, void *))
		List_push_back(&r, data);
	va_end(ap);
	return r;
}

void *List_i(T l, s32 i)
{
	T_node *j;
	int k;
    
	assert(l.head && l.tail);
    
	for (j=l.head, k=0; j && k<i; j=j->next, ++k)
		;
	if (k==i)
		return j->data;
	else
		return 0;
}

void **List_ia(T l, s32 i)
{
	T_node *j;
	int k;
    
	assert(l.head && l.tail);
    
	for (j=l.head, k=0; j && k<i; j=j->next, ++k)
		;
	if (k==i)
		return &j->data;
	else
		return 0;
}

void List_push_front(T *l, void *d)
{
	T_node *n = xalloc(sizeof *n);
	n->data = d;
	if (!l->head)
		l->tail = l->head = n;
	else
	{
		n->next = l->head;
		l->head = n;	
	}
	++l->count;
}

void *List_pop_front(T *l)
{
	T_node *n;
	void *r;
    
	r=0;
	if(l->head && l->tail) {
		r=l->head->data;
		n=l->head->next;
		xfree(l->head);
		l->head=n;
		--l->count;
	}
    
	return r;
}

void List_push_back(T *l, void *d)
{
	T_node *n = xalloc(sizeof *n);
    
	n->data = d;
	n->next = 0;
	if (!l->tail)
		l->tail = l->head = n;
	else
	{
		l->tail->next = n;
		l->tail = n;
	}
	++l->count;
}

void List_reverse(T *l)
{
	T o = *l;
	void *d;
    
	*l = List_empty();
	while (o.count>0)
	{
		d = List_pop_front(&o);
		List_push_front(l, d);
	}
}

#undef T
#undef T_node

#endif