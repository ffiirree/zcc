#include "parser.h"
#include "error.h"

/**
 * @berif 一个翻译单元，一个文件
 * 
 */
std::vector<Node> Parser::trans_unit()
{
	std::vector<Node> list;
	for (;;) {
		Token t = lex.peek();

		if (t.getType() == K_EOF) {
			error("unit eof!");
			return list;
		}

		if (isFuncDef())
			list.push_back(funcDef());
		else
			declaration(list, true);
	}
	return list;
}

/**
 * @berif 检查是否是函数定义
 */
bool Parser::isFuncDef()
{
	
	int count = 0;
	Token t;

	// 返回类型
	do {
		t = lex.next();
		count++;
	} while (is_type(t));
	
	// 函数名
	if (t.getType() == ID) {

		// 函数参数
		t = lex.next();
		count++;
		if (is_keyword(t, '(')) {
			skip_parenthesis(&count);
			t = lex.next();
			count++;
			for (int i = 0; i < count; ++i)
				lex.back();

			if (is_keyword(t, '{'))
				return true;
			else return false;
		}
	}
	for (int i = 0; i < count; ++i)
		lex.back();
	return false;
}

/**
 * @berif 函数定义
 */
Node Parser::funcDef()
{
	int current_class = 0;                                                  // static ...
	std::string funcName;                                                   // 函数名字
	std::vector<Node> params;

	__IN_SCOPE__(localenv, globalenv);

	Type retty = decl_spec_opt(&current_class);                             // 获取函数的返回类型
	Type functype = declarator(retty, funcName, params, FUNC_BODY);         // 函数定义类型，函数描述
	functype.setStatic(current_class == K_STATIC);                          // 函数是否是static
	expect('{');
	lex.next();
	Node r = func_body(functype, funcName, params);                         // 函数体

	__OUT_SCOPE__(localenv);
	return r;
}


Type Parser::func_param_list(Type *retty, std::vector<Node> params)
{
	if (is_keyword(lex.next(), K_VOID) && next_is(')'))
		return Type((int)FUNC, retty, params);
}


Node Parser::func_body(Type &functype, std::string name, std::vector<Node> params)
{
	Node body = compound_stmt();
	return  createFuncNode(functype, name, params, &body);
}



Type Env::search(std::string &key)
{
	Env *ptr = this;

	while (ptr) {
		for (int i = 0; i < ptr->card.size(); ++i) {
			if (key == ptr->card.at(i).sval)
				return ptr->card.at(i).getType();
		}
	}
	return Type();
}


/**
* 从当前作用域开始查找标识符
*/
Type Parser::get_type(std::string key)
{
	Type type;

	while (!localenv) {
		type = localenv->search(key);
	}
	return type;
}
int Parser::get_compound_assign_op(Token &t)
{
	if (t.getType() != KEYWORD)
		return 0;
	switch (t.getId()) {
	case OP_A_ADD: return '+';
	case OP_A_SUB: return '-';
	case OP_A_MUL: return '*';
	case OP_A_DIV: return '/';
	case OP_A_MOD: return '%';
	case OP_A_AND: return '&';
	case OP_A_OR:  return '|';
	case OP_A_XOR: return '^';
	case OP_A_SAL: return OP_SAL;
	case OP_A_SAR: return OP_SAR;
	default: return 0;
	}
}

bool Parser::next_is(int id)
{
	if (lex.next().getId() == id)
		return true;
	lex.back();
	return false;
}



Node Parser::createFuncNode(Type &ty, std::string & funcName, std::vector<Node> params, Node *body)
{
	Node node(FUNC, ty);
	node.funcName = funcName;
	node.params = params;
	node.body = body;

	return node;
}

Node Parser::createIntNode(int kind, Type &ty, long val)
{
	Node node(kind, ty);
	node.int_val = val;

	return node;
}

Node Parser::createCompoundStmtNode(std::vector<Node> &stmts)
{
	Node node(COMPOUND_STMT);
	node.stmts = stmts;
	return node;
}
Node Parser::createDeclNode(Node &var)
{
	Node node(DECL);
	node.decl_var = &var;
	return node;
}
Node Parser::createDeclNode(Node &var, std::vector<Node> &init)
{
	Node node(DECL);
	node.decl_var = &var;
	node.decl_init = init;
	return node;
}

Node Parser::createGLoVarNode(Type &ty, std::string &name)
{
	Node r(GLO_VAR, ty);
	r.varName = name;
	return r;
}
Node Parser::createLocVarNode(Type &ty, std::string &name)
{
	Node r;
	r.varName = name;
	return r;
}

Node Parser::createBinOpNode(Type &ty, int kind, Node *left, Node *right)
{
	Node r(kind, ty);
	r.left = left;
	r.right = right;
	return r;
}
Node Parser::createUnaryNode(int kind, Type &ty, Node &node)
{
	Node r(kind, ty);
	r.operand = &node;
	return r;
}



bool Parser::is_type(const Token &t)
{
	if (t.getType() != KEYWORD)
		return false;

	switch (t.getId())
	{
#define keyword(id, _, is) case id: return is;
		KEYWORD_MAP
#undef keyword
	default: return false;
	}
}

/**
* 检查是否是关键字id
*/
bool Parser::is_keyword(Token &t, int id)
{
	return (t.getType() == KEYWORD && t.getId() == id);
}

void Parser::skip_parenthesis(int *count)
{
	for (;;)
	{
		Token t = lex.next();
		(*count)++;

		if (t.getType() == K_EOF)
			error("error eof");

		if (is_keyword(t, '('))
			skip_parenthesis(count);

		if (is_keyword(t, ')'))
			break;
	}
}
void Parser::expect(int id)
{
	if (lex.peek().getId() != id)
		error("expect %c, but not", id);
}

bool Parser::is_inttype(Type &ty)
{
	switch (ty.getType())
	{
	case K_BOOL: case K_SHORT:case K_CHAR: case K_INT:
	case K_LONG:
		return true;
	default:
		return false;
	}
}
bool Parser::is_floattype(Type &ty)
{
	switch (ty.getType())
	{
	case K_FLOAT: case K_DOUBLE:
		return true;
	default: return false;
	}
}
bool Parser::is_arithtype(Type &ty)
{
	return is_inttype(ty) || is_floattype(ty);
}
