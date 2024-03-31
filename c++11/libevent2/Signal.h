#pragma once
#include <cstdint>
#include <mutex>
#include <list>
#include <sys/eventfd.h>
#include <unistd.h>

namespace Nt
{
constexpr int EVENTFD_INIT_VALUE = 0;

class Object;
class Signal
{
public:
    enum SignalId
    {
        SIGNAL_START = 0,
        SIGNAL_STOP,
    };
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
    ~SignalBox()
    {
        close(eventFd_);
    }

    int EventFd()
    {
        return eventFd_;
    }
    void Push(const Signal& s);
    void ProcessSignals();

private:
    std::mutex mutex_;
    std::list<Signal> signals_;
    int eventFd_;
};

};