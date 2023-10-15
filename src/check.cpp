#include "parser.h"

bool Parser::ensure_lvalue(const Node& node) { return false; }

bool Parser::cheak_redefined(Env *_env, const std::string& _name)
{
    /**
     * \ Current scope
     */
    for (size_t i = 0; i < _env->size(); ++i) {
        if ((_env->at(i).kind_ == NODE_GLO_VAR || _env->at(i).kind_ == NODE_LOC_VAR) &&
            _env->at(i).name() == _name)
            return true;
    }

    if (!searchEnum(_name).empty()) return true;

    /**
     * \ Cheak whether the name is the same as a function's name.
     */
    if (_env != globalenv) {
        for (size_t i = 0; i < globalenv->size(); ++i) {
            if ((globalenv->at(i).kind_ == NODE_FUNC || globalenv->at(i).kind_ == NODE_FUNC_DECL) &&
                globalenv->at(i).name() == _name)
                return true;
        }
    }
    return false;
}

bool Parser::cheak_is_int_type(const Node& n)
{
    switch (n.type_.type) {
    case K_CHAR:
    case K_SHORT:
    case K_INT:
    case K_LONG: return true;
    default: return false;
    }
}

bool Parser::cheak_is_float(const Node& n)
{
    if (n.type_.type == K_FLOAT || n.type_.type == K_DOUBLE) return true;
    return false;
}

bool Parser::cheak_is_custom_type(const Node& n)
{
    switch (n.type_.type) {
    case K_STRUCT:
    case K_UNION:
    case K_ENUM:
    case K_TYPEDEF: return true;

    default: return false;
    }
}