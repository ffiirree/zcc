#include "type.h"

/**
 * Token
 */
std::ostream &operator<<(std::ostream & os, const Token & t)
{
	os << t.getType() << "\t";

	if (t.getType() == KEYWORD) {
		switch (t.getId())
		{
#define keyword(ty, name, _) case ty: os << name ;break;
#define op(ty,name) case ty: os << name;break;
			KEYWORD_MAP
				OP_MAP
#undef keyword
#undef op
		default: os << (char)t.getId(); break;
		}
		os << std::endl;
	}
	else if (t.getType() == ID)
		os << t.getSval().c_str() << std::endl;
	else if (t.getType() == CHAR_)
		os << (char)t.getCh() << std::endl;
	else if (t.getType() == STRING_ || t.getType() == INTEGER || t.getType() == FLOAT)
		os << t.getSval().c_str() << std::endl;

	return os;
}
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
	loc_off = n.loc_off;
	lvarinit = n.lvarinit;
	glabel = n.glabel;


	left = n.left;
	right = n.right;
	operand = n.operand;

	funcName = n.funcName;
	args = n.args;
	func_type = n.func_type;
	func_ptr = n.func_ptr;
	params = n.params;
	localvars = n.localvars;
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
	struc = n.struc;
	field = n.field;
	fieldtype = n.fieldtype;
}