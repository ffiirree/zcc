#include <stdio.h>
int main(){
    float pi = 1.0;
    float n = 1.0;

    for(int j = 1; j <= 100; j++,n += 1.0){
        if(j%2 == 0){
            pi *= n / (n + 1.0);
        }
        else{
            pi *= (n + 1.0) / n;
        }
    }
    pi = 2.0 * pi;
    printf("pi = %.7f\n",pi);
    return 0;
}