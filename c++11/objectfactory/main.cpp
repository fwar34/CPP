#include "ObjectFactory.h"
#include <iostream>

void Test()
{
    auto widget = ObjectFactory::Instance().GetWidget(123);
    if (widget) {
        std::cout << "Get widget name = " << widget->name_ << " id_ = " << widget->id_ << std::endl;
        std::cout << "widget use count " << widget.use_count() << std::endl;
    }
}

void Test2()
{
    std::shared_ptr<Widget> pw = ObjectFactory::Instance().GetWidget2(1234);
    if (pw) {
        std::cout << "Get widget name = " << pw->name_ << " id_ = " << pw->id_ << std::endl;
        std::cout << "widget use count " << pw.use_count() << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    Test();
    Test2();

    return 0;
}
