#pragma once
#include <event2/util.h>

class Server
{
    struct event_base* evbase_ = nullptr;
public:
    Server() = default;
    Server(const Server&) = delete;
    Server& operator =(const Server&) = delete;

    int Start();
    void Stop();
    static Server& GetInstance()
    {
        static Server instance;
        return instance;
    }

private:
    // void AcceptCb(struct evconnlistener* evlistener, evutil_socket_t sock, struct sockaddr* addr, int socklen, void* user_data);
    // void ReadCb(struct bufferevent *bev, void *ctx);
    // void WriteCb();
};