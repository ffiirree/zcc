#include "type.h"

#include "logging.h"

/**
 * Type
 */
void Type::coping(const Type& t)
{
    type     = t.type;
    size_    = t.size_;
    isUnsig  = t.isUnsig;
    isSta    = t.isSta;
    ptr      = t.ptr;
    len      = t.len;
    retType  = t.retType;
    params   = t.params;
    _all_len = t._all_len;

    fields    = t.fields;
    is_struct = t.is_struct;
}

void Node::copying(const Node& n)
{
    kind_ = n.kind_;
    type_ = n.type_;

    int_val = n.int_val;

    float_val = n.float_val;

    sval_ = n.sval_;

    name_     = n.name_;
    off_      = n.off_;
    lvarinit_ = n.lvarinit_;

    left_    = n.left_;
    right_   = n.right_;
    operand_ = n.operand_;

    params                = n.params;
    body                  = n.body;
    local_vars_stk_size_  = n.local_vars_stk_size_;
    params_stk_size_      = n.params_stk_size_;
    max_call_params_size_ = n.max_call_params_size_;

    decl_var  = n.decl_var;
    decl_init = n.decl_init;

    cond = n.cond;
    then = n.then;
    els  = n.els;

    label    = n.label;
    newLabel = n.newLabel;

    retval = n.retval;
    stmts  = n.stmts;
}

std::string getOnlyFileName(const std::string& _fn)
{
    size_t _index_separator = 0;
    size_t _index_dot       = 0;
    for (size_t i = 0; i < _fn.length(); ++i) {
        if (_fn.at(i) == '/' || _fn.at(i) == '\\') _index_separator = i + 1;
        if (_fn.at(i) == '.') _index_dot = i;
    }
    if (_index_dot <= _index_separator && _fn.length() > 0) _index_dot = _fn.length() - 1;

    std::string _rfn;
    for (size_t i = 0; i < _index_dot; ++i)
        _rfn.push_back(_fn.at(i));

    return _rfn;
}
