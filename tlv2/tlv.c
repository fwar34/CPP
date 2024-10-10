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
    FieldInfo* prev = NULL;
    // 此函数会给每个字段的 FieldInfo 中的 len 设置上实例 filedAddress 对应字段的大小，
    // 在 Encode 函数中会直接使用此处赋值的 len
    // (FIELD_STRING 和 TAG_BINARY 的 len 设置的是对应 buffer 的长度)
    switch (info->tag)
    {
    case FIELD_STRUCT: // 字段为一个结构体
        info->len = GetSructLen(info->fieldInfo, info->fieldInfoLen, fieldAddress);
        break;
    case FIELD_STRUCT_ARRAY:
        prev = info - 1; // 宏定义保证-1位置存在FieldInfo
        for (int i = 0; i < prev->len; ++i) { // prev->len 是数组的元素个数, prev->type 是数组单个元素字节大小
            info->len += GetSructLen(info->fieldInfo, info->fieldInfoLen, fieldAddress + prev->type * i);
        }
        break;
    case FIELD_STRUCT_PTR:
        prev = info - 1;
        uint16_t arrayLen = *(uint16_t*)(prev->offset);
        for (int i = 0; i < arrayLen; ++i) {
            info->len += GetSructLen(info->fieldInfo, info->fieldInfoLen, 
                *(char**)fieldAddress + prev->type * i);
        }
        break;
    case FIELD_1BYTE:
        info->len = sizeof(uint8_t);
        break;
    case FIELD_2BYTE:
        info->len = sizeof(uint16_t);
        break;
    case FIELD_4BYTE:
        info->len = sizeof(uint32_t);
        break;
    case FIELD_8BYTE:
        info->len = sizeof(uint64_t);
        break;
    case FIELD_STRING:
        uint16_t len = strlen(*(char**)fieldAddress);
        info->len = len ? len + 1 : 0;
        break;
    case FIELD_BYTE_ARRAY:
        prev = info - 1;
        info->len = prev->len;
        break;
    case FIELD_BYTE_PTR:
        prev = info - 1;
        info->len = *(uint16_t*)(prev->offset);
        break;
    case FIELD_LINKED_PTR:
        // TODO
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
    case FIELD_STRUCT: // 当前字段为一个结构体
        len += EncodeStruct(info->fieldInfo, info->fieldInfoLen, fieldAddress, out);
        break;
    case FIELD_1BYTE:
        memcpy(out, fieldAddress, sizeof(uint8_t));
        len += sizeof(uint8_t);
        break;
    case FIELD_2BYTE:
        uint16_t dataShort = htons(*(uint16_t*)fieldAddress);
        memcpy(out, &dataShort, sizeof(uint16_t));
        len += sizeof(uint16_t); // value 长度
        break;
    case FIELD_4BYTE:
        uint32_t dataInt = htonl(*(uint32_t*)fieldAddress);
        memcpy(out, &dataInt, sizeof(uint32_t));
        len += sizeof(uint32_t);
        break;
    case FIELD_8BYTE:
        // 大端转小端
        for (uint8_t i = 0; i < 8; ++i) {
            out[i] = fieldAddress[8 - i - 1];
        }
        len += sizeof(uint64_t);
        break;
    case FIELD_STRING:
    case FIELD_BINARY:
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

char* TlvEncodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, uint16_t* len)
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

    printf("sizeof(TlvHeader) = %zu\n", sizeof(TlvHeader));
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
    case FIELD_STRUCT:
        DecodeStruct(info->fieldInfo, info->fieldInfoLen, fieldAddress, buffer, valueLen);
        break;
    case FIELD_1BYTE:
    case FIELD_2BYTE:
    case FIELD_4BYTE:
    case FIELD_8BYTE:
        for (int i = 0; i < valueLen; ++i) {
            fieldAddress[i] = buffer[valueLen - i - 1];
        }
        // memcpy(fieldAddress, buffer, valueLen);
        break;
    case FIELD_STRING:
    case FIELD_BYTE_PTR:
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
        valueLen = ntohs(*(uint16_t*)(tmp + FIELD_LEN));
        // objAddress + pInfo->offset 依据字段在结构体中的偏移来设置字段值
        DecodeField(pInfo, objAddress + pInfo->offset, tmp, TAG_LEN + VALUE_LEN_LEN + valueLen);
    }

    return TLV_ERROR_OK;
}

int TlvDecodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len)
{
    return DecodeStruct(info, infoLen, objAddress, buffer, len);
}