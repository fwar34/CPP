#include <inttypes.h>
#include <type_traits>
#include <iostream>
using namespace std;

//先移除引用再移除cv，最终获取了原始类型
template <class T>
typename remove_cv<typename remove_reference<T>::type>::type* Create()
{
	typedef typename remove_cv<typename remove_reference<T>::type>::type U;
	return new U();
}