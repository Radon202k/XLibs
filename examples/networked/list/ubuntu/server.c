static void
extract_ip_address(const char *http_response, char *ip_address, size_t ip_address_length) {
    char *ip_start = strstr(http_response, "\r\n\r\n");
    if (ip_start) {
        ip_start += 4; // Move the pointer to the start of the IP address
        xstrncpy(ip_address, ip_start, ip_address_length - 1);
        ip_address[ip_address_length - 1] = '\0'; // Ensure the string is null-terminated
    } else {
        ip_address[0] = '\0'; // Return an empty string if the IP address cannot be found
    }
}

static void
server_ask_for_external_ip() {
    server_print("Asking for external IP...", xcolor[Red]);
    
    SOCKET sock;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    char recv_buf[256];
    int iResult;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    // Resolve the server address and port
    iResult = getaddrinfo("api.ipify.org", "80", &hints, &result);
    if (iResult != 0) {
        server_print("getaddrinfo failed", xcolor[Red]);
    }
    
    // Create a SOCKET
    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (!ISVALIDSOCKET(sock)) {
        server_print("socket failed", xcolor[Red]);
        freeaddrinfo(result);
    }
    
    // Connect to the server
    iResult = connect(sock, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        CLOSESOCKET(sock);
        sock = -1;
    }
    
    freeaddrinfo(result);
    
    if (!ISVALIDSOCKET(sock)) {
        server_print("Unable to connect to server!", xcolor[Red]);
    }
    
    // Send an HTTP request
    const char *http_request = "GET /?format=text HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";
    iResult = send(sock, http_request, (int)strlen(http_request), 0);
    if (iResult == SOCKET_ERROR) {
        server_print("send failed", xcolor[Red]);
        CLOSESOCKET(sock);
    }
    
    // Receive the response
    do {
        iResult = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0);
        if (iResult > 0) {
            recv_buf[iResult] = '\0';
            
            char buf[256]={0};
            
            extract_ip_address(recv_buf, server.ip_address, sizeof(server.ip_address));
            if (server.ip_address[0] != '\0') {
                snprintf(buf, 256, "External IP address: %s", server.ip_address);
                server_print(buf, xcolor[Gold]);
            } else {
                server_print("Failed to parse IP address from response", xcolor[Gold]);
            }
            
            
        } else if (iResult == 0) {
            server_print("Connection closed", xcolor[Azure]);
        } else {
            server_print("recv failed", xcolor[Red]);
        }
    } while (iResult > 0);
    
    // Cleanup
    CLOSESOCKET(sock);
}

static void
server_send_repl_packet(SOCKET socket, ReplicateType type, u32 elId, u32 refId) {
    Stream stream;
    serialize_construct(&stream, PACKET_MAX_SIZE);
    /* Serialize packet type */
    packet_write_type(&stream, PacketType_replicate);
    /* Serialize replication type */
    serialize_u8(&stream, ReplicateType_move);
    /* Serialize net ids */
    serialize_u32(&stream, elId);
    serialize_u32(&stream, refId);
    
    int bytes_sent = send(socket, stream.storage, stream.at, 0);
    serialize_destruct(&stream);
    if (bytes_sent < 1) {
        fprintf(stderr, "Connection closed by peer.\n");
        // assert(!"Can't get disconnected!");
    }
}

