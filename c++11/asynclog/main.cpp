#include "AsyncLog.h"
#include <chrono>

int main(int argc, char const *argv[])
{
    AsyncLog::AsyncLog::Instance().AsyncWrite(AsyncLog::INFO, "test", 333, 44, "nihc");
    AsyncLog::ELog("hello %s", "world");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
