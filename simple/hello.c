#include<stdio.h>
#define  N    19
void main(int argc, char *argv[])
{
    int n = 18, a[N][N];
    for (int i = 1;i <= n; ++i) {
        a[i][i] = 1;
        int t1 = a[i][i];
        a[i][1] = t1;
    }
    for (int i = 3;i <= n; ++i) {
        for (int j = 2;j <= i - 1;j++) {
            int t1 = a[i - 1][j];
            int t2 = a[i - 1][j - 1];
            a[i][j] = t1 + t2;
        }
    }

    for (int i = 1; i <= n; ++i) {
        for (int k = 1; k <= n - i; ++k)
            printf("   ", 1);  
        for (int j = 1; j <= i; ++j)
            printf("%6d", a[i][j]);

        printf("\n", 1);
    }
    printf("\n", 1);
}