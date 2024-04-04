#include "IOServicePool.h"
#include "Reactor.h"
#include "ErrorCode.h"
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
        Reactor* reactor = new Reactor;
        IOThread* thd = new IOThread(reactor);
        reactor->SetThread(thd);
        ioThreads_.push_back(thd); // thd 的引用计数为 1
        thd->Start();
    }

    std::cout << "main thread: " << std::this_thread::get_id() << " wait io thread exit!!!" << std::endl;

    for (auto t : ioThreads_) {
        t->Join();
    }

    return 0;
}

void IOServicePool::Stop()
{
    for (auto t : ioThreads_) {
        t->Stop();
    }
}

};