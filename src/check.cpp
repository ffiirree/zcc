#include "parser.h"

bool Parser::ensure_lvalue(const Node &node)
{
	return false;
}

bool Parser::cheak_redefined(Env *_env, const std::string &_name)
{
    for (size_t i = 0; i < _env->size(); ++i) {
        if (_env->at(i).kind == NODE_FUNC && _env->at(i).funcName == _name)
            return true;
        else if ((_env->at(i).kind == NODE_GLO_VAR || _env->at(i).kind == NODE_LOC_VAR) && _env->at(i).varName == _name)
            return true;
    }
    return false;
}