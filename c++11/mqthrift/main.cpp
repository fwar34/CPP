#include "MqThrift.h"
#include <iostream>

class MsgHandler : public IMqThriftHandler
{
public:
    void RecvMessage(TBase& msg) override
    {
        std::cout << "recv msg: " << msg.id_ << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    MsgHandler handler;
    MqThriftParam param;
    param.callback = &handler;
    MqThrift::Instance().Init(param);
    return 0;
}
