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

std::pair<HttpParser::HttpParseCode, std::optional<HttpRequest>> HttpParser::ParseRequest(size_t recvLen)
{
    if (writeIndex_ == BUF_LEN) {
        // 接收buf_已经满了，直接Clear
        Clear();
        Log::Logger()->info("Client recv buffer is full, clear it!");
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
        case HttpLineState::HTTP_LINE_OK: // 收到了\r\n分割的完整一行
            // auto ret = Execute();
            // if (ret) {
            //     return GenRequest();
            // }
            break;
        case HttpLineState::HTTP_LINE_ERROR: // 解析出错
            Clear();
            return std::pair{HttpParseCode::HTTP_PARSE_CODE_ERROR, std::optional<HttpRequest>()};
        default: // HTTP_LINE_OPEN, 未收到\r\n分割的完整一行，继续接收
            break;
        }
    } while (lineState == HttpLineState::HTTP_LINE_OK);

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
    size_t readIndexOrigin = readIndex_;
    for (; readIndexOrigin < writeIndex_; ++readIndex_) {
        if (buf_[readIndexOrigin] == '\r') {
            if (readIndexOrigin + 1 >= writeIndex_) { // 接收缓冲区已经满了，直接关闭客户端连接
                return HttpLineState::HTTP_LINE_ERROR;
            } else if (buf_[readIndex_ + 1] == '\n') {
                lineContent_.assign(buf_ + readIndexOrigin, writeIndex_ - readIndexOrigin);
                readIndex_ = readIndexOrigin + 2; // 读取一个完整的行后更新readIndex索引
                return HttpLineState::HTTP_LINE_OK;
            }
        }
    }
    return HttpLineState::HTTP_LINE_OPEN;
}

void HttpParser::Clear()
{
    readIndex_ = 0;
    writeIndex_ = 0;
}

std::optional<HttpRequest> HttpParser::Execute()
{
    auto it = callbacks_.find(httpRequestState_);
    if (it == callbacks_.end()) {
        // Log::Logger()->error("Can't find HttpRequestState: {} in callbacks!", httpRequestState_);
        //return;
    }

    it->second(lineContent_);
}
