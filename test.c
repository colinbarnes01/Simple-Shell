#include <stdio.h>

void f(int *p);

int main()
{

	int x = 3;
	int *ptr = &x;
	f(ptr);
	printf("dereference ptr: %d\n", *ptr);

return 0;
}

void f(int *p)
{
	int y = 4;
	*p = y;
}
