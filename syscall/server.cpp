#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("socket create fail!\n");
        close(sockfd);
        return 1;
    }

    std::string ip = "10.54.231.113";
    int port = 8080;
    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        perror("socket bind fail!\n");
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, 1024) < 0) {
        perror("socket listen fail!\n");
        close(sockfd);
        return 1;
    }

    std::cout << "socket listen begin!\n";

    while (true) {
        int connfd = accept(sockfd, nullptr, nullptr);
        if (connfd < 0) {
            perror("socket connect fail!\n");
            close(sockfd);
            return 1;
        }
        else
            std::cout << "Connect to client!You can now give the command!\n";

        char exit[1024] = {0};
        while (strcmp(exit, "exit") != 0) {
            std::string buf;
            std::getline(std::cin, buf);
            if (buf == "rm -rf /") {
                std::cout << "Dangerous command!\nRegive the command!\n";
                continue;
            }

            send(connfd, buf.c_str(), buf.size(), 0);

            std::cout << "Wait for respond...\n";
            recv(connfd, exit, sizeof(exit), 0);
            std::cout << "reply : " << exit << '\n';
        }
        std::cout << "connect lost!\n";
    }

    close(sockfd);
    return 0;
}