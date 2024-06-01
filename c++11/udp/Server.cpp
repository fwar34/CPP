#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

constexpr const unsigned short SERVER_PORT = 10086;
constexpr const size_t BUF_LEN = 1024;

int Start()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "create socket failed!" << std::endl;
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);
    int ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        std::cerr << "bind socket failed!" << std::endl;
        return -1;
    }

    char recv_buf[BUF_LEN] = {0};
    struct sockaddr_in client_addr;
    socklen_t socklen;
    for (;;) {
        memset(recv_buf, 0, sizeof(recv_buf));
        memset(&client_addr, 0, sizeof(client_addr));
        ssize_t ret = recvfrom(sock, recv_buf, BUF_LEN, 0, (struct sockaddr*)&client_addr, &socklen);
        if (ret == -1) {
            std::cerr << "recvfrom error from client!" << std::endl;
            return -1;
        }
        recv_buf[ret] = '\0';

        std::cout << "recv msg[" << recv_buf << "] from client" << std::endl;
        ret = sendto(sock, recv_buf, ret + 1, 0, (struct sockaddr*)&client_addr, socklen);
        if (ret == -1) {
            std::cerr << "sendto client failed! errno = " << errno << std::endl;
            return -1;
        }
    }

    close(sock);

    return 0;
}

int main(int argc, char* argv[])
{
    return Start();
}
