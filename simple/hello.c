int printf(char *_out, ...);

int main(int argc, char *argv[])
{
    unsigned int a  = 100;
    unsigned int b = 50;
    a = a / b;
    printf("%d", a);
    return argc;
}
