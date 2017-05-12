#include <inttypes.h>
#include <typeinfo>
#include <iostream>
using namespace std;

template <class U, class T>
auto add(U u, T t) -> decltype(u + t)
{
	return t + u;
}

int main()
{
	auto ret = add(1, 2.5);
	cout << ret << endl;
	cout << "ret type = " << typeid(ret).name() << endl;

	return 0;
}
