#include <inttypes.h>
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <fstream>
#include <cstdio>
#include <functional>
using namespace std;

struct B
{
	virtual void bar()
	{
		cout << "B::bar() called" << endl;
	}
	virtual ~B() = default;
};

struct D : public B
{
	D()
	{
		cout << "D::D() called" << endl;
	}
	
	~D()
	{
		cout << "D::~D() called" << endl; 
	}
	
	void bar() override
	{
		cout << "D::bar() called" << endl;
	}
};

// a function consuming a unique_ptr can take it by value or by rvalue reference
unique_ptr<D> pass_through(unique_ptr<D> up)
{
	if (up)
	{
		up->bar();
	}
	return up;
}

int main()
{
	//unique ownership semantics demo
	unique_ptr<D> up1(new D);
	auto up2 = pass_through(move(up1));
	assert(!up1);
	up2->bar();
	
	//Runtime polymorphism demo
	unique_ptr<B> up3(new D);
	up3->bar();
	
	vector<unique_ptr<B>> v; //unique_ptr can be stored in a container
	v.push_back(unique_ptr<B>(new D));
	v.push_back(move(up1));
	v.emplace_back(new D);
	for (auto& i : v)
	{
		if (i)
		{
			i->bar();
		}
	}
	
	//Custom deleter demo
	ofstream ("demo.txt") << 'x';
	unique_ptr<FILE, decltype(&fclose)> fp(fopen("demo.txt", "r"), &fclose);
	if (fp)
	{
		cout << fgetc(fp.get()) << endl;
	}
	
	//Custom lambda-expression deleter demo
	unique_ptr<D, function<void(D*)>> up4(new D, [](D* pd)
	{
		if (pd)
		{
			cout << "destroying from a custom deleter..." << endl;
			delete pd;
		}
	});
	up4->bar();
	
	//Array form of unique_ptr demo
	unique_ptr<D[]> up5(new D[5]);
	unique_ptr<D[]> up6{new D[3]};
	
	return 0;
}