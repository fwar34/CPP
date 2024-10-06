#include "common.h"
#include "tlv.h"
#include "buffer.h"
#include <sys/socket.h>
#include <string.h>

extern int Process(Buffer* buffer);
static void MoveBuffer(Buffer* buffer)
{
    uint remainLen = BufferReadableCount(buffer);
    memmove(buffer.data, buffer.data + buffer.readIndex, remainLen);
    BufferSetReadIndex(&buffer, 0);
    BufferSetWriteIndex(&buffer, remainLen);
}

static int Start()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        LOG_ERR("socket create");
        return -1;
    }

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_addr = AF_INET;
    addr.sin_family = ;
    addr.sin_port = htons(8888);

    socklen_t socklen;
    int ret = bind(sock, (struct sockaddr*)&addr, &socklen);
    if (ret < 0) {
        LOG_ERR("bind");
        return -1;
    }

    ret = listen(sock, 100);
    if (ret < 0) {
        LOG_ERR("listen");
        return -1;
    }

    Buffer buffer;
    BufferReset(&buffer);
    for (;;) {
        struct sockaddr_in clientAddr;
        int clientfd = accept(sock, (struct sockaddr*)&clientAddr, &socklen);
        if (clientfd < 0) {
            LOG_ERR("accept");
            return -1;
        }

        while (true) {
            int recvLen = recv(clientfd, buffer.data, BufferReadableCount(&buffer), 0);
            if (recvLen <= 0) {
                close(clientfd);
                ResetBuffer(&buffer);
                break;
            }
            BufferSetWriteIndex(recvLen + BufferGetWriteIndex(&buffer));
            Process(&buffer);
        }
    }
}
int main()
{
    Start();

    return 0;
}