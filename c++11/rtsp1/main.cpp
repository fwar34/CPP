#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

constexpr const unsigned short SERVER_PORT = 554;
constexpr size_t BUF_LEN = 1024;

struct BufSlice
{
    char *buf;
    size_t len;
};

/**
 * 查找下一个 \r\n 字符，将 \r\n 之前的 buf 片段放到 BufSlice 中
 */
BufSlice FindNextRN(char *buf, size_t len)
{
    BufSlice bufSlice = {0};
    bufSlice.buf = buf;
    while (*buf != '\r' || (bufSlice.len < len && *(buf + 1) != '\n')) { // bufSlice.len < len 保证 buf + 1 访问的字符不会数组越界
        buf++;
        bufSlice.len++;
    }

    return bufSlice;
}

BufSlice *SplitString(char *buf, size_t len, char splitter)
{
    return nullptr;
}

void HandleBufSlice(BufSlice& slice)
{

}

// xxxx\r\n
// \r\n
// xxxxxxx
/**
 * 将字符串切分成以 \r\n 分割的 buf 片段，放到 BufSlice 中
 */
int HandleProtocol(char *buf, size_t len)
{
    while (len > 0) {
        BufSlice bufSlice = FindNextRN(buf, len);
        if (bufSlice.len == 0) {
            break;
        }
        std::cout << "HandleProtocol: [" << bufSlice.buf << "] len = " << bufSlice.len << std::endl;

        buf += bufSlice.len + 2;
        len -= bufSlice.len + 2;
    }
    std::cout << "------------------------------------------" << std::endl;

    return 0;
}

void Process(int client_fd)
{
    char buf[BUF_LEN] = {0};
    for (;;) {
        memset(buf, 0, BUF_LEN);
        size_t n = recv(client_fd, buf, BUF_LEN, 0);
        if (n == -1) {
            std::cerr << "recv failed, errno = " << errno << std::endl;
            return;
        }

        HandleProtocol(buf, n);
    }
}

int main(int argc, char *argv[])
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        std::cerr << "socket failed, errno = " << errno << std::endl;
        return -1;
    }

    int opt = 1;
    int ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (ret == -1) {
        std::cerr << "setsockopt failed, errno = " << errno << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        std::cerr << "bind failed, errno = " << errno << std::endl;
        return -1;
    }

    ret = listen(listen_fd, 0);
    if (ret == -1) {
        std::cerr << "listen failed, errno = " << errno << std::endl;
        return -1;
    }

    struct sockaddr client_addr = {0};
    socklen_t client_addr_len;
    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        std::cerr << "accept failed, errno = " << errno << std::endl;
        return -1;
    }

    Process(client_fd);

    return 0;
}
