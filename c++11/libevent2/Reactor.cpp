#include "Reactor.h"
#include "Session.h"
#include "SessionMgr.h"
#include "ErrorCode.h"
#include "IOThread.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>
#include <event2/util.h>
#include <iostream>
#include <thread>
#include <arpa/inet.h>

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
    // 释放 bev 和相关的数据结构，减 Session 引用计数
}

static void ReadCb(struct bufferevent *bev, void *ctx)
{
    Session* session = reinterpret_cast<Session*>(ctx);
    session->HandleInput(bev);
}

static void GetPeerAddress(evutil_socket_t sock, Address& address)
{
    struct sockaddr_storage peerAddr;
    socklen_t peerLen = sizeof(peerAddr);

    if (getpeername(sock, (struct sockaddr*)&peerAddr, &peerLen) == 0) {
        char ip[INET6_ADDRSTRLEN] = {0};
        if (peerAddr.ss_family == AF_INET) {
            struct sockaddr_in* s = (struct sockaddr_in*)&peerAddr;
            inet_ntop(AF_INET, &s->sin_addr, ip, sizeof(ip));
            address.ip_.assign(ip, sizeof(ip));
            address.port_ = ntohs(s->sin_port);
        }
    }
}

static void AcceptCb(struct evconnlistener* evlistener, evutil_socket_t sock, 
    struct sockaddr* addr, int socklen, void* user_data)
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
    GetPeerAddress(sock, address);
    reactor->Thread()->AddRef(); // Session 持有 thread，引用计数加 1
    // session 创建的时候引用计数为 1
    Session* session = new Session(bufevent, address, reactor->Thread());
    session->Start();
    SessionMgr::GetInstance().AddSession(session); // 引用计数加 1，为 2
    // 保存到回调的上下文，引用计数为 2，不用加引用计数，因为 Session 创建的时候引用计数就为 1
    bufferevent_setcb(bufevent, ReadCb, nullptr, EventCb, session);
    bufferevent_enable(bufevent, EV_READ);
    // bufferevent_enable(bufevent, EV_WRITE);
    std::cout << "IOThread: " << std::this_thread::get_id() << " accept a new client: "
        << address << std::endl;
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

int Reactor::Start()
{
    evbase_ = event_base_new();
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    // 这里注意必须使用 htons 转换端口号
    addr.sin_port = htons(PORT);
    // 当多个进程或线程需要监听同一个端口时，REUSEPORT 选项非常有用。
    // 每个进程或线程可以创建自己的套接字，并将它们绑定到同一个端口上。
    // 然后，当新的连接请求到达时，内核会负责将这些连接分发到不同的套接字上，
    // 从而实现负载均衡和并行处理。
    // 这里多个线程同时监听 listen fd
    evlistener_ = evconnlistener_new_bind(evbase_, AcceptCb, this,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE_PORT,
        -1, (struct sockaddr *)&addr, sizeof(addr));
    if (!evlistener_) {
        std::cout << "iothread: " << std::this_thread::get_id() << " create listener failed!" << std::endl;
        return NtErrorCreateListenFailed;
    }
    std::cout << "iothread tid: " << std::this_thread::get_id() << " listener fd: " 
        << evconnlistener_get_fd(evlistener_) << std::endl;

    // 注册接收 linux 停止信号触发的 sockpair 的事件到 event_base
    struct bufferevent* evSockPairEvent_ = bufferevent_socket_new(evbase_, sockPair_[0], BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(evSockPairEvent_, ReadSockPairCb, nullptr, nullptr, this);
    bufferevent_enable(evSockPairEvent_, EV_READ);

    return 0;
}

struct event* Reactor::RegisterEvent(int fd, int event, 
    void (*cb)(evutil_socket_t, short, void*), void* arg)
{
    struct event* ev = event_new(evbase_, fd, event, cb, arg);
    event_add(ev, nullptr);
    return ev;
}

int Reactor::CancelEvent(struct event* ev)
{
    return event_del(ev);
}

void Reactor::DispatchEvents()
{
    event_base_dispatch(evbase_);
    std::cout << "io thread: " << std::this_thread::get_id() << " exit!!!" << std::endl;
}

} // namespace Nt