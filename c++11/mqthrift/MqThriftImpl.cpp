#include "MqThriftImpl.h"
#include <iostream>

int MqThriftImpl::Init(MqThriftParam &param)
{
    std::cout << "MqThriftImpl::Init called" << std::endl;
    return 0;
}

void MqThriftImpl::UnInit()
{
    std::cout << "MqThriftImpl::UnInit called" << std::endl;
}

int MqThriftImpl::SendMessage(TBase &msg)
{
    return 0;
}