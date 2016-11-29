#include <stdio.h>

int global_var;

void foo()
{
	int a;
}

int main(void)
{
	int local_gar;
	int a, b , c, d;

loop:
	// test func call
	foo();

	goto loop;
	// test do_while_stmt
	do{
		global_var = local_gar + 4;
	}while(global_var > 1);

	// test if_stmt
	if(3 != 4){
		if(5 < 6){
			local_gar = global_var;
		}
	}
	else {
		local_gar = 5 * 6;
	}

	// test while
	while (3 + 4 > 5)
	{
		local_gar +=1;
	}

	// test + - * / %
	local_gar = 3 * 5 / 4 + 7 * 8;
	local_gar += 4 * 5;
	
	// test > < >= <=
	a * b > 5 + c <= 5 / 4 * d;

	// test == !=
	4 ==5 *7 +9;
	4 *6 != 9;

	// test ^ | &
	55 & 45 + 44 ^ 3 | 3;

	// test || &&
	334 || 26 && 78 || 12;


	return global_var;
}
