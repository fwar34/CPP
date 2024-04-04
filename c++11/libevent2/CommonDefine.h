#pragma once
#include <string>
#include <iostream>

namespace Nt
{

constexpr size_t LOGIC_THREAD_NUM = 4;

struct Address
{
    std::string ip_;
    unsigned short port_;
    std::string Dump() const;
    // bool operator<(const Address& other);
};

// 返回 ostream 引用，支持链式操作
std::ostream& operator<<(std::ostream& os, const Address& address)
{
    os << "[ip: " << address.ip_ << " port: " << address.port_ << "]";
    return os;
}

bool operator<(const Address& left, const Address& right);

}