#include <iostream>
#include <map>
#include <string>
#include <inttypes.h>

using namespace std;

int main()
{
	map<uint32_t, string> m;
	m.insert(make_pair(1, "1"));
	m.insert(make_pair(2, "2"));
	m.insert(make_pair(3, "3"));

	for(auto it : m)
	{
		cout << it.first << ":" << it.second << endl;
	}

	return 0;
}
		
