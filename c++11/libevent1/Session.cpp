#include "Session.h"
#include <event2/bufferevent.h>

Session::Session(struct bufferevent* bev, const Address& address) : bev_(bev), address_(address), curPos_(0)
{
}

const std::string& Id()
{
    return id_;
}

std::shared_ptr<Message> Session::GetMessage()
{

}

bool Session::ParseHeader()
{

}

bool Session::ParseBody()
{

}