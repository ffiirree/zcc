#include <stdio.h>


int glo_var = 9;

int foo(int c, int d, int e, int f);

int main(int argc, char *argv[])
{
    char loc_var = 0;
    loc_var = loc_var + glo_var;

    //printf("hello World");

	return 0;
}

int foo(int c, int d, int e, int f)
{
    // 局部变量输出到四元式，统一用四元式顺序控制
    c = 1;
    f = 9;
    char a = 0;
    int b = 0;
    int a1 = 9;
    int a2 = 9;
    int a3 = 9;
    int a4 = 9;
    int a5 = 9;
    int a6 = 9;
    int a7 = 9;
    return 0;
}