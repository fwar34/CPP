#pragma once
#include "IOThread.h"
#include <thread>
#include <mutex>
#include <vector>
#include <csignal>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>

namespace Nt
{
constexpr size_t threadNum = 4;

class IOServicePool
{
public:
    ~IOServicePool()
    {
        for (auto ioThread : ioThreads_) {
            delete ioThread;
        }
    }
    static IOServicePool& GetInstance()
    {
        static IOServicePool instance(threadNum);
        return instance;
    }

    // struct event_base* GetEventBase();
    int Start();
    void Stop();

private:
    IOServicePool(size_t threadNum = std::thread::hardware_concurrency()) : 
        threadNum_(threadNum)
    {
    }
    IOServicePool(const IOServicePool&) = delete;
    IOServicePool& operator=(const IOServicePool&) = delete;

    std::vector<IOThread*> ioThreads_;
    size_t threadNum_;
};
};