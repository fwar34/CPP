#include <iostream>
#include <typeinfo>
#include <cxxabi.h>
using namespace std;

class Foo{};

int main()
{
    //__cxa_demangle让低级符号名解码成用户级名字
    //Foo* [10]使用__cxa_demangle函数显示
    //A10_P3Foo未使用显示这样
    char* name = abi::__cxa_demangle(typeid(Foo*[10]).name(), nullptr, nullptr, nullptr);
    cout << name << endl;
    free(name);
    
    cout << typeid(Foo*[10]).name() << endl;
    
    return 0;
}