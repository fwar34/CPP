#pragma once
#include "Message.h"
#include <list>
#include <event2/bufferevent.h>

class Session : public enable_shared_from_this<Session>
{
public:
    Session(bufferevent* bev) : bev_(bev)
    {
    }

private:
    RecvMsgNode recvNode_;
    std::list<SendMsgNode> sendNodes_;
    bufferevent* bev_;
};