#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")

    typedef SOCKET socket_t;

    inline void net_init() {
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
    }

    inline bool socket_create_falied(socket_t s) {
        return s == INVALID_SOCKET;
    }

    inline void net_cleanup() {
        WSACleanup();
    }

    inline void close_socket(socket_t s) {
        closesocket(s);
    }

#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>

    typedef int socket_t;

    inline void net_init() {}

    inline bool socket_create_falied(socket_t s) {
        return s < 0;
    }

    inline void net_cleanup() {}

    inline void close_socket(socket_t s) {
        close(s);
    }

#endif

bool send_all(socket_t sockfd, const char* data, size_t len) {
    size_t total = 0;

    while (total < len) {
        int sent = ::send(sockfd, data + total, static_cast<int>(len - total), 0);
        if (sent <= 0) {
            return false;
        }
        total += sent;
    }

    return true;
}

bool recv_all(socket_t sockfd, char* data, size_t len) {
    size_t total = 0;

    while (total < len) {
        int received = ::recv(sockfd, data + total, static_cast<int>(len - total), 0);
        if (received <= 0) {
            return false;
        }
        total += received;
    }

    return true;
}

bool send_file(socket_t sockfd, const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::cerr << "无法打开文件: " << file_path << '\n';
        return false;
    }

    std::string filename = std::filesystem::path(file_path).filename().string();

    file.seekg(0, std::ios::end);
    uint64_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    uint64_t name_len = filename.size();

    // 1. 发送文件名长度
    if (!send_all(sockfd, reinterpret_cast<char*>(&name_len), sizeof(name_len))) {
        return false;
    }

    // 2. 发送文件名
    if (!send_all(sockfd, filename.c_str(), filename.size())) {
        return false;
    }

    // 3. 发送文件大小
    if (!send_all(sockfd, reinterpret_cast<char*>(&file_size), sizeof(file_size))) {
        return false;
    }

    // 4. 循环发送文件内容
    const size_t BUF_SIZE = 4096;
    char buffer[BUF_SIZE];

    while (file) {
        file.read(buffer, BUF_SIZE);
        std::streamsize bytes_read = file.gcount();

        if (bytes_read > 0) {
            if (!send_all(sockfd, buffer, static_cast<size_t>(bytes_read))) {
                return false;
            }
        }
    }

    std::cout << "文件发送完成: " << filename << '\n';
    return true;
}

bool recv_file(socket_t sockfd, const std::string& save_dir = ".") {
    uint64_t name_len = 0;

    // 1. 接收文件名长度
    if (!recv_all(sockfd, reinterpret_cast<char*>(&name_len), sizeof(name_len))) {
        return false;
    }

    // 2. 接收文件名
    std::vector<char> name_buf(name_len + 1, '\0');

    if (!recv_all(sockfd, name_buf.data(), name_len)) {
        return false;
    }

    std::string filename(name_buf.data());

    // 3. 接收文件大小
    uint64_t file_size = 0;

    if (!recv_all(sockfd, reinterpret_cast<char*>(&file_size), sizeof(file_size))) {
        return false;
    }

    std::string save_path = save_dir + "/" + filename;

    std::ofstream file(save_path, std::ios::binary);
    if (!file) {
        std::cerr << "无法创建文件: " << save_path << '\n';
        return false;
    }

    // 4. 循环接收文件内容
    const size_t BUF_SIZE = 4096;
    char buffer[BUF_SIZE];

    uint64_t received_total = 0;

    while (received_total < file_size) {
        uint64_t remaining = file_size - received_total;
        size_t to_recv = remaining < BUF_SIZE ? static_cast<size_t>(remaining) : BUF_SIZE;

        int received = ::recv(sockfd, buffer, static_cast<int>(to_recv), 0);

        if (received <= 0) {
            return false;
        }

        file.write(buffer, received);
        received_total += received;
    }

    std::cout << "文件接收完成: " << save_path << '\n';
    return true;
}