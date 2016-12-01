#include <stdio.h>

int whichDayofYear(int day, int mouth, int year);

int main(int argc, char *argv[])
{
    int sum = 0;

    // sum 0~99
    for(int i = 0; i < 100; i += 1){
       sum += i;
    }

    // 
    whichDayofYear(2, 4, 2016);

	return sum;
}

/**
 * Compute which day of the year
 */
int whichDayofYear(int day, int mouth, int year) 
{ 
    int sum, leap;
    switch(mouth)
    { 
        case 1:  sum = 0;  break; 
        case 2:  sum = 31; break; 
        case 3:  sum = 59; break; 
        case 4:  sum = 90; break; 
        case 5:  sum = 120;break; 
        case 6:  sum = 151;break; 
        case 7:  sum = 181;break; 
        case 8:  sum = 212;break; 
        case 9:  sum = 243;break; 
        case 10: sum = 273;break; 
        case 11: sum = 304;break; 
        case 12: sum = 334;break; 
        default:break; 
    } 

    sum = sum + day;
    
    if(year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
    　　leap = 1; 
    else 
    　　leap = 0; 

    if(leap == 1 && month > 2)
        sum += 1; 

    return sum;
}