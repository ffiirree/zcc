int printf(char *s, int a);

enum en{
    ONE = 9,
    TWO,
    THREE,
};

int main(void)
{
    int a[10] = {0, 9, 8};

    a[3] = THREE;

    int b = 5;

    b = 5 * TWO + THREE;

    printf("%d\n", a[3]);
    printf("%d\n", b);

    return 0;
}
