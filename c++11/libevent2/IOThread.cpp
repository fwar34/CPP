#include "IOThread.h"
#include "Reactor.h"
#include "ErrorCode.h"

namespace Nt
{

void IOThread::SendSignal(const Signal& s)
{
    signalBox_.Push(s);
}

void IOThread::Start()
{
    thread_ = std::thread([this]() {
        if (dispatch_->Start() != NtErrorSucess) {
            std::cout << "iothread: " << std::this_thread::get_id() << "error, exit!" << std::endl;
            return;
        }
        RegisterSignalEvent();
        dispatch_->DispatchEvents();
    });
}

void IOThread::Stop()
{
    dispatch_->Stop();
}

void IOThread::Join()
{
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
    signalEvent_ = event_new(reactor->EventBase(), signalBox_.EventFd(), EV_READ, SignalCb, this);
    event_add(signalEvent_, nullptr);
    std::cout << "iothread tid: " << std::this_thread::get_id() << " register signal event" << std::endl;
}

} // namespace Nt