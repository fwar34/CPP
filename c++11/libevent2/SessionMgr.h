#pragma once
#include <map>
#include <memory>
#include <mutex>

class Address;
class Session;
class SessionMgr
{
public:
    SessionMgr();
    SessionMgr(const SessionMgr&) = delete;
    SessionMgr& operator =(const SessionMgr&) = delete;
    SessionMgr(const SessionMgr&&) = delete;
    SessionMgr& operator =(const SessionMgr&&) = delete;

    static SessionMgr& GetInstance()
    {
        static SessionMgr instance;
        return instance;
    }

    void AddSession(Session* session);

private:
    std::mutex mutex_;
    std::map<Address, Session*> sessions_;
};