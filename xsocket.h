#ifndef XSOCKET_H
#define XSOCKET_H

/* Includes for Windows or Linux */
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#endif

/* Defines to help work with Windows and Linux */
#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#define WOULDBLOCK(e) ((e)==WSAEWOULDBLOCK)

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define SOCKET_ERROR -1
#define GETSOCKETERRNO() (errno)
#define WOULDBLOCK(e) (((e)==EWOULDBLOCK) || (e)==EAGAIN)
#endif

typedef struct sockaddr_in sockaddr_in;

#define XMAX_PACKET_SIZE 1470


static bool        xsocket_initialize       (void);
static sockaddr_in xsocket_ipv4_from_string (char *address, u16 port);
static bool        xsocket_nonblocking      (SOCKET socket);
static bool        xsocket_blocking         (SOCKET socket);











static bool
xsocket_initialize(void) {
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d)) {
		fprintf(stderr, "Failed to initialize.\n");
		return 0;
	}
#endif
    return 1;
}

static sockaddr_in
xsocket_ipv4_from_string(char *address, u16 port) {
    sockaddr_in result = {0};
    result.sin_family = AF_INET;
    result.sin_port = htons(port);
    inet_pton(AF_INET, address, &result.sin_addr);
    return result;
}

static bool
xsocket_nonblocking(SOCKET socket) {
#if _WIN32
    u_long arg = 1;
    s32 result = ioctlsocket(socket, FIONBIO, &arg);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    s32 result = fcntl(socket, F_SETFL, flags);
#endif
    if (result == SOCKET_ERROR)
        return false;
    else
        return true;
}

static bool
xsocket_blocking(SOCKET socket) {
#if _WIN32
    u_long arg = 0;
    s32 result = ioctlsocket(socket, FIONBIO, &arg);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    s32 result = fcntl(socket, F_SETFL, flags);
#endif
    
    if (result == SOCKET_ERROR)
        return false;
    else
        return true;
}

#endif