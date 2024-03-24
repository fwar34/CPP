#pragma once
#include <map>
#include <string>
#include <memory>
#include <mutex>

class Session;
class SessionMgr
{
    std::mutex mutex_;
    std::map<std::string, std::shared_ptr<Session>> sessions_;
public:
    SessionMgr() = default;
    SessionMgr(const SessionMgr&) = delete;
    SessionMgr& operator=(const SessionMgr&) = delete;
    static SessionMgr& GetInstance()
    {
        static SessionMgr instance;
        return instance;
    }

    bool AddSession(std::shared_ptr<Session> session);
    void RemoveSession(const std::string& id);
    void ClearSessions();
};