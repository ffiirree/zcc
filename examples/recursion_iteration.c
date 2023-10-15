#include <stdio.h>

int factorial_recursion(int n);
int factorial_iteration(int n);

int main()
{
    int N                 = 10;
    long recursion_result = factorial_recursion(N);
    long iteration_result = factorial_iteration(N);

    // %ld 输出长整型，即 int int
    printf("The factorial(recursion) of %ld is %ld!\n", N, recursion_result);
    printf("The factorial(iteration) of %ld is %ld!\n", N, iteration_result);

    return 0;
}

// 递归计算阶乘
int factorial_recursion(int n)
{
    if (n <= 0)
        return 1;
    else
        return n * factorial_recursion(n - 1);

    return 0;
}

// 迭代计算阶乘
int factorial_iteration(int n)
{
    int r = 1;
    while (n > 1) {
        r *= n;
        n--;
    }
    return r;
}