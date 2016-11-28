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
	expect('(');
	Node *cond = new Node(expr());
	expect(')');

	Node *then = new Node(statement());

	if (next_is(K_ELSE)) {
		Node *els = new Node(statement());
		return createIfStmtNode(cond, then, els);
	}

	return createIfStmtNode(cond, then, nullptr);
}


/**
 * @berif while_stmt = 'while' '(' expression ')' statement 
 */
Node Parser::while_stmt()
{
	expect('(');
	Node node = expr();
	expect(')');

	Node body = statement();

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
	Node r;
	return r;
}
Node Parser::goto_stmt()
{
	Node r;
	return r;
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