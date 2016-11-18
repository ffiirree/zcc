#ifndef _ZCC_PARSER_H
#define _ZCC_PARSER_H

#include "lex.h"

class Parser {
public:
	Parser(Lex &l) :lex(l) {  }
	Parser(const Parser &p) = delete;
	Parser operator=(const Parser &p) = delete;


	bool next_is(const Token &t);

private:
	Lex lex;
};
#endif // !_ZCC_PARSER_H
