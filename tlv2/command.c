#include "command.h"

typedef enum
{
    TAG_QUHAO,
    TAG_PHONE,
} TagPhoneNum;
TlvFieldBegin(PhoneNum)
TlvField(TAG_QUHAO, PhoneNum, quhao, FIELD_TYPE_STRING)
TlvField(TAG_PHONE, PhoneNum, phone, FIELD_TYPE_STRING)
TlvFieldEnd(PhoneNum)

typedef enum
{
    TAG_STRESS,
    TAG_ADDRESS_NAME,
} TagAddress;
TlvFieldBegin(Address)
TlvField(TAG_STRESS, Address, stress, FIELD_TYPE_4BYTE)
TlvField(TAG_ADDRESS_NAME, Address, addressName, FIELD_TYPE_STRING)
TlvFieldEnd(Address)

typedef enum
{
    TAG_FIELD_TYPE_MASK,
    TAG_NAME,
    TAG_AGE,
    TAG_PHONENUM,
    TAG_ADDRESS,
    TAG_ADDRESSLEN,
    TAG_DATA,
    TAG_DATALEN,
} TagStudent;
TlvFieldBegin(Student)
TlvField(TAG_NAME, Student, name, FIELD_TYPE_STRING)
TlvField(TAG_AGE, Student, age, FIELD_TYPE_4BYTE)
TlvFieldStruct(TAG_PHONENUM, Student, phoneNum, PhoneNum)
TlvFieldStructPtr(TAG_ADDRESS, Student, address, Address, addressLen, TAG_ADDRESS_NAME, FIELD_TYPE_2BYTE)
TlvFieldBytePtr(TAG_DATA, Student, data, dataLen, TAG_DATALEN, FIELD_TYPE_4BYTE)
TlvFieldEnd(Student)

typedef enum {
    TAG_A,
    TAG_B,
} TestTag; 
TlvFieldBegin(Test)
TlvField(TAG_A, Test, a, FIELD_TYPE_4BYTE)
TlvField(TAG_B, Test, b, FIELD_TYPE_2BYTE)
TlvFieldEnd(Test)

typedef enum
{
    TAG_TEST2_ID,
    TAG_TEST2_ADDRESS,
    TAG_TEST2_ADDRESSLEN,
} Test2Tag;
TlvFieldBegin(Test2)
// TlvField(TAG_TEST2_ID, Test2, id, FIELD_TYPE_4BYTE)
TlvFieldStructPtr(TAG_TEST2_ADDRESS, Test2, address, Address, 
    addressLen, TAG_TEST2_ADDRESSLEN, FIELD_TYPE_4BYTE)
TlvFieldEnd(Test2)

uint32_t g_sequenceNo = 0;
void SerialTlvHeader(char* buffer, uint32_t totalLen, uint16_t version,
    uint16_t commandId, uint32_t sequenceNo)
{
    TlvHeader* header = (TlvHeader*)buffer;
    header->totalLen = htonl(totalLen);
    header->version = htons(version);
    header->commandId = htons(commandId);
    header->sequenceNo = htonl(sequenceNo);
}