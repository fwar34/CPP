#pragma once
#include "Object.h"
#include "Handler.h"
#include "Message.h"
#include "MsgNode.h"
#include "CommonDefine.h"
#include <list>
#include <memory>
#include <cstdint>
#include <iostream>

struct bufferevent;
class Session : public IOHandler, public Object
{
public:
    Session(bufferevent* bev, const Address& address) : address_(address), recvNode_(UINT16_MAX), bev_(bev)
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

    void HandleInput(bufferevent* bev) override;
    void HandleOutput() override;
    void HandleClose(bufferevent* bev) override;
    void HandleTimeout() override;
    int Start() override;
    int Accept() override;
    int Close() override;
    int Connect() override;

private:
    Address address_;
    RecvMsgNode recvNode_;
    std::list<SendMsgNode> sendNodes_;
    bufferevent* bev_;
};