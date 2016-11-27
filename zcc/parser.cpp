#include "parser.h"
#include "error.h"

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

	while (!loclenv) {
		type = loclenv->search(key);
	}
	return type;
}

bool Parser::next_is(int id)
{
	if (lex.next().getId() == id)
		return true;
	lex.back();
	return false;
}

Env Parser::trans_unit()
{
	for (;;) {
		Token t = lex.peek();

		if (t.getType() == K_EOF) {
			error("unit eof!");
			return globalenv;
		}
			

		if (isFuncDef())
			globalenv.push_back(funcDef());
		else
			declaration(globalenv, true);
	}
	return globalenv;
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

bool Parser::is_type(int id)
{
	switch (id)
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

bool Parser::isFuncDef()
{
	int count = 0;
	while (is_type(lex.next().getId())) {
		count++;
	}
	lex.back();

	Token t = lex.next();
	count++;
	if (t.getType() == ID) {
		if (is_keyword(lex.next(), '(')) {
			count++;
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
	lex.back();
}

void Parser::expect(int id)
{
	if (lex.peek().getId() != id)
		error("expect %c, but not", id);
}


Node Parser::funcDef()
{
	int current_class = 0;                  // static ...
	Type retty;                             // 返回类型
	std::string funcName;                         // 函数名字
	std::vector<Node> params;

	Type functype;                         // 函数类型(函数描述)

	loclenv = new Env(globalenv);

	retty = decl_spec_opt(&current_class);

	functype = declarator(retty, funcName, params, FUNC_BODY);
	functype.setStatic(current_class == K_STATIC);
	expect('{');
	Node r = func_body(functype, funcName, params);

	// 出scope
	loclenv = nullptr;
	return r;
}


Type Parser::func_param_list(Type retty, std::vector<Node> params)
{
	if (is_keyword(lex.next(), K_VOID) && next_is(')'))
		return Type()
}


Node Parser::func_body(Type &functype, std::string name, std::vector<Node> params)
{
	Node body = compound_stmt();
	return  createFuncNode(functype, name, params, &body);
}

std::vector<Node> Parser::initializer(Type &ty)
{

}
