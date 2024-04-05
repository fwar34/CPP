#include "ConferenceMgr.h"
#include "Conference.h"

namespace Nt
{

// ConferenceMgr ConfMgrs[LOGIC_THREAD_NUM];
// std::array<std::thread::id, LOGIC_THREAD_NUM> ConfMgrs;

void ConferenceMgr::AddConference(std::shared_ptr<Conference>& conf)
{
    auto it = conferences_.find(conf->Id());
    if (it == conferences_.end()) {
        conferences_[conf->Id()] = conf;
    }
}

void ConferenceMgr::DelConference(uint32_t confId)
{
    conferences_.erase(confId);
}

std::shared_ptr<Conference> ConferenceMgr::FindConference(uint32_t confId)
{
    auto it = conferences_.find(confId);
    if (it == conferences_.end()) {
        return nullptr;
    }
    return it->second;
}

void ConferenceMgr::DispatchCommand(std::shared_ptr<Message> message, Session* session)
{
    MessageHeader& header = message->header_;
    std::shared_ptr<Conference> conf = FindConference(header.confId_);
    if (!conf) {
        conf = std::make_shared<Conference>(header.confId_);
        conferences_[header.confId_] = conf;
    }

    conf->DispatchCommand(message, session);
}

} // namespace Nt