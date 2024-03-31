#include "IOServicePool.h"
#include "Reactor.h"
#include "CommonDefine.h"
#include "Session.h"
#include "SessionMgr.h"
#include <csignal>
#include <iostream>
#include <memory>

namespace Nt
{

int IOServicePool::Start()
{
    for (size_t i = 0; i < threadNum_; ++i) {
        // ioThreads_.emplace_back(new Reactor());
        // ioThreads_[i].Start();
    }

    return 0;
}

void IOServicePool::Stop()
{
    for (auto& t : ioThreads_) {
        t.Stop();
    }
}

};