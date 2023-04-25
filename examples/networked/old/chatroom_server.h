/* date = April 6th 2023 3:57 pm */

#ifndef CHATROOM_SERVER_H
#define CHATROOM_SERVER_H

#define CHATROOM_SERVER_CONSOLE_LINES 7

#include "chatroom.h"

function void chatroom_server_shutdown   (void);
function void chatroom_server_initialize (void);
function void chatroom_server_update     (void);

typedef struct {
    fd_set master;
    SOCKET max_socket;
    SOCKET socket_listen;
    StringQueue console;
    ChatRoom roomMain;
} ChatRoomServer;

global ChatRoomServer server;

function void
chatroom_server_shutdown(void) {
    
    chatroom_console_print("Server shutdown...");
    
    /* Free rooms */
    chatroom_free(&server.roomMain);
    
    chatroom_console_print("Closing all client sockets...");
    // Iterate over the sockets in the master set
    for (SOCKET i = 0; i <= server.max_socket; i++) {
        if (FD_ISSET(i, &server.master)) {
            // Close the socket
            CLOSESOCKET(i);
        }
    }
    
    chatroom_console_print("Closing listening socket...");
	CLOSESOCKET(server.socket_listen);
    chatroom_console_print("Finished.");
    
    string_queue_free(&server.console);
    
    /* Close the log file */
    fclose(logFile);
}

function void
chatroom_server_initialize(void) {
    /* Initial messages */
    chatroom_obj_server_create(&server.roomMain, 0, L"Server initialized");
    chatroom_obj_server_create(&server.roomMain, 0, L"Chat room open");
    
    char timeStamp[512];
    timestamp(timeStamp);
    wchar_t *wtimeStamp = xstrascii(timeStamp);
    chatroom_obj_server_create(&server.roomMain, 0, wtimeStamp);
    xfree(wtimeStamp);
    
    chatroom_console_print("--- Server initialized.");
    
    /* Get our local address */
	chatroom_console_print("Configuring local address...");
    struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	struct addrinfo *bind_address;
	getaddrinfo(0, "8080", &hints, &bind_address);
    
	/* Create socket */
	chatroom_console_print("Creating socket...");
    server.socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, 
                                  bind_address->ai_protocol);
	if (ISVALIDSOCKET(server.socket_listen)) {
        /* Bind our socket */
        chatroom_console_print("Binding socket to local address...");
        if (bind(server.socket_listen, bind_address->ai_addr, 
                 (s32)bind_address->ai_addrlen) == 0) {
            freeaddrinfo(bind_address);
            
            chatroom_console_print("Listening...");
            
            if (listen(server.socket_listen, 10) >= 0) {
                /* Make a set of sockets */
                FD_ZERO(&server.master);
                FD_SET(server.socket_listen, &server.master);
                server.max_socket = server.socket_listen;
                
                chatroom_console_print("Waiting for connections...");
            }
            else {
                char buf[512];
                sprintf_s(buf, 512, "listen() failed. (%d)\n", GETSOCKETERRNO());
                chatroom_console_print(buf);
            }
        }
        else {
            char buf[512];
            sprintf_s(buf, 512, "bind() failed. (%d)\n", GETSOCKETERRNO());
            chatroom_console_print(buf);
        }
    }
    else {
		char buf[512];
        sprintf_s(buf, 512, "socket() failed. (%d)\n", GETSOCKETERRNO());
		chatroom_console_print(buf);
    }
}

