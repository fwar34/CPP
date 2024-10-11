#ifndef __COMMON_H_
#define __COMMON_H_

#include <errno.h>

#define LOG_ERR(...) printf("function(%s) error file(%s) line(%d) error(%s)\n", __VA_ARGS__, __FILE__, __LINE__, strerror(errno));
#define LOG_WARNING(...) printf("function(%s) warning file(%s) line(%d)\n", __VA_ARGS__, __FILE__, __LINE__);

typedef enum
{
    TLV_ERROR_OK,
    TLV_ERROR_TAG,
    TLV_ERROR_TYPE,
} TlvError;

#endif