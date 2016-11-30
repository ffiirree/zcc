#include <iostream>
#include "zcc.h"

int main(int argc, char *argv[])
{
	Parser parser(*++argv);

	parser.trans_unit();

	Generate gen("hello.qd");
	for (;;) {
		std::vector<std::string> quad = gen.getQuad();

		if (quad.empty())
			break;
		gen.getReg(quad);
	}

	//Lex lex(*++argv);
	//Token t;
	//do {
	//	t = lex.next();
	//	std::cout << t;
	//} while (t.getType() != K_EOF);

	system("pause");
	return 0;
}