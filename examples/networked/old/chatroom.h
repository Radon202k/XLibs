/* date = April 6th 2023 3:58 pm */

#ifndef CHATROOM_H
#define CHATROOM_H

#define CHATOBJECT_MAX_LENGTH 512

typedef enum {
    NetworkMessageType_null,
    NetworkMessageType_create_obj,
    NetworkMessageType_obj_created,
} NetworkMessageType;

typedef struct {
    NetworkMessageType type;
    u32 dataLength;
} ChatNetworkPacketHeader;

typedef struct {
    u32 objId;
    u32 userId;
    wchar_t *storage;
    u32 length;
} ChatRoomObject;

typedef struct {
    ChatRoomObject *storage[4096];
    u32 index;
} ChatRoom;

global StringQueue chatConsole;
global FILE *logFile;

function void chat_initialize (void);
function void chat_shutdown   (void);

function void chatroom_free       (ChatRoom *room);

function void chatroom_console_print(char *message);

/* The server creates objs incrementing the index and sets as the objId */
function u32  chatroom_obj_server_create(ChatRoom *room, 
                                         u32 userId, wchar_t *message);
/* The client "creates" objs locally based on the obj that was sent by the
;  server. It does not increase the index, instead updates it if objId > index */
function bool chatroom_obj_client_create(ChatRoom *room, u32 objId,
                                         u32 userId, wchar_t *message);

/* Deletion and Update can be done with the same functions, with the implicit
;  difference that the client is either applying the updates that came from the
;  server, or it is applying its own modifications before sending to server */ 
function void chatroom_obj_delete(ChatRoom *room, u32 objId);
function void chatroom_obj_update(ChatRoom *room, u32 objId, wchar_t *message);

/* NOTE/TODO/TRY: We could instead bake into these obj management functions the
;  send functionality so the server doesn't need to do that automatically, but
;  that will further differentiate the functions from client and server. Any way
;  we can try that out later and see what works best. */



function void chat_initialize(void) {
    /* Open the log file */
    fopen_s(&logFile, "test.log", "a");
}

function void chat_shutdown(void) {
    /* Free console */
    string_queue_free(&chatConsole);
}

function void timestamp(char timeStamp[20]) {
    /* Get the current time */
    time_t rawTime;
    struct tm timeInfo;
    time(&rawTime);
    localtime_s(&timeInfo, &rawTime);
    strftime(timeStamp, 20, "%Y-%m-%d %H:%M:%S", &timeInfo);
}

function void
chatroom_console_print(char *message) {
    /* Get timestmap */
    char timeStamp[20];
    timestamp(timeStamp);
    
    /* Save message to the log file */
    if (logFile != NULL)
        fprintf(logFile, "%s %s\n", timeStamp, message);
    
    /* If this is a headless server */
#ifdef CHATROOM_SERVER_HEADLESS
    printf("%s %s\n", timeStamp, message);
#else
    /* Otherwise, it is a server inside a client's window */
    if (chatConsole.count >= CHATROOM_SERVER_CONSOLE_LINES) {
        String removed = string_queue_remove(&chatConsole);
        xfree(removed.storage);
    }
    string_queue_add(&chatConsole, message);
#endif
    
}

function void
chatroom_free(ChatRoom *room) {
    /* Free chat objects */
    for (u32 i=0; i<room->index; ++i) {
        ChatRoomObject *chatObject = room->storage[i];
        if (chatObject) {
            xfree(chatObject->storage);
            xfree(chatObject);
        }
    }
}

function void
chatobj_client_delete(ChatRoom *room, u32 objId) {
    if (objId < narray(room->storage)) {
        room->storage[objId] = room->storage[--room->index];
    }
}

/* NOTE: This function assumes that the client is always creating objs
;        that were sent by the server. The server obj creation function
;        assigns objects incrementing ids, that is the reason that
;        this function can advance room->index based on objId because it
;        is garanteed that no gap exists, the server takes care of that. */
function bool
chatroom_obj_client_create(ChatRoom *room, u32 objId,
                           u32 userId, wchar_t *message) {
    ChatRoomObject *obj = 0;
    /* If id is inside the array */
    if (objId < narray(room->storage)) {
        /* Get either the length of message or maximum length */
        u32 length = min(xstrlen(message), CHATOBJECT_MAX_LENGTH);
        /* Get the object from room */
        room->storage[objId] = xalloc(sizeof *obj);
        obj = room->storage[objId];
        /* Advance room->index if objId is greater */
        room->index = max(room->index, objId+1);
        /* Fill info */
        obj->objId = objId;
        obj->userId = userId;
        obj->storage = xnalloc(length+1, wchar_t);
        obj->storage[length] = 0;
        
        xncopy(obj->storage, message, length, wchar_t);
        obj->length = length;
    }
    return objId;
}

function u32
chatroom_obj_server_create(ChatRoom *room, u32 userId, wchar_t *message) {
    ChatRoomObject *result = 0;
    u32 objId = 0;
    /* If there is space in the room */
    if (room->index < narray(room->storage)) {
        /* Get either the length of message or maximum length */
        u32 length = min(xstrlen(message), CHATOBJECT_MAX_LENGTH);
        /* Get next available object from room */
        objId = room->index++;
        room->storage[objId] = xalloc(sizeof *result);
        result = room->storage[objId];
        /* Fill info */
        result->objId = objId;
        result->userId = userId;
        result->storage = xnalloc(length, wchar_t);
        xncopy(result->storage, message, length, wchar_t);
        result->length = length;
    }
    return objId;
}


#endif //CHATROOM_H
