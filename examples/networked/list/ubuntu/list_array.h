/* date = April 19th 2023 4:16 am */

#ifndef LIST_ARRAY_H
#define LIST_ARRAY_H

typedef struct List_Array {
    u32 size;
    u32 length;
    List_data **storage;
    u32 hotIndex;
    u32 activeIndex;
    v2f origin;
    v2f drawDim;
    v2f elDim;
    f32 margin;
} List_Array;

function void       list_array_destruct  (List_Array *list);
function void       list_array_construct (List_Array *list, u32 size);
function List_data *list_array_push      (List_Array *list, List_data *data);
function void       list_array_move      (List_Array *list, u32 fromIndex, u32 toIndex);
function void       list_array_remove    (List_Array *list, u32 removeIndex);

#endif //LIST_ARRAY_H
