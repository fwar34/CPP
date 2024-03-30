#include "SessionMgr.h"
#include "Session.h"
#include <iostream>

SessionMgr::SessionMgr()
{

}

void SessionMgr::AddSession(Session* session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(session->GetAddress());
    if (it != sessions_.end()) {
        // 已经存在的 session 打印日志，直接替换为新的 session
        std::cout << "Address = " << session->GetAddress().Dump() << " exist!!!" << std::endl;
    }
    sessions_[session->GetAddress()] = session;
    session->AddRef();
}