#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>

using namespace std;
void test_copy()
{
    std::string str = "";
    std::string str1;
    if (str.empty() && str1.empty()) {
        std::cout << "empty" << std::endl;
    }

    std::ostringstream oss;
    std::vector<uint32_t> test = {1, 2, 3, 4};
    std::copy(test.begin(), test.end(), std::ostream_iterator<uint32_t>(oss, " "));
    cout << oss.str() << endl;

    std::for_each(test.begin(), test.end(), [&](const uint32_t i){oss << i << " ";});
    cout << oss.str() << endl;
}

void test_copy_if()
{
    std::vector<int> v1{1, 2, 3, 4, 5};
    std::vector<int> v2;
    std::copy_if(v1.begin(), v1.end(), std::back_inserter(v2), [](int x) { return x % 2 == 0; });
    for (int x : v2) {
        std::cout << x << ' ';
    }
    std::cout << '\n';

    std::cout << "0--0------------------\n";
    std::copy_if(v1.begin(), v1.end(), std::ostream_iterator<int>(std::cout, " "), [] (int x) { return x % 2 == 0; });
    std::cout << '\n';
    std::copy(v1.begin(), v1.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
}
 
int main()
{
    std::vector<int> v1{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> v2{            5,    7,    9, 10};
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
 
    std::vector<int> v_symDifference;
 
    std::set_symmetric_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                                  std::back_inserter(v_symDifference));
 
    for (int n : v_symDifference)
        std::cout << n << ' ';
    std::cout << '\n';
    test_copy_if();
    test_copy();
}
