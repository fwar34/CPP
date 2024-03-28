#include "Server.h"
#include "EventBasePool.h"

bool Server::Start()
{
    EventBasePool::GetInstance().Start();
    return true;
}

void Server::Stop()
{

}