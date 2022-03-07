#include <inttypes.h>
#include <iostream>
#include <memory>
using namespace std;

struct A
{
	shared_ptr<A> GetSelf()
	{
		return shared_ptr<A>(this);
	}
};

struct B : enable_shared_from_this<B>
{
    // enable_shared_from_this 模板类有函数 shared_from_this()
};

int main()
{
    shared_ptr<B> spb1(new B);
    shared_ptr<B> spb2 = spb1->shared_from_this();
    cout << spb1.use_count() << endl;

	//错误，sp1和sp2指向了同一个A对象，use_count 是1，所以会造成重复析构，正确的方法是继承 enable_shared_from_this
	shared_ptr<A> sp1(new A);
	shared_ptr<A> sp2 = sp1->GetSelf();
	cout << sp2.use_count() << endl;
	
	return 0;
}
