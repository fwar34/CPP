#pragma once
#include "Dispatch.h"
#include <mutex>
#include <vector>

struct event_base;
struct evconnlistener;
struct bufferevent;

namespace Nt
{

class IOThread;
class Reactor : public Dispatch
{
public:
    Reactor() : thread_(nullptr)
    {
    }
    Reactor(IOThread* thread) : thread_(thread)
    {
    }
    ~Reactor();
    void SetThread(IOThread* thread)
    {
        thread_ = thread;
    }
    int Start() override;
    void DispatchEvents() override;
    struct event_base* EventBase()
    {
        return evbase_;
    }
    IOThread* Thread()
    {
        return thread_;
    }

private:
    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;
    Reactor(const Reactor&&) = delete;
    Reactor& operator=(const Reactor&&) = delete;

    IOThread* thread_;
    // std::mutex mutex_;
    struct event_base* evbase_ = nullptr;
    struct evconnlistener* evlistener_ = nullptr;
    struct bufferevent* evSockPairEvent_ = nullptr;
};

} // namespace Nt