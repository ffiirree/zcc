#include "token.h"

#include "logging.h"

#include <iomanip>

void Token::copying(const Token& t)
{
    hs_       = t.hs_;
    kind_     = t.kind_;
    location_ = t.location_;
    isbol_    = t.isbol_;

    copyUnion(t);
}

void Token::copyUnion(const Token& t)
{
    switch (t.getType()) {
    case T_KEYWORD:
    case T_EOF: id_ = t.id_; break;

    case T_CHAR: ch_ = t.ch_; break;

    case T_IDENTIFIER:
    case T_STRING:
    case T_INTEGER:
    case T_FLOAT: new (&sval_) std::string(t.sval_); break;
    }
}

bool Token::needExpand()
{
    if (!hs_) return true;

    // 是不是只有ID需要展开
    if (kind_ != T_IDENTIFIER) return false;

    if (hs_->count(sval_))
        return false;
    else
        return true;
}

std::ostream& operator<<(std::ostream& os, const Token& t)
{
    os << std::left << std::setw(15);
    switch (t.kind_) {
    case T_CHAR: os << "T_CHAR"; break;
    case T_FLOAT: os << "T_FLOAT"; break;
    case T_IDENTIFIER: os << "T_IDENTIFIER"; break;
    case T_INTEGER: os << "T_INTEGER"; break;
    case T_KEYWORD: os << "T_KEYWORD"; break;
    case T_NEWLINE: os << "T_NEWLINE"; break;
    case T_OP: os << "T_OP"; break;
    case T_SPACE: os << "T_SPACE"; break;
    case T_STRING: os << "T_STRING"; break;
    case T_EOF: os << "T_EOF"; break;
    default: log_e(t.location(), "unknown token type."); break;
    }

    os << t.toString() << std::endl;
    return os;
}

std::string Token::toString() const
{
    std::string _r;

    switch (kind_) {
    case T_KEYWORD:
        switch (id_) {
#define keyword(_t, _n, _)                                                                                 \
    case _t: _r = _n; break;
#define op(_t, _n)                                                                                         \
    case _t: _r = _n; break;
            KEYWORD_MAP
            OP_MAP
#undef keyword
#undef op
        default: _r.push_back(static_cast<char>(id_)); break;
        }
        break;

    case T_IDENTIFIER:
    case T_INTEGER:

    case T_FLOAT: _r = sval_; break;
    case T_STRING: _r = "\"" + sval_ + "\""; break;

    case T_CHAR:
        _r += "'";
        switch (ch_) {
        case '\n': _r += "\\n"; break;
        case '\t': _r += "\\t"; break;
        case '\r': _r += "\\r"; break;
        case '\'': _r += "\\'"; break;
        case '\f': _r += "\\f"; break;
        case '\0': _r += "\\0"; break;
        case '\"': _r += "\\\""; break;
        case '\\': _r += "\\\\"; break;
        default: _r.push_back(static_cast<char>(ch_)); break;
        }

        _r += "'";
        break;
    case T_EOF: break;
    case T_NEWLINE: _r = "\n"; break;
    default: log_e(location_, "error token"); break;
    }
    return _r;
}