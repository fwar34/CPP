#pragma once
#include <vector>

class RecvBuffer
{
private:
    std::vector<char> buffer_;
    size_t readPtr_;
    size_t writePtr_;
};