#include <inttypes.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
using namespace std;

void print(uint32_t i)
{
	cout << i << endl;
}

template <class T>
class Test
{
public:
	Test(T* t) : m_t(t){}
	void func()
	{
		typename T::iterator it = m_t->begin();
		typename T::iterator it_end = m_t->end();
		for_each(it, it_end, print);
	}
private:
	T* m_t;
};

void print2(uint32_t i)
{
	cout << i + 2 << endl;
}

template <>
class Test<list<uint32_t> >
{
public:
	Test(list<uint32_t>* t) : m_t(t){}
        void func()
        {
                list<uint32_t>::iterator it = m_t->begin();
                list<uint32_t>::iterator it_end = m_t->end();
                for_each(it, it_end, print2);
        }
private:
	list<uint32_t>* m_t;
};

int main()
{
	vector<uint32_t> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);

	Test<vector<uint32_t> > test(&v);
	test.func();

	list<uint32_t> list1;
	list1.push_back(1);
	list1.push_back(2);
	list1.push_back(3);

	Test<list<uint32_t> > test2(&list1);
	test2.func();

	return 0;
}
