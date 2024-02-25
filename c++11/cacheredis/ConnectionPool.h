#pragma once
#include "Cache.h"
#include "IConnectionPool.h"
#include <vector>
#include <memory>

class IConnection;
class ConnectionPool
{
    std::vector<std::shared_ptr<IConnection>> conns_;
public:
    ConnectionPool();
    int Init(RedisStartType startType, std::vector<Address>& addresses, size_t count) override;
    void UnInit() override;
};