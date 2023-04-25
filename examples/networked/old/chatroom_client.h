#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

#include "chatroom.h"

typedef struct {
    bool isWaitingConnectionAck;
    bool isConnected;
    bool showDev;
    
    f32 waitingTimer;
    
    u32 loginButtonId;
    u32 runServerButtonId;
    ImguiInputState usernameInput;
    ImguiInputState passwordInput;
    ImguiInputState serverInput;
    ImguiInputState messageInput;
    
    ChatRoom roomMain;
    SOCKET socket_peer;
    XDiv *divRoot;
    XDiv *divTop;
    XDiv *divBot;
    
    XDiv *divBotLeft;
    XDiv *divBotRight;
    
} ChatClient;



function void chatroom_client_initialize      (void);
function void chatroom_client_shutdown        (void);
function void chatroom_client_update          (void);
function void chatroom_client_connect         (char *ip, char *port);
function void chatroom_client_process_command (void);
function void chatroom_client_update_main     (void);
function void chatroom_client_update_login    (void);
function void chatroom_client_update_waiting  (void);

global ChatClient client;

function void send_message(wchar_t *message, u32 length) {
    /* Send message over network to server */
    char *ascii = xstrtoascii(message);
    
    ChatNetworkPacketHeader packetHeader;
    packetHeader.type = NetworkMessageType_create_obj;
    packetHeader.dataLength = length;
    
    /* Allocate memory for the packet */
    u32 packetSize = sizeof packetHeader + length;
    char *packet = xalloc(packetSize);
    
    /* Write the header */
    xcopy(packet, &packetHeader, sizeof packetHeader);
    
    /* Write the data */
    xcopy(packet + sizeof packetHeader, ascii, length);
    
    /* Send the data */
    int bytes_sent = send(client.socket_peer, packet, packetSize, 0);
    
    /* Free buffers */
    xfree(ascii);
    xfree(packet);
    
    /* Reset input field */
    client.messageInput.index = 0;
    client.messageInput.storage[0] = 0;
}

function void 
chatroom_client_initialize(void) {
    /* Initialize tab state for inputs */
    client.usernameInput.tabId = &client.passwordInput;
    client.usernameInput.tabType = ImguiType_input;
    
    client.passwordInput.tabId = &client.serverInput;
    client.passwordInput.tabType = ImguiType_input;
    
    client.serverInput.tabId = &client.loginButtonId;
    client.serverInput.tabType = ImguiType_button;
    
    client.messageInput.enter = send_message;
    
    /* Initialize server input width default IP */
    
    xstrcpy(client.serverInput.storage, 512, L"20.228.204.90:8080");
    client.serverInput.index = xstrlen(client.serverInput.storage);
    
    /* Initialize chat */
    chat_initialize();
    
    /* Div layout */
    client.divRoot = xdiv_make(0, (v2f){.2f,.3f}, (v2f){.8f,.7f}, goldf);
    
    client.divTop = xdiv_make(client.divRoot, (v2f){0,0}, (v2f){1,.5f}, whitef);
    client.divBot = xdiv_make(client.divRoot, (v2f){0,.5f}, (v2f){1,1}, (v4f){.89f,.89f,.89f,1});
    
    client.divBotLeft = xdiv_make(client.divBot, (v2f){0,0}, (v2f){.25f,1}, (v4f){.8f,.8f,.8f,1});
    client.divBotRight = xdiv_make(client.divBot, (v2f){.25f,0}, (v2f){1,1}, (v4f){.87f,.87f,.87f,1});
}

function void 
chatroom_client_shutdown(void) {
    /* Free XDivs */
    xdiv_free(client.divRoot);
    
    /* Free rooms */
    chatroom_free(&client.roomMain);
    
    /* Shutdown chat */
    chat_shutdown();
    
    /* Close the server socket */
    CLOSESOCKET(client.socket_peer);
}

