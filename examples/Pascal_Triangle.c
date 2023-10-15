#include <stdio.h>

#define N 19

void main(int argc, char *argv[])
{
    int i, j, k, n = 18, a[N][N];
    for (i = 1; i <= n; i++) {
        a[i][i] = 1;
        int t1  = a[i][i];
        a[i][1] = t1;
    }
    for (i = 3; i <= n; i++) {
        for (j = 2; j <= i - 1; j++) {
            int t1  = a[i - 1][j];
            int t2  = a[i - 1][j - 1];
            a[i][j] = t1 + t2;
        }
    }

    for (i = 1; i <= n; i++) {
        for (k = 1; k <= n - i; k++)
            printf("   ");
        for (j = 1; j <= i; j++)
            printf("%6d", a[i][j]);

        printf("\n");
    }
    
    printf("\n");
}