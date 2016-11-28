int global_var;                                      // 2

int main(void)                                       // 7
{                                                    // 8
	int local_gar;                                   // 11

	global_var = 2;                                  // 15
	local_gar = 3;                                   // 19

	global_var = global_var + local_gar;             // 25

	printf("Hello World!\n");
	return 0;
}
