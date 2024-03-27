#include "Session.h"
#include "SessionMgr.h"
#include <event2/bufferevent.h>
#include <event2/event.h>

Session::Session(struct bufferevent* bev, const Address& address) : bev_(bev), address_(address), parseHeaderComplete_(false), curPos_(0)
{
}

void Session::Close()
{
    SessionMgr::GetInstance()->RemoveSession(id_);
    bufferevent_disable(bev_, EV_READ | EV_WRITE);
    bufferevent_free(bev_);
}

const std::string& Session::Id()
{
    return id_;
}

MessageHeader &Session::Header()
{
    return header_;
}

void Session::ParseHeaderComplete(bool flag)
{
    parseHeaderComplete_ = flag;
}

bool Session::ParseHeaderComplete()
{
    return parseHeaderComplete_;
}

bool Session::AllocBody(size_t length)
{
    if (buffer_)
    {
        return false;
    }
    buffer_ = new char[length];
    return true;
}

char *Session::Buffer()
{
    return buffer_;
}

void Session::BufferReset()
{
    buffer_ = nullptr;
}