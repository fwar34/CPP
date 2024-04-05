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

class SendQueue
{
public:
    SendQueue() : sendQueueMaxLength_(SEND_QUEUE_MAX_LENGTH), event_(nullptr)
    {
        eventFd_ = eventfd(0, 0);
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

    bool Push(MsgSendNode* msg);
    void ProcessSend();

private:
    std::mutex mutex_;
    size_t sendQueueMaxLength_;
    std::list<MsgSendNode> sendNodes_;
    struct event* event_;
    int eventFd_;
};
    
} // namespace Nt