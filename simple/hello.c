#include<stdio.h>

int main() 
{
    int arr[4][5][5] = {11, 22, 33, 44};

    arr[3][4][3] = 45;
    int b = arr[3][4][3];

    printf("%d\n", b);

    return 0;
}