#pragma once
#include <string>
#include <cstdint>

namespace RTSP
{
class User
{
public:
    User() : rtpPort_(0), rtcpPort_(0)
    {
    }

    void SetUrl(const std::string &url)
    {
        url_ = url;
    }

    void SetVersion(const std::string &version)
    {
        version_ = version;
    }


private:
    std::string url_;
    std::string version_;
    uint16_t rtpPort_;
    uint16_t rtcpPort_;
};
}
