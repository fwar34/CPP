//C++11 P106
#include <iostream>
using namespace std;

template <class T>
void printarg(T t)
{
	cout << t << endl;
}

template <class ...Args>
void expend(Args ...args)
{
	int arr[] = {(printarg(args), 0)...};
}

template<class ...Args>
void expend2(Args ...args)
{
	initializer_list<int>{(printarg(args), 0)...};
}

template<class ...Args>
void expend3(Args ...args)
{
	initializer_list<int>{([&args]{cout << args << endl;}(), 0)...}
}

int main()
{
	expend(1, 2, 3, 4);
	expend2(5, 6, 7, 8);
	expend3(9, 10, 11, 12);
	
	return 0;
}