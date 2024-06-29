// bittest.cpp: 定义应用程序的入口点。
//

#include "bittest.h"

#if 0
struct Foo
{
	uint8_t a : 8 = 0xFF;
	// uint32_t a : 8 = 0xFF;
	uint32_t b : 28 = 0x1234567;
	uint32_t c : 28 = 0x8912345;
};
#else
#pragma pack(1)
struct Foo
{
	uint8_t a : 8 = 0xFF; // vs 中1字节
	// uint32_t a : 8 = 0xFF; // 此位域在 vs 中 uint32_t 位域会占用 4 个字节，而在 gcc 中只会占用 1 个字节，可参考 https://blog.csdn.net/Bluetangos/article/details/135719189?spm=1001.2014.3001.5506
	uint32_t b : 28; // vs 中4字节
	uint32_t c : 28; // vs 中4字节
};
#pragma pack()
#endif

void TestFoo()
{
	Foo foo2 = { 0 };
	std::cout << "sizeof(foo2) = " << sizeof(foo2) << std::endl; // vs 中9字节
	foo2.a = 0xFF;
	foo2.b = 0x1234567;
	foo2.c = 0x8912345;
}

#pragma pack(4)
struct BitStruct
{
	uint8_t a;
	uint8_t b : 1;
	uint8_t c : 2;
	uint8_t d : 3;
};
#pragma pack()

static void TestBitStruct()
{
	char array[4] = { 0 };
	BitStruct* bitStruct = (BitStruct*)array;
	bitStruct->a = 2;
	bitStruct->b = 3;
	bitStruct->c = 4;
	bitStruct->d = 5;
	printf("0x%02x, 0x%02x, 0x%02x, 0x%02x\n", array[0], array[1], array[2], array[3]); // windows 下面结果为 0x02, 0x29, 0x00, 0x00
}

#pragma pack(1)
struct st1
{
    uint8_t  a1 : 2;
    uint8_t  a2 : 6;
    uint16_t b1 : 3;
    uint16_t b2 : 4;
    uint16_t b3 : 5;
    uint16_t b4 : 4;
};
#pragma pack()
 
void TestST1()
{
    struct st1 data = {
        .a1 = 0x1,
        .a2 = 0x3,
        .b1 = 0x4,
        .b2 = 0x8,
        .b3 = 0xb,
        .b4 = 0x3
    };
	uint8_t* array = (uint8_t*)&data;
	std::cout << sizeof(data) << std::endl;
	printf("0x%02x, 0x%02x, 0x%02x, 0x%02x\n", array[0], array[1], array[2], array[3]);

	struct st1 data2 = { 0 };
	data2.a1 = 0x1;
	data2.a2 = 0x3;
	data2.b1 = 0x4;
	data2.b2 = 0x8;
	data2.b3 = 0xb;
	data2.b4 = 0x3;
}

void TestKuaUnit()
{
#pragma pack(1)
	struct TestUnit
	{
		uint8_t a : 5;
		uint8_t b : 5;
		uint8_t c : 3;
		uint8_t d : 7;
	} testUnit = { 0 };
#pragma pack()

	std::cout << "TestKuaUnit " << sizeof(testUnit) << std::endl;

	testUnit.a = 0x3;
	testUnit.b = 0x4;
	testUnit.c = 0x6;
	testUnit.d = 0x8;
}

extern void TestAdtsHeader();

int main()
{
	std::cout << "Hello CMake." << std::endl;
	std::cout << sizeof(Foo) << std::endl;
	Foo foo;
	std::cout << foo.a << std::endl;
	TestFoo();
	TestBitStruct();
	TestAdtsHeader();
	TestST1();
	TestKuaUnit();
	
	return 0;
}
