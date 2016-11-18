#include <iostream>
#include "zcc.h"
#include "lex.h"

int main(int argc, char *argv[])
{
	Lex lex(*++argv);

	system("pause");
	return 0;
}