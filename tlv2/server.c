#include "command.h"
#include "common.h"
#include "tlv.h"
#include "buffer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void DumpStudent(Student* student)
{
    printf("name = %s\n", student->name);
    printf("age = %d\n", student->age);
    printf("quhao = %s\n", student->phoneNum.quhao);
    printf("phone = %s\n", student->phoneNum.phone);
    printf("address num = %d\n", student->addressLen);
    // printf("stress = %d\n", student->address.stress);
    // printf("addressName = %s\n", student->address.addressName);
    FILE* file = fopen("./out.bin", "wb+");
    if (!file) {
        LOG_ERR("fopen ./out.bin");
        return;
    }
    size_t wsize = fwrite(student->data, 1, student->dataLen, file);
    printf("write ./out.bin %zu bytes\n", wsize);
    fclose(file);
}

static void DumpTest2(const Test2* test2)
{
    printf("id = %d\n", test2->id);
    printf("addressLen = %d\n", test2->addressLen);
    for (int i = 0; i < test2->addressLen; ++i) {
        printf("address[%d].stress = %d\n", i, test2->address[i].stress);
        printf("address[%d].addressName = %s\n", i, test2->address[i].addressName);
    }

    for (int i = 0; i < 2; ++i) {
        if (test2->phoneNum[i].quhao) 
            printf("phoneNum[%d].quhao = %s\n", i, test2->phoneNum[i].quhao);
        if (test2->phoneNum[i].phone) 
            printf("phoneNum[%d].phone = %s\n", i, test2->phoneNum[i].phone);
    }
}

static void DumpTest3(const Test3* test3)
{
    for (int i = 0; i < 2; ++i) {
        if (test3->phoneNum[i].quhao) 
            printf("phoneNum[%d].quhao = %s\n", i, test3->phoneNum[i].quhao);
        if (test3->phoneNum[i].phone) 
            printf("phoneNum[%d].phone = %s\n", i, test3->phoneNum[i].phone);
    }
}

static void DumpTest(const Test* test)
{
    printf("test.a = 0x%x\n", test->a);
    printf("test.b = 0x%x\n", test->b);
}

static void FreeStudent(Student* student)
{
    if (student->name) {
        free(student->name);
    }

    if (student->phoneNum.phone) {
        free(student->phoneNum.quhao);
    }

    if (student->phoneNum.phone) {
        free(student->phoneNum.phone);
    }

    // if (student->address.addressName) {
    //     free(student->address.addressName);
    // }
}

static void FreeTest2(const Test2* test2)
{
    if (test2->address) {
        for (int i = 0; i < test2->addressLen; ++i) {
            if (test2->address[i].addressName) {
                free(test2->address[i].addressName);
            }
        }
        free(test2->address);
    }

    for (int i = 0; i < 2; i++) {
        if (test2->phoneNum[i].quhao) {
            free(test2->phoneNum[i].quhao);
        }
        if (test2->phoneNum[i].phone) {
            free(test2->phoneNum[i].phone);
        }
    }
}

static void FreeTest3(const Test3* test3)
{
    for (int i = 0; i < 2; i++) {
        if (test3->phoneNum[i].quhao) {
            free(test3->phoneNum[i].quhao);
        }
        if (test3->phoneNum[i].phone) {
            free(test3->phoneNum[i].phone);
        }
    }
}
static void MoveBuffer(Buffer* buffer)
{
    uint remainLen = BufferReadableCount(buffer);
    memmove(buffer->data, buffer->data + buffer->readIndex, remainLen);
    BufferSetReadIndex(buffer, 0);
    BufferSetWriteIndex(buffer, remainLen);
}

static int Process(Buffer* buffer)
{
    for (;;) {
        if (BufferReadableCount(buffer) < sizeof(TlvHeader)) { // TlvHeader 没有接收完整，继续接收
            if (BufferGetReadIndex(buffer) != 0) { // 如果 buffer 中的内容不是从0开始的则移动 buffer 中的内容到 buffer 起始
                MoveBuffer(buffer);
            }
            return -1;
        }

        TlvHeader *tmp = (TlvHeader *)(buffer->data);
        uint16_t totalLen = ntohl(tmp->totalLen);
        if (BufferReadableCount(buffer) < totalLen) { // 整个消息没有接收完整，继续接收
            if (BufferGetReadIndex(buffer) != 0) { // 如果 buffer 中的内容不是从0开始的则移动 buffer 中的内容到 buffer 起始
                MoveBuffer(buffer);
            }
            return -2;
        }

        TlvHeader tlvhdr;
        bzero(&tlvhdr, sizeof(tlvhdr));
        tlvhdr.totalLen = ntohl(tmp->totalLen);
        tlvhdr.version = ntohs(tmp->version);
        tlvhdr.commandId = ntohs(tmp->commandId);
        tlvhdr.sequenceNo = ntohl(tmp->sequenceNo);

        switch (tlvhdr.commandId)
        {
        case CMD_STUDENT:
            Student student;
            TlvDecode(Student, &student, BufferReadBuf(buffer) + sizeof(TlvHeader), 
                tlvhdr.totalLen - sizeof(TlvHeader));
            DumpStudent(&student);
            FreeStudent(&student);
            break;
        case CMD_TEST2:
            Test2 test2;
            TlvDecode(Test2, &test2, BufferReadBuf(buffer) + sizeof(TlvHeader), 
                tlvhdr.totalLen - sizeof(TlvHeader));
            DumpTest2(&test2);
            FreeTest2(&test2);
            break;
        case CMD_TEST:
            Test test;
            TlvDecode(Test, &test, BufferReadBuf(buffer) + sizeof(TlvHeader), 
                tlvhdr.totalLen - sizeof(TlvHeader));
            DumpTest(&test);
        case CMD_TEST3:
            Test3 test3;
            TlvDecode(Test3, &test3, BufferReadBuf(buffer) + sizeof(TlvHeader), 
                tlvhdr.totalLen - sizeof(TlvHeader));
            DumpTest3(&test3);
            FreeTest3(&test3);
        default:
            break;
        }
        BufferRetrieve(buffer, tlvhdr.totalLen);
    }
}

static int Start()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        LOG_ERR("socket create");
        return -1;
    }

    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        LOG_ERR("setsockopt");
        return -1;
    }

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8888);

    socklen_t socklen = sizeof(addr);
    int ret = bind(sock, (struct sockaddr*)&addr, socklen);
    if (ret < 0) {
        LOG_ERR("bind");
        printf("errno = %d\n", errno);
        return -1;
    }

    ret = listen(sock, 100);
    if (ret < 0) {
        LOG_ERR("listen");
        return -1;
    }

    Buffer buffer;
    BufferReset(&buffer);
    struct sockaddr_in clientAddr;
    for (;;) {
        bzero(&clientAddr, sizeof(clientAddr));
        int clientfd = accept(sock, (struct sockaddr*)&clientAddr, &socklen);
        if (clientfd < 0) {
            LOG_ERR("accept");
            return -1;
        }

        while (1) {
            int recvLen = recv(clientfd, buffer.data, BufferWriteableCount(&buffer), 0);
            if (recvLen <= 0) {
                close(clientfd);
                BufferReset(&buffer);
                break;
            }
            BufferSetWriteIndex(&buffer, recvLen + BufferGetWriteIndex(&buffer));
            Process(&buffer);
        }
    }
}
int main()
{
    Start();

    return 0;
}