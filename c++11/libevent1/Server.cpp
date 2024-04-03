#include "Server.h"
#include "Session.h"
#include "ThreadPool.h"
#include "SessionMgr.h"
#include <functional>
#include <iostream>
#include <iomanip>
#include <cstring>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>
#include <csignal>

constexpr size_t headerLength = sizeof(MessageHeader);

struct Context
{
    std::shared_ptr<Session> session;
};

bool DecodeHeader(const char* buf, size_t len)
{
    return true;
}

static void PrintBuf(const char* buf, size_t len)
{
    std::stringstream ss;
    std::string fmt = "0x";
    for (size_t i = 0; i < len; ++i) {
        std::string hexStr;
        ss << std::hex << std::setw(2) << std::setfill('0') << int(buf[i]) << std::endl;
        ss >> hexStr;
        fmt += hexStr;
    }
    std::cout << "recv rawdata is: " << fmt << std::endl;
}

// 原文链接：https://blog.csdn.net/u013229693/article/details/52168874
// 结论：
// 　基于文件描述符的读回调确实是LT触发的。bufferevent底层也是LT触发的，bufferevent底层会尽量读去来自网络的数据，放到input evbuffer中。
// 　但基于bufferevent的读回调则类似ET触发，用户绑定在bufferevent上触发的读回调如果没有一次读走bufferevent下input evbuffer中的数据，
//   则除非再次有网络数据到达，否则不会触发读回调。
void ReadCb(struct bufferevent *bev, void *ctx)
{
    static uint32_t times = 0;
    struct event_base* evbase = reinterpret_cast<struct event_base*>(ctx);
    (void)evbase;
    struct evbuffer* input = bufferevent_get_input(bev);
    uint16_t totalLen = evbuffer_get_length(input);
    std::cout << "ReadCb count = " << ++times << " input length = " << totalLen << std::endl;

    static char buf[UINT16_MAX] = {0};
    static size_t curPos = 0;
    // 读出所有的包
    size_t readLen = bufferevent_read(bev, buf + curPos, UINT16_MAX);
    curPos += readLen;
    if (curPos < headerLength) {
        return;
    }

#if 0
    char buf[1024] = {0};
    uint16_t readLen = bufferevent_read(bev, buf, 1024);
    PrintBuf(buf, readLen);
    std::cout << "readLen = " << readLen << std::endl;
#endif
    
#if 0
    bufferevent_read(bev, buf, 3);
    len = evbuffer_get_length(input);
    std::cout << "after read 3, ReadCb count = " << ++times << " input length = " << len << std::endl;
#endif
    // 切包逻辑: 每取出一个完整包，将剩余的内存向前平移
    // 每个包组成: 头部2字节(内容包的长度) + body
    while (curPos > headerLength) {
        MessageHeader* header = reinterpret_cast<MessageHeader*>(buf);
        // body 没有收全
        if (curPos - headerLength < header->length) {
            return;
        }

        uint16_t msgTotalLen = headerLength + header->length;
        char* msgBuf = new char[msgTotalLen];
        memcpy(msgBuf, buf, msgTotalLen);
        memmove(buf, buf + msgTotalLen, curPos - msgTotalLen);
        curPos -= msgTotalLen;

        std::shared_ptr<char[]> msg(msgBuf, [](char* p) { 
            delete[] p; 
            std::cout << "delete[] p address = " << p << std::endl; 
        });
        ThreadPool::GetInstance().Commit([msg, bev]() {
            std::cout << "msg shared_ptr ref count = " << msg.use_count() << " in thread: " << std::this_thread::get_id() << std::endl;
            Message* message = reinterpret_cast<Message*>(msg.get());
            int ret = bufferevent_write(bev, reinterpret_cast<char*>(message), headerLength + message->header_.length);
            std::cout << "reply to client ret = " << ret << " in thread: " << std::this_thread::get_id() << std::endl;
        });
    }
}

