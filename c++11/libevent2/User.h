#pragma once
#include "Session.h"
#include <memory>

namespace Nt
{
    
class Session;
struct Message;
class User
{
public:    
    User(uint32_t id, Session* session) : id_(id), session_(session)
    {
        session_->AddRef();
    }
    ~User()
    {
        if (session_) {
            session_->ReleaseRef();
        }
    }

    void ProcessMessage(std::shared_ptr<Message>& message);

private:
    uint32_t id_;
    Session* session_ = nullptr;
};

} // namespace Nt