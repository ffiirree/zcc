#include "parser.h"

bool Parser::ensure_lvalue(const Node &node)
{
	return false;
}

bool Parser::cheak_redefined(Env *_env, const std::string &_name)
{
    for (size_t i = 0; i < _env->size(); ++i) {
        if ((_env->at(i).kind == NODE_GLO_VAR || _env->at(i).kind == NODE_LOC_VAR) && _env->at(i).varName == _name)
            return true;
    }

	if (!searchEnum(_name).empty())
		return true;

	if (_env != globalenv) {
		for (size_t i = 0; i < globalenv->size(); ++i) {
			if ((globalenv->at(i).kind == NODE_FUNC || globalenv->at(i).kind == NODE_FUNC_DECL) && globalenv->at(i).funcName == _name)
				return true;
		}
	}
    return false;
}