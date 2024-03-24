#pragma once

struct MessageHeader
{
    unsigned short len;
};

struct MessageBody
{
    char data[0];
};

struct Message
{
    MessageHeader header;
    MessageBody body;
};