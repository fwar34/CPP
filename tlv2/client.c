#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "common.h"
#include "command.h"
#include "tlv.h"
#include "buffer.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int ClientSend(char* buffer, uint16_t len)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("create socket error, file(%s) line(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    const char* serverIp = "127.0.0.1";
    // const char* serverIp = "192.168.125.142";
    unsigned short port = 8888;

    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIp, &serverAddr.sin_addr);

    int ret = connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        printf("create socket error, file(%s) line(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    ret = send(sock, buffer, len, 0);
    if (ret < 0) {
        printf("create socket error, file(%s) line(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    close(sock);
}

static void TestStudent()
{
    Student xiaoming;
    xiaoming.age = 16;
    xiaoming.name = "xiaoming";
    Address addr[2];
    addr[0].stress = 99;
    addr[0].addressName = "china99";
    addr[1].stress = 100;
    addr[1].addressName = "china100";
    xiaoming.address = addr;
    xiaoming.addressLen = 2;
    xiaoming.phoneNum.quhao = "123";
    xiaoming.phoneNum.phone = "189723i4u9234";
    xiaoming.data = (char*)malloc(999);
    bzero(xiaoming.data, 999);
    FILE* file = fopen("./data.bin", "rb");
    if (!file) {
        LOG_ERR("fopen");
        return;
    }
    size_t rsize = fread(xiaoming.data, 1, 999, file);
    fclose(file);
    printf("read %zu bytes from ../data.bin\n", rsize);
    xiaoming.dataLen = rsize;
    uint16_t len = 0;
    char *buffer = TlvEncode(Student, &xiaoming, &len);
    if (!buffer) {
        LOG_ERR("ElvEncode");
        return;
    }
    ClientSend(buffer, len);
}

void TestFunction()
{
    Test test;
    test.a = 0x12345678;
    test.b = 0x1234;
    uint16_t len = 0;
    char* buffer = TlvEncode(Test, &test, &len);
}

//  nc -l -p 8888 | hexdump -e '16/1 "%02x " "\n"' 可以使用 nc 和 hexdump 来模拟 server，以16进制打印出收到的内容
int main()
{
    TestFunction();
    TestStudent();

    return 0;
}

#endif