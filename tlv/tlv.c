#include "tlv.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

uint16_t GetSructLen(FieldInfo* info, char* objAddress)
{
    uint16_t len = 0;
    for (int i; i < sizeof(info) / sizeof(info[0]); ++i) {
        len += GetFieldLen(&info[i], objAddress + info[i].offset);
    }

    return len;
}

uint16_t GetFieldLen(FieldInfo* info, char* fieldAddress)
{
    switch (info->tag)
    {
    case TAG_STRUCT:
        info->len = GetSructLen(info->fieldInfo, fieldAddress);
        break;
    case TAG_SHORT:
        info->len = sizeof(short);
        break;
    case TAG_INT:
        info->len = sizeof(int);
        break;
    case TAG_STRING:
        info->len = strlen(fieldAddress) + 1;
        break;
    default:
        printf("GetFieldLen file(%s) line(%d) tag(%d)", __FILE__, __LINE__, info->tag);
        return 0;
    }

    return info->len + sizeof(uint8_t) + sizeof(uint16_t);
}

uint16_t EncodeField(FieldInfo* info, char* fieldAddress, char* out)
{
    uint16_t len = 0;

    memcpy(out, &info->tag, sizeof(uint8_t)); // tag
    out += sizeof(uint8_t);
    len = htons(info->len);
    memcpy(out, &len, sizeof(uint16_t)); // len
    out += sizeof(uint16_t);

    len = sizeof(uint8_t) + sizeof(uint16_t); // tag+len 长度
    switch (info->tag) // value
    {
    case TAG_STRUCT: // 当前字段为一个结构体
        len += EncodeStruct(info->fieldInfo, fieldAddress, out);
        break;
    case TAG_SHORT:
        short* pdataShort = (short*)fieldAddress;
        short dataShort = htons(*pdataShort);
        memcpy(out, &dataShort, sizeof(short));
        len += sizeof(short); // value 长度
        break;
    case TAG_INT:
        int* pdataInt = (int*)fieldAddress;
        int dataInt = htonl(*pdataInt);
        memcpy(out, &dataInt, sizeof(int));
        len += sizeof(int);
        break;
    case TAG_STRING:
        memcpy(out, fieldAddress, info->len);
        len += info->len;
        break;
    default:
        printf("EncodeField failed! invalidate tag:%d\n", info->tag);
        return 0;
    }

    return len;
}

uint16_t EncodeStruct(FieldInfo* info, char* objAddress, char* out)
{
    uint16_t len = 0;
    for (int i = 0; i < sizeof(info) / sizeof(info[0]); ++i) {
        len = EncodeField(&info[i], objAddress + info[i].offset, out);
        out += len;
    }
    return len;
}

// Class ban2;
// TlvEncode(Class, ban2);
char* TlvEncodeImpl(FieldInfo* info, char* objAddress)
{
    static uint32_t sequenceNo = 0;
    uint16_t bufLen = GetSructLen(info, objAddress);
    char* buffer = (char*)malloc(bufLen);
    if (!buffer) {
        printf("malloc failed! file(%s) line %d\n", __FILE__, __LINE__);
        return NULL;
    }
    memset(buffer, 0, bufLen);

    TlvHeader* header = (TlvHeader*)buffer;
    header->totolLen = htonl(bufLen);
    header->version = htons(MSG_VERSION);
    header->commandId = htons(MSG_CMD);
    header->sequenceNo = htonl(sequenceNo++);

    char* bufTmp = buffer + sizeof(TlvHeader);
    EncodeStruct(info, objAddress, bufTmp);
}