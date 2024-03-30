#include "IOServicePool.h"
#include "Reactor.h"
#include "CommonDefine.h"
#include "Session.h"
#include "SessionMgr.h"
#include <csignal>
#include <iostream>
#include <memory>

struct event_base* IOServicePool::GetEventBase()
{
    static size_t index = 0;
    return evbases_[index++ % threadNum_];
}

int IOServicePool::Start()
{
    for (size_t i = 0; i < threadNum_; ++i) {
        threads_.emplace_back([this, i] {
            std::thread::id id = std::this_thread::get_id();
            struct event_base* evbase = event_base_new();
            struct sockaddr_in addr = {0};
            addr.sin_family = AF_INET;
            addr.sin_port = 18888;
            struct evconnlistener *evlistener = evconnlistener_new_bind(
                evbase, AcceptCb, (void *)evbase, 
                LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE_PORT, -1,
                (struct sockaddr *)&addr, sizeof(addr)); 

            struct event* sig_event = nullptr;
            if (i == 0) {
                // 给第一个线程的 event_base 注册一个 SIGINT 信号事件回调
                sig_event = evsignal_new(evbase, SIGINT, SignalCb, (void *)evbase);
                if (!sig_event || event_add(sig_event, nullptr) < 0) {
                    std::cout << "Could not create a signal event!" << std::endl;
                    return -2;
                }
            }

            {
                std::lock_guard<std::mutex> lock(mutex_);
                evbases_.push_back(evbase);
            }

            event_base_dispatch(evbase);
            evconnlistener_free(evlistener);
            if (i == 0) {
                event_free(sig_event);
            }
            event_base_free(evbase);
            std::cout << "io thread: " << std::this_thread::get_id() << " exit!!!" << std::endl;
        });
    }

    for (auto& t : threads_) {
        t.join();
    }

    return 0;
}

void IOServicePool::Stop()
{
    EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);
    event_debug_assert_is_setup_(ev);
	event_active_nolock_(ev, res, ncalls);
	EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);

    struct timeval tv = {1, 0};
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "evbases_ size = " << evbases_.size() << std::endl;
    for (auto& evbase : evbases_) {
        std::cout << "evbase address = " << evbase << std::endl;
        event_base_loopexit(evbase, &tv);
    }
}