#include "Cache.h"

Cache& Cache::Instance()
{
    static Cache instance;
    return instance;
}

int Cache::Init(const CacheParam& param)
{
    return impl_->Init(param);
}