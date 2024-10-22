#ifndef __COMMAND_H_
#define __COMMAND_H_

/**
 * @file command.h
 * @author your name (you@domain.com)
 * @brief 添加了 fieldMask 字段来表示发送方的结构体中哪些字段有效，接收方根据 fieldMask 
 * 就可以判断发送发哪些字段时有效的，结构体中支持子结构体数据
 * @version 0.1
 * @date 2024-10-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "tlv.h"

#define MSG_VERSION 31111
#define CMD_TEST2 1
#define CMD_STUDENT 2
#define CMD_TEST 3

typedef struct {
    uint32_t totalLen;
    uint16_t version;
    uint16_t commandId;
    uint32_t sequenceNo;
    // char checkNumber[16];
} TlvHeader;

extern uint32_t g_sequenceNo;
void SerialTlvHeader(char* buffer, uint32_t totalLen, uint16_t version,
    uint16_t commandId, uint32_t sequenceNo);

typedef struct
{
    char* quhao;
    char* phone;
} PhoneNum;
TlvImport(PhoneNum);

typedef struct
{
    int stress;
    char* addressName;
} Address;
TlvImport(Address);

 // fieldMask 标识下面的字段哪个是生效的（bit位标识)，接收端收到后可根据此字段来识别发送端哪些字段是有效的
typedef struct
{
    uint32_t fieldMask;
    char* name;
    uint32_t age;
    PhoneNum phoneNum;
    Address* address;
    uint16_t addressLen; // address数组元素个数
    char* data;
    uint32_t dataLen;
} Student;
TlvImport(Student);

typedef struct
{
    uint32_t a;
    uint16_t b;
} Test;
TlvImport(Test);

typedef struct
{
    uint32_t id;
    Address* address;
    uint32_t addressLen;
    PhoneNum phoneNum[2];
} Test2;
TlvImport(Test2);

#endif