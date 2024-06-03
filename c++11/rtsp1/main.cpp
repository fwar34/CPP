#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

constexpr const unsigned short SERVER_PORT = 554;
constexpr size_t BUF_LEN = 1024;

// RTSP 协议状态
enum RTSP_PROCESS_STATUS
{
    RTSP_PROCESS_REQUEST = 0,
    RTSP_PROCESS_BODY,
    RTSP_PROCESS_RESPONSE,
    RTSP_PROCESS_ERROR,
};

struct BufSlice
{
    char *buf;
    size_t len;
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

static std::unordered_map<BufSlice, std::function<void()>> protocol_table;
void RegisterProtocolProcess()
{
    // TODO
    protocol_table.insert(std::make_pair());
}

typedef int (*RtspProcess)(const BufSlice &slice);
struct RtspStatus
{
    int32_t status;
    RtspProcess process;
};

static uint32_t current_status = RTSP_PROCESS_REQUEST;

void BuildProcessFunctions()
{
}

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

// 将 bufSlice 拆分成 splitter 分割的多个 BufSlice
std::vector<BufSlice> SplitString(const BufSlice &slice, char splitter)
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

bool HandleRequest(const BufSlice &slice)
{
    auto lines = SplitString(slice, ' ');
    if (lines.empty()) {
        std::cerr << "SplitString for HandleRequest failed" << std::endl;
        return false;
    }

    return true;
}

bool HandleBody(const BufSlice &slice)
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

void HandleBufSlice(BufSlice& slice)
{
    switch (current_status) 
    {
        case RTSP_PROCESS_REQUEST:
            if (HandleRequest(slice)) {
                current_status = RTSP_PROCESS_BODY;
            } else {
                ResetRtspStatus();
            }
            break;
        case RTSP_PROCESS_BODY:
            if (HandleBody(slice)) {
                current_status = RTSP_PROCESS_RESPONSE;
            } else {
                ResetRtspStatus();
            }
            break;
        case RTSP_PROCESS_RESPONSE:
            HandleResponse(slice);
            current_status = RTSP_PROCESS_REQUEST;
            break;
        default:
            HandleError(slice);
            current_status = RTSP_PROCESS_REQUEST;
    }
}

// xxxx\r\n
// \r\n
// xxxxxxx
/**
 * 将字符串切分成以 \r\n 分割的 buf 片段，放到 BufSlice 中
 */
int HandleProtocol(char *buf, size_t len)
{
    while (len > 0) {
        BufSlice bufSlice = FindNextRN(buf, len); // 查找出以 \r\n 分割的字符串
        if (bufSlice.len == 0) {
            break;
        }
        std::cout << "HandleProtocol: [" << std::string(bufSlice.buf, bufSlice.len) << "] len = " << bufSlice.len << std::endl;
        // std::cout << "HandleProtocol: [" << DumpBufSlice(bufSlice) << "] len = " << bufSlice.len << std::endl;
        HandleBufSlice(bufSlice);

        buf += bufSlice.len + 2;
        len -= bufSlice.len + 2;
    }
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

        HandleProtocol(buf, n);
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
