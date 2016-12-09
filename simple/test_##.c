#include <stdio.h>

#define str(a, b, c, d) a##b##c##d


int main() 
{
    printf("%s\n", str(11, 12, 13, 14))
    return 0;
}

