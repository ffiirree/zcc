int printf(char *_s, int a);

typedef struct Test{
    int cols;
    int rows;
    char type;
} Hask;
Hask t = {1, 3, 1};

int main(void)
{

    int a = t.rows;
    printf("%d\n", a);

    t.rows = 7;
    printf("%d\n", t.rows);

    return 0;
}