function void
chatroom_server_update(void) {
    fd_set reads;
    reads = server.master;
    
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    if (select((int)server.max_socket+1, &reads, 0, 0, &timeout) < 0) {
        char buf[512];
        sprintf_s(buf, 512, "select() failed. (%d)", GETSOCKETERRNO());
		chatroom_console_print(buf);
        assert(!"Select can't fail!");
    }
    
    /* Loop through all possible sockets to handle those that exist */
    {
        SOCKET i;
        for (i = 1; i <= server.max_socket; ++i) {
            if (FD_ISSET(i, &reads)) {
                if (i == server.socket_listen) {
                    /* Accept new connection */
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(server.socket_listen,
                                                  (struct sockaddr *)&client_address,
                                                  &client_len);
                    if (!ISVALIDSOCKET(socket_client)) {
                        char buf[512];
                        sprintf_s(buf, 512, "accept() failed. (%d)", GETSOCKETERRNO());
                        chatroom_console_print(buf);
                        
                        assert(!"accept can't fail!");
                    }
                    
                    /* Add new socket to master set */
                    FD_SET(socket_client, &server.master);
                    if (socket_client > server.max_socket)
                        server.max_socket = socket_client;
                    
                    /* Print address of new connection */
                    char address_buffer[100];
                    getnameinfo((struct sockaddr *)&client_address,
                                client_len,
                                address_buffer,
                                100,
                                0, 0,
                                NI_NUMERICHOST);
                    
                    char buf[512];
                    sprintf_s(buf, 512, "New connection from %s", address_buffer);
                    chatroom_console_print(buf);
                    send(socket_client, "Hello", 5, 0);
                }
                else {
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &server.master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    
                    /* Interpret the packet */
                    ChatNetworkPacketHeader *packetHeader = (ChatNetworkPacketHeader *)read;
                    u32 dataLength = *(u32 *)(read + sizeof(u32));
                    
                    if (packetHeader->type == NetworkMessageType_create_obj) {
                        chatroom_console_print("Received create object packet");
                        
                        char *message = xalloc(dataLength);
                        xcopy(message, read + sizeof *packetHeader, dataLength);
                        chatroom_console_print(message);
                        
                        wchar_t *wmessage = xstrascii(message);
                        
                        /* Create chat object */
                        u32 userId = (u32)i;
                        u32 objId = chatroom_obj_server_create(&server.roomMain, 
                                                               userId, wmessage);
                        
                        /* Send object created messaged to all clients */
                        int j;
                        for (j=0; j<=server.max_socket; ++j) {
                            if (FD_ISSET(j, &server.master)) {
                                if (j == server.socket_listen)
                                    continue;
                                else {
                                    /* Object created packet */
                                    
                                    ChatNetworkPacketHeader sendPacketHeader;
                                    sendPacketHeader.type = NetworkMessageType_obj_created;
                                    sendPacketHeader.dataLength = dataLength + sizeof(u32);
                                    
                                    /* Allocate memory for the packet */
                                    u32 sendPacketSize = sizeof sendPacketHeader + dataLength + 2*sizeof(u32);
                                    char *packet = xalloc(sendPacketSize);
                                    
                                    /* Write the header */
                                    xcopy(packet, &sendPacketHeader, sizeof packetHeader);
                                    
                                    /* Write the obj id */
                                    xcopy(packet + sizeof sendPacketHeader, &objId, sizeof(u32));
                                    
                                    /* Write the user id */
                                    xcopy(packet + sizeof sendPacketHeader + sizeof(u32), &userId, sizeof(u32));
                                    
                                    /* Write the data */
                                    xcopy(packet + sizeof sendPacketHeader + 2*sizeof(u32), message, dataLength);
                                    
                                    /* Send the data */
                                    int bytesSent = send(j, packet, sendPacketSize, 0);
                                    
                                    /* Free buffers */
                                    xfree(packet);
                                } // if (j == server.socket_listen)
                            } // if (FD_ISSET
                        } // for
                        
                        xfree(message);
                        xfree(wmessage);
                        
                    }
                }
            } //if FD_ISSET
        } //for i to max_socket
    }
    
#ifndef CHATROOM_SERVER_HEADLESS
    /* Print in the server console */
    StringQueueNode *at = server.console.head;
    f32 screen_width = xd11.back_buffer_size.x;
    f32 screen_height = xd11.back_buffer_size.y;
    f32 height = 16*CHATROOM_SERVER_CONSOLE_LINES;
    v2f pos = {0, screen_height - height};
    v2f dim = {screen_width, height};
    draw_rect(&layer1, pos, dim, (v4f){0,0,0,.9f});
    while (at) {
        char buf[1024];
        sprintf_s(buf, 1024, "%.*s: %.*s\n", 
                  6, "Server", 
                  at->data.length, at->data.storage);
        /* Conver the message to wide string (UTF-16 or UTF-8) */
        wchar_t *wbuf = xstrascii(buf);
        /* Print to server console in client's window */
        draw_text(&layer1, pos, goldf, &font16, wbuf);
        pos.y += 16.f;
        /* Free wide string buffer */
        xfree(wbuf);
        at = at->next;
    }
#endif
}


#endif //CHATROOM_SERVER_H
