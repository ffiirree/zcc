int printf(char *shift_expr, int a);

int main(int argc, char *argv[])
{
    int a = 7;
    int b = 5;
    int c = a % b;

    printf("%d", c);
    return argc;
}
