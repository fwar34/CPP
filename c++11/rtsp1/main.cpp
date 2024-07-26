#include "User.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <string_view>

constexpr const unsigned short SERVER_PORT = 554;
constexpr const size_t BUF_LEN = 1024;
constexpr const size_t REQUEST_LINE_SEGMENT_NUM = 3;

enum class RtspErrorCode {
    RTSP_OK = 0,
    RTSP_ERROR_INVALID_REQUEST,
};

// RTSP 协议状态
enum RtspProcessStatus
{
    RTSP_PROCESS_REQUEST = 0,
    RTSP_PROCESS_BODY,
    RTSP_PROCESS_RESPONSE,
    RTSP_PROCESS_ERROR,
};

/*
 * 请求行的三个字段
 */
enum RequestLineStatus
{
    REQUEST_LINE_METHOD = 0,
    REQUEST_LINE_URL,
    REQUEST_LINE_VERSION,
};

enum RtspRequestStatus
{
    REQUEST_LINE = 0,
    REQUEST_HEADERES,
    REQUEST_BODY
};

struct BufSlice
{
    static constexpr size_t strMaxLen = 25;
    const char *buf = nullptr;
    size_t len = 0;;
    bool Equal(const BufSlice &other)
    {
        for (size_t i = 0; i < other.len; ++i) {
            if (buf[i] != other.buf[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator==(const char *other)
    {
        size_t otherLen = strnlen(other, strMaxLen);
        if (len != otherLen) {
            return false;
        }

        for (size_t i = 0; i < len; ++i) {
            if (buf[i] != other[i]) {
                return false;
            }
        }

        return true;
    }
};

/**
 * 查找下一个 \r\n 字符，将 \r\n 之前的 buf 片段放到 BufSlice 中
 */
BufSlice FindNextRN(const char *buf, size_t len)
{
    BufSlice bufSlice = {0};
    bufSlice.buf = buf;
    while (*buf != '\r' || (bufSlice.len < len && *(buf + 1) != '\n')) { // bufSlice.len < len 保证 buf + 1 访问的字符不会数组越界
        buf++;
        bufSlice.len++;
    }

    return bufSlice;
}

struct BufSliceHash
{
    size_t operator()(const BufSlice &slice) const
    {
        return std::hash<const char *>()(slice.buf) ^ (std::hash<size_t>()(slice.len) << 1);
    }
};

struct BufSliceEqual
{
    bool operator()(const BufSlice &lhs, const BufSlice &rhs) const
    {
        return lhs.buf == rhs.buf && lhs.len == rhs.len;
    }
};

void DumpBufSlices(const std::vector<BufSlice> &slices)
{
    std::cout << "====================================== begin" << std::endl;
    for (auto& slice : slices) {
        std::cout << "[" << std::string_view(slice.buf, slice.len) << "] len = " << slice.len << std::endl;
    }
    std::cout << "====================================== end size = " << slices.size() << std::endl;
}

// 将 bufSlice 拆分成 splitter 分割的多个 BufSlice
std::vector<BufSlice> SplitString(const BufSlice &slice, const char splitter)
{
    size_t lastPos = 0;
    std::vector<BufSlice> splitStrings;
    const char *buf = slice.buf;
    size_t i = 0;
    for (; i <= slice.len; ++i) {
        if (buf[i] == splitter || i == slice.len) { // i == slice.len 判断最后一个分片
            size_t len = i - lastPos;
            // std::cout << "found i = " << i << " lastPos = " << lastPos << " buf[i] = " << 
            //     static_cast<uint32_t>(buf[i]) << " len = " << len << " buf = " << buf << std::endl;
            BufSlice bufSlice = {buf + lastPos, len};
            lastPos = i + 1;
            splitStrings.push_back(bufSlice);
        }
    }

    DumpBufSlices(splitStrings);
    return std::move(splitStrings);
}

void ResponseOptions(int client_fd, Rtsp::User &user, bool processRet)
{   
    constexpr size_t RESPONSE_BUF_LEN = 1024;
    char buf[RESPONSE_BUF_LEN] = {0};
    if (processRet) {
        std::sprintf(buf, "%s 200 OK\r\n"
                          "CSeq: %s\r\n"
                          "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, ANNOUNCE, RECORD\r\n"
                          "Server: FTest\r\n"
                          "\r\n",
                     user.GetVersion().c_str(), user.GetCSeq().c_str());
    } else {
        std::sprintf(buf, "%s 201 Err\r\n"
                          "CSeq: %s\r\n"
                          "Server: FTest\r\n"
                          "\r\n", user.GetVersion().c_str(), user.GetCSeq().c_str());
    }
    ssize_t ret = send(client_fd, buf, strlen(buf), 0);
    if (ret == -1) {
        std::cerr << "send options response failed, errno = " << errno << std::endl;
    }
}

bool ProcessOptionInner(const BufSlice &slice, Rtsp::User &user)
{
    auto slices = SplitString(slice, ' '); // 分割每一行字符串为以空格分割的多个字符串
    if (slices.empty()) {
        return false;
    }

    if (slices[0] == "OPTIONS") {
        if (slices.size() != 3) {
            return false;
        }
        user.SetUrl(std::string(slices[1].buf, slices[1].len));
        user.SetVersion(std::string(slices[2].buf, slices[2].len));
        std::cout << "Url: " << std::string_view(slices[1].buf, slices[1].len) << std::endl;
        std::cout << "Version: " << std::string_view(slices[2].buf, slices[2].len) << std::endl;
    } else if (slices[0] == "CSeq:") {
        user.SetCSeq(std::string(slices[1].buf, slices[1].len));
        std::cout << "CSeq: " << std::string_view(slices[1].buf, slices[1].len) << std::endl;
    } else if (slices[0] == "User-Agent:") {
        std::cout << "User-Agent: " << std::string_view(slices[1].buf, slices[1].len) << std::endl;
    } else {
        return false;
    }
    return true;
}

bool ProcessOptions(int client_fd, const char *buf, size_t len, Rtsp::User &user)
{
    bool processRet = true;
    while (len > 0) {
        BufSlice bufSlice = FindNextRN(buf, len); // 查找出以 \r\n 分割的字符串, 分割成了一行一行的字符串
        if (bufSlice.len == 0) {
            break;
        }
        std::cout << "HandleProtocol: [" << std::string_view(bufSlice.buf, bufSlice.len) << "] len = " << bufSlice.len << std::endl;
        if (!ProcessOptionInner(bufSlice, user)) {
            std::cerr << "ProcessOptionInner bufSlice[" << std::string_view(bufSlice.buf, bufSlice.len) << std::endl;
            processRet = false;
            break;
        }

        buf += bufSlice.len + 2; // +2 是跳过 \r\n
        len -= bufSlice.len + 2; // +2 是跳过 \r\n
    }

    ResponseOptions(client_fd, user, processRet);
    std::cout << "------------------------------------------" << std::endl;
    return true;
}

bool ProcessAnnounce(int client_fd, const char *buf, size_t len, Rtsp::User &user)
{
    return true;
}

bool ProcessSetup(int client_fd, const char *buf, size_t len, Rtsp::User &user)
{
    return true;
}

static std::unordered_map<std::string_view, std::function<bool(int client_fd, const char *buf, size_t len, Rtsp::User &user)>> g_rtspCmdTable = {
    {"OPTIONS", ProcessOptions},
    {"ANNOUNCE", ProcessAnnounce},
    {"SETUP", ProcessSetup},
};

BufSlice GetFirstSplitString(const char *buf, size_t len, const char splitter)
{
    BufSlice slice;
    slice.buf = buf;
    size_t i = 0;
    for (; i < len; ++i) {
        if (buf[i] == splitter) {
            slice.len = i;
            return slice;
        }
    }
    slice.len = len;
    return slice;
}

bool HandleRequestHeader(const BufSlice &slice)
{
    return true;
}

bool HandleResponse(const BufSlice &slice)
{
    return true;
}

bool HandleError(const BufSlice &slice)
{
    return true;
}

static Rtsp::User g_user;

// xxxx\r\n
// \r\n
// xxxxxxx
/**
 * 将字符串切分成以 \r\n 分割的 buf 片段，放到 BufSlice 中
 */
RtspErrorCode HandleRtspCmd(int client_fd, const char *buf, size_t len)
{
    // 获取请求行的 method
    BufSlice method = GetFirstSplitString(buf, len, ' ');
    if (method.len == 0) {
        return RtspErrorCode::RTSP_ERROR_INVALID_REQUEST;
    }

    auto it = g_rtspCmdTable.find(std::string_view(method.buf, method.len));
    if (it == g_rtspCmdTable.end()) {
        return RtspErrorCode::RTSP_ERROR_INVALID_REQUEST;
    }

    if (!it->second(client_fd, buf, len, g_user)) {
        return  RtspErrorCode::RTSP_ERROR_INVALID_REQUEST;
    }


    return RtspErrorCode::RTSP_OK;
}

void Process(int client_fd)
{
    char buf[BUF_LEN] = {0};
    for (;;) {
        memset(buf, 0, BUF_LEN);
        ssize_t n = recv(client_fd, buf, BUF_LEN, 0);
        if (n == -1) {
            std::cerr << "recv failed, errno = " << errno << std::endl;
            return;
        } else if (n == 0) {
            std::cout << "peer connect close, close client_fd" << std::endl;
            close(client_fd);
            return;
        }

        HandleRtspCmd(client_fd, buf, n);
    }
}

int main(int argc, char *argv[])
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        std::cerr << "socket failed, errno = " << errno << std::endl;
        return -1;
    }

    int opt = 1;
    int ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (ret == -1) {
        std::cerr << "setsockopt failed, errno = " << errno << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        std::cerr << "bind failed, errno = " << errno << std::endl;
        return -1;
    }

    ret = listen(listen_fd, 0);
    if (ret == -1) {
        std::cerr << "listen failed, errno = " << errno << std::endl;
        return -1;
    }

    struct sockaddr client_addr = {0};
    socklen_t client_addr_len;
    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        std::cerr << "accept failed, errno = " << errno << std::endl;
        return -1;
    }

    Process(client_fd);
    std::cout << "main exit..." << std::endl;

    return 0;
}
