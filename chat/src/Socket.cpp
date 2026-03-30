#include "Socket.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket() {
    sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("socket create fail!\n");
        exit(1);
    }
    memset(&sockaddr, 0, sizeof(sockaddr));
}

int Socket::getsocketfd() {
    return sockfd;
}

void Socket::bind(std::string ip, int port) {
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);

    if (::bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        perror("socket bind fail!\n");
        close(sockfd);
        exit(1);
    }
}

void Socket::listen() {
    if (::listen(sockfd, 1024) < 0) {
        perror("socket listen fail!\n");
        close(sockfd);
        exit(1);
    }
}

void Socket::connect() {
    int connfd = ::connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    if (connfd < 0) {
        perror("socket connect fail!\n");
        close(sockfd);
        exit(1);
    }
}

int Socket::accept() {
    int connfd = ::accept(sockfd, nullptr, nullptr);
    if (connfd < 0) {
        perror("socket accept error!\n");
        close(sockfd);
        exit(1);
    }
    return connfd;
}

Socket::~Socket() {
    close(sockfd);
}