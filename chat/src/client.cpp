#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

// ================= 登录阶段 =================
bool handle_login(int sockfd) {
    char buf[1024];

    while (true) {
        memset(buf, 0, sizeof(buf));

        int n = recv(sockfd, buf, sizeof(buf), 0);
        if (n <= 0) {
            std::cout << "server closed\n";
            return false;
        }

        std::cout << buf;

        // 判断登录成功
        if (strstr(buf, "A user has logged in!\n") != nullptr ||
            strstr(buf, "log in success!\n") != nullptr) {
            return true;
        }

        std::string input;
        std::getline(std::cin, input);
        input += "\n";

        send(sockfd, input.c_str(), input.size(), 0);
    }
}

// ================= 聊天接收线程 =================
void recv_loop(int sockfd) {
    char buf[1024];

    while (true) {
        memset(buf, 0, sizeof(buf));

        int n = recv(sockfd, buf, sizeof(buf), 0);
        if (n <= 0) {
            std::cout << "server closed\n";
            exit(0);
        }

        std::cout << buf << std::flush;
    }
}

// ================= 聊天发送 =================
void chat_mode(int sockfd) {
    std::thread t(recv_loop, sockfd);

    std::string input;
    while (true) {
        std::getline(std::cin, input);
        input += "\n";

        send(sockfd, input.c_str(), input.size(), 0);
    }

    t.join();
}

// ================= main =================
int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "10.54.183.52", &addr.sin_addr);

    if (connect(sockfd, (sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect error");
        return 1;
    }

    std::cout << "Connected to server!\n";

    // ===== 阶段1：登录/注册（不改原逻辑）=====
    if (!handle_login(sockfd)) {
        close(sockfd);
        return 0;
    }

    std::cout << "==== Enter Chat Room ====\n";

    // ===== 阶段2：聊天模式 =====
    chat_mode(sockfd);

    close(sockfd);
    return 0;
}