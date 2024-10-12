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
        if (IsNeedEncode(info->tag)) {
            len += GetFieldLen(&info[i], objAddress + info[i].offset);
        }
    }

    return len;
}

uint16_t GetFieldLen(FieldInfo* info, char* fieldAddress)
{
    FieldInfo* prev = NULL;
    char* objAddress = NULL;
    // 此函数会给每个字段的 FieldInfo 中的 len 设置上实例 filedAddress 对应字段的大小，
    // 在 Encode 函数中会直接使用此处赋值的 len
    switch (info->type)
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
        objAddress = fieldAddress - info->offset; // 根据字段的偏移计算出来实例的起始地址
        prev = info - 1;
        uint16_t arrayLen = *(uint16_t*)(objAddress + prev->offset);
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
        objAddress = fieldAddress - info->offset;
        prev = info - 1;
        info->len = *(uint16_t*)(objAddress + prev->offset);
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
    FieldInfo* prev = NULL;
    char* objAddress = NULL;

    memcpy(out, &info->tag, sizeof(uint8_t)); // tag
    out += sizeof(uint8_t);
    len = htons(info->len);
    memcpy(out, &len, sizeof(uint16_t)); // len
    out += sizeof(uint16_t);

    len = sizeof(uint8_t) + sizeof(uint16_t); // tag+len 长度
    switch (info->type) // value
    {
    case FIELD_STRUCT: // 当前字段为一个结构体
        len += EncodeStruct(info->fieldInfo, info->fieldInfoLen, fieldAddress, out);
        break;
    case FIELD_STRUCT_ARRAY:
        prev = info - 1;
        for (int i = 0; i < prev->len; ++i) {
            len += EncodeStruct(info->fieldInfo, info->fieldInfoLen, fieldAddress + prev->type * i, out);
        }
        break;
    case FIELD_STRUCT_PTR:
        prev = info - 1;
        objAddress = fieldAddress - info->offset;
        uint16_t arryaLen = *(uint16_t*)(objAddress + prev->offset);
        for (int i = 0; i < arryaLen; ++i) {
            len += EncodeStruct(info->fieldInfo, info->fieldInfoLen, 
                *(char**)fieldAddress + prev->type * i);
        }
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
    case FIELD_BYTE_PTR:
        memcpy(out, *(char**)fieldAddress, info->len);
        len += info->len;
        break;
    case FIELD_BYTE_ARRAY:
        prev = info - 1;
        objAddress = fieldAddress - info->offset;
        uint16_t byteArrayLen = *(uint16_t*)(objAddress + prev->offset);
        memcpy(out, fieldAddress, byteArrayLen);
        len += byteArrayLen;
        break;
    default:
        printf("EncodeField failed! invalidate tag:%d\n", info->tag);
        return 0;
    }

    return len;
}

static bool IsNeedEncode(uint8_t tag)
{
    switch (tag)
    {
    case TAG_PRIVATE_ARRAY_LEN:
    case TAG_PRIVATE_FIELD_MASK:
        return false;
    default:
        return true;
    }
}

uint16_t EncodeStruct(FieldInfo* info, uint16_t infoLen, char* objAddress, char* out)
{
    uint16_t len = 0;
    for (int i = 0; i < infoLen; ++i) {
        if (IsNeedEncode(info->tag)) {
            len += EncodeField(&info[i], objAddress + info[i].offset, out + len);
        }
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
    FieldInfo* prev = NULL;
    char* objAddress = NULL;
    uint16_t arrayLen = 0;
    uint8_t tag = *buffer;
    buffer += TAG_LEN;
    uint16_t valueLen = ntohs(*(uint16_t*)buffer);
    buffer += VALUE_LEN_LEN;

    switch (info->type)
    {
    case FIELD_STRUCT:
        DecodeStruct(info->fieldInfo, info->fieldInfoLen, fieldAddress, buffer, valueLen);
        break;
    case FIELD_STRUCT_ARRAY:
        // TODO
        break;
    case FIELD_STRUCT_PTR:
        prev = info - 1;
        objAddress = fieldAddress - info->offset;
        arrayLen = *(uint16_t*)(objAddress + prev->type);
        char* tmp = (char*)malloc(prev->type * arrayLen);
        for (int i = 0; i < arrayLen; ++i) {
            DecodeStruct(info->fieldInfo, info->fieldInfoLen, 
                fieldAddress + prev->type * i, buffer + i * prev->type, );
        }
        // TODO
        break;
    case FIELD_1BYTE:
    case FIELD_2BYTE:
    case FIELD_4BYTE:
    case FIELD_8BYTE:
        // int fieldSize = MIN(valueLen, info->len);
        int fieldSize = valueLen;
        if (valueLen > info->len) { // 发送端字段字节长度大于接收端定义的字段字节长度
            LOG_WARNING("DecodeField warning valueLen > info->len");
            fieldSize = info->len;
        }
        // 这里使用 valueLen 的循环来进行大小端转换到目标字段，
        // 可以防止接收的 valueLen 比本地的字段字段长度小的情况下反序列化错误
        // 比如 valueLen 是 2 字节（发送端定义的是 uint16），本地字段是 4 字节（接收端定义的是 uint32）
        for (int i = 0; i < fieldSize; ++i) {
            fieldAddress[i] = buffer[valueLen - i - 1];
        }
        break;
    case FIELD_STRING:
    case FIELD_BYTE_PTR:
        char *str = (char *)malloc(valueLen);
        memcpy(str, buffer, valueLen);
        // *(char **)fieldAddress = str;
        memcpy(fieldAddress, &str, sizeof(char *));
        break;
    case FIELD_BYTE_ARRAY:
        memcpy(fieldAddress, buffer, valueLen);
        break;
    default:
        return TLV_ERROR_TYPE;
    }
    return TLV_ERROR_OK;
}
FieldInfo* FindFieldInfoByTag(FieldInfo* info, uint16_t infoLen, uint8_t tag)
{
    for (int i = 0; i < infoLen; ++i) {
        if (info[i].tag == tag) {
            return &info[i];
        }
    }

    return NULL;
}

int DecodeStruct(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len)
{
    FieldInfo* pInfo = NULL;
    uint8_t tag = 0;
    uint16_t bufferIndex = 0;
    uint16_t valueLen = 0;
    for (char* tmp = buffer; tmp < buffer + len; tmp += TAG_LEN + VALUE_LEN_LEN + valueLen) {
        tag = *tmp;
        valueLen = ntohs(*(uint16_t*)(tmp + TAG_LEN));

        if (!IsNeedEncode(tag)) {
            continue;
        }

        pInfo = FindFieldInfoByTag(info, infoLen, tag);
        if (!pInfo) {
            // 不需要反序列化的 tlv 字段直接跳过: tmp += TAG_LEN + VALUE_LEN_LEN + valueLen
            continue;
        }
        // objAddress + pInfo->offset 依据字段在结构体中的偏移来设置字段值
        DecodeField(pInfo, objAddress + pInfo->offset, tmp, TAG_LEN + VALUE_LEN_LEN + valueLen);
    }

    return TLV_ERROR_OK;
}

int TlvDecodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len)
{
    return DecodeStruct(info, infoLen, objAddress, buffer, len);
}