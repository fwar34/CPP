#include <inttypes.h>
#include <iostream>
#include <tr1/functional>

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
	tr1::function<uint32_t ()> func;

	base b(1, 2);
	func = tr1::bind(&base::add, &b);
	cout << func() << endl;

	func = tr1::bind(&base::add2, &b, 3);
	cout << func() << endl;

	tr1::function<void (uint32_t)> func2;
	func2 = tr1::bind(&base::print, &b, tr1::placeholders::_1);
	func2(5);

	return 0;
}
