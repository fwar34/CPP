#pragma once
#include "CommonDefine.h"
#include "Singleton.h"
#include <iostream>
#include <unordered_map>
#include <memory>

class Conference;
class ConferenceMgr
{
public:
    ConferenceMgr() = default;
    void AddConference(std::shared_ptr<Conference>& conf);
    void DelConference(uint32_t confId);

private:
    // unordered_map 不排序，插入删除查找O(1)，map 排序，插入删除查找O（logN）
    std::unordered_map<uint32_t, std::shared_ptr<Conference>> conferences_;
};

extern ConferenceMgr ConfMgrs[LOGIC_THREAD_NUM];