#pragma once
#include <thread>
#include <vector>
#include <event2/event.h>

class BaseEventPool
{
public:
    ~BaseEventPool() = default;
    static BaseEventPool& GetInstance()
    {
        static BaseEventPool instance;
        return instance;
    }

    struct event_base* GetBaseEvent();
    bool Start();
    void Stop();

private:
    BaseEventPool(size_t threadNum = std::thread::hardware_concurrency()) : threadNum_(threadNum)
    {
    }
    std::vector<struct event_base*> evbases_;
    std::vector<std::thread> threads_;
    size_t threadNum_;
};