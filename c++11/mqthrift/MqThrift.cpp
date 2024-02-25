#include "MqThrift.h"
#include "MqThriftImpl.h"

MqThrift& MqThrift::Instance()
{
    static MqThrift instance;
    return instance;
}

int MqThrift::Init(MqThriftParam& param)
{
    return impl_->Init(param);
}

void MqThrift::UnInit()
{
    return impl_->UnInit();
}

int MqThrift::SendMessage(TBase& msg)
{
    return impl_->SendMessage(msg);
}