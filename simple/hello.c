#include <stdio.h>

#define MACRO_A 1
#define MACRO_B 2

#if MACRO_A && MACRO_B
#define Hi "Hello~"
#else 
#define Hi "Hi~"
#endif

#undef MACRO_A
#undef MACRO_B

int main(void)
{
    char *s hi = Hi;
    return 0;
}