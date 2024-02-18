#include "ObjectFactory.h"
#include <unordered_map>
#include <iostream>
#include <mutex>

class ObjectFactory::ObjectFactoryImpl : public std::enable_shared_from_this<ObjectFactoryImpl>
{
    std::mutex mtx_;
    // std::unordered_map<uint32_t, std::shared_ptr<Widget>> pool_;
    std::unordered_map<uint32_t, std::weak_ptr<Widget>> pool_;
    void Remove(Widget* pw)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        std::cout << "xxxxxxx" << std::endl;
        auto it =  pool_.find(pw->id_);
        if (it->second.expired()) {
            pool_.erase(it);
            std::cout << "remove " << pw->id_ << " from pool_, after size = " << pool_.size() << std::endl;
        }
    }
public:
    std::shared_ptr<Widget> GetWidget(uint32_t id)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        auto it = pool_.find(id);
        if (it != pool_.end()) {
            auto ret = it->second.lock();
            if (ret) {
                return ret;
            }
        }
        auto widget = std::make_shared<Widget>(id);
        // return pool_.emplace(std::make_pair(id, widget)).first->second;
        pool_.emplace(std::make_pair(id, widget));
        return widget;
    }

    std::shared_ptr<Widget> GetWidget2(uint32_t id)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        auto&& wkWidget = pool_[id];
        std::shared_ptr<Widget> retWidget = wkWidget.lock();
        if (!retWidget) {
            retWidget.reset(new Widget(id), 
                [wkImpl = std::weak_ptr<ObjectFactoryImpl>(shared_from_this())](Widget *pw) {
                    auto impl = wkImpl.lock();
                    if (impl) {
                        impl->Remove(pw); //
                    } else {
                        std::cout << "ObjectFactory has been destroy" << std::endl;
                    }
                    delete pw;
                });
            // retWidget.reset(new Widget(id));
            wkWidget = retWidget;
        }
        return retWidget;
    }
};

// ObjectFactory::ObjectFactory() : impl_(std::make_unique<ObjectFactoryImpl>())
ObjectFactory::ObjectFactory() : impl_(std::make_shared<ObjectFactoryImpl>())
{
}
ObjectFactory::~ObjectFactory() = default;

std::shared_ptr<Widget> ObjectFactory::GetWidget(uint32_t id)
{
    return impl_->GetWidget(id);
}

std::shared_ptr<Widget> ObjectFactory::GetWidget2(uint32_t id)
{
    return impl_->GetWidget2(id);
}