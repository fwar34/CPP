#ifndef _TLV_H
#define _TLV_H

#include "buffer.h"
#include <inttypes.h>

#define MSG_VERSION 31111
#define MSG_CMD 1

#define TAG_LEN 1
#define VALUE_LEN_LEN 2

#define OFFSET(type, field) (&(((type*)0)->field))
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

#define TlvImport(type) extern FieldInfo type##Info[]
#define TlvFieldBegin(type) FieldInfo type##Info[] = {
#define TlvField(type, field, tagType) \
    {                                  \
        .tag = tagType,                \
        .offset = OFFSET(type, field), \
    },
#define TlvFieldStruct(type, fieldType, field, tagType)                       \
    {                                                                         \
        .tag = tagType,                                                       \
        .offset = OFFSET(type, field),                                        \
        .fieldInfo = fieldType##Info,                                         \
        .fieldInfoLen = sizeof(fieldType##Info) / sizeof(fieldType##Info[0]), \
    },
#define TlvFieldEnd(type) };

#define TlvEncode(type, objAddress, lenAddress) \
    TlvEncodeImpl(type##Info, ARRAY_LEN(type##Info), objAddress, lenAddress)
#define TlvDecode(type, objAddress, buffer, len) \
    TlvDecodeImpl(type##Info, ARRAY_LEN(type##Info), objAddress, buffer, len)

typedef enum
{
    TAG_STRUCT = 1,
    TAG_SHORT,
    TAG_INT,
    TAG_STRING,
    TAG_MAX,
} TagType;

typedef struct 
{
    uint32_t totalLen;
    uint16_t version;
    uint16_t commandId;
    uint32_t sequenceNo;
    char checkNumber[16];
} TlvHeader;

typedef struct _FieldInfo
{
    uint8_t tag;
    uint16_t len;
    uint16_t offset; // 字段在结构体中的偏移字节
    struct _FieldInfo* fieldInfo; // 如果字段是个 struct，则有此字段
    // struct _FieldInfo (*fieldInfo)[];
    uint16_t fieldInfoLen; // fieldInfo 数组元素个数
} FieldInfo;

/**
 * @brief Get the Sruct Len object
 * 
 * @param info 此结构体对应的 FieldInfo 数组
 * @param infoLen 此结构体对应的 FieldInfo 数组元素个数
 * @param objAddress 结构体实例地址
 * @return uint16_t 返回结构体 tag+len+value 序列化的字节大小
 */
uint16_t GetSructLen(FieldInfo* info, uint16_t infoLen, char* objAddress);
/**
 * @brief Get the Field Len object
 * 
 * @param info 此字段对应的 FieldInfo 结构体指针
 * @param fieldAddress 此字段起始地址
 * @return uint16_t 返回此字段 tag+len+value 序列化的字节大小
 */
uint16_t GetFieldLen(FieldInfo* info, char* fieldAddress);
/**
 * @brief 序列化一个字段
 * 
 * @param info 此字段对应的 FieldInfo 结构体指针
 * @param fieldAddress 此字段的起始地址
 * @param out 序列化的目的 buffer
 * @return uint16_t 返回此字段 tag+len+value 序列化的字节大小
 */
uint16_t EncodeField(FieldInfo* info, char* fieldAddress, char* out);
/**
 * @brief 序列化结构体到 buffer
 * 
 * @param info 此结构体对应的 FieldInfo 数组
 * @param infoLen 此结构体对应的 FieldInfo 数组元素个数
 * @param objAddress 此结构体实例地址
 * @param out 序列化的目标 buffer
 * @return uint16_t 返回此结构体实例 tag+len+value 序列化此实例的字节大小
 */
uint16_t EncodeStruct(FieldInfo* info, uint16_t infoLen, char* objAddress, char* out);
/**
 * @brief 序列化结构体
 * 
 * @param info 此结构体对应的 FieldInfo 数组
 * @param infoLen 此结构体对应的 FieldInfo 数组元素个数
 * @param objAddress 此结构体实例地址
 * @param len 返回序列化后的 buffer 长度
 * @return char* 返回序列化后的 buffer
 */
char* TlvEncodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, int* len);
/**
 * @brief 反序列化结构体字段
 * 
 * @param info 此结构体字段对应的 FieldInfo 结构体指针
 * @param fieldAddress 此字段在结构体实例中的地址
 * @param buffer 输入 buffer
 * @param len 输入 buffer 的长度
 * @return int 返回错误码 TLV_ERROR_OK 代表成功
 */
int DecodeField(FieldInfo* info, char* fieldAddress, char* buffer, uint32_t len);
/**
 * @brief 反序列化结构体，依据字段在结构体中的偏移来设置字段值
 * 
 * @param info 结构体对应的 FieldInfo 数组
 * @param infoLen 结构体对应的 FieldInfo 数组元素个数
 * @param objAddress 此结构体实例地址
 * @param buffer 输入的 buffer
 * @param len 输入 buffer 的长度
 * @return int 返回错误码 TLV_ERROR_OK 代表成功
 */
int DecodeStruct(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len);
/**
 * @brief 反序列化 buffer 到结构体
 * 
 * @param info 此结构体对应的 FieldInfo 数组
 * @param infoLen 此结构体对应的 FieldInfo 数组元素个数
 * @param objAddress 要反序列化的目标结构体实例地址
 * @param buffer 输入的 buffer
 * @param len 输入的 buffer 的长度
 * @return int 返回错误码 TLV_ERROR_OK 代表成功
 */
int TlvDecodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len);

#endif