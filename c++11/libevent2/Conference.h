#pragma once
#include "Session.h"
#include <cstdint>
#include <unordered_map>
#include <memory>

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

private:
    uint32_t id_;
    std::unordered_map<uint32_t, std::shared_ptr<User>> users_;
};