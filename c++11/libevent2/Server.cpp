#include "Server.h"
#include "IOServicePool.h"

namespace Nt
{

int Server::Start()
{
    return IOServicePool::GetInstance().Start();
}

void Server::Stop()
{
    IOServicePool::GetInstance().Stop();
}

}