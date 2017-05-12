#include <inttypes.h>
#include <sys/time.h>
#include <iostream>
#include <list>
#include <string>
using namespace std;

int main()
{
	list<string> tokens;
	for (uint32_t i = 0; i < 300; i++)
	{
		tokens.push_back("sssss");
	}
	
	struct timeval tv1, tv11;
	gettimeofday(&tv1, nullptr);
	list<string> tokens_copy = tokens;
	gettimeofday(&tv11, nullptr);
	
	//move将左值变成右值引用，直接会调用list的右值引用为参数的拷贝构造函数，stl都提供了右值引用的拷贝构造和赋值构造函数
	struct timeval tv2, tv22;
	gettimeofday(&tv2, nullptr);
	list<string> tokens_move = move(tokens);
	gettimeofday(&tv22, nullptr);
	
	//43 usec
	cout << "copy time : " << (tv11.tv_sec - tv1.tv_sec) * 1000000 + (tv11.tv_usec - tv1.tv_usec) << " usec" << endl;
	//1 usec
	cout << "move time : " << (tv22.tv_sec - tv2.tv_sec) * 1000000 + (tv22.tv_usec - tv2.tv_usec) << " usec" << endl;
	
	return 0;
}