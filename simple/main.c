#include <stdio.h>

int main(void)
{	
	int i = 9;
	int a = 8;
	if(a > i--){
		printf("%d\n", i);
	}
	printf("%d\n", i);
	return 0;
}