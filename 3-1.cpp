#include <inttypes.h>
#include <iostream>
#include <type_traits>
#include <memory>
using namespace std;

template <class T>
struct Construct
{
	typedef typename remove_reference<T>::type U;
	
private:
	unique_ptr<U> m_ptr;
	
public:
	//构造函数要创建一个智能指针，需要获取T的原始类型，所以来移除T可能的引用
	Construct() : m_ptr(new U)
	{
	}
	
	//获取只能指针对象时候要对原始类型U添加左值引用
	typename add_lvalue_reference<U>::type Get() const
	{
		return *m_ptr.get();
	}
};

int main()
{
	Construct<uint32_t> c;
	uint32_t i = c.Get();
	cout << i << endl;
	
	return 0;
}

