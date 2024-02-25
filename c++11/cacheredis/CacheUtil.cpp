#include "CacheUtil.h"

// "192.168.0.123:7000,192.168.0.124:7000,192.168.0.125:7000"
// std::tuple<bool, std::vector<std::string>> SplitString(const std::string& str, const std::string& cutStr)
std::vector<std::string> SplitString(const std::string& str, const std::string& cutStr)
{
    std::vector<std::string> strs;
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = str.find(cutStr);
    while (pos2 != std::string::npos) {
        std::string sub = str.substr(pos1, pos2 - pos1);
        if (!sub.empty()) {
            strs.push_back(sub);
        }
        pos1 = pos2 + cutStr.size();
        pos2 = str.find(cutStr, pos1);
    }

    if (pos1 != str.length()) {
        strs.push_back(str.substr(pos1));
    }
    
    return std::move(strs);
}