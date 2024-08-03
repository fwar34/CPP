#pragma once
#include "HttpRequest.h"
#include <string>
#include <functional>
#include <optional>

class HttpParser
{
public:
    using CallBack = std::function<void(const std::string&)>;
    constexpr static const size_t BUF_LEN = 1024 * 1024; // 接收缓冲区1M
    // 子状态机
    enum class HttpLineState
    {
        HTTP_LINE_OK,   // 接收到\r\n分割的完整一行
        HTTP_LINE_OPEN, // 未接收到\r\n分割的完整一行
        HTTP_LINE_BAD   // 接收出错
    };

    // 主状态机
    enum class HttpRequestState
    {
        HTTP_REQUEST_LINE,     // 处理请求行
        HTTP_REQUEST_HEADER,   // 处理请求头
        HTTP_REQUEST_CONTENT,  // 处理请求体
    };

    enum class HttpParseCode
    {
        HTTP_PARSE_CODE_ERROR,
        HTTP_PARSE_CODE_GET_REQUEST,
    };

    bool RegisterCallback(HttpRequestState state, CallBack callback);

    std::optional<HttpRequest> ParseRequest(size_t recvLen);

    inline size_t GetWriteIndex()
    {
        return writeIndex_;
    }

    inline size_t GetReadIndex()
    {
        return readIndex_;
    }

    inline size_t GetBufRemainSize()
    {
        return BUF_LEN - writeIndex_;
    }

    inline char* GetBuf()
    {
        return buf_;
    }

    inline char* GetWritePtr()
    {
        return buf_ + writeIndex_;
    }

    inline char* GetReadPtr()
    {
        return buf_ + readIndex_;
    }

    inline HttpRequestState GetHttpRequestState()
    {
        return httpRequestState_;
    }

    inline void SetHttpRequestState(HttpRequestState state)
    {
        httpRequestState_ = state;
    }

private:
    HttpLineState ParseHttpLine();
    void Reset();
    void Execute();

    std::unordered_map<HttpRequestState, CallBack> callbacks_;
    HttpRequestState httpRequestState_ = HttpRequestState::HTTP_REQUEST_LINE; // http 处理状态，初始先处理请求行
    std::string lineContent_;
    size_t readIndex_ = 0;
    size_t writeIndex_ = 0;
    char buf_[BUF_LEN] = {0};
};