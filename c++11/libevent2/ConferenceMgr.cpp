#include "ConferenceMgr.h"
#include "Conference.h"

ConferenceMgr ConfMgrs[LOGIC_THREAD_NUM];

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

std::shared_ptr<Conference> FindConference(uint32_t confId)
{
    auto it = conferences_.find(confId);
    if (it == conferences_.end()) {
        return nullptr;
    }
    return it->second;
}