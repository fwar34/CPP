//C++11 P176 5-5
//条件变量实现的同步队列
#include <inttypes.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <memory>
using namespace std;



template <typename T>
class SyncQueue
{
private:
	list<T> m_list;
	mutex m_mutex;
	condition_variable_any m_notFull;	//通知不满的条件变量
	condition_variable_any m_notEmpty;	//通知不空的条件变量
	int m_maxSize;

	bool IsFull()
	{
		return m_list.size() == m_maxSize;
	}

	bool IsEmpty()
	{
		return m_list.empty();
	}

public:
	SyncQueue(int size) : m_maxSize(size)
	{
	}

	void Push(const T& t)
	{
		lock_guard<mutex> lock(m_mutex);
		while (IsFull())
		{
			//列表满了就等待不满的信号
			m_notFull.wait(m_mutex);
		}

		m_list.emplace_back(t);
		m_notEmpty.notify_one();
	}

	void Pop(T& t)
	{
		lock_guard<mutex> lock(m_mutex);
		while (IsEmpty())
		{
			//列表为空就等待非空的信号
			m_notEmpty.wait(m_mutex);
		}

		t = m_list.front();
		m_list.pop_front();
		m_notFull.notify_one();
	}

	bool Empty()
	{
		lock_guard<mutex> lock(m_mutex);
		return m_list.empty();
	}

	bool Full()
	{
		lock_guard<mutex> lock(m_mutex);
		return m_list.size() == m_maxSize;
	}

	size_t Size()
	{
		lock_guard<mutex> lock(m_mutex);
		return m_list.size();
	}

	int Count()
	{
		return m_list.size();
	}
};


void func1(SyncQueue<uint32_t>* queue)
{
	uint32_t i = 0;
	while (true)
	{
		cout << "push thread1 : " << i << endl;
		queue->Push(i++);
		//i = i % 20;
		this_thread::sleep_for(chrono::milliseconds(500));
	}
}

void func2(SyncQueue<uint32_t>* queue)
{
	uint32_t t = 0;
	while (true)
	{
		queue->Pop(t);
		cout << "pop thread : " << t << endl;
		this_thread::sleep_for(chrono::milliseconds(800));
	}
}

//注意这里传递的是引用，thread的构造函数中要用ref传递
void func3(SyncQueue<uint32_t>& queue)
{
	uint32_t i = 0xFFFFFFFF;
	while (true)
	{
		cout << "push thread2 : " << i << endl;
		queue.Push(i--);
		//i = i % 20;
		this_thread::sleep_for(chrono::milliseconds(500));
	}
}

std::list<shared_ptr<thread>> list_thread;

int main()
{
	SyncQueue<uint32_t> queue(30);
	//func2(queue);
	list_thread.emplace_back(make_shared<thread>(func1, &queue));
	list_thread.emplace_back(make_shared<thread>(func2, &queue));
	//func3传递的是引用，所以这里thread的构造函数中要用ref传递
	list_thread.emplace_back(make_shared<thread>(func3, ref(queue)));

	for (auto& l : list_thread)
	{
		l->join();
	}

	return 0;
}