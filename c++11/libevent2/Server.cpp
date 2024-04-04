#include "Server.h"
#include "IOServicePool.h"
#include "LogicThreadPool.h"

namespace Nt
{

int Server::Start()
{
    LogicThreadPool::GetInstance().Start();
    return IOServicePool::GetInstance().Start();
}

void Server::Stop()
{
    IOServicePool::GetInstance().Stop();
}

}