#include "Reactor.h"
#include "Session.h"
#include "SessionMgr.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>
#include <iostream>
#include <thread>

namespace Nt
{

constexpr unsigned short PORT = 18888;

static void WriteCb(struct bufferevent* bev, void* ctx)
{
    static int count = 0;
    std::cout << "count = " << count++ << std::endl;
}

static void EventCb(struct bufferevent *bev, short what, void *ctx)
{
    // 释放 bev 和相关的数据结构，减引用计数
}

static void ReadCb(struct bufferevent *bev, void *ctx)
{
    Session* session = reinterpret_cast<Session*>(ctx);
    session->HandleInput(bev);
}

static void AcceptCb(struct evconnlistener* evlistener, evutil_socket_t sock, struct sockaddr* addr, int socklen, void* user_data)
{
    Reactor* reactor = reinterpret_cast<Reactor*>(user_data);
    struct bufferevent* bufevent = bufferevent_socket_new(
        evconnlistener_get_base(evlistener), sock, BEV_OPT_CLOSE_ON_FREE);
    if (!bufevent) {
        std::cout << "Could not create bufferevent!" << std::endl;
        event_base_loopbreak(evconnlistener_get_base(evlistener));
        return;
    }

    Address address = {"", 0};
    Session* session = new Session(bufevent, address, reactor->Thread());
    SessionMgr::GetInstance().AddSession(session);
    bufferevent_setcb(bufevent, ReadCb, nullptr, EventCb, session);
    session->AddRef(); // 保存到 EventCb 的上下文
    bufferevent_enable(bufevent, EV_READ);
    // bufferevent_enable(bufevent, EV_WRITE);
}

static void ReadSockPairCb(struct bufferevent *bev, void *ctx)
{
    Reactor* reactor = reinterpret_cast<Reactor*>(ctx);
    std::cout << "io thread: " << std::this_thread::get_id() << " break" << std::endl;
    event_base_loopbreak(bufferevent_get_base(bev));
}

Reactor::~Reactor()
{
    if (evlistener_) {
        evconnlistener_free(evlistener_);
    }

    if (evSockPairEvent_) {
        bufferevent_free(evSockPairEvent_);
    }

    if (evbase_) {
        event_base_free(evbase_);
    }
}

void Reactor::Start()
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
    evlistener_ = evconnlistener_new_bind(
        evbase_, AcceptCb, this,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE_PORT,
        -1, (struct sockaddr *)&addr, sizeof(addr));

    struct bufferevent* evSockPairEvent_ = bufferevent_socket_new(evbase_, sockPair_[0], BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(evSockPairEvent_, ReadSockPairCb, nullptr, nullptr, this);
    bufferevent_enable(evSockPairEvent_, EV_READ);
}

void Reactor::DispatchEvents()
{
    event_base_dispatch(evbase_);
    std::cout << "io thread: " << std::this_thread::get_id() << " exit!!!" << std::endl;
}

} // namespace Nt