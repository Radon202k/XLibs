#ifndef XLIB_LIST
#define XLIB_LIST

#include <stdarg.h>

#include "xmemory.h"

/* List_T must hold POINTERS to data. 
  Avoid string literals, use dynamically allocated strings. */

#define T List_T
#define T_node List_node

typedef struct T T;
typedef struct T_node T_node;

T       List_list       (void *data, ...);
void    List_free       (T *list);
void    List_push       (T *list, void *data);
void    List_push_front (T *list, void *data);
void    List_swap       (T *list, T_node *a, T_node *b);
void *  List_remove     (T *list, T_node *node);
void    List_put_before (T *list, void *data, T_node *ref);
void    List_put_after  (T *list, void *data, T_node *ref);
void    List_reverse    (T *list);
T_node *List_find       (T *list, bool (*cmp)(void *a, void *b), void *data);

/* End of Interface */







/* Intentional white space */







/* Implementation */

struct T_node 
{
	void *data;
	T_node *prev;
	T_node *next;
};

struct T
{
	T_node *head;
	T_node *tail;
    s32 count;
};

T List_list(void *data, ...)
{
	T result = {0};
    
	va_list args;
    
	va_start(args, data);
	
    assert(data);
    while (data)
    {
		List_push(&result, data);
        
        data = va_arg(args, void *);
    }
    
    va_end(args);
	
    return result;
}

void List_free(T *list)
{
	assert(list);
    T_node *node, *next;
    
	if (list->count > 0)
    {
        node = list->head;
		while (node)
        {
            next = node->next;
            xfree(node->data);
            xfree(node);
            node = next;
        }
        list->count = 0;
        list->head = list->tail = 0;
    }
}

void List_push(T *list, void *data)
{
	assert(list);
    T_node *node = xalloc(sizeof *node);
    
	node->data = data;
	node->prev = 0;
    node->next = 0;
	
    if (list->count == 0)
    {
        assert(list->tail == 0);
        assert(list->head == 0);
		list->tail = list->head = node;
    }
	else
	{
        node->prev = list->tail;
		list->tail->next = node;
		list->tail = node;
	}
	++list->count;
}

void List_push_front(T *list, void *data)
{
	assert(list);
    T_node *node = xalloc(sizeof *node);
	node->data = data;
    node->prev = 0;
    node->next = 0;
    
	if (list->count == 0)
    {
		list->tail = list->head = node;
    }
	else
	{
		node->next = list->head;
		list->head->prev = node;
        list->head = node;
	}
	++list->count;
}

void List_swap(T *list, T_node *a, T_node *b)
{
    assert(list);
    assert(a);
    assert(b);
    
    // Switch a pointers
    if (a->prev)
        a->prev->next = b;
    if (a->next)
        a->next->prev = b;
    
    // Switch b pointers
    if (b->prev)
        b->prev->next = a;
    if (b->next)
        b->next->prev = a;
    
    // If a is head or tail, switch to b
    if (list->head == a)
        list->head = b;
    else if (list->tail == a)
        list->tail = b;
    
    // If b is head or tail, switch to a
    if (list->head == b)
        list->head = a;
    else if (list->tail == b)
        list->tail = a;
}

void *List_remove(T *list, T_node *node)
{
    assert(list);
    assert(node);
    assert(list->count > 0);
    
    // Update node pointers
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;
    
    // If node is head, update it
    if (list->head == node)
        list->head = node->next;
    
    // If node is tail, update it
    if (list->tail == node)
        list->tail = node->prev;
    
    // Grab data to return
    void *result = node->data;
    
    // Free node
    xfree(node);
    
    --list->count;
    
    return result;
}

void List_put_before(T *list, void *data, T_node *ref)
{
    assert(list);
    assert(data);
    assert(ref);
    
    // If ref is the head of the list, put data as new head
    if (list->head == ref)
    {
        List_push_front(list, data);
    }
    // Else, put data before ref
    else
    {
        T_node *node = xalloc(sizeof *node);
        node->data = data;
        node->prev = ref->prev;
        node->next = ref;
        ref->prev->next = node;
        ref->prev  = node;
        ++list->count;
    }
}

void List_put_after(T *list, void *data, T_node *ref)
{
    assert(list);
    assert(data);
    assert(ref);
    
    // If ref is the head of the list, put data as new head
    if (list->tail == ref)
    {
        List_push(list, data);
    }
    // Else, put data after ref
    else
    {
        T_node *node = xalloc(sizeof *node);
        node->data = data;
        node->next = ref->next;
        if (ref->next)
            ref->next->prev = node;
        ref->next = node;
        node->prev = ref;
        ++list->count;
    }
}

void List_reverse(T *list)
{
	T temp = *list;
	void *data;
    
	*list = (List_T){0};
	
    while (temp.count > 0)
	{
		data = List_remove(&temp, temp.head);
		List_push_front(list, data);
	}
}

T_node *List_find(T *list, bool (*cmp)(void *a, void *b), void *data)
{
    assert(list);
    assert(data);
    assert(cmp);
    
    T_node *node = list->head;
    while (node)
    {
        if (cmp(node->data, data))
        {
            return node;
        }
        
        node = node->next;
    }
    return 0;
}

/* Tests 

state.myList = List_list(xstrnew(L"String"),
                             xstrnew(L"List"),
                             xstrnew(L"Relatively"),
                             xstrnew(L"Easy"),
                             NULL);
    
    List_reverse(&state.myList);
    
    List_node *temp = List_find(&state.myList, 
                                cmp_str, 
                                L"List");
    
    List_put_after(&state.myList, 
                   xstrnew(L"Inserted After"),
                   temp);
    
    List_put_before(&state.myList, 
                    xstrnew(L"Inserted Before"),
                    temp);
    
    List_put_before(&state.myList, 
                    xstrnew(L"New Head"),
                    state.myList.head);
    
    List_put_after(&state.myList, 
                   xstrnew(L"New Tail"),
                   state.myList.tail);
    
bool cmp_str(void *a, void *b)
{
    return xstrcmp((wchar_t *)a, (wchar_t *)b);
}

*/

#undef T
#undef T_node

#endif