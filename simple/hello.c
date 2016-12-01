#include <stdio.h>

char ch = 3;

int glo_var = 9;

int main(int argc, char *argv[])
{
    char loc_var = 0;

    do{
        if(3 > 4){
            loc_var = 5;
        }
    }while(loc_var != 6);
        
	return 0;
}
