#pragma once
#include "Dispatch.h"
#include "Signal.h"
#include "Object.h"
#include <thread>
#include <event2/event.h>

namespace Nt
{

class Signal;
class IOThread : public Object
{
public:
    IOThread(Dispatch* dispatch) : Object(this), dispatch_(dispatch)
    {
    }
    ~IOThread()
    {
        if (signalEvent_) {
            event_free(signalEvent_);
        }

        if (dispatch_) {
            delete dispatch_;
        }
    }
    Dispatch* GetDispatch()
    {
        return dispatch_;
    }
    SignalBox& GetSignalBox()
    {
        return signalBox_;
    }
    
    void SendSignal(const Signal& s);
    void Start();
    void Stop();
    void Join();

private:
    void RegisterSignalEvent();

    Dispatch* dispatch_; // reactor
    std::thread thread_;
    SignalBox signalBox_;
    struct event* signalEvent_;  // Signal 的 event
};

}