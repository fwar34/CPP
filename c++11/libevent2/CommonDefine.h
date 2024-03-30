#pragma once
#include <string>

struct Address
{
    std::string ip_;
    unsigned short port_;
    std::string Dump() const;
    // bool operator<(const Address& other);
};

bool operator<(const Address& left, const Address& right);