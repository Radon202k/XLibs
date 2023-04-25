/* date = April 21st 2023 6:23 am */

#ifndef LIST_LINKED_H
#define LIST_LINKED_H

typedef struct List_data {
    char text[256];
    u32 textLength;
    v4 color;
    v2 drawP;
    struct List_linked_node *node;
} List_data;

typedef struct List_linked_node {
    List_data *data;
    struct List_linked_node *prev;
    struct List_linked_node *next;
} List_linked_node;

typedef struct List_linked {
    List_linked_node *head;
    List_linked_node *tail;
    u32 length;
    List_linked_node *hot;
    List_linked_node *active;
    v2 origin;
    v2 drawDim;
    v2 elDim;
    f32 margin;
} List_linked;

static void              list_linked_destruct  (List_linked *l);
static void              list_linked_construct (List_linked *l);
static List_linked_node *list_linked_push      (List_linked *l, List_data *data);
static void              list_linked_move      (List_linked *l, 
                                                List_data *el, List_data *ref);
static void              list_linked_remove    (List_linked *l, List_data *node);

#endif //LIST_LINKED_H
