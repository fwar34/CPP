#include "Server.h"
#include "IOServicePool.h"
#include <iostream>
#include <csignal>

static void SignalCb(int signo)
{
	std::cout << "Caught an interrupt signal; exiting cleanly in one second." << std::endl;
    Nt::IOServicePool::GetInstance().Stop();
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