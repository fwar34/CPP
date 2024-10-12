#include <iostream>
#include <functional>
#include <inttypes.h>
using namespace std;

int main()
{
	uint32_t a = 90;
	uint32_t b = 80;
	//lambda可以认为是仿函数，可以用function和bind来存储
	function<uint32_t(uint32_t)> f1 = bind([&](uint32_t i, uint32_t j){return i + j + a - b;}, placeholders::_1, 32);
	cout << f1(33) << endl;
	
	function<uint32_t(uint32_t)> f2 = bind([&](uint32_t i, uint32_t j){return i + j + a - b;}, 33, 32);
	cout << f2(10000) << endl;
	
	//对于没有捕获任何参数的lambda可以用函数指针来直接存储
	using fun_t = uint32_t (*)(uint32_t);
	fun_t f3 = [](uint32_t i){return i + 3;};
	cout << f3(3) << endl;
	
	return 0;
}

template <class T>
class Base
{
public:
	Base();
	{
		cout << std::vector<string> v;
	}
	~Base();
	
};