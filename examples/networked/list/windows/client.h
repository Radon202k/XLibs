#ifndef CLIENT_H
#define CLIENT_H

typedef struct Client {
    bool isConnected;
    bool isWaitingResponse;
    f32 waitCounter;
    bool sentTestPacket;
    SOCKET socket_peer;
} Client;

#endif //CLIENT_H
