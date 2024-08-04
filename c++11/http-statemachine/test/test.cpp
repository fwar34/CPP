#include "Log.h"
#include "HttpParser.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unistd.h>

constexpr const uint32_t BACKLOG = 1024;

static HttpRequest request;

void HandleHttpRequestLine(const std::string& lineContent)
{
    // 每次处理一个新的请求前，先清理上次的请求内容
    request.Clear();

}

void HandleHttpRequestHeader(const std::string& lineContent)
{

}

void HandleHttpRequestContent(const std::string& lineContent)
{

}

void ProcessHttpRequest(const HttpRequest& request)
{

}

int main(int argc, char* argv[])
{
    Log::InitLog(spdlog::level::info);

    Log::Logger()->info("Server start...");

    uint16_t port = 10086;
    std::string ip = "0.0.0.0";
    if (argc >= 2) {
        ip = argv[1];
    } 
    if (argc >= 3) {
        port = std::stoul(std::string(argv[2]));
    }

    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd == -1) {
        Log::Logger()->error("Create accept socket failed, errno = {}", errno);
        return -1;
    }

    int opt = 1;
    int ret = setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (ret == -1) {
        Log::Logger()->error("setsockopt failed, errno = {}", errno);
        return -1;
    }

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    ret = bind(listenFd, (struct sockaddr*)&address, sizeof(address));
    if (ret == -1) {
        Log::Logger()->error("setsockopt failed, errno = {}", errno);
        return -1;
    }

    ret = listen(listenFd, BACKLOG);
    if (ret == -1) {
        Log::Logger()->error("Listen failed, errno = {}", errno);
        return -1;
    }

    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientFd = accept(listenFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientFd == -1) {
        Log::Logger()->error("Accept client failed, errno = {}", errno);
        return -1;
    }

    HttpParser httpParser;
    httpParser.RegisterCallback(HttpParser::HttpRequestState::HTTP_REQUEST_LINE, HandleHttpRequestLine);
    httpParser.RegisterCallback(HttpParser::HttpRequestState::HTTP_REQUEST_HEADER, HandleHttpRequestHeader);
    httpParser.RegisterCallback(HttpParser::HttpRequestState::HTTP_REQUEST_CONTENT, HandleHttpRequestContent);

    while (true) {
        ssize_t recvLen = recv(clientFd, httpParser.GetWritePtr(), httpParser.GetBufRemainSize(), 0);
        if (recvLen == -1) {
            Log::Logger()->error("Recv clientFd failed, errno = {}", errno);
            continue;
        } else if (recvLen == 0) {
            Log::Logger()->info("Recv clientFd[{}] close", clientFd);
            close(clientFd);
            break;
        }

        auto ret = httpParser.ParseRequest(recvLen);
        if (ret.first == HttpParser::HttpParseCode::HTTP_PARSE_CODE_OPEN) {
            Log::Logger()->error("Recv buffer is full, clientFd[{}], continue recv", clientFd);
            continue;
        } else if (ret.first == HttpParser::HttpParseCode::HTTP_PARSE_CODE_ERROR) {
            Log::Logger()->error("Parse http error, close client fd {}", clientFd);
            close(clientFd);
            break;
        }

        auto request = *ret.second;
        Log::Logger()->info("Get request:[{}]", request.ToString());
        // 获取到一个Http请求
        ProcessHttpRequest(request);
    }

    Log::Logger()->info("Server exit...");

    return 0;
}