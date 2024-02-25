#pragma once
#include "IConnection.h"

class SentinelConnection : public IConnection
{
public:
    int Execute(const std::vector<std::string>& cmd) override;
};