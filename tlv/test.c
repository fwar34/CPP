#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "tlv.h"

typedef struct
{
    char* quhao;
    char* phone;
} PhoneNum;

typedef struct
{
    int stress;
    char* addressName;
} Address;

typedef struct
{
    char* name;
    uint32_t age;
    PhoneNum* phoneNum;
    Address address;
} Student;

FieldInfo StudentInfo[4] = 
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
        .tag = TAG_STRING,
        .offset = OFFSET(Student, PhoneNum)
    },
};

int main()
{

}

#endif