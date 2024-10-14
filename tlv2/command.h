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

typedef struct
{
    char* quhao;
    char* phone;
} PhoneNum;

#if 1
typedef enum
{
    TAG_QUHAO,
    TAG_PHONE,
} TagPhoneNum;

TlvFieldBegin(PhoneNum)
TlvField(TAG_QUHAO, PhoneNum, quhao, FIELD_TYPE_STRING)
TlvField(TAG_PHONE, PhoneNum, phone, FIELD_TYPE_STRING)
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
typedef enum
{
    TAG_STRESS,
    TAG_ADDRESS_NAME,
} TagAddress;

TlvFieldBegin(Address)
TlvField(TAG_STRESS, Address, stress, FIELD_TYPE_4BYTE)
TlvField(TAG_ADDRESS_NAME, Address, addressName, FIELD_TYPE_STRING)
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

#if 1
typedef enum
{
    TAG_FIELD_TYPE_MASK,
    TAG_NAME,
    TAG_AGE,
    TAG_PHONENUM,
    TAG_ADDRESS,
    TAG_ADDRESSLEN,
    TAG_DATA,
    TAG_DATALEN,
} TagStudent;

TlvFieldBegin(Student)
TlvField(TAG_NAME, Student, name, FIELD_TYPE_STRING)
TlvField(TAG_AGE, Student, age, FIELD_TYPE_4BYTE)
TlvFieldStruct(TAG_PHONENUM, Student, phoneNum, PhoneNum)
TlvFieldStructPtr(TAG_ADDRESS, Student, address, Address, addressLen, TAG_ADDRESS_NAME, FIELD_TYPE_2BYTE)
TlvFieldBytePtr(TAG_DATA, Student, data, dataLen, TAG_DATALEN, FIELD_TYPE_4BYTE)
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

typedef struct
{
    uint32_t a;
    uint16_t b;
} Test;

typedef enum {
    TAG_A,
    TAG_B,
} TestTag; 

TlvFieldBegin(Test)
TlvField(TAG_A, Test, a, FIELD_TYPE_4BYTE)
TlvField(TAG_B, Test, b, FIELD_TYPE_2BYTE)
TlvFieldEnd(Test)

#endif