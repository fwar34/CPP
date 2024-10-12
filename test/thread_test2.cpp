//C++11 P170
#include <iostream>
#include <vector>
#include <thread>
#include <memory>
using namespace std;

std::vector<thread> v1;
std::vector<shared_ptr<thread>> v2;

void func1(int i, const string& s)
{
	cout << i << " " << s << endl;
}

void func(int i)
{
	cout << i << endl;
	cout << "sleep_for 3 seconds" << endl;
	this_thread::sleep_for(chrono::seconds(3));

}

void CreateThread()
{
	thread t(func, 333);
	cout << "tid : " << t.get_id() << endl;
	cout << "core count : " << thread::hardware_concurrency() << endl;
	v1.push_back(move(t));

	v2.push_back(make_shared<thread>(func, 32323));
	v2.push_back(make_shared<thread>(func1, 444, "ssssss"));
	v2.push_back(make_shared<thread>([](int i){cout << "lambda " << i << endl;}, 44444));
}

int main(int argc, char const *argv[])
{
	CreateThread();
	for (auto& t : v1)
	{
		t.join();
	}

	for (auto& t : v2)
	{
		t->join();
	}

	return 0;
}