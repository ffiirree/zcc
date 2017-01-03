#include<stdio.h>

#define  N    19

void main(int argc, char *argv[])
{
    int a = 90;
    int i = sizeof(a) + 90;
    printf("%d\n", i);
}