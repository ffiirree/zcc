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
	__IN_SCOPE__(localenv, localenv, newLabel("Env"));

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
    out << "clr" << std::endl;
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
	boolLabel.back()._true = _if._true;
	boolLabel.back()._false = _if._false;
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

	// break 和 continue
	_stk_ctl_bg_l.push_back(_begin);
	_stk_ctl_end_l.push_back(_snext);

	_while._true = newLabel("wt");         // B.true = newLabel
	_while._false = _snext;                // B.false = S.next

	std::string _s1next = _begin;          // S1.next = begin

	out << _begin << ":" << std::endl;     // Label(begin)

	expect('(');
	Node node = expr();                    
	expect(')');

	boolLabel.back()._true = _while._true;
	boolLabel.back()._false = _while._false;           
	generateIfGoto();                      // B.code
	out << _while._true << ":" << std::endl;           // Label(B.true)

	Node body = statement();               // S1.code
	out << "goto " << _begin << std::endl; // gen(goto begin)

	out << _snext << ":" << std::endl;

	_stk_ctl_bg_l.pop_back();
	_stk_ctl_end_l.pop_back();

	std::vector<Node> list;
	return createCompoundStmtNode(list);
}


Node Parser::do_stmt()
{
	BoolLabel _do;
	std::string _begin = newLabel("db");   // begin = newLabel
	std::string _snext = newLabel("sn");

	_stk_ctl_bg_l.push_back(_begin);
	_stk_ctl_end_l.push_back(_snext);

	_do._true = _begin;
	_do._false = _snext;

	out << _begin << ":" << std::endl;
	Node *r = new Node(statement());
	expect(K_WHILE);
	expect('(');
	Node *_b = new Node(expr());
	expect(')');
	expect(';');

	boolLabel.back()._true = _do._true;
	boolLabel.back()._false = _do._false;
	generateIfGoto();                      // B.code

	out << _snext << ":" << std::endl;

	_stk_ctl_bg_l.pop_back();
	_stk_ctl_end_l.pop_back();

	return *r; ////////////////////这里要修改
}

Node Parser::switch_stmt()
{
	std::string _next = newLabel("swn");

	_stk_ctl_end_l.push_back(_next);

	expect('(');
	Node r = expr();
	if (r.type.getType() != K_INT && r.type.getType() != K_CHAR
		&& r.type.getType() != K_SHORT && r.type.getType() != K_LONG)
		error("Switch only integer!");

	if (r.kind == NODE_INT || r.kind == NODE_CHAR || r.kind == NODE_SHORT || r.kind == NODE_LONG)
		switch_expr = r.int_val;
	else if (r.kind == NODE_LOC_VAR || NODE_GLO_VAR)
		switch_expr = r.varName;
	else 
		switch_expr = _stk_quad.back();
	expect(')');
	
	expect('{');
	compound_stmt();

	out << _next << ":" << std::endl;
	_stk_ctl_end_l.pop_back();
	return Node();
}
/**
 * for_stmt = 'for' '(' [expression] ';' [expression] ';' [expression] ')' statement 
 *          | 'for' '(' declaration ';' [expression] ';' [expression] ')' statement
 */
Node Parser::for_stmt()
{
	std::string _next = newLabel("forn");
	std::string _begin = newLabel("forb");
	std::string _exp3 = newLabel("fe3");


	BoolLabel _for;

	_stk_ctl_bg_l.push_back(_begin);
	_stk_ctl_end_l.push_back(_next);

	_for._false = _next;
	_for._true = newLabel("fort");

	expect('(');
    __IN_SCOPE__(localenv, localenv, newLabel("for"));
	if (is_type(lex.peek())) {
		std::vector<Node> list;
		declaration(list,false);
	}
	else if (is_keyword(lex.peek(), ';')) {
		expect(';');
	}
	else {
		expr();
		expect(';');
	}

	out << _begin << ":" << std::endl;

	if (is_keyword(lex.peek(), ';')) {
		expect(';');
	}
	else {
		expr();
		expect(';');
	}

	// bool判断部分
	boolLabel.push_back(_for);
	generateIfGoto();

	// 后循环部分
	out << _exp3 << ":" << std::endl;
	if (is_keyword(lex.peek(), ')')) {
		expect(')');
	}
	else {
		expr();
		expect(')');
	}

	out << "goto " << _begin << std::endl;
	out << _for._true << ":" << std::endl;

	// 循环体
	statement();
    __OUT_SCOPE__(localenv);
	out << "goto " << _exp3 << std::endl;
	out << _next << ":" << std::endl;

	_stk_ctl_bg_l.pop_back();
	_stk_ctl_end_l.pop_back();

	return Node();
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
	out << "goto " << _stk_ctl_bg_l.back() << std::endl;
	return  Node();
}


/**
 * @berif return_stmt = 'return' [expression] ';'
 */
Node Parser::return_stmt()
{
	Node *retval = new Node(expr_opt());
	expect(';');

    out << "ret\t" + _stk_quad.back() << std::endl;

	return createRetStmtNode(retval);
}


Node Parser::case_stmt()
{
	int val = com_conditional_expr().int_val;
	expect(':');

	out << "if " << switch_expr << " != " << val << " goto " << switch_case_label << std::endl;
	statement();

	out << switch_case_label << ":" << std::endl;
	switch_case_label = newLabel("case");

	return Node();
}
Node Parser::default_stmt()
{
	expect(':');
	statement();
	return Node();
}

/**
 * @berif break_stmt = 'break' ';'
 */
Node Parser::break_stmt()
{
	out << "goto " << _stk_ctl_end_l.back() << std::endl;
	expect(';');
	return createJumpNode(label_break);
}