#pragma once
#include "CommonDefine.h"
#include "Message.h"
#include <string>
#include <memory>
#include <vector>

struct bufferevent;
class Session : public std::enable_shared_from_this<Session>
{
    struct bufferevent* bev_;
    Address address_;
    std::vector<uint8_t> buffer_;
    size_t curPos_;
    std::string id_;

public:
    Session(struct bufferevent* bev, const Address& address);
    Session(const Session&) = delete;
    Session& operator =(const Session&) = delete;

    const std::string& Id();
    std::shared_ptr<Message> GetMessage();

private:
    bool ParseHeader();
    bool ParseBody();
};