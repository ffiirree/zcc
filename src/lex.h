#ifndef _ZCC_LEX_H
#define _ZCC_LEX_H

#include "source-file.h"
#include "token-sequence.h"
#include "token.h"

class Lex
{
public:
    Lex() = default;
    Lex(const std::string& filename, TokenSequence& ts) { scan(filename, ts); }
    Lex(const Lex&)            = delete;
    Lex& operator=(const Lex&) = delete;
    ~Lex()                     = default;

    /**
     * @berif Scan a file and return the TokenSequence.
     */
    void scan(const std::string& filename, TokenSequence& ts);

private:
    Token next();
    void skipComments();
    Token read_string();
    Token read_char();
    Token read_id(char c);
    Token read_num(char c);

    int read_escaped_char();
    int read_octal_char(int c);
    int read_hex_char();

    /**
     * @berif Cheak whether the string is a keyword.
     */
    int isKeyword(const std::string& str);

    source_file f_;
    Token last_;

    static std::vector<std::string> keywords_;
};

#endif // !_ZCC_LEX_H