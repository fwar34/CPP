#pragma once
#include "Object.h"
#include "Handler.h"
#include "Message.h"
#include "MsgNode2.h"
#include "SendQueue.h"
#include "CommonDefine.h"
#include <list>
#include <memory>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <event2/util.h>

struct bufferevent;
namespace Nt
{

class Session final : public IOHandler, public Object
{
public:
    Session(struct bufferevent* bev, const Address& address, IOThread* thd) : 
        Object(thd), address_(address), headerParseComplete_(false), 
        sendQueueMaxLength_(SEND_QUEUE_MAX_LENGTH), bev_(bev)
    {
    }
    ~Session()
    {
        std::cout << "~Session called!" << std::endl;
    }

    const Address& GetAddress()
    {
        return address_;
    }
    bool SendMessage(std::shared_ptr<Message>& msg);
    SendQueue& GetSendQueue()
    {
        return sendQueue_;
    }

    void HandleInput(struct bufferevent* bev) override;
    void HandleOutput() override;
    void HandleClose(struct bufferevent* bev) override;
    void HandleTimeout() override;
    int Start() override;
    int Close() override;

private:
    Address address_;
    Message recvMsg_;
    SendQueue sendQueue_;
    bool headerParseComplete_;
    struct bufferevent* bev_;
};

}