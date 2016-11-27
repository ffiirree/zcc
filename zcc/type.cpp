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
void Node::copyUnion(const Node &n)
{
	switch (n.getType().getType())
	{

	case K_CHAR:
	case K_INT:
	case K_SHORT:
	case K_LONG:
		int_val = n.int_val;
		break;

	case K_FLOAT:
	case K_DOUBLE:
		float_val = n.float_val;
		break;

	case STRING_:
		new(&sval) std::string(n.sval);
		break;

	case VAR:
		new(&varName) std::string(n.varName);
		loc_off = n.loc_off;
		lvarinit = n.lvarinit;
		new (&glabel) std::string(n.glabel);
		break;

	case BIN_OP:
		left = n.left;
		right = n.right;
		break;

	case UNARY_OP:
		operand = n.operand;
		break;

	case FUNC:
		new(&funcName) std::string(funcName);
		args = n.args;
		func_type = n.func_type;
		func_ptr = n.func_ptr;
		params = n.params;
		localvars = n.localvars;
		body = n.body;
		break;

	case DECL:
		decl_var = n.decl_var;
		decl_init = n.decl_init;
		break;


	case INIT:
		init_val = n.init_val;
		init_off = n.init_off;
		to_type = n.to_type;
		break;


	case STMT:
	case TERN_OP:
		cond = n.cond;
		then = n.then;
		els = n.els;
		break;

	case K_GOTO:
	case LABEL:
		label = n.label;
		newLabel = n.newLabel;
		break;

	case K_RETURN:
		retval = n.retval;
		break;

	case COM_STMT:
		stmts = n.stmts;
		break;

	case STRUCT_REF:
		struc = n.struc;
		field = n.field;
		fieldtype = n.fieldtype;
		break;


	default:
		break;
	}
}
