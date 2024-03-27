// 假设以数组Q[m]存放循环队列中的元素, 同时以rear和length分别指示环形队列中的队尾位置和队列中所含元素的个数。
// 试给出该循环队列的队空条件和队满条件, 并写出相应的插入(enqueue)和删除(dlqueue)元素的操作。
#include <iostream>
#include <cstring>

constexpr size_t BUFFER_LEN = 4;

template <typename T>
class CircleBuffer
{
public:
    CircleBuffer() : rear_(BUFFER_LEN - 1), length_(0)
    {
        memset(buffer_, 0, sizeof buffer_);
    }
    CircleBuffer(const CircleBuffer<T>&) = delete;
    CircleBuffer<T>& operator =(const CircleBuffer<T>&) = delete;
    static CircleBuffer<T>& GetInstance()
    {
        static CircleBuffer<T> instance;
        return instance;
    }

    bool Empty()
    {
        return length_ == 0;
    }

    bool Full()
    {
        return length_ == BUFFER_LEN;
    }

    bool Enqueue(const T& t)
    {
        if (Full()) {
            return false;
        }

        length_++;
        rear_ = (rear_ + 1) % BUFFER_LEN;
        buffer_[rear_] = t;
        return true;
    }

    bool Dequeue(T& t)
    {
        if (Empty()) {
            return false;
        }

        length_--;
        t = buffer_[(rear_ - length_ + BUFFER_LEN) % BUFFER_LEN];
        return true;
    }

    bool Front(T& t)
    {
        if (Empty()) {
            return false;
        }

        t = buffer_[(rear_ - length_ + 1 + BUFFER_LEN) % BUFFER_LEN];
        return true;
    }

private:

    T buffer_[BUFFER_LEN];
    size_t rear_; // 队尾，指向最后一个元素
    size_t length_; // 含有元素个数
};

int main()
{
    CircleBuffer<int> buffer;
    for (size_t i = 0; i < BUFFER_LEN + 1; ++i) {
        if (!buffer.Enqueue(i)) {
            std::cout << "enqueue index " << i << " failed!" << std::endl;
        }
    }

    for (size_t i = 0; i < BUFFER_LEN + 1; ++i) {
        int ret = 0;
        if (!buffer.Dequeue(ret)) {
            std::cout << "dequeue index " << i << " failed!" << std::endl;
        } else {
            std::cout << "ret = " << ret << std::endl;
        }
    }

    return 0;
}