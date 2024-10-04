#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "tlv.h"

typedef struct
{
    char* quhao;
    char* phone;
} PhoneNum;

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

typedef struct
{
    int stress;
    char* addressName;
} Address;

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

typedef struct
{
    char* name;
    uint32_t age;
    PhoneNum phoneNum;
    Address address;
} Student;

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

void TestStudent()
{
    Student xiaoming;
    xiaoming.age = 16;
    xiaoming.name = "xiaoming";
    xiaoming.address.addressName = "china";
    xiaoming.address.stress = 99;
    xiaoming.phoneNum.quhao = "123";
    xiaoming.phoneNum.phone = "189723i4u9234";
    char *buffer = TlvEncode(Student, &xiaoming);
}

int main()
{
    TestStudent();

    return 0;
}

#endif