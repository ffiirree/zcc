int printf(char *_s, int a);

int max(int i, int b);
char max(char c, short d);

int main(void)
{
    int c = 6, d = 0;
    int a = max(c, d);

    printf("%d\n", a);
    return 0;
}

int max(int i, int b)
{
    if(i > b) 
        return i;
    else 
        return b;
}

char max(char c, short d)
{
    if(c > d) 
        return c;
    else 
        return c;
}