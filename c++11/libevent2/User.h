#pragma once
#include <memory>

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
        if (session_) [
            session_->ReleaseRef();
        ]
    }

    void SendReply(std::shared_ptr<Message>& message);

private:
    uint32_t id_;
    Session* session_ = nullptr;
};