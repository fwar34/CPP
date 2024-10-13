#ifndef _TLV2_H_
#define _TLV2_H_

/**
 * @file tlv.h
 * @author your name (you@domain.com)
 * @brief 和 tlv 对比最主要的区别就是 FieldInfo 添加了 type 类型，反序列化的时候根据 tag 遍历查找 
 * FieldInfo，tlv 和 tlv2 都只支持同类型的结构体单线程序列化，因为 FieldInfo 中的 len 是在 
 * GetFieldTlvLen 中设置的，在 Encode 中使用，如果同个结构体的多个实例同时序列化则 GetFieldLen 的值会覆盖，
 * 在多线程 Encode 的时候就会出问题
 * @version 0.1
 * @date 2024-10-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <inttypes.h>

#define MSG_VERSION 31111
#define MSG_CMD 1

#define TAG_LEN 1
#define VALUE_LEN_LEN 2

#define ARRAY_LEN(arrayName) (sizeof(arrayName) / sizeof(arrayName[0]))
#define OFFSET(structType, field) (uintptr_t)(&(((structType*)0)->field))
#define STRUCT_FIELD_SIZE(structType, field) sizeof(((structType*)0)->field)

#define TlvImport(structType) extern FieldInfo structType##Info[]
#define TlvFieldBegin(structType) FieldInfo structType##Info[] = {
/**
 * @brief 普通字段使用
 *
 */
#define TlvField(tagType, structType, fieldName, fieldType) \
    {                                                       \
        .tag = tagType,                                     \
        .type = fieldType,                                  \
        .offset = OFFSET(structType, fieldName),            \
    },
/**
 * @brief 字段为结构体对象时使用
 * type 结构体类型
 * fieldName 字段名称
 * fieldType 子结构体类型
 */
