#pragma once
#include "Session.h"
#include <string>
#include <map>
#include <cstdint>

namespace Nt
{

class Server
{
public:
    ~Server()
    {
    }
    static Server& GetInstance()
    {
        static Server instance;
        return instance;
    }
    int Start();
    void Stop();

private:
    Server() {}
};

}