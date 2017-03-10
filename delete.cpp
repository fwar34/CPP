//析构函数在del函数后面实现，编译器会警告未定义的析构，最后运行程序析构就是没有被调用
#include <stdio.h>
class abc;
void del(abc *pobj)
{ 
    delete pobj;

}
class abc{
public: 
    abc()
    {
     printf("abc\r\n"); 
     } 
     ~abc()
     {
     printf("~abc\r\n"); 
     }
 };

/*void del(abc *pobj)
{
    delete pobj;

}*/

 int main(int argc, char *argv[])
 { 
    abc *pobj = new abc; 
    del(pobj);
    return 0;
 }
