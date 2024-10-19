#include "tlv.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdbool.h>

static bool IsNeedEncode(uint8_t tag)
{
    switch (tag)
    {
    case TAG_PRIVATE_ARRAY_LEN:
    case TAG_PRIVATE_FIELD_MASK:
    case TAG_PRIVATE_SIZEOF:
        return false;
    default:
        return true;
    }
}

uint16_t GetStructTlvLen(FieldInfo* info, uint16_t infoLen, char* objAddress)
{
    uint16_t len = 0;
    for (int i = 0; i < infoLen; ++i) {
        if (IsNeedEncode(info[i].tag)) {
            len += GetFieldTlvLen(&info[i], objAddress + info[i].offset);
        }
    }

    return len;
}

uint16_t GetFieldTlvLen(FieldInfo* info, char* fieldAddress)
{
    FieldInfo* prev = NULL;
    char* objAddress = NULL;
    uint16_t len = TAG_LEN + VALUE_LEN_LEN;
    uint32_t arrayLen = 0;
    // 此函数会给每个字段的 FieldInfo 中的 len 设置上实例 filedAddress 对应字段的大小，
    // 在 Encode 函数中会直接使用此处赋值的 len
    switch (info->type)
    {
    case FIELD_TYPE_STRUCT: // 字段为一个结构体
        len += GetStructTlvLen(info->fieldInfo, info->fieldInfoLen, fieldAddress);
        break;
    case FIELD_TYPE_STRUCT_ARRAY:
    {
        prev = info - 1; // 宏定义保证-1位置存在 FieldInfo
        arrayLen = info->len / prev->len;
        for (int i = 0; i < arrayLen; ++i) { // prev->len 是数组单个元素字节大小
            len += GetStructTlvLen(info->fieldInfo, info->fieldInfoLen,
                fieldAddress + prev->len * i);
        }
        break;
    }
    case FIELD_TYPE_STRUCT_PTR:
    {
        objAddress = fieldAddress - info->offset; // 根据字段的偏移计算出来实例的起始地址
        prev = info - 1;
        char bufLen2[prev->len];
        memset(bufLen2, 0, prev->len);
        memcpy(bufLen2, objAddress + prev->offset, prev->len);
        ConvertBuf2Len(arrayLen, bufLen2, prev->len);
        for (int i = 0; i < arrayLen; ++i) {
            len += GetStructTlvLen(info->fieldInfo, info->fieldInfoLen, 
                *(char**)fieldAddress + info->len * i);
        }
        break;
    }
    case FIELD_TYPE_1BYTE:
    case FIELD_TYPE_2BYTE:
    case FIELD_TYPE_4BYTE:
    case FIELD_TYPE_8BYTE:
    case FIELD_TYPE_BYTE_ARRAY:
        len += info->len;
        break;
    case FIELD_TYPE_STRING:
        char* str = *(char**)fieldAddress;
        if (!str) {
            return len;
        }
        len += strlen(*(char**)fieldAddress) + 1;
        break;
    case FIELD_TYPE_BYTE_PTR:
    {
        objAddress = fieldAddress - info->offset;
        prev = info - 1;
        char lenBuf[prev->len];
        memset(lenBuf, 0, prev->len);
        memcpy(lenBuf, objAddress + prev->offset, prev->len);
        ConvertBuf2Len(arrayLen, lenBuf, prev->len);
        len += arrayLen;
        break;
    }
    case FIELD_TYPE_LINKED_PTR:
        // TODO
        break;
    default:
        printf("GetFieldTlvLen file(%s) line(%d) tag(%d)", __FILE__, __LINE__, info->tag);
        return 0;
    }

    return len;
}

static bool IsHostLittleEndian()
{
    uint32_t data = 0x12345678;
    char* p = (char*)&data;
    if (p[0] == 0x78) {
        return true;
    }
    return false;
}

/**
 * @brief 本地是小端的时候将 src 的字节序交换后拷贝到 dst，否则直接拷贝到 dst
 */
static void BufferChangeEndianCopy(char* dst, const char* src, uint16_t len)
{
    if (IsHostLittleEndian()) {
        for (int i = 0; i < len; ++i) {
            dst[i] = src[len - i - 1];
        }
    } else {
        memcpy(dst, src, len);
    }
}

