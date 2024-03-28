#pragma once
#include <thread>
#include <vector>
#include <event2/event.h>

class EventBasePool
{
public:
    ~EventBasePool() = default;
    static EventBasePool& GetInstance()
    {
        static EventBasePool instance;
        return instance;
    }

    struct event_base* GetEventBase();
    bool Start();
    void Stop();

private:
    EventBasePool(size_t threadNum = std::thread::hardware_concurrency()) : threadNum_(threadNum)
    {
    }
    std::vector<struct event_base*> evbases_;
    std::vector<std::thread> threads_;
    size_t threadNum_;
};