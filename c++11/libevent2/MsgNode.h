#pragma once

class MsgNode
{
public:
    MsgNode(size_t maxLen) : curPos_(0), maxLen_(maxLen)
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
        curPos_ = 0;
    }
protected:
    char* data_;
    size_t curPos_;
    size_t maxLen_;
};

class RecvMsgNode : public MsgNode
{
public:
    RecvMsgNode(size_t maxLen) : MsgNode(maxLen)
    {

    }
};

class SendMsgNode : public MsgNode
{
public:
    SendMsgNode(const char* data, size_t dataLen) : MsgNode(dataLen)
    {
        memcpy(data_, data, dataLen);
    }
};