#pragma once
#include "Dispatch.h"
#include <mutex>
#include <vector>

namespace Nt
{

struct event_base;
class Reactor : public Dispatch
{
public:
    Reactor() = default;
    ~Reactor();
    void Start() override;
    void DispatchEvents() override;
    struct event_base* EventBase()
    {
        return evbase_;
    }

private:
    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;
    Reactor(const Reactor&&) = delete;
    Reactor& operator=(const Reactor&&) = delete;

    std::mutex mutex_;
    struct event_base* evbase_ = nullptr;
    struct evlistener* evlistener_ = nullptr;
    struct ebufferevent* evSockPairEvent_ = nullptr;
};

}; // namespace