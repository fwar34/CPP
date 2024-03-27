#include <iostream>

void BubbleSort(int array[], size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        for (size_t j = i + 1; j < len; ++j) {
            if (array[i] > array[j]) {
                int tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }
    }
}

int main()
{
    int array[] = {3, 4, 9, 1, -3, 100};
    // std::cout << sizeof array << std::endl;
    BubbleSort(array, sizeof array / sizeof array[0]);
    for (int i : array) {
        std::cout << i << std::endl;
    }

    return 0;
}