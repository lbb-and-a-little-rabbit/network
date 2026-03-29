#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

int main() {
    //1.create socket
    int socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketfd < 0) {
        std::cerr << "socket created error!\n";
        perror("");
        close(socketfd);
        return 1;
    }
    else 
        std::cout << "socket create success!\n";

    //2.bind socket
    std::string ip = "127.0.0.1";
    int port = 8080;

    struct sockaddr_in sockaddr;
    std::memset(&sockaddr , 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);
    if (bind(socketfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cerr << "socket bind error!\n";
        perror("");
        close(socketfd);
        return 1;
    }
    else
        std::cout << "socket bind success!\n";

    //3.monitor
    if (listen(socketfd, 1024) < 0) { 
        std::cerr << "socket listen error!\n";
        perror("");
        close(socketfd);
        return 1;
    }
    else
        std::cout << "socket listen success!\n";

    while (true) {
        //4.accept client connection
        int connfd = accept(socketfd, nullptr, nullptr);
        if (connfd < 0) {
            std::cerr << "socket acceept error!\n";
            close(socketfd);
            perror("");
            return 1;
        }

        char buf[1024] = {0};
        //5.accept client data
        ssize_t len = recv(connfd, buf, sizeof(buf), 0);
        std::cout << "Data from client : " << buf << '\n';

        //6.send data to client
        send(connfd, buf, len, 0);
    }

    //close socket
    close(socketfd);
    return 0;
}