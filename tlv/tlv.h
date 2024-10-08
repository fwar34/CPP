#ifndef _TLV_H
#define _TLV_H

#include <inttypes.h>

#define MSG_VERSION 31111
#define MSG_CMD 1

#define TAG_LEN 1
#define VALUE_LEN_LEN 2

#define OFFSET(type, field) (uintptr_t)(&(((type*)0)->field))
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

#define TlvImport(type) extern FieldInfo type##Info[]
#define TlvFieldBegin(type) FieldInfo type##Info[] = {
/**
 * @brief 普通字段使用
 *
 */
#define TlvField(type, fieldName, tagType) \
    {                                      \
        .tag = tagType,                    \
        .offset = OFFSET(type, fieldName), \
    },
/**
 * @brief 结构体字段使用
 * field 结构体字段名称
 * fieldType 结构体字段对应的结构体
 */
#define TlvFieldStruct(type, fieldName, fieldType)                            \
    {                                                                         \
        .tag = TAG_STRUCT,                                                    \
        .offset = OFFSET(type, fieldName),                                    \
        .fieldInfo = fieldType##Info,                                         \
        .fieldInfoLen = sizeof(fieldType##Info) / sizeof(fieldType##Info[0]), \
    },
/**
 * @brief 二进制字段使用
 * field 二进制字段名称
 * fieldLen 二进制字段对应长度字段的名称
 */
#define TlvFieldBinary(type, fieldName, fieldLenName)  \
    {                                                  \
        .tag = TAG_BINARY,                             \
        .offset = OFFSET(type, fieldName),             \
        .binaryLenOffset = OFFSET(type, fieldLenName), \
    },
#define TlvFieldEnd(type) };

#define TlvEncode(type, objAddress, lenAddress) \
    TlvEncodeImpl(type##Info, ARRAY_LEN(type##Info), (char*)(objAddress), lenAddress)
#define TlvDecode(type, objAddress, buffer, len) \
    TlvDecodeImpl(type##Info, ARRAY_LEN(type##Info), (char*)(objAddress), buffer, len)

/**
 * @brief tlv 支持的数据类型
 * 
 */
typedef enum
{
    TAG_STRUCT = 1,
    TAG_1BYTE,
    TAG_2BYTE,
    TAG_4BYTE,
    TAG_8BYTE,
    TAG_STRING, // c风格字符串
    TAG_BINARY, // 二进制数据
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

/**
 * @brief 结构体中字段描述信息
 *  tag 字段标识
 *  len 字段的字节长度，TAG_STRING 和 TAG_BINARY 的是对应的 buffer 的长度
 *  offset 字段在结构中的偏移
 *  binaryLenOffset 如果字段是二进制类型时，二进制类型长度字段在结构体中的偏移
 *  fieldInfo 字段是个结构体时候指向了此字段对应的结构体类型的 FieldInfo 数据
 *  fieldInfoLen fieldInfo 数组元素个数
 */
typedef struct _FieldInfo
{
    uint8_t tag;
    uint16_t len;
    uint16_t offset;
    uint16_t binaryLenOffset;
    struct _FieldInfo* fieldInfo;
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
 * @param binaryLen 此字段是二进制的时候对应的二进制长度
 * @return uint16_t 返回此字段 tag+len+value 序列化的字节大小
 */
uint16_t GetFieldLen(FieldInfo* info, char* fieldAddress, uint16_t binaryLen);
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
char* TlvEncodeImpl(FieldInfo* info, uint16_t infoLen, char* objAddress, uint16_t* len);
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