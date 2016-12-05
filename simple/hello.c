int printf(char *_s, int a);

int main(void)
{
    int a[10] = {0, 9, 8};
    a[4] = 9;

    printf("%d\n", a[4]);
    return 0;
}