uint16_t EncodeFieldTlv(FieldInfo* info, char* fieldAddress, char* out)
{
    FieldInfo* prev = NULL;
    char* objAddress = NULL;
    char* origBuf = out;
    uint32_t arrayLen = 0;
    uint16_t encodeLen = 0;
    uint16_t len = 0; // tag+len 长度
    out += TAG_LEN + VALUE_LEN_LEN;

    switch (info->type) // value
    {
    case FIELD_TYPE_STRUCT: // 当前字段为一个结构体
        len += EncodeStructTlv(info->fieldInfo, info->fieldInfoLen, fieldAddress, out);
        break;
    case FIELD_TYPE_STRUCT_ARRAY:
    {
        prev = info - 1;
        arrayLen = info->len / prev->len;
        for (int i = 0; i < arrayLen; ++i) {
            encodeLen = EncodeStructTlv(info->fieldInfo, info->fieldInfoLen,
                fieldAddress + prev->len * i, out);
            out += encodeLen;
            len += encodeLen;
        }
        break;
    }
    case FIELD_TYPE_STRUCT_PTR:
    {
        prev = info - 1;
        objAddress = fieldAddress - info->offset;
        char lenBuf[prev->len];
        memset(lenBuf, 0, prev->len);
        memcpy(lenBuf, objAddress + prev->offset, prev->len);
        ConvertBuf2Len(arrayLen, lenBuf, prev->len);
        for (int i = 0; i < arrayLen; ++i) {
            encodeLen = EncodeStructTlv(info->fieldInfo, info->fieldInfoLen, 
                *(char**)fieldAddress + info->len * i, out);
            out += encodeLen;
            len += encodeLen;
        }
        break;
    }
    case FIELD_TYPE_1BYTE:
    case FIELD_TYPE_2BYTE:
    case FIELD_TYPE_4BYTE:
    case FIELD_TYPE_8BYTE:
        BufferChangeEndianCopy(out, fieldAddress, info->len);
        len += info->len;
        break;
    case FIELD_TYPE_STRING:
    {
        char* str = *(char**)fieldAddress;
        if (!str) {
            break;
        }
        uint16_t strLen = strlen(str) + 1;
        memcpy(out, *(char **)fieldAddress, strLen);
        len += strLen;
        break;
    }
    case FIELD_TYPE_BYTE_PTR:
    {
        prev = info - 1;
        objAddress = fieldAddress - info->offset;
        char lenBuf[prev->len];
        memset(lenBuf, 0, prev->len);
        memcpy(lenBuf, objAddress + prev->offset, prev->len);
        ConvertBuf2Len(arrayLen, lenBuf, prev->len);
        memcpy(out, *(char**)fieldAddress, arrayLen);
        len += arrayLen;
        break;
    }
    case FIELD_TYPE_BYTE_ARRAY:
        memcpy(out, fieldAddress, info->len);
        len += info->len;
        break;
    default:
        printf("EncodeFieldTlv failed! invalidate tag:%d\n", info->tag);
        return 0;
    }

    memcpy(origBuf, &info->tag, TAG_LEN); // tag
    BufferChangeEndianCopy(origBuf + TAG_LEN, (char*)&len, VALUE_LEN_LEN);

    return len + TAG_LEN + VALUE_LEN_LEN;
}

uint16_t EncodeStructTlv(FieldInfo* info, uint16_t infoLen, char* objAddress, char* out)
{
    uint16_t len = 0;
    for (int i = 0; i < infoLen; ++i) {
        if (IsNeedEncode(info[i].tag)) {
            len += EncodeFieldTlv(&info[i], objAddress + info[i].offset, out + len);
        }
    }
    return len;
}

char* TlvEncodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, uint16_t* len)
{
    static uint32_t sequenceNo = 0;
    uint16_t bufLen = sizeof(TlvHeader) + GetStructTlvLen(info, infoLen, objAddress);
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
    uint16_t ret = EncodeStructTlv(info, infoLen, objAddress, bufTmp);
    if (ret != bufLen - sizeof(TlvHeader)) {
        LOG_ERR("TlvEncodeImpl");
        free(buffer);
        return NULL;
    }
    *len = bufLen;
    return buffer;
}

