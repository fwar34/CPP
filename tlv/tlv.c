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

int DecodeStruct(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len)
{
    uint8_t infoIndex = 0;
    uint16_t bufferIndex = 0;
    uint8_t tag = *(uint8_t*)(buffer + bufferIndex); // tag:1byte
    bufferIndex += TAG_LEN;
    while (bufferIndex < len) {
        switch (tag)
        {
        case TAG_STRUCT:
            // DecodeStruct
            break;
        case TAG_SHORT:
            break;
        case TAG_INT:
            break;
        case TAG_STRING:
            FieldInfo* pInfo = &info[infoIndex++];
            if (pInfo->tag != tag) {
                printf("DecodeStruct failed! file(%s) line(%d)\n", __FILE__, __LINE__);
                return TLV_ERROR_TAG;
            }
            uint16_t stringLen = ntohs(*(uint16_t*)(buffer + bufferIndex));
            bufferIndex += LEN_LEN; // len:2bytes
            char* str = (char*)malloc(stringLen);
            memcpy(str, buffer + bufferIndex, stringLen);
            bufferIndex += stringLen;
            *(char**)(objAddress + pInfo->offset) = str; // 依据字段在结构体中的偏移来设置字段值
            break;
        default:
            break;
        }
    }
}

int TlvDecodeImpl(Buffer* buffer)
{
    for (;;) {
        if (BufferReadableCount(buffer) < sizeof(TlvHeader)) { // TlvHeader 没有接收完整，继续接收
            if (BufferGetReadIndex(buffer) != 0) { // 如果 buffer 中的内容不是从0开始的则移动 buffer 中的内容到 buffer 起始
                MoveBuffer(buffer);
            }
            return -1;
        }

        TlvHeader *tmp = (TlvHeader *)(buffer->data);
        uint16_t totalLen = ntohl(tmp->totalLen);
        if (BufferReadableCount(buffer) < totalLen) { // 整个消息没有接收完整，继续接收
            if (BufferGetReadIndex(&buffer) != 0) { // 如果 buffer 中的内容不是从0开始的则移动 buffer 中的内容到 buffer 起始
                MoveBuffer(buffer);
            }
            return -2;
        }

        TlvHeader tlvhdr;
        bzero(&tlvhdr, sizeof(tlvhdr));
        tlvhdr.totalLen = ntohl(tmp->totalLen);
        tlvhdr.version = ntohs(tmp->version);
        tlvhdr.commandId = ntohs(tmp->commandId);
        tlvhdr.sequenceNo = ntohl(tmp->sequenceNo);

        switch (tlvhdr.commandId)
        {
        case MSG_CMD:
            Student student;
            DecodeStruct(Student, &student, BufferReadBuf(buffer) + sizeof(TlvHeader), 
                tlvhdr.totalLen - sizeof(TlvHeader));
            break;
        default:
            break;
        }
        BufferRetrieve(buffer, tlvhdr.totalLen);
    }
}