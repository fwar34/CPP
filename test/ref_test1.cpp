#include <inttypes.h>
#include <iostream>
using namespace std;

void print(uint32_t& i)
{
    cout << "lprint " << i << endl;
}

void print(uint32_t&& i)
{
    cout << "rprint " << i << endl;
}

void Forward(uint32_t&& i)
{
    print(i);
}

int main()
{
    uint32_t i = 0;
    //i����ֵ�����õ�����ֵ������print
    print(i);
    //4��ֵ�����õ�����ֵ������print
    print(4);
    //5����ֵ��������Forward�����д��ݸ�print������ʱ��������ֵ
    //��Ϊ��Forward����print��ʱ����ֵ5�����һ���������󣬱���������������ֵ����
    //������������������ֵ������Ϊ��ֵ����δ��������ֵ������Ϊ��ֵ
    Forward(5);
    
    return 0;
}