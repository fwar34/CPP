//C++11 P174 5-4
#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
using namespace std;

timed_mutex g_mutex;

void work()
{
	chrono::milliseconds timeout(100);

	while (true)
	{
		if (g_mutex.try_lock_for(timeout))
		{
			cout << "thread : " << this_thread::get_id() << " get lock, working..." << endl;
			chrono::milliseconds sleepDuration(250);
			this_thread::sleep_for(sleepDuration);
			g_mutex.unlock();
			this_thread::sleep_for(sleepDuration);
		}
		else
		{
			cout << "thread : " << this_thread::get_id() 
				 << " can't get lock, wait a time to retry" << endl;
			chrono::milliseconds sleepDuration(100);
			this_thread::sleep_for(sleepDuration);
		}
	}
}

int main(int argc, char const *argv[])
{
	thread t1(work);
	thread t2(work);
	t1.join();
	t2.join();

	return 0;
}