//C++11 P103
#include <inttypes.h>
#include <iostream>
using namespace std;

template <class...T>
void f(T...args)
{
	cout << sizeof...(args) << endl;
}

//递归终止函数
/*void print()
{
	cout << "empty" << endl;
}*/

/*template <class T>
void print(T t)
{
	cout << t << endl;
}*/

template <class T1, class T2>
void print(T1 t1, T2 t2)
{
	cout << t1 << " " << t2 << endl;
}

//参数包展开函数，是一个递归函数
template <class T, class... Args>
void print(T head, Args... rest)
{
	cout << "parameter " << head << endl;
	print(rest...);
}

int main()
{
	f();
	f(1);
	f(1, 2.3, "");
	
	print(1, 2, 3, 4);
	
	return 0;
}