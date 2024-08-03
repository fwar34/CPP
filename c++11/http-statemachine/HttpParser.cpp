#include "Log.h"
#include "HttpParser.h"
#include <iostream>
#include <cstring>

bool HttpParser::RegisterCallback(HttpRequestState state, CallBack callback)
{
    auto it = callbacks_.find(state);
    if (it != callbacks_.end()) {
        return false;
    }

    callbacks_[state] = callback;
    return true;
}

std::pair<HttpParseCode, std::optional<HttpRequest>> HttpParser::ParseRequest(size_t recvLen)
{
    if (writeIndex_ == BUF_LEN) {
        // 接收buf_已经满了，直接Reset
        Reset();
        Log::Logger()->info("Client recv buffer is full, reset it!");
        return std::pair{HttpParseCode::HTTP_PARSE_CODE_ERROR, std::optional<HttpRequest>{}};
    }

    // 这里不用判断 writeIndex_ + recvLen 会缓冲区溢出，因为外层
    // recv 传递的 len 参数是 BUF_LEN - writeIndex_，即剩余可用缓冲区长度
    writeIndex_ += recvLen;

    // HttpLineState state = HttpLineState::HTTP_LINE_ERROR;
    // while ((state = ParseHttpLine()) == HttpLineState::HTTP_LINE_OK) { // 收到了\r\n分割的完整一行
    //     Execute();
    // }

    HttpLineState lineState = HttpLineState::HTTP_LINE_ERROR;
    do {
        lineState = ParseHttpLine();
        switch (lineState)
        {
        case HttpLineState::HTTP_LINE_OK:
            Execute();
            break;
        case HttpLineState::HTTP_LINE_ERROR:
            Reset();
            return std::pair{HttpParseCode::HTTP_PARSE_CODE_ERROR, std::optional<HttpRequest>()};
        default: // HTTP_LINE_OPEN
            break;
        }
    } while (lineState == HttpLineState::HTTP_LINE_OK)

    // 处理完本次接收的数据，将数据移动到buf_头部
    if (readIndex_ != 0 && writeIndex_ > readIndex_) {
        memmove(buf_, buf_ + readIndex_, writeIndex_ - readIndex_);
        writeIndex_ = writeIndex_ - readIndex_;
        readIndex_ = 0;
    }

    return std::pair{HttpParseCode::HTTP_PARSE_CODE_OPEN, std::optional<HttpRequest>()};
}

HttpParser::HttpLineState HttpParser::ParseHttpLine()
{
    for (; readIndex_ < writeIndex_; ++readIndex_) {
        if (buf[readIndex_] == '\r') {
            if (readIndex_ + 1 == writeIndex_) { // 接收缓冲区已经满了，直接关闭客户端连接
                return HttpLineState::HTTP_LINE_ERROR;
            } else {
                lineContent_.assign(buf_ + readIndex_, writeIndex_ - readIndex_);
                return HttpLineState::HTTP_LINE_OK;
            }
        }
    }
    return HttpLineState::HTTP_LINE_OPEN;
}

void HttpParser::Reset()
{
    readIndex_ = 0;
    writeIndex_ = 0;
}

 HttpParser::Execute()
{
    auto it = callbacks_.find(httpRequestState_);
    if (it == callbacks_.end()) {
        // Log::Logger()->error("Can't find HttpRequestState: {} in callbacks!", httpRequestState_);
        return;
    }

    it->second(lineContent_);
}
