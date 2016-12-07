#ifndef _ZCC_LEX_H
#define _ZCC_LEX_H

#include <iostream>
#include <string>
#include "file.h"

class Lex {
public:
	Lex(){}
	Lex(const std::string &filename);

	Lex(const Lex &lex) :f(lex.f), keywords(lex.keywords), tokens(lex.tokens), index(index) {  }
	inline Lex operator=(const Lex &lex) { f = lex.f; keywords = lex.keywords; tokens = lex.tokens; index = lex.index; return *this; }

	Token next();
	void back();
	Token peek();
    Pos getPos() { return tokens.at(index).getPos(); }

	void scan(const std::string &filename);
	inline File getCurrentFile() { return f; }

private:
	Token readToken();
	Token read_rep(char exp, int _k, int _else);
	Token read_rep2(char exp1, int _k1, char exp2, int _k2, int _else);
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