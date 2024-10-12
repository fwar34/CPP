#include <inttypes.h>
#include <iostream>
#include <functional>

using namespace std;

class base
{
private:
	uint32_t m_a;
	uint32_t m_b;
public:
	base(uint32_t a, uint32_t b) : m_a(a), m_b(b)
	{
	}

	uint32_t add()
	{
		return m_a + m_b;
	}

	uint32_t add2(uint32_t c)
	{
		return m_a + m_b + c;
	}

	void print(uint32_t x)
	{
		cout << "a = " << m_a << endl;
		cout << "b = " << m_b << endl;
		cout << "x = " << x << endl;
	}
};

int main()
{
	function<uint32_t ()> func;

	base b(1, 2);
	func = bind(&base::add, &b);
	cout << func() << endl;

	func = bind(&base::add2, &b, 3);
	cout << func() << endl;

	function<void (uint32_t)> func2;
	func2 = bind(&base::print, &b, placeholders::_1);
	func2(5);

	function<void(uint32_t)> func3;
	func3 = bind(&base::print, &b, 333);
	//这里的333333不会传递到print函数去，因为333已经通过bind传递了，这里333333只是为了编译通过
	//因为func3的原型是void(uint32_t)，调用的时候必须要传递一个uint32_t参数
	func3(333333);	

	return 0;
}
