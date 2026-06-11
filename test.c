#include <stdio.h>

typedef struct test
{
	/* data */
	int a;
	int b;
	double c;
} test;

int main() {
	int a, y;
	int b = 2;
	int c = 0;
	int d[3] = {0};
	c = a << b;
	c = a >> b;
	a &= b;
	a |= b;
	a ^= b;
	c = (a > b) ? a : b;

	
	return c;
}
