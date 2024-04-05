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
        thd->AddRef(); // reactor 持有 thd 的引用计数加 1
        reactor->SetThread(thd);
        // ioThreads 添加 thd，这里的引用计数不用加 1
        //（因为 thd 创建的时候引用计数就为 1），现在为 2
        ioThreads_.push_back(thd);
        thd->Start(); // 这里执行后为 3
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