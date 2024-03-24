#include "SessionMgr.h"
#include "Session.h"
#include <event2/bufferevent.h>

bool SessionMgr::AddSession(std::shared_ptr<Session> session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(session->Id());
    if (it != sessions_.end()) {
        return false;
    }
    sessions_.insert(std::make_pair(session->Id(), std::move(session)));
}

void SessionMgr::RemoveSession(const std::string& id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(id);
}

void SessionMgr::ClearSessions()
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.clear();
}
