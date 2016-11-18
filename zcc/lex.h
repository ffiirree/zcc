#ifndef _ZCC_LEX_H
#define _ZCC_LEX_H

#include <iostream>
#include <vector>
#include "zcc.h"
#include "file.h"

class Lex {
public:
	Lex(const std::string &filename);

	Lex(Lex &lex) = delete;
	Lex operator=(Lex &lex) = delete;


	Token next();

	

	int isKeyword(std::string &word);
	bool next_is(char e);

	

	
private:

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


	File f;
	std::vector<std::string> keywords;
};
std::ostream &operator<<(std::ostream & os, const Token & t);

#endif // !_ZCC_LEX_H