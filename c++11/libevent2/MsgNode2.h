#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

// 当 data_ 剩余数据小于等于该值是进行 memmove，将剩余数据移动到 data_ 起始
constexpr size_t MEMMOVE_THRESHOLD = 10;
// 接收缓冲区最大 10M
constexpr size_t BUFFER_MAX_LENGTH = 10 * 1024 * 1024;
// 实际存储的数据最大长度，大于这个数值的时候直接报错 buffer_ 满了,
// 小于这个数值并且 IsFull() == true 的时候进行 memmove
constexpr size_t DATA_MAX_LENGTH = BUFFER_MAX_LENGTH - 1024 * 1024;

// 使用可扩展的 buffer，在读写指针相等（buffer 数据读完了）的时候充值读写指针为 0，
// 或者读写指针相减（buffer 内数据剩余长度）小于一个阈值的时候将剩余的数据使用 
// memmove 移动到 buffer 起始位置，并且设置相应的读写指针位置
class MsgNode
{
public:
    MsgNode() : readPos_(0), writePos_(0)
    {
    }
    void Clear()
    {
        readPos_ = 0;
        writePos_ = 0;
    }

    bool IsFull()
    {
        return buffer_.capcatity() >= BUFFER_MAX_LENGTH;
    }

    bool IsEmpty()
    {
        return readPos_ = writePos_;
    }

    void Adjust()
    {

    }

    // 返回 buffer_ 实际数据长度
    size DataSize()
    {
        return writePos_ - readPos_;
    }

    size_t Write(const char* data, size_t dataLen)
    {
        if (IsFull()) {
            if (DataSize() >= DATA_MAX_LENGTH) {
                std::cout << "Recv buffer full!" << std::endl;
                return 0;
            } else { // 进行 memmove
                std::copy(buffer_.begin() + readPos_, buffer.begin() + writePos_, buffer.begin());
            }
        }
    }

    size_t Read(char* data, size_t dataLen);

private:
    std::vecotr<char> buffer_;
    size_t readPos_;
    size_t writePos_;
};