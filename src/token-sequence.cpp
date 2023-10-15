#include "token-sequence.h"

#include "logging.h"

void TokenSequence::back()
{
    if (index_ == 0) log_e("begin of token sequence.");
    index_--;
}

bool TokenSequence::next_is(const char e)
{
    if (peek().getType() == T_KEYWORD && peek().getId() == e) {
        index_++;
        return true;
    }

    return false;
}

bool TokenSequence::expect(const char id)
{
    if (next_is(id)) return true;
    log_e(tokens_.at(index_).location(), "expect : " + (char)id);
    return false;
}

void TokenSequence::insertFront(TokenSequence l)
{
    for (size_t i = l.size(); i > 0; --i) {
        tokens_.insert(tokens_.begin(), l.at(i - 1));
    }
}
void TokenSequence::insertBack(TokenSequence l)
{
    for (size_t i = 0; i < l.size(); ++i) {
        tokens_.push_back(l.at(i));
    }
}

void TokenSequence::insert(TokenSequence l)
{
    for (size_t i = l.size(); i > 0; --i) {
        tokens_.insert(tokens_.begin() + index_, l.at(i - 1));
    }
}

std::ostream& operator<<(std::ostream& os, const TokenSequence& ts)
{
    for (int i = 0; i < ts.size(); ++i) {
        os << ts.at(i);
    }
    return os;
}