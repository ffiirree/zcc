#ifndef ZCC_TOKEN_SEQUENCE_H
#define ZCC_TOKEN_SEQUENCE_H

#include "token.h"

class TokenSequence
{
public:
    TokenSequence() = default;
    TokenSequence(const TokenSequence& ts)
        : tokens_(ts.tokens_),
          index_(ts.index_),
          filename_(ts.filename_)
    {}

    TokenSequence& operator=(const TokenSequence& ts)
    {
        tokens_   = ts.tokens_;
        index_    = ts.index_;
        filename_ = ts.filename_;
        return *this;
    }

    ~TokenSequence() = default;

    /**
     * @berif Get, peek or test a token.
     */
    inline Token& next()
    {
        if (index_ >= tokens_.size()) return *(new Token());
        return tokens_.at(index_++);
    }
    inline Token& at(size_t i) { return tokens_.at(i); }
    inline Token at(size_t i) const { return tokens_.at(i); }
    void back();
    inline Token peek() const
    {
        if (index_ >= tokens_.size()) return {};
        return tokens_.at(index_);
    }
    inline Token peek2() const
    {
        if (index_ + 1 >= tokens_.size()) return {};
        return tokens_.at(index_ + 1);
    }
    inline bool test(int id) const { return (peek().getType() == T_KEYWORD && peek().getId() == id); }
    inline bool test2(int id) const { return (peek2().getType() == T_KEYWORD && peek2().getId() == id); }

    /**
     * @berif Cheak whether the next is character 'e'.
     */
    bool next_is(const char e);
    bool expect(const char id);

    // location of current token in source file
    source_location location() const { return tokens_.at(index_ - 1).location(); }

    inline void push_back(const Token& t) { tokens_.push_back(t); }
    inline void pop_back() { tokens_.pop_back(); }

    inline size_t size() const { return tokens_.size(); }
    inline bool empty() const { return tokens_.empty(); }
    inline bool end() const { return index_ >= tokens_.size(); }
    inline size_t restSize() const { return tokens_.size() - index_; }

    /**
     * @berif Insert a TokenSequence to this.
     * \ the firs postion
     * \ the last postion
     * \ current postion
     */
    void insertFront(TokenSequence l);
    void insertBack(TokenSequence l);
    void insert(TokenSequence l);

    inline void setFileName(const std::string& fn) { filename_ = fn; }
    inline std::string getFileName() const { return filename_; }

private:
    std::vector<Token> tokens_;
    size_t index_ = 0;
    std::string filename_;
};

std::ostream& operator<<(std::ostream& os, const TokenSequence& ts);

#endif //! ZCC_TOKEN_SEQUENCE_H