#pragma once
#include "CommonDefine.h"
#include "Singleton.h"
#include "Message.h"
#include <iostream>
#include <unordered_map>
#include <memory>
#include <array>

namespace Nt
{

class Conference;
class ConferenceMgr
{
public:
    ConferenceMgr() = default;
    void AddConference(std::shared_ptr<Conference>& conf);
    void DelConference(uint32_t confId);
    std::shared_ptr<Conference> FindConference(uint32_t confId);
    void DispatchCommand(std::shared_ptr<Message> message);

private:
    // unordered_map 不排序，插入删除查找O(1)，map 排序，插入删除查找O（logN）
    std::unordered_map<uint32_t, std::shared_ptr<Conference>> conferences_;
};

// extern ConferenceMgr ConfMgrs[LOGIC_THREAD_NUM];
// extern std::array<std::thread::id, LOGIC_THREAD_NUM> ConfMgrs;
    
} // namespace Nt