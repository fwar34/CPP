#include "tlv.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

uint16_t GetSructLen(FieldInfo* info, uint16_t infoLen, char* objAddress)
{
    uint16_t len = 0;
    for (int i = 0; i < infoLen; ++i) {
        len += GetFieldLen(&info[i], objAddress + info[i].offset);
    }

    return len;
}

uint16_t GetFieldLen(FieldInfo* info, char* fieldAddress)
{
    switch (info->tag)
    {
    case TAG_STRUCT: // 字段为一个结构体
        info->len = GetSructLen(info->fieldInfo, info->fieldInfoLen, fieldAddress);
        break;
    case TAG_SHORT:
        info->len = sizeof(short);
        break;
    case TAG_INT:
        info->len = sizeof(int);
        break;
    case TAG_STRING:
        uint16_t len = strlen(*(char**)fieldAddress);
        info->len = len ? len + 1 : 0;
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
        len += EncodeStruct(info->fieldInfo, info->fieldInfoLen, fieldAddress, out);
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
        memcpy(out, *(char**)fieldAddress, info->len);
        len += info->len;
        break;
    default:
        printf("EncodeField failed! invalidate tag:%d\n", info->tag);
        return 0;
    }

    return len;
}

uint16_t EncodeStruct(FieldInfo* info, uint16_t infoLen, char* objAddress, char* out)
{
    uint16_t len = 0;
    for (int i = 0; i < infoLen; ++i) {
        len += EncodeField(&info[i], objAddress + info[i].offset, out + len);
    }
    return len;
}

char* TlvEncodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress)
{
    static uint32_t sequenceNo = 0;
    uint16_t bufLen = GetSructLen(info, infoLen, objAddress);
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
    uint16_t ret = EncodeStruct(info, infoLen, objAddress, bufTmp);
    return buffer;
}