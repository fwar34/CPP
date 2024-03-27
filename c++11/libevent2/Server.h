#pragma once
#include "Session.h"
#include <string>
#include <map>

class Server
{
public:
    ~Server()
    {
    }
    static Server& GetInstnce()
    {
        static Server instance;
        return instance;
    }
    bool Start();
    void Stop();

private:
    Server() {}
};