#include <stdio.h>

struct test
{
	int a;
	int b;
	int c;
};

int main()
{
	struct test t;
	printf("t.a(%p)\nt.b(%p)\nt.c(%p)\n", &(t.a), &(t.b), &(t.c));
	printf("t(%p)\n", &t);

	return 0;
}

