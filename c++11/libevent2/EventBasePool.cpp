#include "EventBasePool.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>
#include <csignal>

static void AcceptCb(struct evconnlistener* listener, evutil_socket_t sock, 
                     struct sockaddr* addr, int socklen, void* args)
{
    struct event_base *evbase = EventBasePool::GetInstance().GetBaseEvent();
    struct bufferevent* bufevent = bufferevent_socket_new(evbase, sock, BEV_OPT_CLOSE_ON_FREE);
    if (!bufevent) {
        std::cout << "Could not create bufferevent!" << std::endl;
        event_base_loopbreak(evbase);
        return;
    }
}

struct event_base *GetEventBase()
{
    static size_t index = 0;
    return evbases_[index++ % threadNum_];
}

static void WriteCb(struct bufferevent* bev, void* ctx)
{
    static int count = 0;
    std::cout << "count = " << count++ << std::endl;
}

static void EventCb(struct bufferevent *bev, short what, void *ctx)
{

}

static void SignalCb(evutil_socket_t sig, short events, void* user_data)
{
	struct event_base *base = (event_base*)user_data;
	struct timeval delay = { 1, 0 };
	std::cout << "Caught an interrupt signal; exiting cleanly in one second." << std::endl;
	event_base_loopexit(base, &delay);
}


// static void ReadCb2(struct bufferevent* bev, void* arg)
static void AcceptCb(struct evconnlistener* evlistener, evutil_socket_t sock, struct sockaddr* addr, int socklen, void* user_data)
{
    struct event_base *evbase = reinterpret_cast<struct event_base *>(user_data);
    struct bufferevent* bufevent = bufferevent_socket_new(evbase, sock, BEV_OPT_CLOSE_ON_FREE);
    if (!bufevent) {
        std::cout << "Could not create bufferevent!" << std::endl;
        event_base_loopbreak(evbase);
        return;
    }

    Address address = {"", 0};
    auto session = std::make_shared<Session>(bufevent, address);
    SessionMgr::GetInstance()->AddSession(session);
    bufferevent_setcb(bufevent, [session](struct bufferevent* bev, void* arg) {
        // TODO
    }, [session](struct bufferevent *bev, short what, void *ctx) {

    }, EventCb, user_data);

    bufferevent_enable(bufevent, EV_READ);
    // bufferevent_enable(bufevent, EV_WRITE);
}

bool EventBasePool::Start()
{
    for (size_t i = 0; i < threadNum_; ++i) {
        threads_.emplace_back([this, i] {
            evbases_[i] = event_base_new();
            // 当多个进程或线程需要监听同一个端口时，REUSEPORT 选项非常有用。
            // 每个进程或线程可以创建自己的套接字，并将它们绑定到同一个端口上。
            // 然后，当新的连接请求到达时，内核会负责将这些连接分发到不同的套接字上，
            // 从而实现负载均衡和并行处理。
            // 这里多个线程同时监听 listen fd
            struct sockaddr_in addr = {0};
            addr.sin_family = AF_INET;
            addr.sin_port = 18888;
            struct evconnlistener *evlistener = evconnlistener_new_bind(
                evbase_[i], AcceptCb, (void *)evbase_[i], 
                LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE_PORT, -1,
                (struct sockaddr *)&addr, sizeof(addr)); });

        if (i == 0) {
            // 给第一个线程的 event_base 注册一个信号事件回调
            struct event *sig_event = evsignal_new(evbase_[i], SIGINT, SignalCb, (void *)evbase_[i]);
            if (!sig_event || event_add(sig_event, nullptr) < 0) {
                std::cout << "Could not create a signal event!" << std::endl;
                return -2;
            }
        }

        event_base_dispatch(evbase_[i]);
        evconnlistener_free(evlistener);
        if (i == 0) {
            event_free(sig_event);
        }
        event_base_free(evbase_[i]);
        std::cout << "thread: " << std::this_thread::get_id() << " exit!!!" << std::endl;
    }
}

void EventBasePool::Stop()
{
    for (auto& evbase : evbases_) {
        event_base_loopexit(evbase);
    }
}