#include "tlv.h"
#include "common.h"
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

char* TlvEncodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, int* len)
{
    static uint32_t sequenceNo = 0;
    uint16_t bufLen = sizeof(TlvHeader) + GetSructLen(info, infoLen, objAddress);
    char* buffer = (char*)malloc(bufLen);
    if (!buffer) {
        printf("malloc failed! file(%s) line %d\n", __FILE__, __LINE__);
        return NULL;
    }
    memset(buffer, 0, bufLen);

    TlvHeader* header = (TlvHeader*)buffer;
    header->totalLen = htonl(bufLen);
    header->version = htons(MSG_VERSION);
    header->commandId = htons(MSG_CMD);
    header->sequenceNo = htonl(sequenceNo++);

    char* bufTmp = buffer + sizeof(TlvHeader);
    uint16_t ret = EncodeStruct(info, infoLen, objAddress, bufTmp);
    *len = bufLen;
    return buffer;
}

int DecodeField(FieldInfo* info, char* fieldAddress, char* buffer, uint32_t len)
{
    uint8_t tag = *(uint8_t *)buffer;
    if (info->tag != tag) {
        LOG_ERR("DecodeField");
        return TLV_ERROR_TAG;
    }

    buffer += TAG_LEN;
    uint16_t valueLen = ntohs(*(uint16_t*)buffer);
    buffer += VALUE_LEN_LEN;

    switch (tag)
    {
    case TAG_STRUCT:
        DecodeStruct(info->fieldInfo, info->fieldInfoLen, fieldAddress, buffer, valueLen);
        break;
    case TAG_SHORT:
    case TAG_INT:
        for (int i = 0; i < valueLen; ++i) {
            fieldAddress[i] = buffer[valueLen - i - 1];
        }
        // memcpy(fieldAddress, buffer, valueLen);
        break;
    case TAG_STRING:
        char *str = (char *)malloc(valueLen);
        memcpy(str, buffer, valueLen);
        // *(char **)fieldAddress = str;
        memcpy(fieldAddress, &str, sizeof(char *));
        break;
    default:
        return TLV_ERROR_OK;
    }
    return TLV_ERROR_OK;
}

int DecodeStruct(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len)
{
    FieldInfo* pInfo = NULL;
    uint8_t infoIndex = 0;
    uint16_t bufferIndex = 0;
    uint16_t valueLen = 0;
    for (char* tmp = buffer; tmp < buffer + len; tmp += TAG_LEN + VALUE_LEN_LEN + valueLen) {
        if (infoIndex >= infoLen) {
            LOG_ERR("DecodeStruct");
            break;
        }
        pInfo = &info[infoIndex++];
        valueLen = ntohs(*(uint16_t*)(tmp + TAG_LEN));
        // objAddress + pInfo->offset 依据字段在结构体中的偏移来设置字段值
        DecodeField(pInfo, objAddress + pInfo->offset, tmp, TAG_LEN + VALUE_LEN_LEN + valueLen);
    }

    return TLV_ERROR_OK;
}

int TlvDecodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len)
{
    return DecodeStruct(info, infoLen, objAddress, buffer, len);
}