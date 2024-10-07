#ifndef __COMMAND_H_
#define __COMMAND_H_

#include "tlv.h"

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

#endif