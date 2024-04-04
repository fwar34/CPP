#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <sstream>

namespace Nt
{

#pragma pack(4)
struct MessageHeader
{
    MessageHeader() = default;
    uint32_t msgId_ = 0;
    uint32_t bodyLength_ = 0;
    uint32_t confId_ = 0;
};
#pragma pack()

constexpr size_t MSG_HEADER_LENGTH = sizeof(MessageHeader);

struct Message
{
    // body_ 的空间在接收到 header 后根据 bodyLength_ 来创建
    Message() : body_(0) {}
    MessageHeader header_;
    std::vector<char> body_;
    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;
    Message(Message&& other) noexcept = default;
    Message& operator=(Message&& other) noexcept = default;
    std::string Dump()
    {
        std::stringstream ss;
        ss << "msgId: " << header_.msgId_ << " bodyLength: " << header_.bodyLength_ << " confId: " << header_.confId_;
        return ss.str();
    }
};
    
} // namespace Nt
