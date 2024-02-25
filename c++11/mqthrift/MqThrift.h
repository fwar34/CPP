#pragma once
#include <string>

class TBase
{
public:
    int id_;
};

/*
继承实现
*/
class IMqThriftHandler
{
    virtual void RecvMessage(TBase& msg) = 0;
};

struct MqThriftParam
{
    IMqThriftHandler* callback;
};

class MqThriftImpl;
class MqThrift
{
    MqThriftImpl* impl_;
public:
    static MqThrift& Instance();
    int Init(MqThriftParam& param);
    void UnInit();
    int SendMessage(TBase& msg);
};