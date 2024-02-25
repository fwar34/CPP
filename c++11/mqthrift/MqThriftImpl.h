#pragma once
#include "MqThriftConsumer.h"
#include "MqThriftProducer.h"
#include "MqThrift.h"

class MqThriftImpl
{
    MqThriftProducer producer_;
    MqThriftConsumer consumer_;
public:
    MqThriftImpl() {};
    int Init(MqThriftParam& param);
    void UnInit();
    int SendMessage(TBase& msg);
};