/* Basic array based list data structure functions */

function void
list_array_construct(List_Array *list, u32 size) {
    list->storage = xalloc(size * sizeof(List_data *));
    list->size = size;
    list->hotIndex = UINT32_MAX;
    list->activeIndex = UINT32_MAX;
}

function void
list_array_destruct(List_Array *list) {
    for (u32 i=0; i<list->length; ++i) 
        list_data_destruct(list->storage[i]);
    
    xfree(list->storage);
}

function List_data *
list_array_push(List_Array *list, List_data *data) {
    assert(list->length < list->size);
    u32 index = list->length++;
    list->storage[index] = data;
    return data;
}

function void
list_array_move(List_Array *list, u32 fromIndex, u32 toIndex) {
    List_data *temp = list->storage[fromIndex];
    if (fromIndex < toIndex) {
        for (u32 i=fromIndex; i<toIndex; ++i)
            list->storage[i] = list->storage[i+1];
    }
    else if (fromIndex > toIndex) {
        for (u32 i=fromIndex; i>toIndex; --i)
            list->storage[i] = list->storage[i-1];
    }
    list->storage[toIndex] = temp;
}

function void
list_array_remove(List_Array *list, u32 removeIndex) {
    list->storage[removeIndex] = 0;
    for (u32 i=removeIndex; i<list->length; ++i)
        list->storage[i] = list->storage[i+1];
    --list->length;
}
