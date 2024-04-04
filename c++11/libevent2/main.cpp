#include "Server.h"
#include "IOServicePool.h"
#include "LogicThreadPool.h"
#include <iostream>
#include <csignal>
#include <thread>

static void SignalCb(int signo)
{
	std::cout << "Caught an interrupt signal; exiting "
        "cleanly in one second. main thread id: " 
        << std::this_thread::get_id() << std::endl;
    Nt::IOServicePool::GetInstance().Stop();
    LogicThreadPool::GetInstance().Stop();
}

int main()
{
    signal(SIGINT, SignalCb);
    signal(SIGTERM, SignalCb);

    int32_t ret = Nt::Server::GetInstance().Start();
    if (ret != 0) {
        std::cout << "Server start ret = " << ret << std::endl;
        return -1;
    }

    return 0;
}