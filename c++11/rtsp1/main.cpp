#include "User.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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
    char *buf = nullptr;
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
};

struct BufSliceHash
{
    size_t operator()(const BufSlice &slice) const
    {
        return std::hash<char *>()(slice.buf) ^ (std::hash<size_t>()(slice.len) << 1);
    }
};

struct BufSliceEqual
{
    bool operator()(const BufSlice &lhs, const BufSlice &rhs) const
    {
        return lhs.buf == rhs.buf && lhs.len == rhs.len;
    }
};

bool ProcessOptions(const char *buf, const size_t len, User &user)
{

}

// static std::unordered_map<BufSlice, std::function<void(const BufSlice &slice, User& user)>, BufSliceHash, BufSliceEqual> protocol_table;
static std::unordered_map<std::string_view, std::function<void(const char *buf, const size_t len, User &user)>> g_rtspCmdTable = {
    {"OPTIONS", ProcessOptions},
};

struct StateMachineStep
{
    uint32_t step;
    std::function<void(const BufSlice &slice, User &user)> process;
};

bool ProcessURL(const BufSlice &slice, User &user)
{

}

bool ProcessVersion(const BufSlice &slice, User &user)
{
    
}

static uint32_t request_line_step = REQUEST_LINE_METHOD;
static StateMachineStep request_line_machine[REQUEST_LINE_SEGMENT_NUM] = {
    {REQUEST_LINE_METHOD, ProcessMethod},
    {REQUEST_LINE_URL, ProcessURL},
    {REQUEST_LINE_VERSION, ProcessVersion}
};

static uint32_t current_status = RTSP_PROCESS_REQUEST;

/**
 * 查找下一个 \r\n 字符，将 \r\n 之前的 buf 片段放到 BufSlice 中
 */
BufSlice FindNextRN(char *buf, size_t len)
{
    BufSlice bufSlice = {0};
    bufSlice.buf = buf;
    while (*buf != '\r' || (bufSlice.len < len && *(buf + 1) != '\n')) { // bufSlice.len < len 保证 buf + 1 访问的字符不会数组越界
        buf++;
        bufSlice.len++;
    }

    return bufSlice;
}

void DumpBufSlices(const std::vector<BufSlice> &slices)
{
    std::cout << "====================================== begin" << std::endl;
    for (auto& slice : slices) {
        std::cout << "[" << std::string(slice.buf, slice.len) << "] len = " << slice.len << std::endl;
    }
    std::cout << "====================================== end size = " << slices.size() << std::endl;
}

BufSlice GetFirstString(const char *buf, size_t len, const char splitter)
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

// 将 bufSlice 拆分成 splitter 分割的多个 BufSlice
std::vector<BufSlice> SplitString(const BufSlice &slice, const char splitter)
{
    size_t lastPos = 0;
    std::vector<BufSlice> splitStrings;
    char *buf = slice.buf;
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

void HandleRequestLineInner(const BufSlice &slice)
{

}


bool HandleRequestLine(const BufSlice &slice)
{
    auto lines = SplitString(slice, ' ');
    if (lines.empty() || lines.size() != REQUEST_LINE_SEGMENT_NUM) { // method url version
        std::cerr << "SplitString for HandleRequestLine failed" << std::endl;
        return false;
    }

    for (line : lines) {

    }

    return true;
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

void ResetRtspStatus()
{
    current_status = RTSP_PROCESS_REQUEST;
}

// void HandleRtspCmds(BufSlice& slice)
// {
//     switch (current_status) 
//     {
//         case RTSP_PROCESS_REQUEST:
//             if (HandleRequestLine(slice)) {
//                 current_status = RTSP_PROCESS_BODY;
//             } else {
//                 ResetRtspStatus();
//             }
//             break;
//         case RTSP_PROCESS_BODY:
//             if (HandleRequestHeader(slice)) {
//                 current_status = RTSP_PROCESS_RESPONSE;
//             } else {
//                 ResetRtspStatus();
//             }
//             break;
//         case RTSP_PROCESS_RESPONSE:
//             HandleResponse(slice);
//             current_status = RTSP_PROCESS_REQUEST;
//             break;
//         default:
//             HandleError(slice);
//             current_status = RTSP_PROCESS_REQUEST;
//     }
// }

BufSlice GetRequestLineMethod(const char *buf, size_t len)
{
    BufSlice method = 
}

static User g_user;

// xxxx\r\n
// \r\n
// xxxxxxx
/**
 * 将字符串切分成以 \r\n 分割的 buf 片段，放到 BufSlice 中
 */
RtspErrorCode HandleRtspCmd(const char *buf, const size_t len)
{
    // 获取请求行的 method
    BufSlice method = GetFirstString(buf, len, ' ');
    if (method.len == 0) {
        return RTSP_ERROR_INVALID_REQUEST;
    }

    auto it = g_rtspCmdTable.find(std::string_view(method.buf, method.len));
    if (it == g_rtspCmdTable.end()) {
        return RTSP_ERROR_INVALID_REQUEST;
    }

    if (!it->second(buf, len, g_user)) {
        return RTSP_ERROR_INVALID_REQUEST;
    }

    // while (len > 0) {
    //     BufSlice bufSlice = FindNextRN(buf, len); // 查找出以 \r\n 分割的字符串
    //     if (bufSlice.len == 0) {
    //         break;
    //     }
    //     std::cout << "HandleProtocol: [" << std::string(bufSlice.buf, bufSlice.len) << "] len = " << bufSlice.len << std::endl;
    //     // std::cout << "HandleProtocol: [" << DumpBufSlice(bufSlice) << "] len = " << bufSlice.len << std::endl;
    //     HandleBufSlice(bufSlice);

    //     buf += bufSlice.len + 2; // +2 是跳过 \r\n
    //     len -= bufSlice.len + 2; // +2 是跳过 \r\n
    // }
    std::cout << "------------------------------------------" << std::endl;

    return 0;
}

void Process(int client_fd)
{
    char buf[BUF_LEN] = {0};
    for (;;) {
        memset(buf, 0, BUF_LEN);
        size_t n = recv(client_fd, buf, BUF_LEN, 0);
        if (n == -1) {
            std::cerr << "recv failed, errno = " << errno << std::endl;
            return;
        }

        HandleRtspCmd(buf, n);
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

    return 0;
}
