int printf(char *_out, int a);

char b = 8;

int main(int argc, char *argv[])
{
    char *a = &b;
    int c = *a;

    printf("%d\n", b);
    return argc;
}

