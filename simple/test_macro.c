#include <stdio.h>

#define MACRO_A 1
#define MACRO_B 0

#if MACRO_A && MACRO_B
#define Hi "Hi"
#else 
#define Hi "Hello"
#endif

int main(void)
{
    char *s hi = Hi;
    return 0;
}