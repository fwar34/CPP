#include "SessionMgr.h"
#include "Session.h"
#include <iostream>

namespace Nt
{
    
void SessionMgr::AddSession(Session* session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(session->GetSockFd());
    if (it != sessions_.end()) {
        // 已经存在的 session 打印日志，直接替换为新的 session
        std::cout << "Address = " << session->GetAddress().Dump() << " exist!!!" << std::endl;
    }
    sessions_[session->GetSockFd()] = session;
    session->AddRef();
}

void SessionMgr::DelSession(evutil_socket_t sock)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(sock);
    if (it != sessions_.end()) {
        it->second->ReleaseRef();
        sessions_.erase(sock);
    }
}

};