#include "HttpParser.h"

int HttpParser::ParseContent(size_t recvLen)
{
    for (;;) {
        if (!ParseHttpLine(recvLen)) {
            // 本次 recv 消息处理完成
            break;
        }

        switch (httpLineState_)
        {
        case HTTP_LINE_OK:
            break;
        case HTTP_LINE_OPEN:
            break;
        case HTTP_LINE_BAD:
            break;
        default:
        }
    }
}

bool HttpParser::ParseHttpLine(size_t recvLen)
{
    // if (writeIndex_ + recvLen == BUF_LEN) { // 接收缓冲区满
    //     if (buf[writeIndex_ + recvLen - 1] != '\n') { // 满了的时候也没有解析出完整的一个http行，返回错误
    //         Reset();
    //         return HTTP_LINE_BAD;
    //     }
    // }

    while (httpLineState_ != HTTP_LINE_OK && ) {
        // 这里不用判断 writeIndex_ + recvLen 会缓冲区溢出，因为外层
        // recv 传递的 len 参数是 BUF_LEN - writeIndex_，即剩余可用缓冲区长度
        if (buf_[writeIndex_ + recvLen] == '\r') {

        }
    }
    return HttpLineState();
}

void HttpParser::Reset()
{
    writeIndex_ = 0;
}
