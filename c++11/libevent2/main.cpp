#include <iostream>
#include "Server.h"

static void SignalCb(int signo)
{
	std::cout << "Caught an interrupt signal; exiting cleanly in one second." << std::endl;
    IOServicePool::GetInstance().Stop();
}

int main()
{
    signal(SIGINT, SignalCb);
    signal(SIGTERM, SignalCb);

    int32_t ret = Server::GetInstance().Start();
    if (ret != 0) {
        std::cout << "Server start ret = " << ret << std::endl;
        return -1;
    }

    return 0;
}