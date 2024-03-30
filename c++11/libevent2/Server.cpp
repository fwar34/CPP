#include "Server.h"
#include "EventBasePool.h"

int Server::Start()
{
    return EventBasePool::GetInstance().Start();
}

void Server::Stop()
{

}