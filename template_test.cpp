// https://blog.csdn.net/Windgs_YF/article/details/80927091
#include <iostream>

template <class T1>
struct Base
{
    T1 t1;
    virtual void operator()() {
        std::cout << t1 << std::endl;
    }
};

template <class T1, class T2>
struct Driver : public Base<T1>
{
    T2 t2;
    virtual void operator()() {
        std::cout << Base<T1>::t1 << " " << t2 << std::endl;
        std::cout << this->t1 << " " << t2 << std::endl;
        Base<T1>::t1 = 100;
        std::cout << Base<T1>::t1 << " " << t2 << std::endl;
    }
};

int main(int argc, char* argv[]) {
    Driver<int, long> driver;
    driver.t1 = 3;
    driver.t2 = 4;
    driver();
    std::cout << "----------------------\n";

    Driver<int, long> driver2;
    // driver2.t1 = 77;
    // driver2.t2 = 88;
    // std::cout << driver.t1 << std::endl << driver.t2 << std::endl;
    driver();
    std::cout << "----------------------\n";
    driver2();
}

// C++的模板中的名称会进行两次查找，称为两阶段查找（two-phase lookup）。
// 对于一个非依赖型名称（不以任何方式依赖于模板参数的名称），在模板声明进行解析的时候就会进行查找。
// 但C++标准中规定（14.6.2 3），一个非受限的名称查找的时候将不会考虑依赖型的基类。
// 所以
// template<typename>
// struct B
// {
//     int x;
// };
// 
// template<typename T>
// struct X : B<T>
// {
//     void foo() { x = 0; }
// };
// 
// 查找不到就会错误。
// 解决办法是把它变成一个依赖型名称：
// template<typename>
// struct B
// {
//     int x;
// };
// 
// template<typename T>
// struct X : B<T>
// {
//     void foo() { this->x = 0; }  // this是依赖型名称
// };
// 或者
// template<typename>struct B
// {
//     int x;
// };
// template<typename T>
// struct X : B<T>
// {
//     void foo() { B<T>::x = 0; }  // B<T>是依赖型名称
// };
