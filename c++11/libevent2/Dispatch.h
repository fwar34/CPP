#pragma once

class Dispatch
{
public:
    Dispatch()
    {
        evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, sockPair_);
    }
    virtual ~Dispatch() = default;
    virtual bool Stop()
    {
        const char *stopMsg = "stop Reactor!!!";
        ssize_t n = write(sockPair_[1], stopMsg, strlen(stopMsg));
        if (n == -1) {
            std::cout << "Reactor Stop write error" << std::endl;
            return false
        }
        return true;
    }
    virtual void Dispatch() = 0;

protected:
    int sockPair_[2];
};