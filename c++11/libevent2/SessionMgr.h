#pragma once
#include "CommonDefine.h"
#include <map>
#include <memory>
#include <mutex>
#include <event2/util.h>

namespace Nt
{

class Session;
class SessionMgr
{
public:
    SessionMgr() = default;
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
    void DelSession(evutil_socket_t sock);

private:
    std::mutex mutex_;
    std::map<evutil_socket_t, Session*> sessions_;
};

};