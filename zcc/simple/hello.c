
int global_var;

int main(void)
{
	int local_gar;

	global_var = 2;
	local_gar = 3;

	global_var = global_var + local_gar;

	printf("Hello World!\n");
	return 0;
}
