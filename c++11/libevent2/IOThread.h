#pragma once
#include "Signal.h"
#include <vector>

namespace NewNt
{
class Dispatch;
class Signal;
class IOThread
{
public:
    IOThread(Dispatch* dispatch) : dispatch_(dispatch)
    {
    }
    void SendSignal(const Signal& s);
private:
    Dispatch* dispatch_; // reactor
    SignalBox signalBox_;

};
};