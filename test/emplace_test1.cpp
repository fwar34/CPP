#include <inttypes.h>
#include <vector>
#include <iostream>
using namespace std;

class A
{
private:
	uint32_t m_x;
	uint32_t m_y;
public:
	A(uint32_t x, uint32_t y) : m_x(x), m_y(y){}
};

int main()
{
	vector<uint32_t> v;
	v.emplace_back(1);
	
	vector<A> v1;
	v1.emplace_back(1, 2);
	v1.emplace_back(3, 4);
	
	cout << "v size : " << v.size() << endl
		 << "v1 size : " << v1.size() << endl;
		 
	return 0;
}