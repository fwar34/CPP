#pragma once
#include "Object.h"
#include "Handler.h"
#include "Message.h"
// #include "MsgNode.h"
#include "MsgNode2.h"
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
    Session(struct bufferevent* bev, const Address& address, 
        IOThread* thd, evutil_socket_t sock) : 
        Object(thd), address_(address), headerParseComplete_(false), 
        bev_(bev), sock_(sock)
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

    evutil_socket_t SockFd()
    {
        return sock_;
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
    bool headerParseComplete_;
    std::list<MsgSendNode> sendNodes_;
    struct bufferevent* bev_;
    evutil_socket_t sock_;
};

}