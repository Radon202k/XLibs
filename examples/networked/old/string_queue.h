#ifndef STRING_QUEUE_H
#define STRING_QUEUE_H

/* NOTE: FILO Queue */

typedef struct StringQueueNode
{
    struct StringQueueNode *prev;
    struct StringQueueNode *next;
    String data;
} StringQueueNode;

typedef struct
{
    StringQueueNode *head;
    StringQueueNode *tail;
    u32 count;
} StringQueue;

function void   string_queue_free   (StringQueue *queue);
function void   string_queue_add    (StringQueue *queue, char *data);
function String string_queue_remove (StringQueue *queue);







function void
string_queue_free(StringQueue *queue) {
    StringQueueNode *at = queue->head;
    u32 i=0;
    while (at) {
        char buf[1024];
        sprintf_s(buf, 1024, "%.*s: %.*s\n", 
                  6, "Server", 
                  at->data.length, at->data.storage);
        
        StringQueueNode *next = at->next;
        
        xfree(at->data.storage);
        xfree(at);
        
        at = next;
    }
}

/* Adds a string to the end of the queue */
function void
string_queue_add(StringQueue *queue, char *data) {
    /* Allocate the new node */
    StringQueueNode *newNode = xalloc(sizeof *newNode);
    newNode->prev = 0;
    newNode->next = 0;
    newNode->data = string_make(data);
    
    if (queue->count == 0) {
        /* Make it both head and tail */
        queue->head = newNode;
        queue->tail = newNode;
    }
    else {
        /* Push it to the end of the list */
        newNode->prev = queue->tail;
        if (queue->tail)
            queue->tail->next = newNode;
        queue->tail = newNode;
    }
    
    ++queue->count;
}

/* Removes a string from the start and returns it */
function String
string_queue_remove(StringQueue *queue) {
    String r = {0};
    
    /* If the queue is not empty */
    if (queue->count > 0) {
        /* Get the head of the queue */
        StringQueueNode *node = queue->head;
        r = node->data;
        
        /* If this is the only stringin the queue */
        if (queue->count == 1) {
            /* Make the queue empty */
            queue->head = 0;
            queue->tail = 0;
        }
        else {
            /* Remove the node from the queue */
            queue->head->next->prev = 0;
            queue->head = queue->head->next;
        }
        
        /* Free the node */
        xfree(node);
        
        --queue->count;
    }
    
    return r;
}

#endif
