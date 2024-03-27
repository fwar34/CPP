#include "BaseEventPool.h"

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>
#include <csignal>

static void AcceptCb(struct evconnlistener* listener, evutil_socket_t sock, 
                     struct sockaddr* addr, int socklen, void* args)
{
    struct event_base *evbase = BaseEventPool::GetInstance().GetBaseEvent();
    struct bufferevent* bufevent = bufferevent_socket_new(evbase, sock, BEV_OPT_CLOSE_ON_FREE);
    if (!bufevent) {
        std::cout << "Could not create bufferevent!" << std::endl;
        event_base_loopbreak(evbase);
        return;
    }
}

struct event_base *GetBaseEvent()
{
    static size_t index = 0;
    return evbases_[index++ % threadNum_];
}

bool BaseEventPool::Start()
{
    for (size_t i = 0; i < threadNum_; ++i) {
        threads_.emplace_back([this, i] {
            evbases_[i] = event_base_new();

            if (i == 0) {
                // 只在第一个线程里面监听 listen socket
                struct sockaddr_in addr = {0};
                addr.sin_family = AF_INET;
                addr.sin_port = 18888;
                struct evconnlistener *evlistener = evconnlistener_new_bind(
                    evbase_[i], AcceptCb, (void *)evbase_[i], LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                    (struct sockaddr *)&sin, sizeof(sin)); 
            });


        }
    }
}

void BaseEventPool::Stop()
{
}