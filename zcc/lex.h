#ifndef _ZCC_LEX_H
#define _ZCC_LEX_H

#include <iostream>
#include <string>
#include "file.h"

class Lex {
public:
	Lex(const std::string &filename);

	Lex(Lex &lex) :f(lex.f), keywords(lex.keywords), tokens(lex.tokens), index(index) {  }
	inline Lex operator=(Lex &lex) { f = lex.f; keywords = lex.keywords; tokens = lex.tokens; index = lex.index; return *this; }

	Token next();
	void back();
	Token peek();

	void scan(const std::string &filename);

private:
	Token readToken();
	Token read_rep(char exp, int _k, char _else);
	Token read_rep2(char exp1, int _k1, char exp2, int _k2, char _else);
	Token read_string(char c);
	Token read_char();
	Token read_id(char c);
	Token read_num(char c);


	int read_escaped_char();
	int read_octal_char(int c);
	int read_hex_char();
	int read_universal_char(int len);
	bool nextoct();

	int isKeyword(std::string &word);
	bool next_is(char e);

	File f;
	std::vector<std::string> keywords;
	std::vector<Token> tokens;
	int index;
};



#endif // !_ZCC_LEX_H