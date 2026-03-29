#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>

int main() {
    //1.create socket
    int socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketfd < 0) {
        std::cerr << "socket create error!\n";
        perror("");
        close(socketfd);
        return 1;
    }
    else
        std::cout << "socket create success!\n";

    //2.connect server
    std::string ip= "127.0.0.1";
    int port = 8080;

    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);
    if (connect(socketfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cerr << "socket connect error!\n";
        perror("");
        close(socketfd);
        return 1;
    }
    else 
        std::cout << "socket connect success!\n";
    
    //3.send data to server
    std::string data;
    std::cin >> data;
    send(socketfd, data.c_str(), data.size(), 0);

    //4.recv data from server
    char buf[1024] = {0};
    recv(socketfd, buf, sizeof(buf), 0);
    std::cout << "Data from server : " << buf << '\n';

    close(socketfd);
    return 0;
}