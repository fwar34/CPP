//C++11 P99,这个一直编译不过，下来再看哪里有问题
#include <inttypes.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <type_traits>
using namespace std;

struct Person
{
	Person(uint32_t id, string& name) : m_id(id), m_name(name)
	{
	}
	
	uint32_t m_id;
	string m_name;
};

string& getKey(Person& per)
{
	return per.m_name;
}

template <class Fn>
auto GroupBy(vector<Person>& v, Fn& keySelector)->multimap<decltype(keySelector(*(Person*)nullptr)), Person>
{
	typedef decltype(keySelector(*(Person*)nullptr)) key_type;
	multimap<key_type, Person> map;
	for_each(v.begin(), v.end(), [&](Person& per)
	{
		map.emplace_insert(make_pair(keySelector(per), per));
	});
	
	return map;
}

template <class Fn>
multimap<typename result_of<Fn(Person)>::type, Person> GroupBy2(vector<Person>& v, Fn& keySelector)
{
	using key_type = typename result_of<Fn(Person)>::type;
	multimap<key_type, Person> map;
	for_each(v.begin(), v.end(), [&](const Person& per)
	{
		map.emplace_insert(make_pair(keySelector(per), per));
	});
	
	return map;
}

int main()
{
	string name = "ss";
	vector<Person> v{Person(1, name)};
	auto map1 = GroupBy<string&(Person&)>(v, getKey);
	for (auto ele : map1)
	{
		cout << ele.name << endl;
	}
	
	auto map2 = GroupBy2(v, getKey);
	for (auto ele : map2)
	{
		cout << ele.name << endl;
	}
	
	return 0;
}