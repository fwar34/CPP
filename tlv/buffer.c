#include "buffer.h"

uint16_t BufferGetWriteIndex(Buffer* buffer)
{
    return buffer->writeIndex;
}

uint16_t BufferGetReadIndex(Buffer* buffer)
{
    return buffer->readIndex;
}

void BufferSetWriteIndex(Buffer* buffer, uint16_t index)
{
    buffer->writeIndex = index;
}

void BufferSetReadIndex(Buffer* buffer, uint16_t index)
{
    buffer->readIndex = index;
}

uint16_t BufferReadableCount(Buffer* buffer)
{
    if (buffer->writeIndex > buffer->readIndex) {
        return buffer->writeIndex - buffer->readIndex;
    }
    return 0;
}

uint16_t BufferWriteableCount(Buffer* buffer)
{
    return BUF_LEN - buffer->writeIndex;
}

void BufferReset(Buffer* buffer)
{
    buffer->readIndex = 0;
    buffer->writeIndex = 0;
}

uint16_t BufferRetrieve(Buffer* buffer, uint16_t len)
{
    buffer->readIndex += len;
}

char* BufferReadBuf(Buffer* buffer)
{
    return buffer->data + buffer->readIndex;
}