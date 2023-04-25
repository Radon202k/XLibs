#ifndef SERVER_H
#define SERVER_H

typedef struct Console {
    char *text[1024];
    v4 colors[1024];
    u32 index;
    f32 scrollbarPoint;
    f32 scrollbarPointVel;
} Console;

typedef struct Server {
    List_linked list;
    
    Console console;
    u32 clients[32];
    u32 clientIndex;
    bool clientConnected;
    fd_set master;
    SOCKET max_socket;
    SOCKET socket_listen;
    char ip_address[INET6_ADDRSTRLEN];
    
} Server;

static void server_construct (void);
static void server_destruct  (void);
static void server_update    (f32 dt);
static void server_print     (char *text, v4 color);

#endif //SERVER_H
