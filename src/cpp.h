#ifndef _ZCC_PP_H
#define _ZCC_PP_H

#include <fstream>
#include <ctime>
#include "zcc.h"

typedef enum {
    M_ZERO,
    M_FUNCLIKE,
    M_OBJLIKE,
    M_PRE,
}MacroType;


class Date {
#define __s(x) std::to_string(x)
public:
    Date() { reset(); }

    static std::string getDate() {
        Date date;
        return __s(date.getYear()) + "-" + __s(date.getMouth()) + "-" + __s(date.getHour()) + "-" + __s(date.getMin()) + "-" + __s(date.getSec());
    }

    inline int getYear() { reset(); return t->tm_year + 1900; }
    inline int getMouth() { reset(); return t->tm_mon + 1; }
    inline int getHour() { reset(); return t->tm_hour; }
    inline int getMin() { reset(); return t->tm_min; }
    inline int getSec() { reset(); return t->tm_sec; }

private:
    time_t tt;
    tm* t;

    inline void reset() { tt = time(NULL);t = localtime(&tt); }
};

class Macro {
public:
    Macro() :_name(), _params(), _replist() { }
    Macro(const std::string &_n, MacroType _ty) :_name(_n), _type(_ty), _params(), _replist() {}
    Macro(const std::string &_n, Token & _v, MacroType _ty) :_name(_n), _type(_ty), _params() { _replist.push_back(_v); }

    Macro(const Macro&_m) :_type(_m._type), _name(_m._name), _params(_m._params), _replist(_m._replist) {  }
    Macro operator=(const Macro &_m) { _type = _m._type; _name = _m._name; _params = _m._params; _replist = _m._replist; return *this; }

    MacroType _type = M_ZERO;
    std::string _name;
    std::vector<std::string> _params;
    TokenSequence _replist;
};



class Preprocessor {
public:
    Preprocessor(bool _only) : isOnlyPP(_only) { init(); }
    Preprocessor(const std::string &_fn) { init(); }
    
	Preprocessor(const Preprocessor &) = delete;
	Preprocessor operator= (const Preprocessor &) = delete;

    void group_part(TokenSequence &is, TokenSequence &os);


    void expand(TokenSequence is, TokenSequence &os);
    void subst(TokenSequence &is, std::vector<std::string> fp, TokenSequence &ap, HideSet* hs, TokenSequence& os);
    void glue(TokenSequence &ls, TokenSequence &rs);
    void hasadd(HideSet *hs, TokenSequence &ts);
    TokenSequence ts(const std::string &_macro_name);
    TokenSequence fp(const std::string &_macro_name);
    TokenSequence select(int i, TokenSequence &ts);
    TokenSequence stringize(TokenSequence &ts);
    TokenSequence getAP(TokenSequence &is);
    int isInFP(Token &t, std::vector<std::string> fp);

    Macro *searchMacro(const std::string &_n);
    int isMacro(const std::string &_n);
    bool deleteMacro(const std::string &_n);


    void Include(TokenSequence &is, TokenSequence &os);
    void _define_(TokenSequence &is);
    void _if_(TokenSequence &is);
    void _ifndef_(TokenSequence &is);
    void _ifdef_(TokenSequence &is);
    void _undef_(TokenSequence &is);
    void _elif_(TokenSequence &is);
    void _else_(TokenSequence &is);
    void _endif_(TokenSequence &is);
    void _line_(TokenSequence &is);
    void _pragma_(TokenSequence &is);

    bool invalid() { return invalid_; }

private:
    void init();
    //Lex lex;
    std::vector<Macro> macros;
    std::vector<std::string> _paths;
    bool isOnlyPP = false;

    std::vector<std::pair<std::string, bool>> stk_if_else;

    bool invalid_ = false;
    bool preInvalid_ = false;
    std::vector<bool> stK_invalid;

    bool isExpandExpr = false;

    bool cheak_else();
};

#define _BEGIN_IF_()   do{stK_invalid.push_back(preInvalid_); preInvalid_ = invalid_; invalid_ = false; }while(0)
#define _END_IF_()     do{preInvalid_ = stK_invalid.back(); stK_invalid.pop_back(); invalid_ = preInvalid_; }while(0)

#endif
