// 使用 writev 来发送 http 头和 http body 两块内存数组
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main()
{
    struct sockaddr_in address = {0};
    address.
}