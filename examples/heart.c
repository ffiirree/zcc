#include <stdio.h>

int main()
{
    for (float y = 1.5; y > -1.5; y -= 0.1) {
        for (float x = -1.5; x < 1.5; x += 0.05) {
            float a = x * x + y * y - 1;
            a *a *a - x *x *y *y *y <= 0.0 ? putchar('*') : putchar(' ');
        }
        putchar('\n');
    }
    
    return 0;
}