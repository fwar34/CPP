#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

constexpr const size_t BUF_LEN = 1024;
constexpr const unsigned short SERVER_PORT = 10086;

int Start()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "create socket failed!" << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    socklen_t socklen = sizeof(server_addr);
    char recv_buf[BUF_LEN] = {0};

    struct sockaddr_in src_addr;
    socklen_t src_socklen;

    for (;;) {
        std::ostringstream oss;
        static size_t index = 0;
        oss << "This is " << index++;
        ssize_t ret = sendto(sock, oss.str().c_str(), oss.str().size(), 0, (struct sockaddr*)&server_addr, socklen);
        if (ret == -1) {
            std::cerr << "send to server failed! errno = " << errno << std::endl;
            return -1;
        }

        memset(recv_buf, 0, sizeof(recv_buf));
        memset(&src_addr, 0, sizeof(src_addr));
        ret = recvfrom(sock, recv_buf, BUF_LEN, 0, (struct sockaddr*)&src_addr, &src_socklen);
        if (ret == -1) {
            std::cerr << "recvfrom failed! errno = " << errno << std::endl;
            return -1;
        }
        recv_buf[ret] = '\0';
        std::cout << "recv reply msg[" << recv_buf << "] from server" << std::endl;
    }

    close(sock);

    return 0;
}

int main(int argc, char* argv[])
{
    return Start();
}
