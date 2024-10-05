#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "tlv.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
    char* quhao;
    char* phone;
} PhoneNum;

#if 1
TlvFieldBegin(PhoneNum)
TlvField(PhoneNum, quhao, TAG_STRING)
TlvField(PhoneNum, phone, TAG_STRING)
TlvFieldEnd(PhoneNum)
#else
FieldInfo PhoneNumInfo[2] =
{
    {
        .tag = TAG_STRING,
        .offset = OFFSET(PhoneNum, quhao),
    },
    {
        .tag = TAG_STRING,
        .offset = OFFSET(PhoneNum, phone)
    }
};
#endif

typedef struct
{
    int stress;
    char* addressName;
} Address;

#if 1
TlvFieldBegin(Address)
TlvField(Address, stress, TAG_INT)
TlvField(Address, addressName, TAG_STRING)
TlvFieldEnd(Address)
#else
FieldInfo AddressInfo[2] =
{
    {
        .tag = TAG_INT,
        .offset = OFFSET(Address, stress)
    },
    {
        .tag = TAG_STRING,
        .offset = OFFSET(Address, addressName)
    }
};
#endif

typedef struct
{
    char* name;
    uint32_t age;
    PhoneNum phoneNum; // 结构体嵌套只接受组合的类型，不支持下面的 PhoneNum*
    // PhoneNum* phoneNum;
    Address address;
} Student;

#if 1
TlvFieldBegin(Student)
TlvField(Student, name, TAG_STRING)
TlvField(Student, age, TAG_INT)
TlvFieldStruct(Student, PhoneNum, phoneNum, TAG_STRUCT)
TlvFieldStruct(Student, Address, address, TAG_STRUCT)
TlvFieldEnd(Student)
#else
FieldInfo StudentInfo[] = 
{
    {
        .tag = TAG_STRING,
        .offset = OFFSET(Student, name)
    },
    {
        .tag = TAG_INT,
        .offset = OFFSET(Student, age)
    },
    {
        .tag = TAG_STRUCT,
        .offset = OFFSET(Student, phoneNum),
        .fieldInfo = PhoneNumInfo,
        .fieldInfoLen = sizeof(PhoneNumInfo) / sizeof(PhoneNumInfo[0])
    },
    {
        .tag = TAG_STRUCT,
        .offset = OFFSET(Student, address),
        .fieldInfo = AddressInfo,
        .fieldInfoLen = sizeof(AddressInfo) / sizeof(AddressInfo[0])
    }
};
#endif

int ClientSend(char* buffer, uint16_t len)
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

void TestStudent()
{
    Student xiaoming;
    xiaoming.age = 16;
    xiaoming.name = "xiaoming";
    xiaoming.address.addressName = "china";
    xiaoming.address.stress = 99;
    xiaoming.phoneNum.quhao = "123";
    xiaoming.phoneNum.phone = "189723i4u9234";
    uint16_t len = 0;
    char *buffer = TlvEncode(Student, &xiaoming, &len);
    ClientSend(buffer, len);
}

//  nc -l -p 8888 | hexdump -e '16/1 "%02x " "\n"' 可以使用 nc 和 hexdump 来模拟 server，以16进制打印出收到的内容
int main()
{
    TestStudent();

    return 0;
}

#endif