// len: tag + valueLen + value 整个长度
int DecodeFieldTlv(FieldInfo* info, char* fieldAddress, char* buffer, uint32_t len)
{
    FieldInfo* prev = NULL;
    char* objAddress = NULL;
    uint32_t arrayLen = 0;
    uint8_t tag = *buffer;
    buffer += TAG_LEN;
    // len -= TAG_LEN;
    uint16_t valueLen = ntohs(*(uint16_t*)buffer);
    buffer += VALUE_LEN_LEN;
    // len -= VALUE_LEN_LEN;

    switch (info->type)
    {
    case FIELD_TYPE_STRUCT:
        DecodeStructTlv(info->fieldInfo, info->fieldInfoLen, fieldAddress, buffer, valueLen);
        break;
    case FIELD_TYPE_STRUCT_ARRAY:
        prev = info - 1;
        arrayLen = info->len / prev->len;
        for (int i = 0; i < arrayLen; ++i) {
            // tag = *(buffer + next->len * i);
            valueLen = ntohs(*(uint16_t*)(buffer + TAG_LEN));
            // BufferChangeEndianCopy(&valueLen, buffer + TAG_LEN, VALUE_LEN_LEN);
            uint16_t tlvLen = TAG_LEN + VALUE_LEN_LEN + valueLen;
            DecodeStructTlv(info->fieldInfo, info->fieldInfoLen,
                fieldAddress + prev->len * i, buffer, tlvLen);
            buffer += tlvLen;
        }
        break;
    case FIELD_TYPE_STRUCT_PTR:
        prev = info - 1;
        objAddress = fieldAddress - info->offset;
        /* 宏定义保证了 ptr 的元素数量会先反序列化到实例字段，
        所以这里反序列化 ptr 内容的时候已经可以获取到 ptr 的元素数量字段*/
        ConvertBuf2Len(arrayLen, objAddress + prev->offset, prev->len);
        char* tmp = (char*)malloc(info->len * arrayLen);
        for (int i = 0; i < arrayLen; ++i) {
            valueLen = ntohs(*(uint16_t*)(buffer + TAG_LEN));
            uint16_t tlvLen = TAG_LEN + VALUE_LEN_LEN + valueLen;
            DecodeStructTlv(info->fieldInfo, info->fieldInfoLen,
                fieldAddress + prev->type * i, buffer, tlvLen);
            buffer += tlvLen;
        }
        break;
    case FIELD_TYPE_1BYTE:
    case FIELD_TYPE_2BYTE:
    case FIELD_TYPE_4BYTE:
    case FIELD_TYPE_8BYTE:
        // int fieldSize = MIN(valueLen, info->len);
        int fieldSize = valueLen;
        if (valueLen > info->len) { // 发送端字段字节长度大于接收端定义的字段字节长度
            LOG_WARNING("DecodeFieldTlv warning valueLen > info->len");
            fieldSize = info->len;
        }
        // 这里使用 valueLen 的循环来进行大小端转换到目标字段，
        // 可以防止接收的 valueLen 比本地的字段字段长度小的情况下反序列化错误
        // 比如 valueLen 是 2 字节（发送端定义的是 uint16），本地字段是 4 字节（接收端定义的是 uint32）
        BufferChangeEndianCopy(fieldAddress, buffer, fieldSize);
        break;
    case FIELD_TYPE_STRING:
    case FIELD_TYPE_BYTE_PTR:
        char *str = (char *)malloc(valueLen);
        memcpy(str, buffer, valueLen);
        // *(char **)fieldAddress = str;
        memcpy(fieldAddress, &str, sizeof(char *));
        break;
    case FIELD_TYPE_BYTE_ARRAY:
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

int DecodeStructTlv(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len)
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
        DecodeFieldTlv(pInfo, objAddress + pInfo->offset, tmp, TAG_LEN + VALUE_LEN_LEN + valueLen);
    }

    return TLV_ERROR_OK;
}

int TlvDecodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len)
{
    return DecodeStructTlv(info, infoLen, objAddress, buffer, len);
}