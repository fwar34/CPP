#include "IOThread.h"
#include "Reactor.h"
#include <event2/event.h>

void IOThread::SendSignal(const Signal& s)
{
    signalBox_->Push(s);
}

void IOThread::Start()
{
    thread_ = std::thread([this]() {
        dispatch_->Start();
        RegisterSignalEvent();
        dispatch_->Dispatch();
    });
}

void IOThread::Stop()
{
    dispatch_->Stop();

    if (thread_.joinable()) {
        thread_.join();
    }
}

static void SignalCb(evutil_socket_t fd, short events, void *arg)
{

}

void IOThread::RegisterSignalEvent()
{
    Reactor* reactor = dynamic_cast<Reactor*>(dispatch_);
    signalEvent_ = event_new(reactor->EventBase(), EV_READ, SignalCb, this);
    event_add(signalEvent_, nullptr);
}

};