static void
server_construct(void) {
    server_print("Server initialized.", xcolor[Gold]);
    
    /* Create initial state */
    list_linked_construct(&server.list);
    List_linked_node *a = list_linked_push(&server.list, list_data("A", xcolor[Azure]));
    link_server_set_obj(a->data);
    
    List_linked_node *b = list_linked_push(&server.list, list_data("B", xcolor[Emerald]));
    link_server_set_obj(b->data);
    
    List_linked_node *c = list_linked_push(&server.list, list_data("C", xcolor[Gold]));
    link_server_set_obj(c->data);
    
    List_linked_node *d = list_linked_push(&server.list, list_data("D", xcolor[Olive]));
    link_server_set_obj(d->data);
    
    List_linked_node *e = list_linked_push(&server.list, list_data("E", xcolor[Crimson]));
    link_server_set_obj(e->data);
    
    List_linked_node *f = list_linked_push(&server.list, list_data("F", xcolor[Blue]));
    link_server_set_obj(f->data);
    
    char buf[512];
    snprintf(buf, 512, "List length: %d.", server.list.length);
    
    server_print(buf, xcolor[Emerald]);
    
    server_print("Initial state created.", xcolor[Emerald]);
    
    // server_ask_for_external_ip();
    
    /* Get our local address */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *bind_address;
    getaddrinfo("127.0.0.1", "8080", &hints, &bind_address);
    server_print("Configured local address.", xcolor[Emerald]);
    
    /* Create socket */
    server.socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(server.socket_listen)) {
        snprintf(buf, 512, "socket() failed. (%d)", GETSOCKETERRNO());
        server_print(buf, xcolor[Red]);
    }
    else {
        /* Bind our socket */
        if (bind(server.socket_listen, bind_address->ai_addr, (s32)bind_address->ai_addrlen)) {
            snprintf(buf, 512, "bind() failed. (%d).", GETSOCKETERRNO());
            server_print(buf, xcolor[Red]);
            freeaddrinfo(bind_address);
        }
        else {
            freeaddrinfo(bind_address);
            
            server_print("Listening for clients...", xcolor[Emerald]);
            if (listen(server.socket_listen, 10) < 0) {
                snprintf(buf, 512, "listen() failed. (%d).", GETSOCKETERRNO());
                server_print(buf, xcolor[Red]);
            } 
            else {
                /* Make a set of sockets */
                FD_ZERO(&server.master);
                FD_SET(server.socket_listen, &server.master);
                server.max_socket = server.socket_listen;
            }
        }
    }
    
    /* Console scroll bar scissor */
    // plat.layers[2].scissor = xalloc(sizeof(rect2));
}

static void
server_destruct(void) {
    for (u32 i=0; i<server.console.index; ++i)
        xfree(server.console.text[i]);
    
    list_linked_destruct(&server.list);
    
    // xfree(plat.layers[2].scissor);
}

