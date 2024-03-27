#pragma once
#include "CommonDefine.h"
#include "Message.h"
#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include <climits>

struct bufferevent;
class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(struct bufferevent* bev, const Address& address);
    Session(const Session&) = delete;
    Session& operator =(const Session&) = delete;

    void Close();
    const std::string& Id();
    MessageHeader& Header();
    void ParseHeaderComplete(bool flag);
    bool ParseHeaderComplete();
    bool AllocBody(size_t length);
    char* Buffer();
    void BufferReset();

private:
    struct bufferevent* bev_;
    Address address_;
    MessageHeader header_;  // 存储切包的 header
    bool parseHeaderComplete_; // 上次切包 header 是否处理完成
    // std::vector<uint8_t> buffer_;
    // char buffer_[UINT16_MAX] = {0};
    char* buffer_ = nullptr;
    // std::shared_ptr<char[]> buffer_;
    size_t curPos_;
    std::string id_;
};