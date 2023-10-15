#ifndef ZCC_TOKEN_H
#define ZCC_TOKEN_H

#include "source-location.h"
#include "type.h"

class Token
{
public:
    Token()
        : kind_(T_EOF),
          id_(0)
    {}
    Token(int id)
        : kind_(T_KEYWORD),
          id_(id)
    {}
    Token(int kind, const std::string& sval)
        : kind_(kind),
          sval_(sval)
    {}
    Token(int kind, char ch)
        : kind_(kind),
          ch_(ch)
    {}
    Token(const Token& t) { copying(t); }
    Token& operator=(const Token& t)
    {
        copying(t);
        return (*this);
    }
    ~Token()
    {
        if (kind_ == T_IDENTIFIER || kind_ == T_STRING || kind_ == T_INTEGER || kind_ == T_FLOAT)
            sval_.~basic_string();
    }

    inline int getType() const { return kind_; }

    source_location location() const { return location_; }
    void location(const source_location& loc) { location_ = loc; }

    inline int getId() const { return id_; }
    inline std::string getSval() const { return sval_; }
    inline int getCh() const { return ch_; }

    std::string toString() const;
    void setBOL() { isbol_ = true; }
    bool isBOL() { return isbol_; }

    bool needExpand();

    int kind_ = 0;
    source_location location_{};
    bool isbol_  = false;
    HideSet *hs_ = nullptr;

    union
    {
        int id_;           // which keyword or operator
        std::string sval_; // STRING value
        int ch_;           // CHAR_ value
    };

private:
    void copyUnion(const Token& t);
    void copying(const Token& t);
};

std::ostream& operator<<(std::ostream& os, const Token& t);

#endif //! ZCC_TOKEN_H