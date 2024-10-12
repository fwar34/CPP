#include <inttypes.h>
#include <iostream>
#include <vector>
using namespace std;

class element
{
private:
	vector<uint32_t> m_va;
public:
	element()
	{
		cout << "construct" << endl;
	}
	//右值引用的拷贝构造函数，参数是右值引用，这里用move的原因是右值
	//通过参数传递成了已命名的对象，从而编译器认为其实左值，所以使用move函数将左值转成右值引用
	element(element&& e) : m_va(move(e.m_va))
	{
		cout << "move construct" << endl;
	}
	element(const element& e) : m_va(e.m_va)
	{
		cout << "copy construct" << endl;
	}
	~element()
	{
		cout << "destruct" << endl;
	}
};

int main()
{
	element e;
	vector<element> ve;
	ve.push_back(e);
	ve.push_back(move(e));
	
	return 0;
}