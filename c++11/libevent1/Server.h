#pragma once
#include "Singleton.h"
#include <event2/util.h>

class Server : public Singleton<Server>
{
    friend class Singleton<Server>;
public:
    ~Server() = default;
    Server(const Server&) = delete;
    Server& operator =(const Server&) = delete;

    int Start();
    void Stop();

private:
    Server() = default;
    struct event_base* evbase_ = nullptr;
};