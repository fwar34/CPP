#include "SendQueue.h"
#include "Session.h"
#include <event2/event.h>
#include <cerrno>
#include <cstring>

namespace Nt
{

bool SendQueue::Push(const MsgSendNode& msg)
{
    bool needNotify = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (sendNodes_.size() >= sendQueueMaxLength_) {
            return false;
        }

        std::cout << "SendQueue::Push size:" << sendNodes_.size() << std::endl;
        needNotify = sendNodes_.empty();
        sendNodes_.push_back(std::move(msg));
    }

    if (needNotify) {
        uint64_t value = 1;
        ssize_t ret = write(eventFd_, &value, sizeof(value));
        std::cout << "SendQueue::Push notify ret: " << ret 
            << " error: " << strerror(errno) << std::endl;
    }
    return true;
}

void SendQueue::ProcessSend(bool needReadEventFd)
{
    std::cout << "SendQueue::ProcessSend needReadEventFd: " << needReadEventFd << std::endl;
    if (needReadEventFd) {
        uint64_t value = 0;
        read(eventFd_, &value, sizeof(value));
    }

    std::list<MsgSendNode> slist;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        sendNodes_.swap(slist);
    }

    int ret = 0;
    for (auto it = slist.begin(); it != slist.end(); ) {
        ret = bufferevent_write(session_->GetBufferEvent(), 
            (*it).GetData(), (*it).GetDataSize());
        std::cout << "session fd: " << session_->GetBufferEvent() << " bufferevent_write ret: " << ret << std::endl;
        if (ret == 0) {
            it = slist.erase(it);
        } else {
            break;
        }
    }

    if (!slist.empty()) {
        std::cout << "slist size: " << slist.size() << std::endl;
        std::lock_guard<std::mutex> lock(mutex_);
        sendNodes_.splice(sendNodes_.begin(), slist);
        bufferevent_enable(session_->GetBufferEvent(), EV_WRITE);
    }
}

} // namespace Nt