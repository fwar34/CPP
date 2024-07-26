#pragma once

class IMessage
{
public:
    
};

struct RequestLine : public IMessage
{
        /* data */
};


struct ProtocolMsg
{
    union line
    {
        /* data */
    };
    
};