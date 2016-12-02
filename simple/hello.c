int printf(char *s);

int main(int argc, char *argv[])
{
    for(int i = 0; i < 10; i += 1){
        for(int j = 0; j < 15; j+= 1){
            if(j < i || j > 10){
                printf("*");
            }
        }
        printf("\n");
    }

    return argc;
}
