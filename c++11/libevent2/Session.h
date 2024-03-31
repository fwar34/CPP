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
namespace Nt
{

class Session : public IOHandler, public Object
{
public:
    Session(struct bufferevent* bev, const Address& address, IOThread* thd) : 
        Object(thd), address_(address), recvNode_(UINT16_MAX), bev_(bev)
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

    void HandleInput(struct bufferevent* bev) override;
    void HandleOutput() override;
    void HandleClose(struct bufferevent* bev) override;
    void HandleTimeout() override;
    int Start() override;
    int Accept() override;
    int Close() override;
    int Connect() override;

private:
    Address address_;
    RecvMsgNode recvNode_;
    std::list<SendMsgNode> sendNodes_;
    struct bufferevent* bev_;
};

}