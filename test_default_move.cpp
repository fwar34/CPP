#include <iostream>
#include <string>
#include <memory>

struct Person
{
    std::string name;
    int age;
    std::shared_ptr<char[]> content;
    Person() : age(0), content(new char[100]) {};
    Person(const Person& other)
    {
        if (&other != this) {
            std::cout << "Person copy constructor!" << std::endl;
            name = other.name;
            age = other.age;
            content = other.content;
        }
    }

    Person& operator=(const Person& other)
    {
        if (&other != this) {
            std::cout << "Person assign constructor!" << std::endl;
            name = other.name;
            age = other.age;
            content = other.content;
        }
        return *this;
    }

    // Person(Person&& other) noexcept : name(std::move(other.name)), age(other.age)
    // {
    //     std::cout << "Person move constructor!" << std::endl;
    // }
    Person(Person&& other)
    {
        std::cout << "Person move constructor!" << std::endl;
        // std::swap 主要是交换两个对象，通常用于交换容器、
        // 自定义类型等，要求交换的对象支持移动构造和移动赋值
        std::swap(name, other.name);
        // std::exchange 交换两个值，返回第一个值交换前的数值
        age = std::exchange(other.age, 0);
        // std::swap(content, other.content);
        content.swap(other.content);
    }

    Person& operator=(Person&& other) noexcept
    {
        name = std::move(other.name);
        age = other.age;
        content = std::exchange(other.content, nullptr);

        std::cout << "Person assign constructor!" << std::endl;
        return *this;
    }
};

class Family
{
    friend int main();
public:
    Family() : num_(3), data_(new char[100])
    {
    }
    Family(Family&& other) noexcept = default;
    Family& operator=(Family&& other) noexcept = default;

private:
    Person father_;
    Person mother_;
    int num_;
    std::unique_ptr<char[]> data_;
};

int main()
{
    Family family1;
    family1.father_.name = "Famil1_father";
    family1.father_.age = 25;
    sprintf(family1.data_.get(), "%s:%d", "hello!!!", 1000);

    Family family2(std::move(family1));

    std::cout << "after move ----------------------------" << std::endl;
    std::cout << "dest family2:----------" << std::endl;
    std::cout << family2.father_.name << std::endl;
    std::cout << family2.father_.age << std::endl;
    std::cout << family2.data_.get() << std::endl;

    std::cout << "src family1:----------" << std::endl;
    std::cout << family1.father_.name << std::endl;
    std::cout << family1.father_.age << std::endl;
    std::cout << family1.data_.get() << std::endl;

    return 0;
}