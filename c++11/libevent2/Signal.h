#pragma once
#include <cstdint>
#include <mutex>
#include <list>

namespace NewNt
{
constexpr int EVENTFD_INIT_VALUE = 0;

class Object;
class Signal
{
    enum signalId
    {
        SIGNAL_START = 0,
        SIGNAL_STOP,
    };
public:
    Signal(uint32_t signalId, Object* receiver) : 
        signalId_(signalId), receiver_(receiver)
    {
    }

private:
    uint32_t signalId_;
    Object* receiver_;
};

class SignalBox
{
public:
    SignalBox()
    {
        eventFd_ = eventfd(EVENTFD_INIT_VALUE, 0);
    }
    ~SignalBox();
    {
        close(eventFd_);
    }

    int EventFd()
    {
        return eventFd_;
    }
    bool Empty()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return singals_.empty();
    }
    bool Pop(Signal& s);
    void Push(const Signal& s);

private:
    std::mutex mutex_;
    std::list<Signal> signals_;
    int eventFd_;
}
};