function void
chatroom_client_update(void) {
    
    /* Update the client */
    
    if (client.showDev) {
        /* Run server button */
        if (!isRunningServer) {
            if (imgui_button(&layer1, &font32,
                             (v2f){xd11.back_buffer_size.x-150,0},   // pos
                             (v2f){150,28},                          // dim
                             goldf,                                  // bgColor
                             (v4f){0,0,0,1},                         // fgColor
                             L"Run Server",                          // label
                             &client.runServerButtonId)) {           // id
                /* Clicked */
                chatroom_server_initialize();
                isRunningServer = true;
            }
        }
    }
    
    if (client.isWaitingConnectionAck || client.isConnected) {
        /* Make a silly little set with only the server socket in it?!?! */
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(client.socket_peer, &reads);
        
        /* No waiting at all for new data ("non-blocking mode") */
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        
        if (select((u32)client.socket_peer+1, &reads, 0, 0, &timeout) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            assert(!"Select can't fail!");
        }
        
        /* Check for TCP data from the server (the only socket in the set) */
        if (FD_ISSET(client.socket_peer, &reads)) {
            /* If not yet flagged as connected */
            if (!client.isConnected) {
                /* Flag it and print to console */
                client.isConnected = true;
                chatroom_console_print("Connected.");
            }
            
            char read[4096];
            int bytes_received = recv(client.socket_peer, read, 4096, 0);
            if (bytes_received < 1) {
                fprintf(stderr, "Connection closed by peer.\n");
                assert(!"Can't get disconnected!");
            }
            
            /* Interpret the packet */
            ChatNetworkPacketHeader *packetHeader = (ChatNetworkPacketHeader *)read;
            
            if (packetHeader->type == NetworkMessageType_obj_created) {
                u32 objId = *(u32 *)(read + sizeof *packetHeader);
                u32 userId = *(u32 *)(read + sizeof *packetHeader + sizeof(u32));
                
                char *message = xalloc(packetHeader->dataLength);
                xcopy(message, read + sizeof *packetHeader + 2*sizeof(u32), packetHeader->dataLength);
                chatroom_console_print(message);
                
                wchar_t *wmessage = xstrascii(message);
                
                chatroom_obj_client_create(&client.roomMain, objId, userId, wmessage);
                
                xfree(wmessage);
                xfree(message);
            }
            
            printf("Received (%d bytes): %.*s",
                   bytes_received, bytes_received, read);
        }
    }
    
    /* If the client is connected to server */
    if (client.isConnected) {
        chatroom_client_update_main();
    }
    else {
        if (client.isWaitingConnectionAck) {
            chatroom_client_update_waiting();
        }
        else {
            /* Show "login page" */
            chatroom_client_update_login();
        }
    }
}

function void
chatroom_client_connect(char *ip, char *port) {
    /* Configure a remote address for connection */
	chatroom_console_print("Configuring remote address...");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *peer_address;
	if (getaddrinfo(ip, port, &hints, &peer_address)) {
		
        char buf[512]={0};
        sprintf_s(buf, 512, "getaddrinfo() failed. (%d)", GETSOCKETERRNO());
		chatroom_console_print(buf);
    }
    else {
        /* Print out the address */
        chatroom_console_print("Remote address is: ");
        char address_buffer[100] = {0};
        char service_buffer[100] = {0};
        getnameinfo(peer_address->ai_addr, (u32)peer_address->ai_addrlen,
                    address_buffer, sizeof(address_buffer),
                    service_buffer, sizeof(service_buffer),
                    NI_NUMERICHOST);
        
        char buf[512]={0};
        sprintf_s(buf, 512, "%s %s", address_buffer, service_buffer);
        chatroom_console_print(buf);
        
        /* Create our socket */
        chatroom_console_print("Creating socket...");
        client.socket_peer = socket(peer_address->ai_family,
                                    peer_address->ai_socktype, peer_address->ai_protocol);
        
        if (!ISVALIDSOCKET(client.socket_peer)) {
            sprintf_s(buf, 512, "socket() failed. (%d)", GETSOCKETERRNO());
            chatroom_console_print(buf);
        }
        else {
            /* Set socket as non-blocking */
            socket_nonblocking(client.socket_peer);
            
            /* Establish a connection to the remote server */
            chatroom_console_print("Connecting...");
            if (connect(client.socket_peer,
                        peer_address->ai_addr, (u32)peer_address->ai_addrlen) == SOCKET_ERROR) {
                
                u32 error = GETSOCKETERRNO();
                if (error == WSAEWOULDBLOCK) {
                }
                else {
                    sprintf_s(buf, 512, "connect() failed. (%d)", GETSOCKETERRNO());
                    chatroom_console_print(buf);
                }
            }
        }
        
        freeaddrinfo(peer_address);
    }
}

