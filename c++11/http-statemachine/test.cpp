#include "HttpParser.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

constexpr const uint32_t BACKLOG = 1024;
constexpr const uint32_t RECV_MAX_LEN = 4096;

int main(int argc, char* argv[])
{
    uint16_t port = 10086;
    std::string ip = "0.0.0.0";
    if (argc >= 2) {
        ip = argv[1];
    } 
    if (argc >= 3) {
        port = std::stoul(std::string(argv[2]));
    }

    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd == -1) {
        std::cerr << "Create accept socket failed, errno = " << errno << std::endl;
        return -1;
    }

    int opt = 1;
    int ret = setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (ret == -1) {
        std::cerr << "setsockopt failed, errno = " << errno << std::endl;
        return -1;
    }

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    ret = bind(listenFd, (struct sockaddr*)&address, sizeof(address));
    if (ret == -1) {
        std::cerr << "Bind accept socket failed, errno = " << errno << std::endl;
        return -1;
    }

    ret = listen(listenFd, BACKLOG);
    if (ret == -1) {
        std::cerr << "Listen failed, errno = " << errno << std::endl;
        return -1;
    }

    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientFd = accept(listenFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientFd == -1) {
        std::cerr << "Accept client failed, errno = " << errno << std::endl;
        continue;
    }

    HttpParser httpParser;
    while (true) {
        ssize_t recvLen = recv(clientFd, httpParser.GetWritePtr(), httpParser.GetBufRemainSize(), 0);
        if (recvLen == -1) {
            std::cout << "Recv clientFd failed, errno = " << errno << std::endl;
            continue;
        } else if (recvLen == 0) {
             std::cout << "ClientFd " << clientFd << " closed" << std::endl;
             close(clientFd);
             break;
        }

        httpParser.ParseContent(recvLen);
    }
}