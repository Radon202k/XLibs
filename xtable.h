#ifndef XLIB_TABLE
#define XLIB_TABLE

#include "xmemory.h"

#define T Table_T
#define T_node Table_node

typedef struct T * T;
typedef struct T_node * T_node;

T     Table_new (s32 size, bool cmp(void *a, void *b), u32 hash(void *key));
void  Table_free (T table);

void  Table_set (T table, void *key, void *value);
void *Table_get (T table, void *key);
void  Table_del (T table, void *key);

/* End of Interface */







/* Intentional white space */







/* Implementation */

struct T_node
{
	void *key;
	void *value;
	T_node next;
};

struct T
{
	s32 size;
	T_node *storage;
	bool (*cmp)(void *a, void *b);
	u32 (*hash)(void *k);
};

T Table_new(s32 sz,	bool cmp(void *a, void *b),	u32 hash(void *k))
{
	T r;
    
	r=xalloc(sizeof *r);
	r->size=sz;
	r->storage=xnalloc(sz, T_node);
	r->cmp=cmp;
	r->hash=hash;
	return r;
}

void Table_free(T t)
{
	s32 i;
	Table_node m, n;
    
	for (i=0; i<t->size; ++i)
		for (m=t->storage[i]; m; m=n)
    {
        n=m->next;
        xfree(m);
    }
    
	xfree(t->storage);
	xfree(t);
}

void Table_set(T t, void *k, void *v)
{
	u32 h;
	T_node i;
    
	h=t->hash(k);
	i=xalloc(sizeof *i);
	i->key=k;
	i->value=v;
	i->next=t->storage[h];
	t->storage[h]=i;
}

void *Table_get(T t, void *k)
{
	u32 h;
	T_node i;
    
	h=t->hash(k);
	for (i=t->storage[h]; i; i=i->next)
		if (t->cmp(i->key, k))
        break;
	if (i)
		return i->value;
	else
		return 0;
}

void Table_del(T t, void *k)
{
	u32 h;
	T_node *p, n, l;
    
	l=0;
	h=t->hash(k);
	for (p=t->storage+h; *p; )
	{
		n=(*p)->next;
		if (t->cmp((*p)->key, k))
		{
			if (l==0)
				t->storage[h]=n;
			else
				l->next=n;
		}
		l=(*p);
		*p=n;
	}
}


#undef T
#undef T_node

#endif