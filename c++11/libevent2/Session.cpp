#include "Session.h"
#include "IOThread.h"
#include "ConferenceMgr.h"
#include "LogicThreadPool.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>

namespace Nt
{
thread_local ConferenceMgr confMgr;

bool Session::SendMessage(std::shared_ptr<Message>& message)
{
    static std::string response = "This is reponse ";
    MsgSendNode msg(response.data(), response.size());
    // message.Serial(&msg);

    sendQueue_.Push(msg);
    return true;
}

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

            // std::cout << "test recvheader[" << recvMsg_.Dump() << "]" << std::endl;
            // std::cout << "test recvmesg[" << recvMsg_ << "]" << std::endl;

            // Message 没有拷贝语义，所以调用 Message 的移动构造生成一个 shared_ptr<Message>，
            // 然后传递到闭包，延长 Message 的生命周期
            std::shared_ptr<Message> message = std::make_shared<Message>(std::move(recvMsg_));
            std::cout << "test shared message[" << message << "]" << std::endl;
            // 消息发送到逻辑线程去处理
            AddRef(); // 将 Session 也传递到逻辑线程了，所以引用计数加 1
            LogicThreadPool::GetInstance().Commit(recvMsg_.header_.confId_, [message, this] {
                std::cout << "Logic thread: " << std::this_thread::get_id() 
                    << " confMgr address: " << &confMgr << " " << message << std::endl;
                // std::thread::id tid = std::this_thread::get_id();
                // ConferenceMgr& mgr = ConfMgrs[tid % LOGIC_THREAD_NUM];
                confMgr.DispatchCommand(message, this); // 进行逻辑处理
                ReleaseRef(); // 执行完消息处理减去 session 的引用计数
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

static void SendEventCb(evutil_socket_t sock, short event, void* arg)
{
    Session* session = reinterpret_cast<Session*>(arg);
    session->GetSendQueue().ProcessSend(true);
}

int Session::Start()
{
    // 注册发送队列的可读事件
    struct event *ev = thread_->GetDispatch()->RegisterEvent(
        sendQueue_.EventFd(), EV_READ, SendEventCb, this);
    std::cout << "Session Start queue eventfd: " << sendQueue_.EventFd() << std::endl;
    sendQueue_.SetEvent(ev);
    AddRef(); // 将 Session 注册到 reactor 中去，引用计数加 1

    return 0;
}
int Session::Close()
{
    return 0;
}

}