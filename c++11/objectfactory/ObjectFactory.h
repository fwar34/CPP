#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <unordered_map>

struct Widget
{
    Widget(uint32_t id) : id_(id), name_("widget" + std::to_string(id)) {}
    std::string name_;
    uint32_t id_;
// public:
//     Widget(const std::string& name, uint32_t id)
//     {
//         name_ = name;
//         id_ = id;
//     }
//     std::string Name() { return name_; }
//     uint32_t Id() { return id_; }
};

class ObjectFactory
{
    class ObjectFactoryImpl;
    // std::unique_ptr<ObjectFactoryImpl> impl_;
    std::shared_ptr<ObjectFactoryImpl> impl_;
public:
    ObjectFactory();
    ~ObjectFactory();
    std::shared_ptr<Widget> GetWidget(uint32_t id);
    std::shared_ptr<Widget> GetWidget2(uint32_t id);
    static ObjectFactory& Instance()
    {
        static ObjectFactory instance;
        return instance;
    }
};