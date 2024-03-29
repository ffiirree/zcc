#ifndef _ZCC_PP_H
#define _ZCC_PP_H

#include "token-sequence.h"

#include <ctime>

class Date
{
public:
    Date() { reset(); }
    Date(const Date&)            = delete;
    Date& operator=(const Date&) = delete;
    ~Date()                      = default;

    static std::string getDate()
    {
        Date date;
#define __s(x) std::to_string(x)
        return __s(date.getYear()) + "-" + __s(date.getMouth()) + "-" + __s(date.getHour()) + "-" +
               __s(date.getMin()) + "-" + __s(date.getSec());
#undef __s
    }

    inline int getYear()
    {
        reset();
        return t->tm_year + 1'900;
    }
    inline int getMouth()
    {
        reset();
        return t->tm_mon + 1;
    }
    inline int getHour()
    {
        reset();
        return t->tm_hour;
    }
    inline int getMin()
    {
        reset();
        return t->tm_min;
    }
    inline int getSec()
    {
        reset();
        return t->tm_sec;
    }

private:
    time_t tt;
    tm *t;

    inline void reset()
    {
        tt = time(NULL);
        t  = localtime(&tt);
    }
};

/**
 * @class Macro
 */
class Macro
{
public:
    enum MacroType
    {
        M_ZERO,
        M_FUNCLIKE,
        M_OBJLIKE,
        M_PRE,
    };

public:
    Macro()
        : name_(),
          params_(),
          replist_()
    {}
    Macro(const std::string& n, MacroType ty)
        : name_(n),
          type_(ty),
          params_(),
          replist_()
    {}
    Macro(const std::string& n, const Token& v, MacroType ty)
        : name_(n),
          type_(ty),
          params_()
    {
        replist_.push_back(v);
    }

    Macro(const Macro& m)
        : type_(m.type_),
          name_(m.name_),
          params_(m.params_),
          replist_(m.replist_)
    {}
    Macro& operator=(const Macro& m)
    {
        type_    = m.type_;
        name_    = m.name_;
        params_  = m.params_;
        replist_ = m.replist_;
        return *this;
    }
    ~Macro() = default;

    MacroType type_ = M_ZERO;
    std::string name_;
    std::vector<std::string> params_;
    TokenSequence replist_;
};

/**
 * @class Preprocessor
 */
class Preprocessor
{
public:
    Preprocessor(bool _only)
        : isOnlyPP(_only)
    {
        init();
    }
    Preprocessor(const Preprocessor&)            = delete;
    Preprocessor& operator=(const Preprocessor&) = delete;
    ~Preprocessor()                              = default;

    /**
     * @berif macro expansion
     */
    void expand(TokenSequence is, TokenSequence& os);

    /**
     * \ A quick overview of subst is that it walks through the input sequence, IS, building up an output
     * sequence, \ OS, by handling each token from left to right. (The order that this operation takes is
     * left to the \ implementation also, walking from left to right is more natural since the rest of the
     * algorithm is constrained \ to this ordering.) Stringizing is easy, pasting requires trickier handling
     * because the operation has a bunch \ of combinations. After the entire input sequence is finished, the
     * updated hide set is applied to the output \ sequence, and that is the result of subst
     */
    void subst(TokenSequence& is, const std::vector<std::string>& fp, const TokenSequence& ap, HideSet *hs,
               TokenSequence& os);

    /**
     * @berif Paste last of left side with first of right side.
     */
    void glue(TokenSequence& ls, TokenSequence& rs);

    /**
     * @berif Add to token sequence's hide sets.
     */
    void hasadd(HideSet *hs, TokenSequence& ts);

    /**
     * @berif Given a macro-name token, ts returns the replacement token
     *        sequence from the macro's definiton.
     */
    TokenSequence ts(const std::string& _macro_name);

    /**
     * @berif Given a macro-name token, fp returns the (ordered) list of formal
     *        parameters from the macro's defintion.
     */
    TokenSequence fp(const std::string& _macro_name);

    /**
     * @berif Given a token sequence and an index i, 'select' returns the i-th
     *        token sequence using the comma tokens in the original token sequence
     *        as delimiters.
     */
    TokenSequence select(int i, const TokenSequence& ts);

    /**
     * @berif Given a token sequence, stringize returns a single string token
     *        containing the concatenated spellings of tokens.
     */
    TokenSequence stringize(const TokenSequence& ts);
    TokenSequence getAP(TokenSequence& is);
    int isInFP(const Token& t, const std::vector<std::string>& fp);

    Macro *searchMacro(const std::string& _n);
    int isMacro(const std::string& _n);
    bool deleteMacro(const std::string& _n);

    /**
     * @berif group part
     * \ #include
     * \ #define
     * \ #if
     * \ #ifndef
     * \ #ifdef
     * \ #undef
     * \ #elif
     * \ #else
     * \ #endif
     */
    void group_part(TokenSequence& is, TokenSequence& os);
    void Include(TokenSequence& is, TokenSequence& os);
    void Define(TokenSequence& is);
    void If(TokenSequence& is);
    void Ifndef(TokenSequence& is);
    void Ifdef(TokenSequence& is);
    void Undef(TokenSequence& is);
    void Elif(TokenSequence& is);
    void Else(TokenSequence& is);
    void Endif(TokenSequence& is);
    void Line(TokenSequence& is);
    void Pragma(TokenSequence& is);

    inline bool invalid() const { return invalid_; }

private:
    void init();
    bool cheak_else();

    std::vector<Macro> macros_;
    std::vector<std::string> paths_;
    bool isOnlyPP = false;

    std::vector<std::pair<std::string, bool>> stk_if_else;

    bool invalid_    = false;
    bool preInvalid_ = false;
    std::vector<bool> stK_invalid;

    bool isExpandExpr = false;
};

#define _BEGIN_IF_()                                                                                       \
    do {                                                                                                   \
        stK_invalid.push_back(preInvalid_);                                                                \
        preInvalid_ = invalid_;                                                                            \
        invalid_    = false;                                                                               \
    } while (0)
#define _END_IF_()                                                                                         \
    do {                                                                                                   \
        preInvalid_ = stK_invalid.back();                                                                  \
        stK_invalid.pop_back();                                                                            \
        invalid_ = preInvalid_;                                                                            \
    } while (0)

#endif
