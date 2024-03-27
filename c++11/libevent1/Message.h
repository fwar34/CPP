#pragma once

#include <sstream>
#include <string>

#pragma pack(1)
struct MessageHeader
{
    unsigned int type;
    unsigned short length;
};
#pragma pack()

constexpr uint32_t TYPE_LENGTH = 4;
constexpr uint32_t LENGTH_LENGTH = 2;
constexpr uint32_t HEADER_LENGTH = TYPE_LENGTH + LENGTH_LENGTH;

struct Message
{
    Message(const MessageHeader& header, char* buf) : header_(header), buf_(buf) {}
    ~Message() 
    { 
        if (buf_) {
            // buf_ 在 libevent 的 io 线程切包完成后 new，然后 构造消息发送到逻辑线程去处理，在 Message 的析构中去 delete
            delete[] buf_;
            buf_ = nullptr;
        }
    }
    std::string Dump()
    {
        std::stringstream ss;
        ss << "header: type = " << header_.type << " length = " << header_.length << "\nvalue = [" << buf_ << "]";
        return ss.str();
    }
    MessageHeader header_;
    char* buf_;
};