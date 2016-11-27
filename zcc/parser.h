#ifndef _ZCC_PARSER_H
#define _ZCC_PARSER_H

#include "lex.h"

class Env {
public:
	Env(Env *_pre) :pre(_pre), card() { }
	void push_back(Node &n) { card.push_back(n); }
	Type search(std::string &key);

	Env *pre;
	std::vector<Node> card;
};


class Parser {
public:
	Parser(Lex &l) :lex(l), globalenv(nullptr), loclenv(nullptr) {  }
	Parser(const Parser &p) = delete;
	Parser operator=(const Parser &p) = delete;

	

private:
	bool next_is(int id);
	void expect(int id);
	bool is_keyword(Token &t, int id);
	bool is_type(int id);
	Type get_type(std::string key);

	//
	Node createFuncNode(Type &ty, std::string & funcName, std::vector<Node> params, Node *body);
	Node createIntNode(int kind, Type &ty, long val);


	//
	Env trans_unit();

	bool isFuncDef();
	Node funcDef();

	void declaration(Env &env, bool isGlo);
	void skip_parenthesis(int *count);

	Type declarator(Type &ty, std::string &name, std::vector<Node> params, int deal_type);
	Type decl_spec_opt(int *sclass);
	Type decl_specifiers(int *rsclass);
	Type direct_decl_tail(Type basetype, std::vector<Node> params);
	Type func_param_list(Type basetype, std::vector<Node> params);
	Node func_body(Type &functype, std::string, std::vector<Node> params);
	std::vector<Node> initializer(Type &ty);

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

	Node expr();
	Node expr_opt();
	Node comma_expr();
	Node assignment_expr();
	Node conditional_expr(Node *node);
	Node logical_or_expr();

	Lex lex;


	Env globalenv;   // 全局作用域
	Env *loclenv;    // 局部作用域
	std::vector<std::string> labels;

};
#endif // !_ZCC_PARSER_H
