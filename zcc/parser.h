#ifndef _ZCC_PARSER_H
#define _ZCC_PARSER_H

#include "lex.h"


#define __IN_SCOPE__(localEnv, preEnv) do{localEnv = new Env(preEnv);}while(0)
#define __OUT_SCOPE__(localEnv) do{localEnv = localEnv->pre;}while(0)

class Env {
public:
	Env():Env(nullptr) {}
	Env(Env *_pre) :pre(_pre), card() { }
	void push_back(Node &n) { card.push_back(n); }
	Type search(std::string &key);

	Env *pre;
	std::vector<Node> card;
};


class Parser {
public:
	Parser(const std::string &filename) :lex(filename) { globalenv = new Env(nullptr); }
	Parser(Lex &l) :lex(l) { globalenv = new Env(nullptr); }
	Parser(const Parser &p) = delete;
	Parser operator=(const Parser &p) = delete;

	std::vector<Node> trans_unit();

private:
	bool next_is(int id);
	void expect(int id);
	bool is_keyword(Token &t, int id);
	bool is_type(const Token &t);
	bool is_inttype(Type &ty);
	bool is_floattype(Type &ty);
	bool is_arithtype(Type &ty);
	Type get_type(std::string key);
	int get_compound_assign_op(Token &t);

	//
	Node createFuncNode(Type &ty, std::string & funcName, std::vector<Node> params, Node *body);
	Node createIntNode(int kind, Type &ty, long val);
	Node createCompoundStmtNode(std::vector<Node> &stmts);
	Node createDeclNode(Node &var);
	Node createDeclNode(Node &var, std::vector<Node> &init);
	Node createGLoVarNode(Type &ty, std::string name);
	Node createLocVarNode(Type &ty, std::string name);
	// 两元操作符
	Node createBinOpNode(Type &ty, int kind, Node *left, Node *right);
	Node createUnaryNode(int kind, Type &ty, Node &node);

	//
	

	bool isFuncDef();
	Node funcDef();

	/**
	 * decl
	 */
	void declaration(std::vector<Node> &list, bool isGlo);
	void skip_parenthesis(int *count);
	Type declarator(Type &ty, std::string &name, std::vector<Node> params, int deal_type);
	Type decl_spec_opt(int *sclass);
	Type decl_specifiers(int *rsclass);
	Type direct_decl_tail(Type &retty, std::vector<Node> params);
	Type func_param_list(Type *basetype, std::vector<Node> params);
	Node func_body(Type &functype, std::string, std::vector<Node> params);
	std::vector<Node> initializer(Type &ty);
	std::vector<Node> decl_init(Type &ty);
	void init_list(std::vector<Node> &r, Type &ty, int off, bool designated);

	void decl_or_stmt(std::vector<Node> &list);

	/**
	 * stmt
	 */
	Node statement();
	Node compound_stmt();
	Node if_stmt();
	Node while_stmt();
	Node switch_stmt();
	Node for_stmt();
	Node do_stmt();
	Node goto_stmt();
	Node continue_stmt();
	Node return_stmt();
	Node case_stmt();
	Node default_stmt();
	Node label_stmt();
	Node break_stmt();

	/**
	 * expr
	 */
	Node expr();
	Node expr_opt();
	Node comma_expr();
	Node assignment_expr();
	Node conditional_expr(Node *node);
	Node logical_or_expr();
	Node logical_and_expr();
	Node bit_or_expr();
	Node bit_xor_expr();
	Node bit_and_expr();
	Node equal_expr();
	Node equal_expr_tail();
	Node relational_expr();
	Node shift_expr();
	Node add_expr();
	Node multi_expr();
	Node cast_expr();
	Node unary_expr();
	Node postfix_expr();
	Node postfix_expr_tail(Node &node);
	Node primary_expr();


	Node binop(int op, Node &lhs, Node &rhs);

	Node sizeof_operand();
	Node unary_incdec(int ty);
	Node label_addr(Token &t);
	Node unary_addr();
	Node unary_deref(Token &t);
	Node unary_minus();
	Node unary_bitnot(Token &t);
	Node unary_lognot();

	Node wrap(Type &t, Node &node);
	Node conv(Node &node);

	void ensure_inttype(Node &node);

	// 检查
	bool ensure_lvalue(const Node &node);
	Type usual_arith_conv(Type &t, Type &u);


	Lex lex;
	Env *globalenv = nullptr;
	Env *localenv = nullptr;
	std::vector<std::string> labels;
};
#endif // !_ZCC_PARSER_H
