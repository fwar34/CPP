#ifndef _TLV_H
#define _TLV_H

#include <inttypes.h>

#define MSG_VERSION 31111
#define MSG_CMD 1

#define OFFSET(type, field) (&(((type*)0)->field))
#define ARRAY_LEN(array, element) (sizeof(array) / sizeof(element))

#define TlvEncode(type, objAddress, lenAddress) TlvEncodeImpl(type##Info, ARRAY_LEN(type##Info, type##Info[0]), objAddress, lenAddress)
#define TlvDecode()

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
    uint32_t totolLen;
    uint16_t version;
    uint16_t commandId;
    uint32_t sequenceNo;
    char checkNumber[16];
} TlvHeader;

typedef struct _FieldInfo
{
    uint8_t tag;
    uint16_t len;
    uint16_t offset;
    struct _FieldInfo* fieldInfo;
    // struct _FieldInfo (*fieldInfo)[];
    uint16_t fieldInfoLen;
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
 * @param objAddress 此结构体示例地址
 * @param out 序列化的目标 buffer
 * @return uint16_t 返回此结构体实例 tag+len+value 序列化此实例的字节大小
 */
uint16_t EncodeStruct(FieldInfo* info, uint16_t infoLen, char* objAddress, char* out);
char* TlvEncodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, int* len);

#endif