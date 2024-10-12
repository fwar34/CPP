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

int main()
{
	//错误，sp1和sp2指向了同一个A对象，所以会造成重复析构
	shared_ptr<A> sp1(new A);
	shared_ptr<A> sp2 = sp1->GetSelf();
	//cout << sp2.use_count() << endl;
	
	return 0;
}