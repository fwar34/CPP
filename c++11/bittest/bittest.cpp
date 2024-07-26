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
	// 在vs和gcc内存中查看跨字节的位域，并且看下vs和gcc是否位域都压缩了么
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
	// 在vs和gcc内存中查看对应的位域，并且查看输出
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
	// 位域跨越了位域定义的单元（uint8）
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

void TestKuaUint2()
{
	// 位域跨越了位域定义的单元（uint16_t)
#pragma pack(1)
	struct TestUint
	{
		uint16_t a : 4;
		uint16_t b : 14;
		uint16_t c : 15;
	}testUint = { 0 };
#pragma pack()
	std::cout << "TestKuaUint2 " << sizeof(testUint) << std::endl;

	testUint.a = 0x7;
	testUint.b = 0x643;
	testUint.c = 0x987;
}

void TestKuaUnit3()
{
#pragma pack(1)
	struct TestUint
	{
		uint8_t a : 5;
		uint16_t b : 10;
		uint16_t c : 10;
	} testUnit = { 0 };
#pragma pack()
	std::cout << "TestKuaUint3 " << sizeof(testUnit) << std::endl;

	testUnit.a = 0x7;
	testUnit.b = 0x643;
	testUnit.c = 0x987;
}

void Test3()
{
#pragma pack(1)
	struct Pack
	{
		uint32_t a;
		uint8_t c1;
		uint8_t c2;
		uint8_t c3;
		uint8_t c4;
	} pack = { 0 };
#pragma pack()
	pack.a = 0x12;
	pack.c1 = 5;
	pack.c2 = 6;
	pack.c3 = 7;
	pack.c4 = 8;
	uint32_t *pc = (uint32_t*)&pack + 1;
	// 清零c2
	*pc = *pc & 0xFFFF00FF;

	std::cout << "Test3: " << std::endl;
	std::cout << pack.a << std::endl;
	printf("%u\n", pack.c1);
	printf("%u\n", pack.c2);
	printf("%u\n", pack.c3);
	printf("%u\n", pack.c4);

	uint16_t* p16 = (uint16_t*)&(pack.c1);
	printf("%u\n", *p16);
	if (*p16 = 0x600) {
		std::cout << "equal" << std::endl;
	}



	// std::cout << (*pc) & 0xFFFF00FF;
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
	TestKuaUint2();
	TestKuaUnit3();
	Test3();
	
	return 0;
}
