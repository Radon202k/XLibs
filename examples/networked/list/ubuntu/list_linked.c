
static void 
list_data_destruct(List_data *data) {
    xfree(data);
}

static List_data *
list_data(char *text, v4 color) {
    assert(xstrlen(text) <= 255);
    List_data *result = xalloc(sizeof *result);
    xstrcpy(result->text, text);
    v4_copy(color, result->color);
    return result;
}

static void
list_linked_construct(List_linked *list) {
    list->hot = 0;
    list->active = 0;
    list->length = 0;
}

static void
list_linked_destruct(List_linked *list) {
    for (List_linked_node *at=list->head; at; ) {
        list_data_destruct(at->data);
        List_linked_node *next = at->next;
        xfree(at);
        at = next;
    }
}

static List_linked_node *
list_linked_push(List_linked *list, List_data *data) {
    List_linked_node *node = xalloc(sizeof *node);
    node->data = data;
    data->node = node;
    if (list->length == 0) {
        list->head = node;
        list->tail = node;
    }
    else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    ++list->length;
    
    return node;
}

static void
list_linked_put_after(List_linked *list, List_data *el, List_data *ref) {
    assert(list);
    assert(el);
    assert(ref && ref->node);
    List_linked_node *elNode = xalloc(sizeof *elNode);
    elNode->data = el;
    el->node = elNode;
    
    List_linked_node *refNode = ref->node;
    if (refNode->next)
        refNode->next->prev = elNode;
    
    elNode->next = refNode->next;
    refNode->next = elNode;
    elNode->prev = refNode;
    
    if (refNode == list->tail)
        list->tail = elNode;
    ++list->length;
}

static void
list_linked_move(List_linked *list, List_data *el, List_data *ref) {
    assert(list);
    assert(el);
    assert(ref);
    list_linked_remove(list, el);
    list_linked_put_after(list, el, ref);
}

static void
list_linked_remove(List_linked *list, List_data *el) {
    assert(list);
    assert(el);
    List_linked_node *node = el->node;
    assert(node);
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;
    if (node == list->head)
        list->head = node->next;
    if (node == list->tail)
        list->tail = node->prev;
    --list->length;
    xfree(node);
}
