#include "Socket.h"
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

std::vector<int> clients;
std::mutex mtx;

void broadcast_message(const std::string &msg, int sender_fd) {
    std::lock_guard<std::mutex> lock(mtx);

    for (auto fd : clients) {
        if (fd != sender_fd) {
            send(fd, msg.c_str(), msg.size(), 0);
        }
    }
}

void register_handle(int sockfd, int connfd) {
    while (true) {
        std::string info = "Please enter your username(less than 1024 characters)\n";
        ssize_t l0 = ::send(connfd, info.c_str(), info.size(), 0);
        char buf[1024] = {0};
        ssize_t l1 = ::recv(connfd, buf, sizeof(buf), 0);

        FILE *fp = fopen("../users.txt","r");
        char userbuf[1024] = {0};
        bool valid = true;
        while (fgets(userbuf, sizeof(userbuf), fp)) {
            if (!strcmp(userbuf, buf)) {
                std::string errorinfo = "This username has been used!Please try another one!\n";
                ssize_t l2 = ::send(connfd, errorinfo.c_str(), errorinfo.size(), 0);
                valid = false;
                break;
            }
        }
        fclose(fp);

        if (valid) {
            FILE *wp = fopen("../users.txt", "a");
            fprintf(wp, "%s", buf);
            fclose(wp);

            info = "Please enter your password(less than 1024 characters)!\n";
            ssize_t l3 = ::send(connfd, info.c_str(), info.size(), 0);
            memset(buf, 0, sizeof(buf));
            ssize_t l4 = ::recv(connfd, buf, sizeof(buf), 0);
            FILE *kp = fopen("../keys.txt", "a");
            fprintf(kp ,"%s", buf);
            fclose(kp);

            std::string succinfo = "Register success!\n";
            ssize_t l5 = ::send(connfd, succinfo.c_str(), succinfo.size(), 0);
            return;
        }
    }
}

void login_handle(int sockfd, int connfd, std::string &username) {
    int cnt = 0;
    while (true) {
        cnt = 0;
        std::string info = "Please enter your username!\n";
        ssize_t l0 = ::send(connfd, info.c_str(), info.size(), 0);
        char buf[1024] = {0};
        ssize_t l1 = ::recv(connfd, buf, sizeof(buf), 0);
        FILE *fp = fopen("../users.txt", "r");
        char userbuf[1024] = {0}; 
        bool valid = false;
        while (fgets(userbuf, sizeof(userbuf), fp)) {
            if (!strcmp(userbuf, buf)){
                valid = true;
                break;
            }
            cnt++;
        }
        if (!valid) {
            std::string errorinfo = "Invalid username!\n";
            ssize_t l2 = ::send(connfd, errorinfo.c_str(), errorinfo.size(), 0);
            continue;
        }

        username = buf;
        username.erase(username.find_last_not_of("\n") + 1);
        break;
    }

    while (true) {
        int idx = 0;
        std::string info = "Please enter your password!\n";
        ssize_t l0 = ::send(connfd, info.c_str(), info.size(), 0);
        char buf[1024] = {0};
        ssize_t l1 = ::recv(connfd, buf, sizeof(buf), 0);
        FILE *fp = fopen("../keys.txt", "r");
        char userbuf[1024] = {0}; 
        bool valid = false;
        while (fgets(userbuf, sizeof(userbuf), fp)) {
            if (idx < cnt) {
                idx++;
                continue;
            }
            if (!strcmp(userbuf, buf)){
                valid = true;
                break;
            }
        }
        if (!valid) {
            std::string errorinfo = "Invalid password!\n";
            ssize_t l2 = ::send(connfd, errorinfo.c_str(), errorinfo.size(), 0);
            continue;
        }
        std::string succinfo = "log in success!\n";
        ssize_t l5 = ::send(connfd, succinfo.c_str(), succinfo.size(), 0);
        break;
    }
}

void RL_handle(int sockfd, int connfd, std::string &username) {
    std::string info = "Register or log in [r/l]?\n";
    while (true) {
        ssize_t l0 = ::send(connfd, info.c_str(), info.size(), 0);
        char buf[1024] = {0};
        ssize_t l1 = ::recv(connfd, buf, sizeof(buf), 0);
        if (!strcmp(buf, "r\n")) {
            register_handle(sockfd, connfd);
        }
        else if (!strcmp(buf, "l\n")) {
            login_handle(sockfd, connfd,username);
            break;
        }
        else {
            std::string errorinfo = "Wrong command!Please try again!\n";
            ssize_t l2 = ::send(connfd, errorinfo.c_str(), errorinfo.size(), 0);
            continue;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1 || (argc == 2 && !strcmp(argv[1], "group_chat"))) {
        std::cout << "creating group chat server!\n";
        Socket socket;
        socket.bind("0.0.0.0", 8888);
        socket.listen();
        std::cout << "server begins to listen!\nPlease wait...\n";

        while (true) {
            int connfd = socket.accept();
            std::thread(
                [connfd,&socket]() {
                    int sockfd = socket.getsocketfd();
                    std::string username;
                    std::cout << "New connection occurs!\n";
                    RL_handle(sockfd, connfd, username);
                    std::cout << "A user has logged in!\n";

                    //TODO
                    {
                        {
                            std::lock_guard<std::mutex> lock(mtx);
                            clients.push_back(connfd);
                        }

                        char buf[1024];

                        while (true) {
                            memset(buf, 0, sizeof(buf));
                            int n = recv(connfd, buf, sizeof(buf), 0);

                            if (n <= 0) {
                                std::cout << username << " disconnected\n";

                                // 从列表移除
                                std::lock_guard<std::mutex> lock(mtx);
                                clients.erase(
                                    std::remove(clients.begin(), clients.end(), connfd),
                                    clients.end()
                                );

                                close(connfd);
                                break;
                            }

                            std::string msg = username + ": " + buf;

                            // 打印到 server
                            std::cout << msg;

                            // 广播给所有人
                            broadcast_message(msg, connfd);
                        }
                    }
                }
            ).detach();
        }
    }
    return 0;
}