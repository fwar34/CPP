#include "CommonDefine.h"
#include <sstream>

namespace Nt
{
    
std::string Address::Dump() const
{
    std::stringstream ss;
    ss << "ip: " << ip_ << " port: " << port_;
    return ss.str();
}

// bool Address::operator<(const Address& other)
// {
//     if (ip_ < other.ip_) {
//         return true;
//     } else if (ip_ > other.ip_) {
//         return false;
//     } else {
//         return port_ < other.port_;
//     }
// }

bool operator<(const Address& left, const Address& right)
{
    if (left.ip_ < right.ip_) {
        return true;
    } else if (left.ip_ > right.ip_) {
        return false;
    } else {
        return left.port_ < right.port_;
    }
}

} // namespace Nt
