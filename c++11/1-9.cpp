#include <inttypes.h>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

class FooVector
{
private:
	vector<uint32_t> m_vec;
public:
	FooVector(initializer_list<uint32_t> list)
	{
		for (auto it : list)
		{
			m_vec.push_back(it);
			cout << it << endl;
		}
	}
};

class FooMap
{
private:
	map<uint32_t, uint32_t> m_map;
public:
	using pair_t = map<uint32_t, uint32_t>::value_type;
	FooMap(initializer_list<pair_t> list)
	{
		for (auto it : list)
		{
			m_map.insert(it);
			cout << it.first << " -> " << it.second << endl;
		}
	}
};

int main()
{
	FooVector foo_v = {1, 2, 3, 4, 5};
	FooMap foo_m = {{1, 2}, {3, 4}, {5, 6}};

	return 0;
}
