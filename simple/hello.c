#define Hi "hello"
#define max(a,b) if(a > b) return a; else return b;


int line = __LINE__;
int main() 
{
    int max_a = 8, max_b = 9;
    char *str = Hi;

    max_a = max(max_a, max_b);
    return 0;
}