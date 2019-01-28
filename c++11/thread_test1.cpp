//C++11 P167
//g++ thread_test1.cpp -lpthread
#include <iostream>
#include <thread>
#include <functional>
#include <string>
using namespace std;

/*void func()
{
	cout << "func...." << endl;
}*/

//string传递引用的话要加const即const string& s
void func2(int i, double j, string s)
{
	cout << "i = " << i << endl;
	cout << "j = " << j << endl;
	cout << "s = " << s << endl;
}

int main()
{
	thread t(func2, 1, 2.3, "sss");
	t.join();

	thread t1(func2, 1, 2.3, "tttt");
	t1.join();

	thread t3(bind(func2, 4, 5.6, "ssss"));
	t3.join();

	//bind函数
	thread t5(bind(func2, placeholders::_2, 3.3, placeholders::_1), "jjjjjjjj", 666);
	t5.join();

	//lambda表达式
	thread t4([](int i)
	{
		cout << i << endl;
	}, 44444);
	t4.join();
	
	return 0;
}
