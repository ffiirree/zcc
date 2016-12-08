#include<stdio.h>

#define Hi "hello"
#define max(a,b) do{ if(a > b) return a; else return b;} while(0 != 0)


int line = __LINE__;
int main() 
{
    int max_a = 8, max_b = 9;
    char *str = Hi;

    max(max_a, max_b);
    return 0;
}