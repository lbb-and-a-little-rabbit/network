#include "net_platform.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <unordered_set>
#include <string>

std::unordered_set<std::string> quit_cmd;

int main(int argc, char *argv[]) {
    quit_cmd.insert("quit");
    quit_cmd.insert("q");
    quit_cmd.insert("exit");

    net_init();

    socket_t sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_create_falied(sockfd)) {
        std::cerr << "socket create failed!\n";
        net_cleanup();
        return 1;
    }

    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(8888);

    if (::bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cerr << "socket bind failed!\n";
        close_socket(sockfd);
        net_cleanup();
        return 1;
    }

    if(::listen(sockfd, 1024) < 0) {
        std::cerr << "socket listen failed!\n";
        close_socket(sockfd);
        net_cleanup();
        return 1;
    }

    std::cout << "Server listening...\n";

    while (true) {
        socket_t connfd = accept(sockfd, nullptr, nullptr);
        if (socket_create_falied(connfd)) {
            std::cerr << "socket connect falied!\n";
            close_socket(sockfd);
            net_cleanup();
            return 1;
        }

        char connect_reply[1024] = {0};
        ::recv(connfd, connect_reply, sizeof(connect_reply), 0);
        std::cout << connect_reply << '\n';

        while (true) {
            std::string cmd;
            std::cout << "Input the command!\n";
            std::getline(std::cin, cmd);
            ssize_t len = ::send(connfd, cmd.c_str(), cmd.size(), 0);
            if (quit_cmd.count(cmd)) break;
            char buf[1024] = {0};
            len = ::recv(connfd, buf, sizeof(buf), 0);
            std::cout << buf << '\n';
        }
        close_socket(connfd);
    }

    net_cleanup();
    return 0;
}