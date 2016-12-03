int printf(char *shift_expr);

int main(int argc, char *argv[])
{
    for(int i = 0; i < 10; ++i){
        for(int j = 0; j < 15; ++j){


            if(j < 10 && i == j)
                printf("*");
            else 
                printf(" ");

        }
        printf("\n");
    }
    return argc;
}