#define TlvFieldStruct(tagType, structType, fieldName, fieldType) \
    {                                                             \
        .tag = tagType,                                           \
        .type = FIELD_TYPE_STRUCT,                                \
        .offset = OFFSET(structType, fieldName),                  \
        .fieldInfo = fieldType##Info,                             \
        .fieldInfoLen = ARRAY_LEN(fieldType##Info),               \
    },
/**
 * @brief 字段为结构体数组时使用
 * tagType 字段的 tag
 * structType 结构体类型
 * fieldName 字段名称
 * fieldType 子结构体类型
 */
#define TlvFieldStructArray(tagType, structType, fieldName, fieldType) \
    {                                                                  \
        .tag = TAG_PRIVATE_ARRAY_LEN,                                  \
        /* 数组元素个数 */                                              \
        .len = ARRAY_LEN(fieldName),                                   \
    },                                                                 \
    {                                                                  \
        .tag = tagType,                                                \
        .type = FIELD_TYPE_STRUCT_ARRAY,                               \
        .offset = OFFSET(structType, fieldName),                       \
        .fieldInfo = fieldType##Info,                                  \
        .fieldInfoLen = ARRAY_LEN(fieldType##Info),                    \
    },                                                                 \
    {                                                                  \
        .tag = TAG_PRIVATE_SIZEOF,                                     \
        /* 数组单个元素字节大小 */                                       \
        .len = sizeof(fieldType),                                      \
    },
/**
 * @brief 字段为结构体指针时使用
 * tagType 字段的 tag
 * structType 结构体类型
 * fieldName 字段名称
 * fieldType 字段的子结构体类型
 * fieldLenName 指针指向的子结构体个数
 * fieldLenTagType fieldLenName 的 tag 
 * fieldLenType fieldLenName的 FieldType（FIELD_TYPE_2BYTE 等）
 */
#define TlvFieldStructPtr(tagType, structType, fieldName, fieldType,            \
                          fieldLenName, fieldLenTagType, fieldLenType)          \
    {                                                                           \
        .tag = fieldLenTagType,                                                 \
        .type = fieldLenType,                                                   \
        .offset = OFFSET(structName, fieldLenName),                             \
    },                                                                          \
    {                                                                           \
        .tag = tagType,                                                         \
        .type = FIELD_TYPE_STRUCT_PTR,                                          \
        .offset = OFFSET(structType, fieldName),                                \
        .fieldInfo = fieldType##Info,                                           \
        .fieldInfoLen = ARRAY_LEN(fieldType##Info),                             \
    },                                                                          \
    {                                                                           \
        .tag = TAG_PRIVATE_SIZEOF,                                              \
        /* len: fieldName 指向的单个元素字节大小 */                               \
        .len = sizeof(fieldType),                                               \
    },
/**
 * @brief 二进制数组字段使用
 * field 二进制字段名称
 * fieldLen 二进制字段对应长度字段的名称
 */
#define TlvFieldByteArray(tagType, structType, fieldName) \
    {                                                     \
        .tag = TAG_PRIVATE_ARRAY_LEN,                     \
        /* 数组元素个数 */                                 \
        .len = ARRAY_LEN(fieldName),                      \
    },                                                    \
    {                                                     \
        .tag = tagType,                                   \
        .type = FIELD_TYPE_BYTE_ARRAY,                    \
        .offset = OFFSET(structType, fieldName),          \
    },
/**
 * @brief 二进制指针字段使用
 * tagType char指针字段的 tag
 * structType 结构体类型
 * fieldName 二进制字段名称
 * fieldLenName 二进制字段对应长度字段的名称
 * fieldLenTagType char指针指向的 buffer 长度字段的 tag
 */
#define TlvFieldBytePtr(tagType, structType, fieldName, fieldLenName, fieldLenTagType, fieldLenType) \
    {                                                                                                \
        .tag = fieldLenTagType,                                                                      \
        .type = fieldLenType,                                                                        \
        .offset = OFFSET(structType, fieldLenName),                                                  \
    },                                                                                               \
    {                                                                                                \
        .tag = tagType,                                                                              \
        .type = FIELD_TYPE_BYTE_PTR,                                                                 \
        .offset = OFFSET(structType, fieldName),                                                     \
    },

#define TlvFieldEnd(type) };

#define TlvEncode(structType, objAddress, lenAddress) \
    TlvEncodeImpl(structType##Info, ARRAY_LEN(structType##Info), (char *)(objAddress), lenAddress)
#define TlvDecode(structType, objAddress, buffer, len) \
    TlvDecodeImpl(structType##Info, ARRAY_LEN(structType##Info), (char *)(objAddress), buffer, len)

    /**
     * @brief tlv 支持的数据类型
     *
     */
    typedef enum {
        FIELD_TYPE_STRUCT = 1,   // 结构体对象
        FIELD_TYPE_STRUCT_ARRAY, // 结构体数组
        FIELD_TYPE_STRUCT_PTR,   // 结构体指针
        FIELD_TYPE_1BYTE,
        FIELD_TYPE_2BYTE,
        FIELD_TYPE_4BYTE,
        FIELD_TYPE_8BYTE,
        FIELD_TYPE_STRING,     // c风格字符
        FIELD_TYPE_BYTE_ARRAY, // char数组
        FIELD_TYPE_BYTE_PTR,   // 二进制指针
        FIELD_TYPE_LINKED_PTR, // 链表指针
        FIELD_TYPE_MAX,
    } FieldType : uint8_t;

/**
 * @brief tlv tag 内部私有类型，业务不会使用
 * 
 */
typedef enum {
    TAG_PRIVATE_SIZEOF = 200,
    TAG_PRIVATE_ARRAY_LEN = 201,
    TAG_PRIVATE_FIELD_MASK = 202,
} TagPrivateType : uint8_t;

typedef struct {
    uint32_t totalLen;
    uint16_t version;
    uint16_t commandId;
    uint32_t sequenceNo;
    char checkNumber[16];
} TlvHeader;

/**
 * @brief 结构体中字段描述信息
 *  tag 字段标识
 *  type 字段类型，char，int等，tag 为 TAG_PRIVATE_ARRAY_LEN 的时候是数组单个元素字节大小
 *  len 字段的字节长度，FIELD_TYPE_STRING 和 FIELD_BINARY 的是
 *  对应的 buffer 的长度，tag 为 TAG_PRIVATE_ARRAY_LEN 的时候是数组的元素个数
 *  offset 字段在结构中的偏移
 *  fieldInfo 字段是个结构体时候指向了此字段对应的子结构体类型的 FieldInfo 数据
 *  fieldInfoLen fieldInfo 数组元素个数
 */
typedef struct _FieldInfo {
    uint8_t tag;
    uint8_t type;
    uint16_t len;
    uint16_t offset;
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
uint16_t GetStructTlvLen(FieldInfo* info, uint16_t infoLen, char* objAddress);
/**
 * @brief Get the Field Len object
 * 
 * @param info 此字段对应的 FieldInfo 结构体指针
 * @param fieldAddress 此字段起始地址
 * @return uint16_t 返回此字段 tag+len+value 序列化的字节大小
 */
uint16_t GetFieldTlvLen(FieldInfo* info, char* fieldAddress);
/**
 * @brief 序列化一个字段
 * 
 * @param info 此字段对应的 FieldInfo 结构体指针
 * @param fieldAddress 此字段的起始地址
 * @param out 序列化的目的 buffer
 * @return uint16_t 返回此字段 tag+len+value 序列化的字节大小
 */
uint16_t EncodeFieldTlv(FieldInfo* info, char* fieldAddress, char* out);
/**
 * @brief 序列化结构体到 buffer
 * 
 * @param info 此结构体对应的 FieldInfo 数组
 * @param infoLen 此结构体对应的 FieldInfo 数组元素个数
 * @param objAddress 此结构体实例地址
 * @param out 序列化的目标 buffer
 * @return uint16_t 返回此结构体实例 tag+len+value 序列化此实例的字节大小
 */
uint16_t EncodeStructTlv(FieldInfo* info, uint16_t infoLen, char* objAddress, char* out);
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
int DecodeFieldTlv(FieldInfo* info, char* fieldAddress, char* buffer, uint32_t len);
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
int DecodeStructTlv(FieldInfo* info, uint16_t infoLen, char* objAddress, char* buffer, uint32_t len);
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