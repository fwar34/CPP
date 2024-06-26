#pragma once

struct bufferevent;
namespace Nt
{

class EventHandler
{
public:
    virtual ~EventHandler() = default;
    virtual void HandleInput(struct bufferevent*) = 0;
    virtual void HandleOutput() = 0;
    virtual void HandleClose(struct bufferevent*) = 0;
    virtual void HandleTimeout() = 0;
};

class IOHandler : public EventHandler
{
public:
    virtual int Accept() { return 0; }
    virtual int Start() { return 0; }
    virtual int Connect() { return 0; }
    virtual int Close() { return 0; }
};

};