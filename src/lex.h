#ifndef _ZCC_LEX_H
#define _ZCC_LEX_H

#include <iostream>
#include <string>
#include "file.h"

class Lex {
public:
	Lex(){}
	Lex(const std::string &filename);

	Lex(const Lex &lex) :f(lex.f), keywords(lex.keywords), tokens(lex.tokens), index(lex.index), last(lex.last) {  }
    inline Lex operator=(const Lex &lex) { f = lex.f; keywords = lex.keywords; tokens = lex.tokens; index = lex.index; last = lex.last; return *this; }

    inline void push_back(const Token &_t) { tokens.push_back(_t); }
    inline void pop_back() { tokens.pop_back(); }
	Token &next();
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
    bool end() { return index >= tokens.size(); }
    size_t size() { return tokens.size(); }
    Token &at(size_t i) { return tokens.at(i); }

    void insertFront(Lex &l);
    void insertBack(Lex &l);
    void insert(Lex &l);

    inline size_t restSize() { return tokens.size() - index; }

private:
	Token readToken();
	Token read_op(char exp, int _k, int _else);
	Token read_op2(char exp1, int _k1, char exp2, int _k2, int _else);
	Token read_string();
	Token read_char();
	Token read_id(char c);
	Token read_num(char c);


	int read_escaped_char();
	int read_octal_char(int c);
	int read_hex_char();

	int isKeyword(std::string &word);

	File f;
	std::vector<std::string> keywords;
	std::vector<Token> tokens;
    size_t index = 0;
    Token last;
};



#endif // !_ZCC_LEX_H