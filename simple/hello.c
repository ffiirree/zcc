

#if 9 > 80
    #if 8 > 7
        #define Hi "HELLO"
    #endif
#elif 4 < 5
    #if __LINE__
    #define Hi "Hii"
    #endif
#endif

int main() 
{
    char *s  = Hi;
    return 0;
}