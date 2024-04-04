#pragma once
// 在自己实现的 reactor 中使用，libevent 已经有了接收缓冲区
#include <cstring>

// ring buffer，不支持扩容，后续可以添加扩容逻辑
class MsgNode
{
public:
    MsgNode(size_t maxLen) : readPos_(0), writePos_(0), curLen_(0), maxLen_(maxLen)
    {
        data_ = new char[maxLen];
    }
    virtual ~MsgNode()
    {
        if (data_) {
            delete[] data_;
            data_ = nullptr;
        }
    }

    void Clear()
    {
        memset(data_, 0, sizeof(data_));
        readPos_ = 0;
        writePos_ = 0;
    }

    size_t ReadPos()
    {
        return readPos_;
    }

    size_t WritePos()
    {
        return writePos_;
    }

    bool IsFull()
    {
        return curLen_ == maxLen_;
    }

    bool IsEmpty()
    {
        return curLen_ == 0;
    }

    size_t Write(const char* data, size_t dataLen)
    {
        if (IsFull()) {
            return 0;
        }

        size_t freeLen = maxLen_ - curLen_;
        if (freeLen < dataLen) {
            return 0;
        }

        // 总共就三种状态：1. 写大于读，2. 写小于读，3. 写等于读，1 和 3 可以合并
        if (writePos_ >= readPos_) {
            if (maxLen_ - writePos_ >= dataLen) {
                memcpy(data_ + writePos_, data, dataLen);
            } else {
                memcpy(data_ + writePos_, data, maxLen_ - writePos_);
                memcpy(data_, data + (maxLen_ - writePos_), dataLen - (maxLen_ - writePos_));
            }
        } else {
            memcpy(data_ + writePos_, data, dataLen);
        }
        writePos_ = (writePos_ + dataLen) % maxLen_;
        curLen_ += dataLen;

        return dataLen;
    }

    size_t Read(char* data, size_t dataLen)
    {
        if (IsEmpty()) {
            return 0;
        }

        if (curLen_ < dataLen) {
            return 0;
        }

        if (writePos_ >= readPos_) {
            memcpy(data, data_ + readPos_, dataLen);
        } else {
            if (maxLen_ - readPos_ >= dataLen) {
                memcpy(data_ + readPos_, data, dataLen);
            } else {
                memcpy(data, data_ + readPos_, maxLen_ - readPos_);
                memcpy(data_, data + (maxLen_ - readPos_), dataLen - (maxLen_ - readPos_));
            }
        }
        readPos_ = (readPos_ + dataLen) % maxLen_;
        curLen_ -= dataLen;

        return dataLen;
    }

    size_t MaxLen()
    {
        return maxLen_;
    }

    size_t CurLen()
    {
        return curLen_;
    }

protected:
    char* data_;
    size_t readPos_; // read 索引，指向了当前可读取的字节
    size_t writePos_;// write 索引，指向了下一个可写入的字节
    size_t curLen_; // 当前长度
    size_t maxLen_; // 最大长度
};

class MsgRecvNode : public MsgNode
{
public:
    MsgRecvNode(size_t maxLen) : MsgNode(maxLen), parseHeaderComplete_(false)
    {
    }
    void ParseHeaderComplete(bool flag)
    {
        parseHeaderComplete_ = flag;
    }
    bool ParseHeaderComplete()
    {
        return parseHeaderComplete_;
    }

private:
    bool parseHeaderComplete_;
};

class MsgSendNode : public MsgNode
{
public:
    MsgSendNode(const char* data, size_t dataLen) : MsgNode(dataLen)
    {
        memcpy(data_, data, dataLen);
        curLen_ = dataLen;
    }
};