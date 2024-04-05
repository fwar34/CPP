#pragma once
// 在自己实现的 reactor 中使用，libevent 已经有了接收缓冲区
#include <iostream>
#include <vector>
#include <algorithm>

constexpr size_t BUFFER_INIT_LENGTH = 1024 * 1024; // buffer_ 初始 1M
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
class MsgRecvNode
{
public:
    MsgRecvNode() : buffer_(BUFFER_INIT_LENGTH), readPos_(0), writePos_(0)
    {
    }
    void Clear()
    {
        readPos_ = 0;
        writePos_ = 0;
    }

    bool IsFull()
    {
        return buffer_.capacity() >= BUFFER_MAX_LENGTH;
    }

    bool IsEmpty()
    {
        return readPos_ = writePos_;
    }

    // 返回 buffer_ 实际数据长度
    size_t DataSize()
    {
        return writePos_ - readPos_;
    }

    // buffer_ 尾部的可用长度
    size_t FreeSize()
    {
        return buffer_.capacity() - writePos_;
    }

    void MemMove()
    {
        std::copy(buffer_.begin() + readPos_, buffer_.begin() + writePos_, buffer_.begin());
        writePos_ -= readPos_;
        readPos_ = 0;
    }

    size_t Write(const char* data, size_t dataLen)
    {
        if (IsFull()) {
            if (DataSize() >= DATA_MAX_LENGTH) { // 实际接收的数据已经超过限制，直接返回 0
                std::cout << "Recv buffer full!" << std::endl;
                return 0;
            } else {
                // 进行 memmove，将数据移动到 buffer_ 的开头，腾出空间进行数据接收
                MemMove();
            }
        }

        if (FreeSize() < dataLen) {
            if (IsFull()) {
                return 0;
            }

            // FreeSize 小于待接收的数据长度并且总长没有超限则进行扩容本次接收数据的长度
            buffer_.resize(buffer_.capacity() + dataLen);
        }

        buffer_.insert(buffer_.begin() + writePos_, data, data + dataLen);
        writePos_ += dataLen;
    }

    size_t Read(char* data, size_t dataLen)
    {
        if (IsEmpty() || DataSize() < dataLen) {
            return 0;
        }

        std::copy_n(buffer_.begin() + readPos_, dataLen, data);
        readPos_ += dataLen;

        // 每次读取的时候都要判断下是否要进行 buffer_ 调整
        if (readPos_ == writePos_) {
            Clear();
        } else if (writePos_ - readPos_ <= MEMMOVE_THRESHOLD) {
            MemMove();
        }
    }

private:
    std::vector<char> buffer_;
    size_t readPos_;
    size_t writePos_;
};

class MsgSendNode
{
public:
    MsgSendNode(const char* data, size_t dataLen) : 
        buffer_(data, data + dataLen), readPos_(0)
    {
    }
    MsgSendNode(const MsgSendNode&) = default;
    MsgSendNode& operator=(const MsgSendNode&) = default;
    MsgSendNode(MsgSendNode&&) = default;
    MsgSendNode& operator=(MsgSendNode&&) = default;

    const char* GetData() const
    {
        return buffer_.data();
    }

    size_t GetDataSize()
    {
        return buffer_.size();
    }
    
    void SetReadPos(size_t dataLen)
    {
        readPos_ += dataLen;
    }

private:
    std::vector<char> buffer_;
    size_t readPos_;
};