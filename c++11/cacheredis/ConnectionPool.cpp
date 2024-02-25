#include "ConnectionPool.h"
#include "ClusterConnection.h"

int ConnectionPool::Init(RedisStartType startType, std::vector<Address> &addresses, size_t count)
{
    for (size_t i = 0; i < count;  ++i) {
        std::shared_ptr<IConnection> conn = std::make_shared<IConnection>(new ClusterConnection)
        conn->Init();
    }
    int ret = conn->Connect();
    conns_.push_back(std::move(conn));
}