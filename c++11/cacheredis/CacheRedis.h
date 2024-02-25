#pragma once

#include "Cache.h"

class ConnectionPool;
class CacheRedis : public ICacheImpl
{
    ConnectionPool pool_;
public:
    int Init(const CacheParam& param) override;
};