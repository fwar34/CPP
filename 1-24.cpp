#include <inttypes.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class CountEven
{
	uint32_t& m_count;
public:
	CountEven(uint32_t& count) : m_count(count)
	{}
	
	void operator()(uint32_t val)
	{
		if (!(val & 1))
		{
			m_count++;
		}
		cout << "operator() call" << endl;
	}
};

int main()
{
	vector<uint32_t> va = {1, 2, 3, 4, 5, 6, 7};
	uint32_t count = 0;
	for_each(va.begin(), va.end(), CountEven(count));
	cout << "count = " << count << endl;
	
	//用lambda简化，实现闭包
	count = 0;
	for_each(va.begin(), va.end(), [&count](uint32_t val){
		if (val % 2 == 0)
		{
			count++;
		}
		cout << "lambda call " << endl;
	});
	cout << "count = " << count << endl;
    
    //lambda实现闭包
    count = count_if(va.begin(), va.end(), [](uint32_t val){
        if (val > 2 && val < 6) 
        {
            return true;
        }
        else
        {
            return false;
        }
    });
    
    cout << "count = " << count << endl;
	
	return 0;
}