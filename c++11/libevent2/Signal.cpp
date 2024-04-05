#include "Object.h"
#include "Signal.h"
#include <list>
#include <event2/event.h>

namespace Nt
{

void SignalBox::ProcessSignals()
{
    int value = 0;
    read(eventFd_, &value, sizeof(value));
    std::list<Signal> signals;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        signals_.swap(signals);
    }

    for (auto& s : signals) {
        s.Receiver()->ProcessSignal(s.Id());
    }
}

void SignalBox::Push(const Signal& s)
{
    bool needNotify = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        needNotify = signals_.empty();
        signals_.push_back(s);
    }

    if (needNotify) {
        int value = 1;
        write(eventFd_, &value, sizeof(value));
    }
}

};