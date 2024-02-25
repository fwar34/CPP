#pragma once
#include <string>

enum RedisStartType
{
    REDIS_START_CLUSTER,
    REDIS_START_SENTINEL
};

enum RedisNodeType
{
    REDIS_NODE_REDIS,
    REDIS_NODE_HTTP,
    REDIS_NODE_THRIFT
};

struct CacheParam
{
    RedisStartType startType;
    RedisNodeType nodeType;
    std::string ipPorts;
    size_t count;
};

class ICacheImpl
{
public:
    virtual int Init(const CacheParam& param) = 0;
    virtual ~ICache() {}
};

class Cache
{
    ICacheImpl* impl_;
public:
    static Cache& Instance();
};