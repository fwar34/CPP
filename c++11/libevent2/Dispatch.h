#pragma once
#include <event2/event.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

class Dispatch
{
public:
    Dispatch()
    {
        evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, sockPair_);
    }
    virtual ~Dispatch() = default;
    virtual int Start() = 0;
    virtual void DispatchEvents() = 0;
    virtual bool Stop()
    {
        const char *stopMsg = "stop Reactor!!!";
        ssize_t n = write(sockPair_[1], stopMsg, strlen(stopMsg));
        if (n == -1) {
            std::cout << "Reactor Stop write error" << std::endl;
            return false;
        }
        return true;
    }

protected:
    int sockPair_[2]; // 负责结束 Dispatch
};