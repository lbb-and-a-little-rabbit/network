#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")

    typedef SOCKET socket_t;

    inline void net_init() {
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
    }

    inline bool socket_create_falied(socket_t s) {
        return s == INVALID_SOCKET;
    }

    inline void net_cleanup() {
        WSACleanup();
    }

    inline void close_socket(socket_t s) {
        closesocket(s);
    }

#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>

    typedef int socket_t;

    inline void net_init() {}

    inline bool socket_create_falied(socket_t s) {
        return s < 0;
    }

    inline void net_cleanup() {}

    inline void close_socket(socket_t s) {
        close(s);
    }

#endif