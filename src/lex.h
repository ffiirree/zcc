#ifndef _ZCC_LEX_H
#define _ZCC_LEX_H

#include <iostream>
#include <string>
#include "file.h"

class Lex {
public:
	Lex(){}
	Lex(const std::string &filename);

	Lex(const Lex &lex) :f(lex.f), keywords(lex.keywords), tokens(lex.tokens), index(lex.index) {  }
	inline Lex operator=(const Lex &lex) { f = lex.f; keywords = lex.keywords; tokens = lex.tokens; index = lex.index; return *this; }

    inline void push_back(const Token &_t) { tokens.push_back(_t); }
	Token next();
	void back();
	Token peek();
    Token peek2();
    bool test(int _id);
    bool test2(int _id);
    bool next_is(const char e);
    bool expect(const char id);
    Pos getPos() { return tokens.at(index).getPos(); }

	void scan(const std::string &filename);
	inline File getCurrentFile() { return f; }

    bool empty() { return tokens.empty(); }
    bool end() { return index == tokens.size(); }
    size_t size() { return tokens.size(); }
    Token &at(size_t i) { return tokens.at(i); }

    void insertFront(Lex &l)
    {
        for (size_t i = l.size(); i > 0; --i) {
            tokens.insert(tokens.begin(), l.at(i - 1));
        }
    }
    void insertBack(Lex &l)
    {
        for (size_t i = 0; i < l.size(); ++i) {
            tokens.push_back(l.at(i));
        }
    }

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

	File f;
	std::vector<std::string> keywords;
	std::vector<Token> tokens;
    int index = 0;
};



#endif // !_ZCC_LEX_H