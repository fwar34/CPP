#pragma once
#include <cstdint>

namespace Nt
{

#pragma pack(1)
struct MessageHeader
{
    uint32_t msgId_;
    uint32_t bodyLength_;
    uint32_t confId_;
    char body[0];
};
#pragma pack()

constexpr size_t MSG_HEADER_LENGTH = sizeof(MessageHeader);
    
} // namespace Nt