static SOCKET
server_accept_client() {
    char buf[512];
    
    /* Accept new connection */
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(server.socket_listen,
                                  (struct sockaddr *)&client_address,
                                  &client_len);
    if (!ISVALIDSOCKET(socket_client)) {
        snprintf(buf, 512, "accept() failed. (%d)", GETSOCKETERRNO());
        server_print(buf, xcolor[Emerald]);
        
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
    
    snprintf(buf, 512, "New connection from %s", address_buffer);
    server_print(buf, xcolor[Emerald]);
    
    /* Send state to new client */
    Stream stream;
    serialize_construct(&stream, PACKET_MAX_SIZE);
    
    packet_write_type(&stream, PacketType_state);
    serialize_u8(&stream, (u8)server.list.length);
    /* For each element of the list */
    for (List_linked_node *at=server.list.head; at; at=at->next) {
        /* Get the data */
        List_data *data = at->data;
        /* Serialize network id for this object */
        u32 objNetworkId = link_obj_to_id(&server_link, data);
        serialize_u32(&stream, objNetworkId);
        /* Serialize the text */
        serialize_text(&stream, data->text);
        /* Serialize the color */
        serialize_v4(&stream, data->color);
    }
    
    int bytes_sent = send(socket_client, 
                          stream.storage, stream.at, 0);
    serialize_destruct(&stream);
    if (bytes_sent < 1) {
        fprintf(stderr, "Connection closed by peer.\n");
        // assert(!"Can't get disconnected!");
    }
    else {
        server_print("Locked server while new client synchronizes...", xcolor[Red]);
        snprintf(buf, 512, "Sent Packet_state to new client: %d elements.", 
                 server.list.length);
        server_print(buf, xcolor[Crimson]);
    }
    
    
    return socket_client;
}

static bool
server_read_packet(SOCKET socket) {
    char read[1024];
    int bytes_received = recv(socket, read, 1024, 0);
    if (bytes_received < 1) {
        FD_CLR(socket, &server.master);
        CLOSESOCKET(socket);
        return false;
    }
    
    /* Interpret the packet */
    
    /* Send object created messaged to all clients */
    int j;
    for (j=0; j<=server.max_socket; ++j) {
        if (FD_ISSET(j, &server.master)) {
            /* This is the listening socket */
            if (j == server.socket_listen)
                continue;
            /* This is a connected client */
            else {
                
                Stream stream;
                deserialize_construct(&stream, read);
                
                
                PacketType type = UINT32_MAX;
                packet_read_type(&stream, &type);
                
                switch(type) {
                    
                    case PacketType_sync: {
                        server_print("Unlocked server...", xcolor[Azure]);
                    } break;
                    
                    case PacketType_input: {
                        /* Deserialize input type */
                        InputType inputType = 0;
                        deserialize_u8(&stream, (u8 *)&inputType);
                        if (inputType == InputType_move) {
                            /* Deserialize net ids */
                            u32 elNetId = 0;
                            u32 refNetId = 0;
                            deserialize_u32(&stream, &elNetId);
                            deserialize_u32(&stream, &refNetId);
                            
                            /* Get objects from ids */
                            List_data *el = link_id_to_obj(&server_link, elNetId);
                            List_data *ref = link_id_to_obj(&server_link, refNetId);
                            
                            server_print("Received input packet", xcolor[Gold]);
                            char buf[256] = {0};
                            snprintf(buf, 256, "Input move: %d -> %d", elNetId, refNetId);
                            server_print(buf, xcolor[Gold]);
                            
                            /* Move item of the list animated */
                            list_linked_move(&server.list, el, ref);
                            
                            /* Send replication packet to all clients */
                            for (s32 clientSocket=0; 
                                 clientSocket<=server.max_socket; 
                                 ++clientSocket) {
                                if (FD_ISSET(clientSocket, &server.master)) {
                                    if (clientSocket == server.socket_listen)
                                        continue;
                                    else {
                                        /* Is a client */
                                        server_send_repl_packet(clientSocket,
                                                                ReplicateType_move,
                                                                elNetId,
                                                                refNetId);
                                    }
                                }
                            }
                        }
                    } break;
                    
                    default: {
                    } break;
                }
                
            } // if (j == server.socket_listen)
        } // if (FD_ISSET
    } // for
    
    return true;
}

static void
server_update(f32 dt) {
    fd_set reads;
    reads = server.master;
    
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    char buf[512];
    if (select((int)server.max_socket+1, &reads, 0, 0, &timeout) < 0) {
        snprintf(buf, 512, "select() failed. (%d)", GETSOCKETERRNO());
		server_print(buf, xcolor[Emerald]);
    }
    
    /* Loop through all possible sockets to handle those that exist */
    for (SOCKET i=1; i <= server.max_socket; ++i) {
        if (FD_ISSET(i, &reads)) {
            /* If there is data in the listening socket */
            if (i == server.socket_listen) {
                SOCKET newClient = server_accept_client(i);
            }
            /* Otherwise, it is a packet from a connected client */
            else {
                if (!server_read_packet(i)) {
                    continue;
                }
            }
        } //if FD_ISSET
    } //for i to max_socket
    
    if (server.clientConnected) {
        
    }
    
    
    // list_render(&server.list, &server_link, 0.3f, dt);
}

static void
server_print(char *text, v4 color) {
#ifdef _WIN32
    u32 length = xstrlen(text);
    u32 index = server.console.index++;
    server.console.text[index] = xalloc(sizeof(char)*(length+1));
    memcpy(server.console.text[index], text, sizeof(char)*length);
    v4_copy(color, server.console.colors[index]);
#else
    printf("%s\n", text);
#endif
}
