#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <iterator>
#include <iostream>

namespace Nt
{

#pragma pack(4)
struct MessageHeader
{
    // MessageHeader() = default;
    // MessageHeader(MessageHeader&&) = default;
    // MessageHeader& operator=(MessageHeader&&) = default;
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
    // Message(Message&& other) noexcept = default;
    Message(Message&& other) noexcept
    {
        std::cout << "Message move constructor called!" << std::endl;
        header_ = other.header_;
        // body_.swap(other.body_);
        body_ = std::move(other.body_);
    }
    // Message& operator=(Message&& other) noexcept = default;
    Message& operator=(Message&& other) noexcept
    {
        header_ = other.header_;
        // body_.swap(other.body_);
        body_ = std::move(other.body_);
        return *this;
    }
    std::string Dump()
    {
        std::stringstream ss;
        ss << "msgId: " << header_.msgId_ << " bodyLength: " 
            << header_.bodyLength_ << " confId: " << header_.confId_;
        return ss.str();
    }
};

std::ostream& operator<<(std::ostream& os, std::shared_ptr<Message>& message);
std::ostream& operator<<(std::ostream& os, Message& message);
std::string PrintMessage(const std::shared_ptr<Message>& message);
void PrintByteArray(const std::vector<char>& body);
    
} // namespace Nt
