#include "AsyncLog.h"
#include <iostream>
#include <chrono>

int main(int argc, char const *argv[])
{
    AsyncLog::Instance().Write(LOG_LEVEL_INFO, "sdkflk", 44, "lskdjf");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
