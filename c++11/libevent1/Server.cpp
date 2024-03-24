#include "Server.h"
#include "Session.h"
#include "ThreadPool.hpp"
#include "SessionMgr.h"
#include <functional>
#include <iostream>
#include <iomanip>
#include <cstring>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>
#include <signal.h>

constexpr size_t headerLength = 2;

bool DecodeHeader(const char* buf, size_t len)
{
    return true;
}

static void PrintBuf(const char* buf, size_t len)
{
    std::stringstream ss;
    std::string fmt = "0x";
    for (int i = 0; i < len; ++i) {
        std::string hexStr;
        ss << std::hex << std::setw(2) << std::setfill('0') << int(buf[i]) << std::endl;
        ss >> hexStr;
        fmt += hexStr;
    }
    std::cout << "recv rawdata is: " << fmt << std::endl;
}

// 原文链接：https://blog.csdn.net/u013229693/article/details/52168874
// 结论：
// 　基于文件描述符的读回调确实是LT触发的。bufferevent底层也是LT触发的，bufferevent底层会尽量读去来自网络的数据，放到input evbuffer中。
// 　但基于bufferevent的读回调则类似ET触发，用户绑定在bufferevent上触发的读回调如果没有一次读走bufferevent下input evbuffer中的数据，
//   则除非再次有网络数据到达，否则不会触发读回调。
void Server::ReadCb(struct bufferevent *bev, void *ctx)
{
    static uint32_t times = 0;
    struct event_base* evbase = reinterpret_cast<struct event_base*>(ctx);
    (void)evbase;
    struct evbuffer* input = bufferevent_get_input(bev);
    uint16_t totalLen = evbuffer_get_length(input);
    std::cout << "read_cb count = " << ++times << " input length = " << totalLen << std::endl;

    static char buf[UINT16_MAX] = {0};
    static size_t curPos = 0;
    // 读出所有的包
    size_t readLen = bufferevent_read(bev, buf + curPos, UINT16_MAX);
    curPos += readLen;
    if (curPos < headerLength) {
        return;
    }

#if 0
    char buf[1024] = {0};
    uint16_t readLen = bufferevent_read(bev, buf, 1024);
    PrintBuf(buf, readLen);
    std::cout << "readLen = " << readLen << std::endl;
#endif
    
#if 0
    bufferevent_read(bev, buf, 3);
    len = evbuffer_get_length(input);
    std::cout << "after read 3, read_cb count = " << ++times << " input length = " << len << std::endl;
#endif
    // 切包逻辑: 每取出一个完整包，将剩余的内存向前平移
    // 每个包组成: 头部2字节(内容包的长度) + body
    while (curPos > headerLength) {
        MessageHeader* header = reinterpret_cast<MessageHeader*>(buf);
        // body 没有收全
        if (curPos - headerLength < header->len) {
            return;
        }

        uint16_t msgTotalLen = headerLength + header->len;
        char* msgBuf = new char[msgTotalLen];
        memcpy(msgBuf, buf, msgTotalLen);
        memmove(buf, buf + msgTotalLen, curPos - msgTotalLen);
        curPos -= msgTotalLen;

        std::shared_ptr<char[]> msg(msgBuf, [](char* p) { 
            delete[] p; 
            std::cout << "delete[] p address = " << p << std::endl; 
        });
        ThreadPool::GetInstance().Commit([msg, bev]() {
            std::cout << "msg shared_ptr ref count = " << msg.use_count() << " in thread: " << std::this_thread::get_id() << std::endl;
            Message* message = reinterpret_cast<Message*>(msg.get());
            int ret = bufferevent_write(bev, reinterpret_cast<char*>(message), headerLength + message->header.len);
            std::cout << "reply to client ret = " << ret << " in thread: " << std::this_thread::get_id() << std::endl;
        });
    }
}

static void read_cb2(struct bufferevent* bev, void* ctx)
{
    
}

static void write_cb(struct bufferevent* bev, void* ctx)
{
    static int count = 0;
    std::cout << "count = " << count++ << std::endl;
}

static void event_cb(struct bufferevent *bev, short what, void *ctx)
{

}

static void signal_cb(evutil_socket_t sig, short events, void* user_data)
{
	struct event_base *base = (event_base*)user_data;
	struct timeval delay = { 2, 0 };

	std::cout << "Caught an interrupt signal; exiting cleanly in two seconds." << std::endl;

	event_base_loopexit(base, &delay);
}

void AcceptCb(struct evconnlistener* evlistener, evutil_socket_t sock, struct sockaddr* addr, int socklen, void* user_data)
{
    struct event_base *evbase = reinterpret_cast<struct event_base *>(user_data);
    struct bufferevent* bufevent = bufferevent_socket_new(evbase, sock, BEV_OPT_CLOSE_ON_FREE);
    if (!bufevent) {
        std::cout << "Could not create bufferevent!" << std::endl;
        event_base_loopbreak(evbase);
        return;
    }

    Address address = {0};
    std::shared_ptr<Session> session = std::make_shared<Session>(bufevent, address);
    SessionMgr::GetInstance().AddSession(session);

    auto readCb = [this, session](struct bufferevent* bev, void* ctx) {
        ReadCb(bev, ctx);
    };

    // bufferevent_setcb(bufevent, read_cb, write_cb, event_cb, (void*)evbase);
    bufferevent_setcb(bufevent, readCb, write_cb, event_cb, user_data);
    bufferevent_enable(bufevent, EV_READ);
    bufferevent_enable(bufevent, EV_WRITE);
   
}

int Server::Start()
{
    evbase_ = event_base_new();

    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(18888);
    auto acceptCb = [](struct evconnlistener *evlistener, evutil_socket_t sock, struct sockaddr *addr, int socklen, void *user_data) {
        AcceptCb(evlistener, sock, addr, user_data);
    };
    typedef void (*Func)(struct evconnlistener *evlistener, evutil_socket_t sock, struct sockaddr *addr, int socklen, void *user_data);
    // CBPtr ptr = static_cast<Func>(acceptCb);
    // auto acceptCb = std::bind(&Server::AcceptCb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);

    struct evconnlistener* evlistener = evconnlistener_new_bind(
        evbase_, std::function<Func>(acceptCb).target<Func>(acceptCb), (void*)evbase_,  LEV_OPT_REUSEABLE  | LEV_OPT_CLOSE_ON_FREE, -1, 
        (struct sockaddr*)&sin, sizeof(sin));
    if (!evlistener) {
        std::cout << "Could not create a listener!" << std::endl;
        return -1;
    }

    struct event* sig_event = evsignal_new(evbase_, SIGINT, signal_cb, (void*)evbase_);
    if (!sig_event || event_add(sig_event, nullptr) < 0) {
        std::cout << "Could not create a signal event!" << std::endl;
        return -2;
    }

    std::cout << "main thread: " << std::this_thread::get_id() << std::endl;
    ThreadPool::GetInstance().Start();

    event_base_dispatch(evbase_);
    evconnlistener_free(evlistener);
    event_free(sig_event);
    event_base_free(evbase_);

    std::cout << "server shutdown!!" << std::endl;

    return 0;
}

void Server::Stop()
{
    event_base_loopbreak(evbase_);
    ThreadPool::GetInstance().Stop();
    SessionMgr::GetInstance().ClearSessions();
}