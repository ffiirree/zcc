#include<stdio.h>

#define  N    19

void main(int argc, char *argv[])
{
    int i, j, k, n = 18, a[N][N];                  /*定义二维数组a[14][14]*/
    for (i = 1;i <= n;i++) {
        a[i][i] = 1;
        int t1 = a[i][i];
        a[i][1] = t1;
    }
    for (i = 3;i <= n;i++) {
        for (j = 2;j <= i - 1;j++) {
            int t1 = a[i - 1][j];
            int t2 = a[i - 1][j - 1];
            a[i][j] = a[i - 1][j] + t2;                     /*除两边的数外都等于上两顶数之和*/
        }
    }


    for (i = 1; i <= n; i++) {
        for (k = 1; k <= n - i; k++)
            printf("   ", 1);  
        for (j = 1; j <= i; j++)
            printf("%6d", a[i][j]);

        printf("\n", 1);
    }
    printf("\n", 1);
}