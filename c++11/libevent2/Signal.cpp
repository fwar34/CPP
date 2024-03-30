#include "Signal.h"

namespace NewNt
{
bool SignalBox::Pop(Signal& s)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (signals_.empty()) {
        return false;
    }
    s = signals_.front();
    signals_.pop_front();
    return true;
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