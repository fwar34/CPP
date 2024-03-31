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
        s.receiver_->ProcessSignal(s.signalId_);
    }
}

void SignalBox::Push(const Signal& s)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bool needNotify = signals_.empty();
    signals_.push_back(s);

    if (needNotify) {
        int value = 1;
        write(eventFd_, &value, sizeof(value));
    }
}

};