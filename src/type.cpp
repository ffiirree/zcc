#include "type.h"
#include "ERROR.H"

/**
 * Token
 */
//std::ostream &operator<<(std::ostream & os, const Token & t)
//{
//	os << t.getType() << "\t";
//
//	if (t.getType() == KEYWORD) {
//		switch (t.getId())
//		{
//#define keyword(ty, name, _) case ty: os << name ;break;
//#define op(ty,name) case ty: os << name;break;
//			KEYWORD_MAP
//				OP_MAP
//#undef keyword
//#undef op
//		default: os << (char)t.getId(); break;
//		}
//		os << std::endl;
//	}
//	else if (t.getType() == ID)
//		os << t.getSval().c_str() << std::endl;
//	else if (t.getType() == CHAR_)
//		os << (char)t.getCh() << std::endl;
//	else if (t.getType() == STRING_ || t.getType() == INTEGER || t.getType() == FLOAT)
//		os << t.getSval().c_str() << std::endl;
//
//	return os;
//}
bool operator==(const Token &t1, const Token &t2)
{
	return (t1.getType() == t2.getType() && t1.getPos() == t2.getPos() && t1.getCounter() == t2.getCounter() &&
		t1.getCh() == t2.getCh() && t1.getId() == t2.getId() && t1.getSval() == t2.getSval());
}
bool operator!=(const Token &t1, const Token &t2)
{
	return !(t1 == t2);
}

void Token::copyUnion(const Token &t)
{
	switch (t.getType()) {
	case KEYWORD:
	case K_EOF:
		id = t.id;
		break;

	case CHAR_:
		ch = t.ch;
		break;

	case ID:
	case STRING_:
	case INTEGER:
	case FLOAT:
		new(&sval) std::string(t.sval);
		break;
	}
}




/**
 * Type
 */
void Type::coping(const Type &t)
{
    type = t.type;
    size = t.size;
    isUnsig = t.isUnsig;
    isSta = t.isSta;
    ptr = t.ptr;
    len = t.len;
    retType = t.retType;
    params = t.params;
    _all_len = t._all_len;

    fields = t.fields;
    is_struct = t.is_struct;
}

/**
 * Node
 */

Node::Node(const Node &n)
{
	copying(n);
}
Node Node::operator=(const Node &n)
{
	copying(n);
	return *this;
}

inline void Node::copying(const Node &n)
{
	kind = n.kind;
	type = n.type;

	int_val = n.int_val;

	float_val = n.float_val;

	sval = n.sval;

	varName = n.varName;
	_off = n._off;
	lvarinit = n.lvarinit;
	glabel = n.glabel;


	left = n.left;
	right = n.right;
	operand = n.operand;

	funcName = n.funcName;
	params = n.params;
	body = n.body;

	decl_var = n.decl_var;
	decl_init = n.decl_init;

	init_val = n.init_val;
	init_off = n.init_off;
	to_type = n.to_type;
	cond = n.cond;
	then = n.then;
	els = n.els;
	label = n.label;
	newLabel = n.newLabel;

	retval = n.retval;
	stmts = n.stmts;
}


std::string getOnlyFileName(const std::string &_fn)
{
    size_t _index_separator = 0;
    size_t _index_dot = 0;
    for (size_t i = 0; i < _fn.length(); ++i) {
        if (_fn.at(i) == '/' || _fn.at(i) == '\\')
            _index_separator = i + 1;
        if (_fn.at(i) == '.')
            _index_dot = i;
    }
    if (_index_dot <= _index_separator && _fn.length() > 0) _index_dot = _fn.length() - 1;

    std::string _rfn;
    for (size_t i = _index_separator; i < _index_dot; ++i)
        _rfn.push_back(_fn.at(i));

    return _rfn;
}

std::ostream &operator<<(std::ostream & os, const Token & t)
{
    os << t.to_string();
    return os;
}

std::string Token::to_string() const
{
    std::string _r;

    switch (type) {
    case KEYWORD:
        switch (id) {
#define keyword(_t, _n, _)  case _t: _r = _n; break;
#define op(_t, _n)          case _t: _r = _n; break;
            KEYWORD_MAP
                OP_MAP
#undef keyword
#undef op
        default: _r.push_back(static_cast<char>(id)); break;
        }
        break;

    case ID:
    case INTEGER:
    
    case FLOAT:  _r = sval; break;
    case STRING_: _r = "\"" + sval + "\""; break;


    case CHAR_:  _r.push_back(static_cast<char>(ch)); break;
    case K_EOF:  break;
    case TNEWLINE: _r = "\n"; break;
    default:
        errorp(pos, "error token");
        break;
    }
    return _r;
}