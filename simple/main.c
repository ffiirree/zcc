#include <stdio.h>

int main(void)
{
	int a[3] = {45, 46, 47};
	
	int off = 2;

	int *ptr = &a;
	ptr += off;
	*ptr = 89;

	for(int i = 0; i < 3; ++i){
		printf("%d\n", a[i]);
	}

	return 0;
}