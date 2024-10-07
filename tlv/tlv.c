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
        uint16_t binaryLen = 0;
        if (info[i].tag == TAG_BINARY) {
            binaryLen = *(uint16_t*)(objAddress + info[i].binaryLenOffset);
        }
        len += GetFieldLen(&info[i], objAddress + info[i].offset, binaryLen);
    }

    return len;
}

uint16_t GetFieldLen(FieldInfo* info, char* fieldAddress, uint16_t binaryLen)
{
    switch (info->tag)
    {
    case TAG_STRUCT: // 字段为一个结构体
        info->len = GetSructLen(info->fieldInfo, info->fieldInfoLen, fieldAddress);
        break;
    case TAG_1BYTE:
        info->len = sizeof(uint8_t);
        break;
    case TAG_2BYTE:
        info->len = sizeof(uint16_t);
        break;
    case TAG_4BYTE:
        info->len = sizeof(uint32_t);
        break;
    case TAG_8BYTE:
        info->len = sizeof(uint64_t);
        break;
    case TAG_STRING:
        uint16_t len = strlen(*(char**)fieldAddress);
        info->len = len ? len + 1 : 0;
        break;
    case TAG_BINARY:
        info->len = binaryLen;
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
    case TAG_1BYTE:
        memcpy(out, fieldAddress, sizeof(uint8_t));
        len += sizeof(uint8_t);
        break;
    case TAG_2BYTE:
        uint16_t dataShort = htons(*(uint16_t*)fieldAddress);
        memcpy(out, &dataShort, sizeof(uint16_t));
        len += sizeof(uint16_t); // value 长度
        break;
    case TAG_4BYTE:
        uint32_t dataInt = htonl(*(uint32_t*)fieldAddress);
        memcpy(out, &dataInt, sizeof(uint32_t));
        len += sizeof(uint32_t);
        break;
    case TAG_8BYTE:
        // 大端转小端
        for (uint8_t i = 0; i < 8; ++i) {
            out[i] = fieldAddress[8 - i - 1];
        }
        len += sizeof(uint64_t);
        break;
    case TAG_STRING:
        memcpy(out, *(char**)fieldAddress, info->len);
        len += info->len;
        break;
    case TAG_BINARY:
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

    printf("sizeof(TlvHeader) = %d\n", sizeof(TlvHeader));
    char* bufTmp = buffer + sizeof(TlvHeader);
    uint16_t ret = EncodeStruct(info, infoLen, objAddress, bufTmp);
    if (ret != bufLen - sizeof(TlvHeader)) {
        LOG_ERR("TlvEncodeImpl");
        free(buffer);
        return NULL;
    }
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
    case TAG_1BYTE:
    case TAG_2BYTE:
    case TAG_4BYTE:
    case TAG_8BYTE:
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
    case TAG_BINARY:
        char *str2 = (char *)malloc(valueLen);
        memcpy(str2, buffer, valueLen);
        // *(char **)fieldAddress = str;
        memcpy(fieldAddress, &str2, sizeof(char *));
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