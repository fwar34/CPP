#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <inttypes.h>

const uint32_t BUF_LEN = 2048;

typedef struct
{
    char data[BUF_LEN];
    uint16_t readIndex;
    uint16_t writeIndex;
} Buffer;

uint16_t BufferGetReadIndex(Buffer* buffer);
uint16_t BufferGetWriteIndex(Buffer* buffer);
void BufferSetReadIndex(Buffer* buffer, uint16_t index);
void BufferSetWriteIndex(Buffer* buffer, uint16_t index);
void BufferReset(Buffer* buffer);
uint16_t BufferReadableCount(Buffer* buffer);
uint16_t BufferRetrieve(Buffer* buffer, uint16_t len);
char* BufferReadBuf(Buffer* buffer);

#endif