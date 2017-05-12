#include <inttypes.h>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>
using namespace std;

struct Key
{
	string first;
	string second;
};

struct KeyHash
{
	size_t operator()(const Key& k) const
	{
		return hash<string>()(k.first) ^ hash<string>()(k.second) << 1;
	}
};

struct KeyEqual
{
	bool operator()(const Key& k1, const Key& k2) const
	{
		return k1.first == k2.first && k1.second == k2.second;
	}
};

int main()
{
	unordered_map<string, string> m1;
	unordered_map<uint32_t, string> m2 = 
	{
		{1, "a"}, {2, "b"}, {3, "c"}
	};
	
	//copy construct
	unordered_map<uint32_t, string> m3 = m2;
	
	//move construct
	unordered_map<uint32_t, string> m4 = move(m2);
	
	//rang construct
	vector<pair<bitset<8>, uint32_t>> v = {{0x12, 33}, {0x33, 44}, {0x44, 44}};
	unordered_map<bitset<8>, uint32_t> m5(v.begin(), v.end());
	
	//construct for a custom type
	unordered_map<Key, string, KeyHash, KeyEqual> m6 = 
	{
		{{"a", "b"}, "aaa"}, {{"c", "d"}, "bbb"}
	};
	
	return 0;
}

