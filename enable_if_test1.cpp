//C++11 P100
#include <inttypes.h>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
typename enable_if<is_arithmetic<T>::value, T>::type foo(T t)
{
	cout << "t = " << t << endl;
	return t;
}

//对入参做了限定
template <class T>
T foo2(T t, typename enable_if<is_integral<T>::value, uint32_t>::type i = 0)
{
	cout << "t = " << t << endl;
	cout << "i = " << i << endl;
	return t;
}

//对模板参数做了限定
template <class T, class = typename enable_if<is_integral<T>::value, uint32_t>::type>
T foo3(T t)
{
	cout << "t = " << t << endl;
	return t;
}

template <class T, class Y = typename enable_if<is_integral<T>::value, uint32_t>::type>
T foo3(T t, Y y)
{
	cout << "t = " << t << endl;
	cout << "y = " << y << endl;
	return t;
}

//模板特化时候对模板参数做了限定
template <class T, class Enable = void>
class A;

//模板偏特化
template <class T>
class A<T, typename enable_if<is_floating_point<T>::value>::type>
{};

//模板特化
template <>
class A<uint32_t, uint32_t>
{};

int main()
{
	foo(1);
	foo(2.3);
	//foo("test");	//编译失败，is_arithmetic判断是算术类型才返回true
	
	foo2(3, 5);
	
	foo3(66);
	foo3(555, 99);
	
	A<double> a;
	//A<uint32_t> b; //编译失败，不满足偏模板特化的限定条件，模板参数的类型应为浮点型
	A<uint32_t, uint32_t> c;
	
	return 0;
}