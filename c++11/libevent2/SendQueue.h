#pragma once
#include "MsgNode2.h"
#include <mutex>
#include <list>
#include <sys/eventfd.h>
#include <unistd.h>

struct event;
namespace Nt
{
constexpr size_t SEND_QUEUE_MAX_LENGTH = 1024 * 1024;

class Session;
class SendQueue
{
public:
    SendQueue(Session* session) : sendQueueMaxLength_(SEND_QUEUE_MAX_LENGTH), 
        session_(session), event_(nullptr)
    {
        // 这里的 session 不要引用计数加 1，否则 Session 持有 SendQueue，
        // SendQueue 持有 session，互相引用造成资源没有释放
        eventFd_ = eventfd(0, 0);
        std::cout << "create eventfd ret: " << eventFd_ << std::endl;
    }
    ~SendQueue()
    {
        close(eventFd_);
    }
    int EventFd()
    {
        return eventFd_;
    }
    void SetEvent(struct event* ev)
    {
        event_ = ev;
    }

    bool Push(const MsgSendNode& msg);
    void ProcessSend(bool needReadEventFd);

private:
    std::mutex mutex_;
    size_t sendQueueMaxLength_;
    std::list<MsgSendNode> sendNodes_;
    Session* session_;
    struct event* event_;
    int eventFd_;
};
    
} // namespace Nt