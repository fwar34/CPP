#pragma once
#include <cstdint>

struct MessageHeader
{
    uint32_t msgId_;
    uint32_t bodyLength_;
    uint32_t confId_;
    char body[0];
};