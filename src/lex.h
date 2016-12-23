#ifndef _ZCC_LEX_H
#define _ZCC_LEX_H

#include <iostream>
#include <string>
#include "file.h"

/**
 * @class TokenSequence
 */
class TokenSequence {
public:
    TokenSequence() = default;
    TokenSequence(const TokenSequence &ts) : tokens(ts.tokens), index(ts.index), filename_(ts.filename_) { }
    TokenSequence operator=(const TokenSequence &ts) { tokens = ts.tokens; index = ts.index; filename_ = ts.filename_; return *this; }

    void push_back(const Token &t) { tokens.push_back(t); }
    inline size_t size() { return tokens.size(); }
    inline void pop_back() { tokens.pop_back(); }

    inline Token next() { if (index >= tokens.size()) return{ T_EOF, 0 }; return tokens.at(index++); }
    void back();
    inline Token peek() { if (index >= tokens.size()) return{ T_EOF, 0 }; return tokens.at(index); }
    inline Token peek2() { if (index + 1 >= tokens.size()) return{ T_EOF, 0 }; return tokens.at(index + 1); }
    inline bool test(int _id) { return (peek().getType() == T_KEYWORD && peek().getId() == _id);  }
    inline bool test2(int _id) { return (peek2().getType() == T_KEYWORD && peek2().getId() == _id); }
    bool next_is(const char e);
    bool expect(const char id);
    Pos getPos() { return tokens.at(index).getPos(); }

    bool empty() { return tokens.empty(); }
    bool end() { return index >= tokens.size(); }
    Token &at(size_t i) { return tokens.at(i); }
    inline size_t restSize() { return tokens.size() - index; }

    void insertFront(TokenSequence &l);
    void insertBack(TokenSequence &l);
    void insert(TokenSequence &l);

    void setFileName(const std::string &fn) { filename_ = fn; }
    std::string getFileName() { return filename_; }

private:
    std::vector<Token> tokens;
    size_t index = 0;
    std::string filename_;
};

/**
 * @class Lex
 */
class Lex {
public:
    Lex();
    Lex(const std::string &filename, TokenSequence &ts) : Lex() { scan(filename, ts); }

    Lex(const Lex &lex) = delete;
    inline Lex operator=(const Lex &lex) = delete;

    void scan(const std::string &filename, TokenSequence &ts);

private:
	Token readToken();
    void skipComments();
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
    Token last;
};



#endif // !_ZCC_LEX_H