int printf(char *s, int a);

int max(int a, int b)
{
    if(a > b) return a; 
    else return b;
}

int main(int argc, char *argv[])
{
    int ac = max(12, 13);

    printf("%d\n", ac);
    return argc;
}
