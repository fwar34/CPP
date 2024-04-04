#include "Session.h"
#include "LogicThreadPool.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>

namespace Nt
{

void Session::HandleInput(struct bufferevent *bev)
{
    struct evbuffer* input = bufferevent_get_input(bev);
    // size_t inputLength = evbuffer_get_length(input);
    size_t totalLen = 0;
    MessageHeader& header = recvMsg_.header_;
    while ((totalLen = evbuffer_get_length(input)) > 0) {
        if (!headerParseComplete_) { // 头部没有处理完成
            if (totalLen < MSG_HEADER_LENGTH) {
                // 读入数据不够头部长度，等待下次读入
                return;
            }
            if (evbuffer_remove(input, &header.msgId_, sizeof(uint32_t)) != sizeof(uint32_t)) {
                std::cout << "read header msgid failed!" << std::endl;
                return;
            }
            header.msgId_ = ntohl(header.msgId_);
            // std::cout << "recv msgid: " << header.msgId_ << std::endl;
            if (evbuffer_remove(input, &header.bodyLength_, sizeof(uint32_t)) != sizeof(uint32_t)) {
                std::cout << "read header bodyLength failed!" << std::endl;
                return;
            }
            header.bodyLength_ = ntohl(header.bodyLength_);
            if (evbuffer_remove(input, &header.confId_, sizeof(uint32_t)) != sizeof(uint32_t)) {
                std::cout << "read header confId failed!" << std::endl;
                return;
            }
            header.confId_ = ntohl(header.confId_);
            recvMsg_.body_.resize(header.bodyLength_);
            headerParseComplete_ = true;
            std::cout << "IOThread: " << std::this_thread::get_id() << " recv msg header[" << recvMsg_.Dump() << "]" << std::endl;
        } else { // 头部处理完成
            if (totalLen < recvMsg_.header_.bodyLength_) {
                // body 没有收全，继续等待收全
                return;
            }

            if (evbuffer_remove(input, &recvMsg_.body_[0], recvMsg_.header_.bodyLength_) != recvMsg_.header_.bodyLength_) {
                std::cout << "read body content failed!" << std::endl;
                return;
            }
            headerParseComplete_ = false;

            // Message 没有拷贝语义，所以调用 Message 的移动构造生成一个 shared_ptr<Message>，
            // 然后传递到闭包，延长 Message 的生命周期
            std::shared_ptr<Message> message = std::make_shared<Message>(std::move(recvMsg_));
            // 消息发送到逻辑线程去处理
            LogicThreadPool::GetInstance().Commit(recvMsg_.header_.confId_, [message] {
                std::cout << "Logic thread: " << std::this_thread::get_id() << " " + message->Dump() << std::endl;
            });
        }
    }

}

void Session::HandleOutput()
{

}

void Session::HandleClose(struct bufferevent *bev)
{

}

void Session::HandleTimeout()
{
}
int Session::Start()
{
    return 0;
}
int Session::Accept()
{
    return 0;
}
int Session::Close()
{

    return 0;
}
int Session::Connect()
{

    return 0;
}

}