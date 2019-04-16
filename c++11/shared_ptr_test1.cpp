#include <inttypes.h>
#include <iostream>
#include <memory>
using namespace std;

template <class T>
void Delete(T* t)
{
	cout << "Delete" << endl;
	delete t;
}

template <class T>
shared_ptr<T> make_shared_array(size_t size)
{
	//注意这里default_delete<T[]>()后面有个成对括号
	return shared_ptr<T>(new T[size], default_delete<T[]>());
}

int main()
{
	shared_ptr<uint32_t> p1(new uint32_t(3));
	shared_ptr<uint32_t> p2;
	p2.reset(new uint32_t(4));
	
	if (p1)
	{
		cout << "p1 = " << *p1 << endl;
	}
	
	if (p2)
	{
		cout << "p2 = " << *p2 << endl;
	}
	
	//注意这里Delete<uint32_t>后面没有成对括号
	shared_ptr<uint32_t> p3(new uint32_t(5), Delete<uint32_t>);
	if (p3)
	{
		cout << "p3 = " << *p3 << endl;
	}
	
	shared_ptr<uint32_t> p4(new uint32_t(6), [](uint32_t* i)
	{
		cout << "lambda delete" << endl;
		delete i;
	});
	if (p4)
	{
		cout << "p4 = " << *p4 << endl;
	}
	
	shared_ptr<uint32_t> p5(new uint32_t[4], [](uint32_t* i)
	{
		cout << "lambda delete[]" << endl;
		delete[] i;
	});
	if (p5)
	{
		//...
	}
	
	shared_ptr<uint32_t> array = make_shared_array<uint32_t>(55);
	if (array)
	{
		//...
	}
	
	shared_ptr<uint32_t> arr1(new uint32_t[5], default_delete<uint32_t[]>());
	
	return 0;
}
