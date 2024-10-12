//C++11 P95
#include <inttypes.h>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
struct SimpFunction
{
	//decay将输入的函数void(uint32_t)转成了函数指针void(*)(uint32_t)，然后用m_fun来存储函数
	using fun = typename decay<T>::type;
	
	SimpFunction(T& t) : m_fun(t)
	{
	}
	
	void exe()
	{
		m_fun(333);
	}
	
	fun m_fun;
};

void test(uint32_t i)
{
	cout << "i = " << i << endl;
}

int main()
{
	SimpFunction<void(uint32_t)> sim(test);
	sim.exe();
	
	return 0;
}