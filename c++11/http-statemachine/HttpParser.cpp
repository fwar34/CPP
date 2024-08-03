#include "Log.h"
#include "HttpParser.h"
#include <iostream>

bool HttpParser::RegisterCallback(HttpRequestState state, CallBack callback)
{
    auto it = callbacks_.find(state);
    if (it != callbacks_.end()) {
        return false;
    }

    callbacks_[state] = callback;
    return true;
}

std::optional<HttpRequest> HttpParser::ParseRequest(size_t recvLen)
{
    if (writeIndex_ == BUF_LEN) {
        // 接收buf_已经满了，直接Reset
        Reset();
        Log::Logger()->info("Client recv buffer is full, reset it!");
        return std::optional<HttpRequest>{};
    }

    // 这里不用判断 writeIndex_ + recvLen 会缓冲区溢出，因为外层
    // recv 传递的 len 参数是 BUF_LEN - writeIndex_，即剩余可用缓冲区长度
    writeIndex_ += recvLen;

    while (ParseHttpLine() == HttpLineState::HTTP_LINE_OK) { // 收到了\r\n分割的完整一行
        Execute();
    }

}

HttpParser::HttpLineState HttpParser::ParseHttpLine()
{
    for (; readIndex_ < writeIndex_; ++readIndex_) {
        if (buf_[readIndex_] == '\r' && readIndex_ + 1 < writeIndex_ && buf_[readIndex_ + 1] == '\n') {
            lineContent_.assign(buf_ + readIndex_, writeIndex_ - readIndex_);
        }
    }
    return HttpLineState::HTTP_LINE_OPEN;
}

void HttpParser::Reset()
{
    readIndex_ = 0;
    writeIndex_ = 0;
}

void HttpParser::Execute()
{
    auto it = callbacks_.find(httpRequestState_);
    if (it == callbacks_.end()) {
        // Log::Logger()->error("Can't find HttpRequestState: {} in callbacks!", httpRequestState_);
        return;
    }

    it->second(lineContent_);
}
