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
#endif

/* Defines to help work with Windows and Linux */
#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

function bool xsocket_initialize(void);
function bool xsocket_udp_listen();


function bool
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

typedef struct sockaddr_in sockaddr_in;

#define XMAX_PACKET_SIZE 1470

function sockaddr_in
ipv4_from_string(char *address, u16 port)
{
    sockaddr_in result = {0};
    
    result.sin_family = AF_INET;
    result.sin_port = htons(port);
    
    inet_pton(AF_INET, address, &result.sin_addr);
    
    return result;
}

function bool
socket_nonblocking(SOCKET socket)
{
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

function bool
socket_blocking(SOCKET socket)
{
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

inline u16
xnetwork_byteswap2(u16 data) {
    return (data >> 8) | (data << 8);
}

inline u32
xnetwork_byteswap4(u32 data) {
    return ((data >> 24) & 0x000000ff) |
    ((data >>  8) & 0x0000ff00) |
    ((data <<  8) & 0x00ff0000) |
    ((data << 24) & 0xff000000);
}

inline u64
xnetwork_byteswap8(u64 data) {
    return ((data >> 56) & 0x00000000000000ff) | 
    ((data >> 40) & 0x000000000000ff00) | 
    ((data >> 24) & 0x0000000000ff0000) | 
    ((data >>  8) & 0x00000000ff000000) | 
    ((data <<  8) & 0x000000ff00000000) | 
    ((data << 24) & 0x0000ff0000000000) | 
    ((data << 40) & 0x00ff000000000000) | 
    ((data << 56) & 0xff00000000000000);
}

#endif