//C++11 P178 5-6
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <list>
#include <thread>
using namespace std;

//使用unqiue_lock和condition_variable实现的同步队列
template <typename T>
class SyncQueue
{
private:
	list<T> m_queue;
	mutex m_mutex;
	condition_variable m_notEmpty;
public:
	void Push(const T& t)
	{
		lock_guard<mutex> lock(m_mutex);
		m_queue.emplace_back(t);
		m_notEmpty.notify_one();
	}

	void Pop(T& t)
	{
		unique_lock<mutex> lock(m_mutex);
		m_notEmpty.wait(lock, [this]{return !m_queue.empty();});
		t = m_queue.front();
		m_queue.pop_front();
	}

	bool Empty()
	{
		lock_guard<mutex> lock(m_mutex);
		return m_queue.empty();
	}

	size_t Size()
	{
		lock_guard<mutex> lock(m_mutex);
		return m_queue.size();
	}
};

template <typename T>
void func1(SyncQueue<T>* queue)
{
	T i = 0;
	while (true)
	{
		cout << "push thread : " << i << endl;
		queue->Push(i++);
		this_thread::sleep_for(chrono::milliseconds(300));
	}
}

template <typename T>
void func2(SyncQueue<T>& queue)
{
	T t = 0;
	while (true)
	{
		queue.Pop(t);
		cout << "pop thread : " << t << endl;
		this_thread::sleep_for(chrono::milliseconds(800));
	}
}

list<shared_ptr<thread>> ts;

int main(int argc, char const *argv[])
{
	SyncQueue<uint32_t> queue;
	//注意func1和func2的参数
	ts.emplace_back(make_shared<thread>(func1<uint32_t>, &queue));
	ts.emplace_back(make_shared<thread>(func2<uint32_t>, ref(queue)));

	for (auto& t : ts)
	{
		t->join();
	}

	return 0;
}