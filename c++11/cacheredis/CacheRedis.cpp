#include "CacheRedis.h"
#include "ConnectionPool.h"
#include "CacheUtil.h"
#include <vector>
#include <string>

// "192.168.0.123:7000,192.168.0.124:7000,192.168.0.125:7000"
int CacheRedis::Init(const CacheParam& param)
{
    std::vector<std::string> ipPorts = SplitString(param.ipPorts, ",");
    std::vector<std::string> ipPort;
    std::vector<Address> addresses;
    for (auto& str : ipPorts) {
        ipPort = SplitString(str, ":");
        addresses.push_back(Address{ipPort[0], std::stoi(ipPort[1])});
    }
    return pool_.Init(param.startType, addresses, param.count);
}