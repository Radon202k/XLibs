/* date = April 7th 2023 1:28 pm */

#ifndef STRING_H
#define STRING_H

typedef struct
{
    char *storage;
    u32 length;
} String;

typedef struct StringListNode
{
    String data;
    struct StringListNode *next;
    struct StringListNode *prev;
} StringListNode;

typedef struct
{
    u32 count;
    StringListNode *head;
    StringListNode *tail;
} StringList;

function String     string_make  (char *str);
function StringList string_split (char *str, char delimiter);

function void string_list_push (StringList *list, String string);
function void string_list_free (StringList *list);







function String
string_make(char *str) {
    u32 length = (u32)strlen(str);
    String r = {
        xalloc(length+1),
        length
    };
    xcopy(r.storage, str, length);
    r.storage[length]=0;
    return r;
}

function StringList
string_split(char *str, char delimiter) {
    StringList list = {0};
    
    char temp[512] = {0};
    int tempIndex = 0;
    assert(strlen(str) < narray(temp));
    
    char *at  = str;
    while (*at) {
        if (*at == delimiter) {
            string_list_push(&list, string_make(temp));
            
            /* Reset temp buffer char index */
            tempIndex = 0;
        }
        else {
            /* Add next char to temp buffer */
            temp[tempIndex++] = *at;
            temp[tempIndex] = 0;
        }
        
        ++at;
    }
    
    string_list_push(&list, string_make(temp));
    return list;
}

function void 
string_list_push(StringList *list, String string) {
    StringListNode *node = xalloc(sizeof *node);
    node->data = string;
    node->next = node->prev = 0;
    if (list->count == 0) {
        list->head = node;
        list->tail = node;
    }
    else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    ++list->count;
}

function void
string_list_free(StringList *list) {
    StringListNode *at = list->head;
    while (at) {
        StringListNode *next = at->next;
        xfree(at->data.storage);
        xfree(at);
        at = next;
    }
}

#endif //STRING_H
