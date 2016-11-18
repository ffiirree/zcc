#include "parser.h"

bool Parser::next_is(const Token &t)
{
	if (lex.next() == t)
		return true;
	lex.back();
	return false;
}