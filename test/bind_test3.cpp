//线程基于closure的设计
#include <iostream>
//#include <tr1/functional>
#include <functional>

//using namespace tr1;
using namespace std;

class Thread
{
private:
	function<void ()> m_callback;
public:
	Thread(function<void ()> cb) : m_callback(cb)
	{
	}

	void run()
	{
		m_callback();
	}
};

class test
{
public:
	void func()
	{
		cout << "test::func called..." << endl;
	}
};

void print(int i)
{
	cout << i << endl;
}

int print2(int i)
{
	cout << i << endl;
	return i;
}

int main()
{
	test t;
	Thread thread(bind(&test::func, &t));
	thread.run();

	Thread thread2(bind(print, 33));
	thread2.run();

	Thread thread3(bind(print2, 44));
	thread3.run();

	return 0;
}
