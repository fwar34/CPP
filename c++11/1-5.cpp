#include <inttypes.h>
#include <iostream>
#include <vector>
using namespace std;

template <class T>
class Foo
{
	typename T::iterator it;
public:
	void func(T& t)
	{
		it = t.begin();
	}

};

//模板特化 对const类型
template <class T>
class Foo<const T>
{
	typename T::const_iterator it;
public:
	void func(T& t)
	{
		it = t.begin();
	}
};

int main()
{
	vector<uint32_t> va;
	Foo<const vector<uint32_t> > foo;
	foo.func(va);

	Foo<vector<uint32_t> > foo2;
	foo2.func(va);
	
	return 0;
}

