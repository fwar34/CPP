#pragma once
#include <string>
#include <cstdint>

namespace Rtsp
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

    void SetCSeq(const std::string &cseq)
    {
        cseq_ = cseq;
    }

    std::string GetUrl() const
    {
        return url_;
    }

    std::string GetVersion() const
    {
        return version_;
    }

    std::string GetCSeq() const
    {
        return cseq_;
    }

private:
    std::string url_;
    std::string version_;
    std::string cseq_;
    uint16_t rtpPort_;
    uint16_t rtcpPort_;
};
}
