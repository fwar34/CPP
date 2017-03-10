#include <inttypes.h>
#include <vector>
using namespace std;

template <class T>
class Foo
{
	decltype(T().begin()) it;
public:
	void func(T& t)
	{
		it = t.begin();
	}
};

int main()
{
	const vector<uint32_t> va;
	Foo<const vector<uint32_t> > foo1;
	foo1.func(va);

	vector<uint32_t> va2;
	Foo<vector<uint32_t> > foo2;
	foo2.func(va2);
	return 0;
}
