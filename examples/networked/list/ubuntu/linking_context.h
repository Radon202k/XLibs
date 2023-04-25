/* date = April 21st 2023 0:33 am */

#ifndef LINKING_CONTEXT_H
#define LINKING_CONTEXT_H

typedef struct List_data_to_ID_hash_table_node {
    List_data *key;
    u32 value;
    struct List_data_to_ID_hash_table_node *next;
} List_data_to_ID_hash_table_node;

typedef struct ID_to_List_data_hash_table_node {
    u32 key;
    List_data *value;
    struct ID_to_List_data_hash_table_node *next;
} ID_to_List_data_hash_table_node;

static u32 ID_to_list_data_hash(u32 key);
static u32 List_data_to_ID_hash(List_data *key);


typedef struct LinkingContext {
    List_data_to_ID_hash_table_node *hashTable_Obj_To_ID[4096];
    ID_to_List_data_hash_table_node *hashTable_ID_To_Obj[4096];
    u32 nextNetworkID;
    
} LinkingContext;

global LinkingContext server_link;
global LinkingContext client_link;


static void       link_destruct        (void);
static void       link_client_set_obj  (List_data *object, u32 netID);
static u32        link_server_set_obj  (List_data *object);
static u32        link_obj_to_id       (LinkingContext *link, List_data *object);
static List_data *link_id_to_obj       (LinkingContext *link, u32 id);










static void 
link_destruct(void) {
    for (u32 i=0; i<4096; ++i) {
        for (List_data_to_ID_hash_table_node *node=server_link.hashTable_Obj_To_ID[i]; node; ) {
            List_data_to_ID_hash_table_node *next = node->next;
            xfree(node);
            node = next;
        }
        
        for (ID_to_List_data_hash_table_node *node=server_link.hashTable_ID_To_Obj[i]; node; ) {
            ID_to_List_data_hash_table_node *next = node->next;
            xfree(node);
            node = next;
        }
    }
    
    for (u32 i=0; i<4096; ++i) {
        for (List_data_to_ID_hash_table_node *node=client_link.hashTable_Obj_To_ID[i]; node; ) {
            List_data_to_ID_hash_table_node *next = node->next;
            xfree(node);
            node = next;
        }
        
        for (ID_to_List_data_hash_table_node *node=client_link.hashTable_ID_To_Obj[i]; node; ) {
            ID_to_List_data_hash_table_node *next = node->next;
            xfree(node);
            node = next;
        }
    }
}

static u32 ID_to_list_data_hash(u32 key) {
    u32 a = 2654435761U; // Knuth's constant (close to the golden ratio)
    u32 hash = key * a;
    u32 result = hash & 4095;
    return result;
}

static u32 List_data_to_ID_hash(List_data *key) {
    u32 prime1 = 16777619U;
    u32 prime2 = 2166136261U;
    
    u32 hash = prime2;
    for (u32 i = 0; i < key->textLength; i += 16) {
        hash = (hash ^ key->text[i]) * prime1;
    }
    
    u32 result = hash & 4095;
    return result;
}

static u32
link_server_set_obj(List_data *object) {
    assert(object);
    
    // Generate hash for the given object
    u32 hashIndex = List_data_to_ID_hash(object);
    
    // Check if the object is already in the hash table
    List_data_to_ID_hash_table_node *node = server_link.hashTable_Obj_To_ID[hashIndex];
    while (node) {
        if (node->key == object) {
            return node->value; // Object is already in the hash table, return its ID
        }
        node = node->next;
    }
    
    // If object not found, create a new entry
    u32 newID = --server_link.nextNetworkID - 1;
    
    // Create and initialize a new node for List_data_to_ID hash table
    List_data_to_ID_hash_table_node *newNode_ObjToID = xalloc(sizeof *newNode_ObjToID);
    newNode_ObjToID->key = object;
    newNode_ObjToID->value = newID;
    newNode_ObjToID->next = server_link.hashTable_Obj_To_ID[hashIndex];
    server_link.hashTable_Obj_To_ID[hashIndex] = newNode_ObjToID;
    
    // Create and initialize a new node for ID_to_List_data hash table
    u32 hashIndex_ID = ID_to_list_data_hash(newID);
    ID_to_List_data_hash_table_node *newNode_IDToObj = xalloc(sizeof *newNode_IDToObj);
    newNode_IDToObj->key = newID;
    newNode_IDToObj->value = object;
    newNode_IDToObj->next = server_link.hashTable_ID_To_Obj[hashIndex_ID];
    server_link.hashTable_ID_To_Obj[hashIndex_ID] = newNode_IDToObj;
    
    return newID;
}

static void
link_client_set_obj(List_data *object, u32 netID) {
    assert(object);
    
    // Generate hash for the given object
    u32 hashIndex = List_data_to_ID_hash(object);
    
    // Check if the object is already in the hash table
    List_data_to_ID_hash_table_node *node = client_link.hashTable_Obj_To_ID[hashIndex];
    while (node) {
        if (node->key == object) {
            assert(netID == node->value);
            return;
        }
        node = node->next;
    }
    
    // Create and initialize a new node for List_data_to_ID hash table
    List_data_to_ID_hash_table_node *newNode_ObjToID = xalloc(sizeof *newNode_ObjToID);
    newNode_ObjToID->key = object;
    newNode_ObjToID->value = netID;
    newNode_ObjToID->next = client_link.hashTable_Obj_To_ID[hashIndex];
    client_link.hashTable_Obj_To_ID[hashIndex] = newNode_ObjToID;
    
    // Create and initialize a new node for ID_to_List_data hash table
    u32 hashIndex_ID = ID_to_list_data_hash(netID);
    ID_to_List_data_hash_table_node *newNode_IDToObj = xalloc(sizeof *newNode_IDToObj);
    newNode_IDToObj->key = netID;
    newNode_IDToObj->value = object;
    newNode_IDToObj->next = client_link.hashTable_ID_To_Obj[hashIndex_ID];
    client_link.hashTable_ID_To_Obj[hashIndex_ID] = newNode_IDToObj;
}

static List_data *
link_id_to_obj(LinkingContext *link, u32 id) {
    List_data *result = 0;
    
    // Get hash index
    u32 hashIndex = ID_to_list_data_hash(id);
    
    // Iterate through items in that bucket
    for (ID_to_List_data_hash_table_node *node = link->hashTable_ID_To_Obj[hashIndex]; node; node = node->next) {
        // Compare keys
        if (node->key == id) {
            // If match, return object found
            result = node->value;
            break;
        }
    }
    
    return result;
}

static u32
link_obj_to_id(LinkingContext *link, List_data *object) {
    u32 result = UINT32_MAX;
    
    // Get hash index
    u32 hashIndex = List_data_to_ID_hash(object);
    
    // Iterate through items in that bucket
    for (List_data_to_ID_hash_table_node *node = link->hashTable_Obj_To_ID[hashIndex]; node; node = node->next) {
        // Compare keys
        if (node->key == object) {
            // If match, return object found
            result = node->value;
            break;
        }
    }
    
    return result;
}

#endif //LINKING_CONTEXT_H
