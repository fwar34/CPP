#include "Session.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>

namespace Nt
{

void Session::HandleInput(struct bufferevent *bev)
{
    struct evbuffer* input = bufferevent_get_input(bev);
    size_t inputLength = evbuffer_get_length(input);
}
void Session::HandleOutput()
{

}
void Session::HandleClose(struct bufferevent *bev)
{

}

void Session::HandleTimeout()
{
}
int Session::Start()
{
    return 0;
}
int Session::Accept()
{
    return 0;
}
int Session::Close()
{

    return 0;
}
int Session::Connect()
{

    return 0;
}

}