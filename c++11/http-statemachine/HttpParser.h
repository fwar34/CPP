#pragma once
#include "HttpParserImpl.h"
// #include <string>

class HttpParser
{
public:
    enum class HttpLineState
    {
        HTTP_LINE_OK,
        HTTP_LINE_OPEN,
        HTTP_LINE_BAD
    };

    enum class HttpRequestState
    {
        HTTP_REQUEST_LINE,
        HTTP_REQUEST_HEADER,
        HTTP_REQUEST_CONTENT,
    };

    int ParseContent(size_t recvLen);

    inline size_t GetWriteIndex()
    {
        return writeIndex_;
    }

    inline size_t GetBufRemainSize()
    {
        return BUF_LEN - writeIndex_;
    }

    inline char* GetBuf()
    {
        return buf_;
    }

    char* GetWritePtr()
    {
        return buf_ + writeIndex_;
    }

private:
    bool ParseHttpLine(size_t recvLen);
    void Reset();

    HttpLineState httpLineState_; // http 的一行是否接收完成状态
    HttpRequestState httpRequestState_; // http 处理状态
    size_t writeIndex_ = 0;
    size_t readIndex_ = 0;
    char buf_[BUF_LEN] = {0};
};