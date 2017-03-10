//C++11 P171 5-1
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <memory>
using namespace std;

mutex g_mutex;
std::vector<shared_ptr<thread>> v;

void func()
{
	g_mutex.lock();
	cout << "enter thread : " << this_thread::get_id() << endl;
	this_thread::sleep_for(chrono::seconds(3));
	cout << "leave thread : " << this_thread::get_id() << endl;
	g_mutex.unlock();
}

void func2()
{
	lock_guard<mutex> locker(g_mutex);
	cout << "enter guard thread : " << this_thread::get_id() << endl;
	this_thread::sleep_for(chrono::seconds(3));
	cout << "leave guard thread : " << this_thread::get_id() << endl;
}

int main()
{
	v.push_back(make_shared<thread>(func2));
	v.push_back(make_shared<thread>(func));
	v.push_back(make_shared<thread>(func));
	for (auto& t : v)
	{
		t->join();
	}

	return 0;
}