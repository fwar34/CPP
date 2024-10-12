//C++11 P105
#include <iostream>
#include <type_traits>
#include <tuple>
using namespace std;

//递归终结函数，索引等于tuple元素个数就停止（4 == 4）
template <size_t I = 0, class Tuple>
typename enable_if<I == tuple_size<Tuple>::value>::type printtp(Tuple t)
{
	cout << "I = " << I << endl;
}

//参数包展开函数（递归函数），索引小于tuple的元素个数就递归
template <size_t I = 0, class Tuple>
typename enable_if<I < tuple_size<Tuple>::value>::type printtp(Tuple t)
{
	cout << "I = " << I << " " << get<I>(t) << endl;
	printtp<I + 1>(t);
}

template <class... Args>
void print(Args... args)
{
	printtp(make_tuple(args...));
}

int main()
{
	print(11, 22, 33, 44);
	
	return 0;
}