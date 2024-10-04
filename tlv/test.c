#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "tlv.h"

typedef struct
{
    char* name;
    uint32_t age;
    char* address;
} Student;

#if 1

typedef struct
{
    int level;
    char* phoneNum;
    Student xiaofeng;
    Student* xiaoming;
} Class;

#endif

FieldInfo* StudentInfo = {
    {
        .tag = TAG_STRING,
    },
    {
        .tag = TAG_INT,
    },
    {
        .tag = TAG_STRING,
    }
};

int main()
{

}

#endif