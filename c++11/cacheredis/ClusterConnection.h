#pragma once

#include "IConnection.h"

class ClusterConnection : public IConnection
{
public:
    int Execute(const std::vector<std::string>& cmd) override;
};