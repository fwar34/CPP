#pragma once
#include "Singleton.h"
#include <map>
#include <string>
#include <memory>
#include <mutex>

class Session;
class SessionMgr : public Singleton<SessionMgr>
{
public:
    ~SessionMgr() = default;
    SessionMgr(const SessionMgr&) = delete;
    SessionMgr& operator=(const SessionMgr&) = delete;

    bool AddSession(std::shared_ptr<Session> session);
    void RemoveSession(const std::string& id);
    void ClearSessions();

private:
    SessionMgr() = default;
    friend class Singleton<SessionMgr>;
    std::mutex mutex_;
    std::map<std::string, std::shared_ptr<Session>> sessions_;
};