// 在 Session 中使用 buffer 来拷贝接收的数据进行切包
static void ReadCb2(struct bufferevent* bev, void* arg)
{
    Context* ctx = reinterpret_cast<Context*>(arg);
    evbuffer* input = bufferevent_get_input(bev);
    MessageHeader& header = ctx->session->Header();
    // 判断接收的 evbuffer 长度大于消息头的长度就处理
    while (evbuffer_get_length(input) > 0) {
        if (!ctx->session->ParseHeaderComplete()) {
            // 头部没有处理完成并且接收的数据大于头部长度则继续处理头部，否则继续等待头部接收完成
            if (evbuffer_get_length(input) < HEADER_LENGTH) {
                break;
            }

            size_t lens = evbuffer_get_length(input);

            // 头部接收完成，则拷贝头部字段到 session 对应字段
            // 1. 处理 type
            if (evbuffer_remove(input, &header.type, TYPE_LENGTH) != TYPE_LENGTH) {
                // 长度错误，没法处理，或者直接断开连接
                std::cout << "read type error!" << std::endl;
                break;
            }
            header.type = ntohl(header.type);

            // 2. 处理 length
            if (evbuffer_remove(input, &header.length, LENGTH_LENGTH) != LENGTH_LENGTH) {
                std::cout << "read length error!" << std::endl;
                break;
            }
            header.length = ntohs(header.length);
            // 处理完成头部则设置头部完成标志为 true，为下来处理 body 准备
            ctx->session->ParseHeaderComplete(true);
            // 分配 body 内存，为存储 body 做准备，在逻辑线程处理完成消息的时候 delete 分配的 buffer
            ctx->session->AllocBody(header.length);
        } else {
            // 头部已经处理完成，则处理 body，已经接收的数据长度（包含了上次接收长度 curPos 和本次 evbuffer 长度）不足 body 长度则继续等待 body 收全
            if (evbuffer_get_length(input) < header.length) {
                // 长度错误，没法处理，或者直接断开连接
                std::cout << "read type error!" << std::endl;
                break;
            }

            // body 已经收全，则处理 body，将 evbuffer 中的数据拷贝到 session 的 buffer_ 中
            if (evbuffer_remove(input, ctx->session->Buffer(), header.length) != header.length) {
                // 长度错误，没法处理，或者直接断开连接
                std::cout << "read type error!" << std::endl;
                break;
            }

            // 构造消息发送到逻辑线程去处理
            std::shared_ptr<Message> message = std::make_shared<Message>(header, ctx->session->Buffer());
            // 闭包捕获了 message，message 的引用计数加 1，这样在逻辑线程处理的时候不会有 Message 已经被析构的情况发生
            std::function<void()> task = [message, bev]() {
                std::cout << "in logic thread => message use_count: " << message.use_count() << std::endl;
                std::cout << "logic thread id: " << std::this_thread::get_id() << " recv msg: " << message->Dump() << std::endl;
                // 回复客户端
                int ret = bufferevent_write(bev, &message->header_, sizeof(message->header_));
                std::cout << "reply header to client ret = " << ret << std::endl;
                bufferevent_write(bev, message->buf_, message->header_.length);
                std::cout << "reply body to client ret = " << ret << std::endl;
            };
            std::cout << "in io thread => message use_count: " << message.use_count() << std::endl;
            ThreadPool::GetInstance().Commit(std::move(task));
            
            // 重新开始处理头部
            ctx->session->ParseHeaderComplete(false);
            ctx->session->BufferReset();
        }
    }
}

static void write_cb(struct bufferevent* bev, void* ctx)
{
    static int count = 0;
    std::cout << "count = " << count++ << std::endl;
}

static void event_cb(struct bufferevent *bev, short what, void *ctx)
{

}

static void signal_cb(evutil_socket_t sig, short events, void* user_data)
{
	struct event_base *base = (event_base*)user_data;
	struct timeval delay = { 2, 0 };

	std::cout << "Caught an interrupt signal; exiting cleanly in two seconds." << std::endl;

	event_base_loopexit(base, &delay);
}

void AcceptCb(struct evconnlistener* evlistener, evutil_socket_t sock, struct sockaddr* addr, int socklen, void* user_data)
{
    struct event_base *evbase = reinterpret_cast<struct event_base *>(user_data);
    struct bufferevent* bufevent = bufferevent_socket_new(evbase, sock, BEV_OPT_CLOSE_ON_FREE);
    if (!bufevent) {
        std::cout << "Could not create bufferevent!" << std::endl;
        event_base_loopbreak(evbase);
        return;
    }

#if 0 
    // ReadCb 使用 memmove 来进行切包
    bufferevent_setcb(bufevent, ReadCb, write_cb, event_cb, (void*)evbase);
#endif

    Context* ctx = new Context;
    Address address = {"", 0};
    ctx->session = std::make_shared<Session>(bufevent, address);
    SessionMgr::GetInstance().AddSession(ctx->session);
    bufferevent_setcb(bufevent, ReadCb2, write_cb, event_cb, ctx);

    bufferevent_enable(bufevent, EV_READ);
    bufferevent_enable(bufevent, EV_WRITE);
}

int Server::Start()
{
    evbase_ = event_base_new();

    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(18888);

    struct evconnlistener* evlistener = evconnlistener_new_bind(
        evbase_, AcceptCb, (void*)evbase_, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
        (struct sockaddr*)&sin, sizeof(sin));
    if (!evlistener) {
        std::cout << "Could not create a listener!" << std::endl;
        return -1;
    }

    struct event* sig_event = evsignal_new(evbase_, SIGINT, signal_cb, (void*)evbase_);
    if (!sig_event || event_add(sig_event, nullptr) < 0) {
        std::cout << "Could not create a signal event!" << std::endl;
        return -2;
    }

    std::cout << "main thread: " << std::this_thread::get_id() << std::endl;
    ThreadPool::GetInstance().Start();

    event_base_dispatch(evbase_);
    evconnlistener_free(evlistener);
    event_free(sig_event);
    event_base_free(evbase_);

    std::cout << "server shutdown!!" << std::endl;

    return 0;
}

void Server::Stop()
{
    event_base_loopbreak(evbase_);
    ThreadPool::GetInstance().Stop();
    SessionMgr::GetInstance().ClearSessions();
}