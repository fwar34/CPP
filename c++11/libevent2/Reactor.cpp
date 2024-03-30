#include "Reactor.h"
#include "Session.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>
#include <iostream>
#include <thread>

constexpr unsigned short PORT = 18888;

static void WriteCb(struct bufferevent* bev, void* ctx)
{
    static int count = 0;
    std::cout << "count = " << count++ << std::endl;
}

static void EventCb(struct bufferevent *bev, short what, void *ctx)
{

}

static void ReadCb(struct bufferevent *bev, void *ctx)
{
    Session* session = reinterpret_cast<Session*>(ctx);
    session->HandleInput(bev);
}

static void AcceptCb(struct evconnlistener* evlistener, evutil_socket_t sock, struct sockaddr* addr, int socklen, void* user_data)
{
    struct event_base *evbase = reinterpret_cast<struct event_base*>(user_data);
    struct bufferevent* bufevent = bufferevent_socket_new(evbase, sock, BEV_OPT_CLOSE_ON_FREE);
    if (!bufevent) {
        std::cout << "Could not create bufferevent!" << std::endl;
        event_base_loopbreak(evbase);
        return;
    }

    Address address = {"", 0};
    Session* session = new Session(bufevent, address);
    SessionMgr::GetInstance().AddSession(session);
    bufferevent_setcb(bufevent, ReadCb, nullptr, EventCb, session);
    bufferevent_enable(bufevent, EV_READ);
    // bufferevent_enable(bufevent, EV_WRITE);
}

static void ReadSockPairCb(struct bufferevent *bev, void *ctx)
{
    Session* session = reinterpret_cast<Session*>(ctx);
}

void Reactor::Dispatch()
{
    evbase_ = event_base_new();
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = PORT;
    // 当多个进程或线程需要监听同一个端口时，REUSEPORT 选项非常有用。
    // 每个进程或线程可以创建自己的套接字，并将它们绑定到同一个端口上。
    // 然后，当新的连接请求到达时，内核会负责将这些连接分发到不同的套接字上，
    // 从而实现负载均衡和并行处理。
    // 这里多个线程同时监听 listen fd
    struct evconnlistener *evlistener = evconnlistener_new_bind(
        evbase_, AcceptCb, evbase_,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE_PORT,
        -1, (struct sockaddr *)&addr, sizeof(addr));

    struct bufferevent* evSockPairEvent = bufferevent_socket_new(evbase_, sockPair_[0], BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(evSockPairEvent, ReadSockPairCb, nullptr, nullptr, nullptr);
    bufferevent_enable(evSockPairEvent, EV_READ);

    event_base_dispatch(evbase_);
    evconnlistener_free(evlistener);
    bufferevent_free(evSockPairEvent);
    event_base_free(evbase_);
    std::cout << "io thread: " << std::this_thread::get_id() << " exit!!!" << std::endl;
}