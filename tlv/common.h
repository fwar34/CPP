#ifndef __COMMON_H_
#define __COMMON_H_

#define LOG_ERR(...) printf("function(%s) error file(%s) line(%d)\n", __VA_ARGS__, __FILE__, __LINE__);

typedef enum
{
    TLV_ERROR_OK,
    TLV_ERROR_TAG,
} TlvError;

#endif