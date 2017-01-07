#include<stdio.h>

int main() 
{
    int a = 90, b = 78;

    a ^= b;
    b ^= a;
    a ^= b;
    printf("%d", a);

	return 0;
}