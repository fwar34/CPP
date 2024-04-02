#include "Session.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>

namespace Nt
{

void Session::HandleInput(struct bufferevent *bev)
{
    struct evbuffer* input = bufferevent_get_input(bev);
    // size_t inputLength = evbuffer_get_length(input);
    size_t totalLen = 0;
    while ((totalLen = evbuffer_get_length(input)) > 0) {
        if (!recvNode_.ParseHeaderComplete()) { // 头部没有处理完成
            if (recvNode_.CurLen() + totalLen < MSG_HEADER_LENGTH) {
                // 读入数据不够头部长度，等待下次读入
                return;
            }

        } else { // 头部处理完成

        }
    }

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