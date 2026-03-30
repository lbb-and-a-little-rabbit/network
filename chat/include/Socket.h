#pragma once

#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

class Socket{
protected:
    int sockfd;
    struct sockaddr_in sockaddr;

public:
    Socket();
    ~Socket();
    int getsocketfd();
    void bind(std::string ip, int port);
    void listen();
    void connect();
    int accept();
};