#include "SendQueue.h"
#include <event2/event.h>

namespace Nt
{

bool SendQueue::Push(MsgSendNode* msg)
{
    bool isEmpty = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (sendNodes_.size() >= sendQueueMaxLength_) {
            return false;
        }

        isEmpty = sendNodes_.empty();
        sendNodes_.push_back(msg);
    }
    if (isEmpty) {
        // TODO notify events
    
    }
    return true;
}

void SendQueue::ProcessSend()
{
    std::list<MsgSendNode> slist;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        sendNodes_.swap(slist);
    }

    for (auto& msg : slist) {
        
    }
}

} // namespace Nt