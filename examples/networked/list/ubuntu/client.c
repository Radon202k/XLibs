static void
client_send_move_packet(u32 elNetId, u32 refNetId) {
    Stream stream;
    serialize_construct(&stream, PACKET_MAX_SIZE);
    /* Serialize packet type */
    packet_write_type(&stream, PacketType_input);
    /* Serialize input type */
    serialize_u8(&stream, InputType_move);
    /* Serialize net ids */
    serialize_u32(&stream, elNetId);
    serialize_u32(&stream, refNetId);
    
    int bytes_sent = send(client.socket_peer, stream.storage, stream.at, 0);
    serialize_destruct(&stream);
    if (bytes_sent < 1) {
        fprintf(stderr, "Connection closed by peer.\n");
        // assert(!"Can't get disconnected!");
    }
}

static void
client_send_sync_packet(void) {
    Stream stream;
    serialize_construct(&stream, PACKET_MAX_SIZE);
    
    packet_write_type(&stream, PacketType_sync);
    int bytes_sent = send(client.socket_peer, 
                          stream.storage, stream.at, 0);
    serialize_destruct(&stream);
    if (bytes_sent < 1) {
        fprintf(stderr, "Connection closed by peer.\n");
        // assert(!"Can't get disconnected!");
    }
}

static void
client_connect(char *ip, char *port) {
    /* Configure a remote address for connection */
	// chatroom_console_print("Configuring remote address...");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *peer_address;
	if (getaddrinfo(ip, port, &hints, &peer_address)) {
        printf("getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
    }
    else {
        /* Print out the address */
        char address_buffer[100] = {0};
        char service_buffer[100] = {0};
        getnameinfo(peer_address->ai_addr, (u32)peer_address->ai_addrlen,
                    address_buffer, sizeof(address_buffer),
                    service_buffer, sizeof(service_buffer),
                    NI_NUMERICHOST);
        printf("%s %s", address_buffer, service_buffer);
        /* Create our socket */
        client.socket_peer = socket(peer_address->ai_family,
                                    peer_address->ai_socktype, peer_address->ai_protocol);
        
        if (!ISVALIDSOCKET(client.socket_peer)) {
            printf("socket() failed. (%d)\n", GETSOCKETERRNO());
        }
        else {
            /* Set socket as non-blocking */
            xsocket_nonblocking(client.socket_peer);
            
            /* Establish a connection to the remote server */
            if (connect(client.socket_peer,
                        peer_address->ai_addr, (u32)peer_address->ai_addrlen) == SOCKET_ERROR) {
                
                u32 error = GETSOCKETERRNO();
                if (WOULDBLOCK(error)) {
                    printf("Would block\n");
                }
                else {
                    printf("connect() failed. (%d)\n", GETSOCKETERRNO());
                }
            }
        }
        
        freeaddrinfo(peer_address);
    }
}

static void
client_recv_packet() {
    char read[4096];
    int bytes_received = recv(client.socket_peer, read, 4096, 0);
    if (bytes_received < 1) {
        // fprintf(stderr, "Connection closed by peer.\n");
        //assert(!"Can't get disconnected!");
    }
    else {
        Stream stream;
        deserialize_construct(&stream, read);
        
        PacketType type = PacketType_null;
        packet_read_type(&stream, &type);
        
        switch(type) {
            case PacketType_state: {
                /* Received a packet with state information */
                u8 listLength = 0;
                deserialize_u8(&stream, &listLength);
                for (u32 textIndex=0; textIndex<listLength; ++textIndex) {
                    /* Deserialize the network id of the element */
                    u32 objNetworkId = UINT32_MAX;
                    deserialize_u32(&stream, &objNetworkId);
                    /* Deserialize the text */
                    char text[256] = {0};
                    u8 textLength = 0;
                    deserialize_text(&stream, text, &textLength);
                    v4 color;
                    /* Deserialize the color */
                    deserialize_v4(&stream, color);
                    /* Push the element */
                    List_linked_node *obj = list_linked_push(&app.list, list_data(text, color));
                    /* Add object to client's linking context */
                    link_client_set_obj(obj->data, objNetworkId);
                }
                
                /* Send a packet telling the server we are synced */
                client_send_sync_packet();
                
            } break;
            
            /* Replicate object */
            case PacketType_replicate: {
                ReplicateType replicateType = ReplicateType_null;
                deserialize_u8(&stream, (u8 *)&replicateType);
                
                if (replicateType == ReplicateType_move) {
                    u32 elNetId = UINT32_MAX;
                    u32 refNetId = UINT32_MAX;
                    deserialize_u32(&stream, &elNetId);
                    deserialize_u32(&stream, &refNetId);
                    
                    List_data *el = link_id_to_obj(&client_link, elNetId);
                    List_data *ref = link_id_to_obj(&client_link, refNetId);
                    /* Move item of the list animated */
                    list_linked_move(&app.list, el, ref);
                }
            }
            /* Payload */
            
            default: {
            } break;
        }
    }
    
}

static void
client_update(f32 dt) {
    if (!client.isConnected) {
        if (!client.isWaitingResponse) {
            printf("Trying to connect\n");
            client.isWaitingResponse = true;
            client_connect("127.0.0.1", "8080");
        }
        else {
            client.waitCounter += dt;
            if (client.waitCounter > 3) {
                printf("Time out\n");
                client.waitCounter = 0;
                client.isWaitingResponse = false;
            }
        }
    }
    
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
        // assert(!"Select can't fail!");
    }
    
    /* Check for TCP data from the server (the only socket in the set) */
    if (FD_ISSET(client.socket_peer, &reads)) {
        if (!client.isConnected) {
            client.isConnected = true;
        }
        
        if (!client.sentTestPacket) {
            client.sentTestPacket = true;
            
#if 0
            /* Create packet byte stream with text */
            Stream stream;
            serialize_construct(&stream, PACKET_MAX_SIZE);
            
            packet_write_type(&stream, PacketType_text);
            packet_write_text(&stream, L"My text");
            
            int bytes_sent = send(client.socket_peer, 
                                  stream.storage, stream.at, 0);
            serialize_destruct(&stream);
            if (bytes_sent < 1) {
                fprintf(stderr, "Connection closed by peer.\n");
                // assert(!"Can't get disconnected!");
            }
#endif
        }
        
        client_recv_packet();
    }
}