#pragma once
#include "Session.h"
#include "Message.h"
#include <cstdint>
#include <unordered_map>
#include <memory>

namespace Nt
{

class Session;
class User;
class Conference
{
public:
    Conference(uint32_t id) : id_(id)
    {
    }
    ~Conference() = default;

    uint32_t Id() { return id_; }
    void AddUser(std::shared_ptr<User>& user);
    void DelUser(uint32_t userId);
    void DispatchCommand(std::shared_ptr<Message> message, Session* session);

private:
    uint32_t id_;
    std::unordered_map<uint32_t, std::shared_ptr<User>> users_;
};
    
} // namespace Nt