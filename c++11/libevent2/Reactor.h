#pragma once
#include "Dispatch.h"
#include <mutex>
#include <vector>

struct event_base;
class Reactor : public Dispatch
{
public:
    void Dispatch() override;

private:
    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;
    Reactor(const Reactor&&) = delete;
    Reactor& operator=(const Reactor&&) = delete;

    std::mutex mutex_;
    struct event_base* evbase_;
};