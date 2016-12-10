#include<stdio.h>
#define _o(x) putchar(x)


struct Point {
    int x;
    int y;
    int z;
};

int main() 
{
    int a[10] = {6, 5, 4, 2};
    int *ptr = &a;
    ++ptr;
    ptr[2] = 89;
    int b = ptr[2];
    
    printf("%d\n", b);

    return 0;
}