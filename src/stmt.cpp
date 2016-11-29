#include "parser.h"
#include "error.h"

/**
 * @berif 每一个句子都是一个节点
 */
Node Parser::statement()
{
	Token t = lex.next();
	if (t.getType() == KEYWORD) {
		switch (t.getId())
		{
		case '{': return compound_stmt();
		case K_IF: return if_stmt();
		case K_FOR:return for_stmt();
		case K_WHILE:return while_stmt();
		case K_DO:return do_stmt();
		case K_RETURN: return return_stmt();
		case K_SWITCH: return switch_stmt();
		case K_CASE: return case_stmt();
		case K_DEFAULT: return default_stmt();
		case K_BREAK: return break_stmt();
		case K_CONTINUE:return continue_stmt();
		case K_GOTO: return goto_stmt();
		}
	}

	// 标签，goto语句的标签
	if (t.getType() == ID && next_is(':')) {
		// label
		labels.push_back(t.getSval());
		out << t.getSval() << ":" << std::endl;
	}
	lex.back();

	Node r = expr_opt();
	expect(';');
	return r;
}
/**
 * @berif compound_stmt = '{' {(declaration | statement)} '}' 
 */
Node Parser::compound_stmt()
{
	__IN_SCOPE__(localenv, localenv);

	std::vector<Node> list;

	for (;;) {
		if (next_is('}')) break;
		decl_or_stmt(list);
	}

	__OUT_SCOPE__(localenv);
	return createCompoundStmtNode(list);
}

/**
 * @berif
 *
 * declaration = decl_specifiers [init_decl_list] ';'
 * statement = labeled_stmt ....
 */
void Parser::decl_or_stmt(std::vector<Node> &list)
{
	if (lex.peek().getId() == K_EOF)
		error("premature end of input");

	if (is_type(lex.peek())) {
		declaration(list, false);
	}
	else {
		list.push_back(statement());
	}
}

/**
 * if_stmt = 'if' '(' expression ')' statement
 *         | 'if' '(' expression ')' statement 'else' statement 
 */
Node Parser::if_stmt()
{
	std::string if_true = newLabel("iftrue");
	std::string if_false = newLabel("iffalse");
	std::string if_end = newLabel("ifend");

	expect('(');
	Node *cond = new Node(expr());
	expect(')');

	createQuadruple("if");
	out << if_true << std::endl;

	out << "goto " << if_false << std::endl;
	
	out << if_true << ":" << std::endl;
	Node *then = new Node(statement());
	
	if (next_is(K_ELSE)) {
		out << "goto " << if_end << std::endl;
		out << if_false << ":" << std::endl;
		Node *els = new Node(statement());

		out << if_end << ":" << std::endl;

		return createIfStmtNode(cond, then, els);
	}
	out << if_false << ":" << std::endl;

	return createIfStmtNode(cond, then, nullptr);
}


/**
 * @berif while_stmt = 'while' '(' expression ')' statement 
 */
Node Parser::while_stmt()
{
	std::string _begin = newLabel("whilebegin");
	out << _begin << ":" << std::endl;

	std::string _true = newLabel("whiletrue");
	std::string _false = newLabel("whilefalse");

	expect('(');
	Node node = expr();
	expect(')');

	createQuadruple("if");
	out << _true << std::endl;
	out << "goto " << _false << std::endl;
	out << _true << ":" << std::endl;

	Node body = statement();
	out << "goto " << _begin << std::endl;
	out << _false << ":" << std::endl;

	std::vector<Node> list;
	return createCompoundStmtNode(list);
}


Node Parser::switch_stmt()
{
	Node r;
	return r;
}

Node Parser::for_stmt()
{
	Node r;
	return r;
}

Node Parser::do_stmt()
{
	std::string _begin = newLabel("dobegin");

	out << _begin << ":" << std::endl;

	Node *r = new Node(statement());
	expect(K_WHILE);
	expect('(');
	Node *_b = new Node(expr());
	createQuadruple("if");
	out << _begin << std::endl;

	return *r; ////////////////////这里要修改
}

Node Parser::goto_stmt()
{
	Token t = lex.next();
	labels.push_back_un(t.getSval());
	expect(';');

	out << "goto " << t.getSval() << std::endl;

	return Node(); /////////修改
}

Node Parser::continue_stmt()
{
	Node r;
	return r;
}


/**
 * @berif return_stmt = 'return' [expression] ';'
 */
Node Parser::return_stmt()
{
	Node *retval = new Node(expr_opt());
	expect(';');
	return createRetStmtNode(retval);
}


Node Parser::case_stmt()
{
	Node r;
	return r;
}
Node Parser::default_stmt()
{
	Node r;
	return r;
}
Node Parser::label_stmt()
{
	Node r;
	return r;
}

/**
 * @berif break_stmt = 'break' ';'
 */
Node Parser::break_stmt()
{
	expect(';');
	return createJumpNode(label_break);
}