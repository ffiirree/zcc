#include <iostream>
#include "zcc.h"
#include "lex.h"

int main(int argc, char *argv[])
{
	Lex lex(*++argv);

	Token t;
	do {
		t = lex.next();
		std::cout << t;
	} while (t.getType() != K_EOF);

	system("pause");
	return 0;
}