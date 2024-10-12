#include <inttypes.h>
#include <iostream>
#include <memory>
using namespace std;

int main()
{
	weak_ptr<uint32_t> wp1;
	{
		shared_ptr<uint32_t> sp1(new uint32_t(3));
		wp1 = sp1;
		cout << "wp1.use_count = " << wp1.use_count() << endl;
	}
	
	if (wp1.expired())
	{
		cout << "weak_ptr 无效" << endl;
	}
	else
	{
		cout << "weak_ptr 有效" << endl;
	}
	
	cout << "wp1.use_count = " << wp1.use_count() << endl;
	shared_ptr<uint32_t> sp2 = wp1.lock();
	if (sp2)
	{
		cout << "sp2 有效" << endl;
	}
	else
	{
		cout << "sp2 无效" << endl;
	}
	
	return 0;
}