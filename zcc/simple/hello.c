#include <stdio.h>

int global_var;

int foo()
{
	int foo_var;
	foo_var = 6 + 3 * 4;
	return foo_var;
}

int main(void)
{
	int local_gar;
	local_gar = foo();

	if(local_gar > 1)
		global_var = 2;

	global_var = global_var + local_gar;

	return global_var;
}
