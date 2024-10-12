//C++11 P173 5-2
#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
using namespace std;

class Complex
{
private:
	int m_i;
	//mutex不能重入，不是递归锁，both函数会死锁
	//mutex m_mutex;
	//recursive_mutex是递归锁，同一个线程可以多次获取，但是尽量别用递归锁，而是简化逻辑
	recursive_mutex m_mutex;
public:
	Complex(int i) : m_i(i)
	{
	}

	void mul(int x)
	{
		lock_guard<recursive_mutex> locker(m_mutex);
		m_i *= x;
	}

	void div(int x)
	{
		lock_guard<recursive_mutex> locker(m_mutex);
		m_i /= x;
	}

	void both(int x)
	{
		lock_guard<recursive_mutex> locker(m_mutex);
		mul(x);
		div(x);
	}
};

int main(int argc, char const *argv[])
	{
		Complex complex(3);
		complex.both(4);

		return 0;
	}