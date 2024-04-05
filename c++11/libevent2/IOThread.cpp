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
    AddRef(); // 创建的线程持有 this，引用计数加 1
    thread_ = std::thread([this] {
        if (dispatch_->Start() != NtErrorSucess) {
            std::cout << "iothread: " << std::this_thread::get_id() << "error, exit!" << std::endl;
            return;
        }
        // 注册 IOThread 的 SignalBox 的 eventfd 事件到 event_base
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
    IOThread* thread = reinterpret_cast<IOThread*>(arg);
    thread->GetSignalBox().ProcessSignals();
}

void IOThread::RegisterSignalEvent()
{
    std::cout << "iothread tid: " << std::this_thread::get_id()
        << " register signal event" << std::endl;
    AddRef(); // 将 IOThread 注册到 reactor 中去，引用计数加 1
    signalEvent_ = dispatch_->RegisterEvent(
        signalBox_.EventFd(), EV_READ, SignalCb, this);
}

} // namespace Nt