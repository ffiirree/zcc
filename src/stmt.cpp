#include<iomanip>
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
		return Node();      ////////////////////////////修改
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

	__OUT_SCOPE__(localenv, newLabel("Env"));
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
	BoolLabel _if;
	std::string snext = newLabel("sn");

	expect('(');
	Node *cond = new Node(expr());          // B.code
	expect(')');

	_if._true = newLabel("ift");
	_if._false = newLabel("iff");
	boolLabel.back() = _if;
	generateIfGoto();
	out << _if._true << ":" << std::endl;   

	Node *then = new Node(statement());     // S1.code

	if (next_is(K_ELSE)) {
		out << "goto\t" << snext << std::endl;
		out << _if._false << ":" << std::endl;
		Node *els = new Node(statement());
		out << snext << ":" << std::endl;
		return createIfStmtNode(cond, then, els);
	}
	else {
		out << _if._false << ":" << std::endl;          // S1.next
	}
	return createIfStmtNode(cond, then, nullptr);
}


/**
 * @berif while_stmt = 'while' '(' expression ')' statement 
 */
Node Parser::while_stmt()
{
	BoolLabel _while;
	std::string _begin = newLabel("wb");   // begin = newLabel
	std::string _snext = newLabel("sn");   

	_while._true = newLabel("wt");         // B.true = newLabel
	_while._false = _snext;                // B.false = S.next

	std::string _s1next = _begin;          // S1.next = begin

	out << _begin << ":" << std::endl;     // Label(begin)

	expect('(');
	Node node = expr();                    
	expect(')');

	boolLabel.back() = _while;             
	generateIfGoto();                      // B.code
	out << _while._true << ":" << std::endl;           // Label(B.true)

	Node body = statement();               // S1.code
	out << "goto " << _begin << std::endl; // gen(goto begin)

	out << _snext << ":" << std::endl;

	std::vector<Node> list;
	return createCompoundStmtNode(list);
}


Node Parser::do_stmt()
{
	BoolLabel _do;
	std::string _begin = newLabel("db");   // begin = newLabel
	std::string _snext = newLabel("sn");

	_do._true = _begin;
	_do._false = _snext;

	out << _begin << ":" << std::endl;
	Node *r = new Node(statement());
	expect(K_WHILE);
	expect('(');
	Node *_b = new Node(expr());
	expect(')');
	expect(';');

	boolLabel.back() = _do;
	generateIfGoto();                      // B.code

	out << _snext << ":" << std::endl;

	return *r; ////////////////////这里要修改
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


Node Parser::goto_stmt()
{
	Token t = lex.next();
	labels.push_back_un(t.getSval());
	expect(';');

	out << std::left << std::setw(10) << "goto " << t.getSval() << std::endl;

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

	out << std::left << std::setw(15) << "ret";

	if (retval->kind == NODE_INT || retval->kind == NODE_LONG
		|| retval->kind == NODE_CHAR || retval->kind == NODE_SHORT)
		out << retval->int_val;
	else if (retval->kind == NODE_FLOAT || retval->kind == NODE_DOUBLE)
		out << retval->float_val;
	else if (retval->kind == NODE_GLO_VAR || retval->kind == NODE_LOC_VAR)
		out << retval->varName;

	out << std::endl;

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