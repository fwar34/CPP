#include <iostream>
#include <cstring>

constexpr size_t ARRAY_LEN = 5;

class RecuriveArray
{
public:
    void SetData(int* array, size_t len)
    {
        memcpy(array_, array, sizeof(int) * len);
    }

    int Max(size_t len)
    {
        if (len == 1) { // 如果只有一个元素就返回
            return array_[0];
        }

        int max = Max(len - 1); // 求出前 n - 1 个元素的最大值 max
        if (array_[len - 1] > max) { // max 与最后一个元素进行比较，返回大的
            return array_[len - 1];
        } else {
            return max;
        }
    }

    int Sum(size_t len)
    {
        if (len == 1) {
            return array_[0];
        }

        int sum = Sum(len - 1);
        return sum + array_[len - 1];
    }

    float Average(size_t len)
    {
        if (len == 1) {
            return array_[0];
        }

        float average = Average(len - 1);
        return (array_[len - 1] + (len - 1) * Average(len - 1)) / len;
    }

    // 递归求数组的平均值
    float Average2(int n)
    {
        if (n == 1)
            return (float)array_[0];
        else
            return ((float)array_[n - 1] + (n - 1) * Average2(n - 1)) / n; // 前
    }

private:
    int array_[ARRAY_LEN] = {0};
    size_t currentSize_;
};

int main()
{
    RecuriveArray array;
    int arr[4] = {1, 2, 3, 4};
    array.SetData(arr, 4);
    std::cout << array.Average(4) << std::endl;

    return 0;
}