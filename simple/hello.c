int printf(char *_s, int a);

typedef struct Test{
    char type;
    int cols;
    int rows;
} Hask;


int main(void)
{
    Hask t = {2, 3, 4};

    t.cols = 5;
    printf("%d\n", t.cols);
    return 0;
}