function void
chatroom_client_process_command(void) {
#if 0
    if (xstrcmp(client.inputText, L"run server")) {
    }
    else if (xstrcmp(client.inputText, L"connect")) {
        /* Connect client to server */
        chatroom_client_connect("127.0.0.1", "8080");
    }
    
    /* Reset input */
    client.inputText[0] = 0;
    client.inputTextIndex = 0;
#endif
}

function void
chatroom_client_update_waiting(void) {
    client.waitingTimer += xd11.dt;
    if (client.waitingTimer > 3) {
        client.isWaitingConnectionAck = false;
        client.isConnected = false;
        client.waitingTimer = 0;
    }
    
    /* Draw console */
    v2f pos = {0,0};
    StringQueueNode *at = chatConsole.head;
    while (at) {
        wchar_t *wideString = xstrascii(at->data.storage);
        draw_text(&layer1, pos, (v4f){0,0,0,1}, &font32, wideString);
        pos.y += 32;
        at = at->next;
        xfree(wideString);
    }
}

function void
chatroom_client_update_login(void) {
    
    v2f divRootPos = (v2f){0,0};
    v2f divRootDim = xd11.back_buffer_size;
    
    // xdiv_draw(&layer1, client.divRoot, divRootPos, divRootDim);
    
    v2f rootMinPx = {0};
    v2f rootMaxPx = {0};
    xdiv_calculate(client.divRoot, client.divRoot, divRootPos, divRootDim,
                   &rootMinPx, &rootMaxPx);
    
    /* Draw box background */
    draw_rect_rounded(&layer1, rootMinPx, sub2f(rootMaxPx, rootMinPx), (v4f){1,1,1,1}, 5);
    
    v2f padding = {4,4};
    
    /* Top div */
    v2f topMinPx = {0};
    v2f topMaxPx = {0};
    xdiv_calculate(client.divRoot, client.divTop, divRootPos, divRootDim,
                   &topMinPx, &topMaxPx);
    padd2f(&topMinPx, padding);
    psub2f(&topMaxPx, padding);
    draw_rect_rounded(&layer1, topMinPx, sub2f(topMaxPx, topMinPx), (v4f){.75f,.75f,.75f,1}, 5);
    
    /* Bot div */
    v2f botRightMinPx = {0};
    v2f botRightMaxPx = {0};
    xdiv_calculate(client.divRoot, client.divBotRight, divRootPos, divRootDim,
                   &botRightMinPx, &botRightMaxPx);
    
    padd2f(&botRightMinPx, padding);
    psub2f(&botRightMaxPx, padding);
    draw_rect_rounded(&layer1, 
                      botRightMinPx, 
                      sub2f(botRightMaxPx, botRightMinPx), 
                      (v4f){.75f,.75f,.75f,1}, 5);
    
    /* Draw inputs */
    v2f inputDim = {botRightMaxPx.x-botRightMinPx.x-padding.x*2, font32.lineHeight};
    
    /* Login input */
    {
        v2f p = add2f(botRightMinPx, (v2f){padding.x,padding.y});
        wchar_t *label = L"Username:";
        v2f labelDim = xrender2d_font_dim(&font32, label);
        
        /* Label */
        draw_text(&layer1,
                  sub2f(p, (v2f){labelDim.x+4,0}),
                  blackf,
                  &font32,
                  label);
        
        /* Actual input */
        if (imgui_input(&layer1, &font32, 
                        p,               // pos
                        inputDim,        // dim
                        (v4f){1,1,1,1},  // bgCol
                        (v4f){0,0,0,1},  // fgCol
                        (v4f){1,1,1,1},  // labelColor
                        0,               // label
                        false,                     // code
                        &client.usernameInput)) {  // id 
            
            /* Contents changed */
        }
    }
    
    /* Password input */
    {
        v2f p = add2f(botRightMinPx, (v2f){padding.x,padding.y + 32 + padding.y});
        wchar_t *label = L"Password:";
        v2f labelDim = xrender2d_font_dim(&font32, label);
        
        /* Label */
        draw_text(&layer1,
                  sub2f(p, (v2f){labelDim.x+4,0}),
                  blackf,
                  &font32,
                  label);
        
        if (imgui_input(&layer1, &font32, 
                        p,              // pos
                        inputDim,                  // dim
                        (v4f){1,1,1,1},            // bgColor
                        (v4f){0,0,0,1},            // fgColor
                        (v4f){1,1,1,1},            // labelColor
                        0,               // label
                        true,                      // code
                        &client.passwordInput)) {  // id
            /* Contents changed */
        }
    }
    
    wchar_t *loginButtonText = L"Login";
    v2f loginButtonDim = xrender2d_font_dim(&font32, loginButtonText);
    loginButtonDim.x += 20;
    v2f loginButtonP = {0};
    
    /* Server IP input */
    {
        v2f p = add2f(botRightMinPx, (v2f){padding.x,padding.y + 2*(32 + padding.y)});
        wchar_t *label = L"Server IP:";
        v2f labelDim = xrender2d_font_dim(&font32, label);
        
        loginButtonP = p;
        loginButtonP.x += inputDim.x -loginButtonDim.x;
        
        /* Label */
        draw_text(&layer1,
                  sub2f(p, (v2f){labelDim.x+4,0}),
                  blackf,
                  &font32,
                  label);
        
        if (imgui_input(&layer1, &font32, 
                        p,      // pos
                        sub2f(inputDim, (v2f){loginButtonDim.x+padding.x,0}), // dim
                        (v4f){1,1,1,1},                // bgColor
                        (v4f){0,0,0,1},                // fgColor
                        (v4f){1,1,1,1},            // labelColor
                        0,              // label
                        false,                     // code
                        &client.serverInput)) {    // id
            /* Contents changed */
        }
    }
    
    {
        /* Login button */
        if (imgui_button(&layer1, &font32,
                         loginButtonP,   // pos
                         loginButtonDim,              // dim
                         goldf,                      // bgColor
                         (v4f){0,0,0,1},             // fgColor
                         L"Login",                   // label
                         &client.loginButtonId)) {    // id
            
            /* Button was clicked */
            client.isWaitingConnectionAck = true;
            
            char *charIp = 0;
            char *charPort = 0;
            
            /* Transform IP from wchar_t to char */
            char *asciiIp = xstrtoascii(client.serverInput.storage);
            
            /* Split IP input data into IP and Port at : */
            StringList ipList = string_split(asciiIp, ':');
            assert(ipList.head && ipList.head->next);
            
            /* Copy ip into our pointer */
            charIp = xalloc(ipList.head->data.length+1);
            xcopy(charIp, ipList.head->data.storage, ipList.head->data.length+1);
            
            /* Copy port into our pointer */
            charPort = xalloc(ipList.head->next->data.length+1);
            xcopy(charPort, ipList.head->next->data.storage, ipList.head->next->data.length+1);
            
            xfree(asciiIp);
            string_list_free(&ipList);
            
            /* Call connect with char versions of IP and port */
            chatroom_client_connect(charIp, charPort);
            
            xfree(charIp);
            xfree(charPort);
        }
    }
}

function void
chatroom_client_update_main(void) {
    /* Message input */
    {
        v2f dim = {xd11.back_buffer_size.x, 30};
        v2f p   = {0,xd11.back_buffer_size.y-dim.y};
        if (imgui_input(&layer1, &font32, p, dim, 
                        (v4f){1,1,1,1},            // bgCol
                        (v4f){0,0,0,1},            // fgCol
                        (v4f){0,0,0,1},            // labelColor
                        L"Message",                // label
                        false,                     // code
                        &client.messageInput)) {   // id 
            
            /* Contents changed */
        }
    }
    
    /* Draw chat objects */
    u32 existingObjIndex=0;
    for (u32 i=0; i<client.roomMain.index; ++i) {
        ChatRoomObject *obj = client.roomMain.storage[i];
        if (obj) {
            wchar_t buf[512] = {0};
            swprintf_s(buf, 512, L"User %d: %s", obj->userId, obj->storage);
            draw_text(&layer1, (v2f){0,existingObjIndex++*16.0f}, 
                      (v4f){0,0,0,1}, &font32, 
                      buf);
        }
    }
}

#endif //CHATROOM_H
