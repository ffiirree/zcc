#include <stdio.h>

int main(void)
{
	int a[3] = {45, 46, 47};
	int *ptr = a;
	ptr +=1;
	*ptr = 3;
	if(a[1] == 3)
		putchar('Y');
	else
		putchar('N');

	return 0;
}