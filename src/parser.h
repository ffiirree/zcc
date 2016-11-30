#ifndef _ZCC_PARSER_H
#define _ZCC_PARSER_H

#include "lex.h"

#define __IN_SCOPE__(localEnv, preEnv) do{localEnv = new Env(preEnv);}while(0)
#define __OUT_SCOPE__(localEnv) do{localEnv = localEnv->pre();}while(0)

class Env {
public:
	Env():Env(nullptr) {}
	Env(Env *p) :_pre(p), nodes() { }
	void push_back(Node &n);
	Node search(std::string &key);

	Env * pre() { return _pre; }

private:
	Env *_pre;
	std::vector<Node> nodes;
};

class Label {
public:
	Label() :Label(nullptr) {}
	Label(Env *p): labels() { }
	void push_back(const std::string &_l);
	void push_back_un(const std::string &_l){
		if (!_exist(_l)) {
			labels.push_back(_l);
			enLabels.push_back(false);
		}
	}
	bool cheak();

private:
	bool _exist(const std::string &_l) {
		for (int i = 0; i < labels.size(); ++i) {
			if (_l == labels.at(i))
				return true;
		}
		return false;
	}

	std::vector<std::string> labels;
	std::vector<bool> enLabels;
};


class Parser {
public:
	Parser(const std::string &filename) :lex(filename) { 
		globalenv = new Env(nullptr); 
		createQuadFile();
	}
	Parser(Lex &l) :lex(l) { 
		globalenv = new Env(nullptr); 
		createQuadFile();
	}
	Parser(const Parser &p) = delete;
	Parser operator=(const Parser &p) = delete;

	std::vector<Node> trans_unit();

private:
	std::string setQuadrupleFileName();
	std::string setQuadrupleFileName(std::string &filename);
	void createQuadFile();

	void pushQuadruple(const std::string &name);
	void pushIncDec(const std::string &name);
	void createQuadruple(const std::string &op);
	void createFuncQuad(std::vector<Node> &params);
	void createIncDec();
	std::string num2str(size_t num);
	int str2int(std::string &str);
	std::string newLabel(const std::string &_l);

	bool next_is(int id);
	void expect(int id);
	bool is_keyword(Token &t, int id);
	bool is_type(const Token &t);
	bool is_inttype(Type &ty);
	bool is_floattype(Type &ty);
	bool is_arithtype(Type &ty);
	Type get_type(std::string key);
	int get_compound_assign_op(Token &t);
	std::string get_compound_assign_op_signal(Token &t);

	//
	Node createFuncNode(Type &ty, std::string & funcName, std::vector<Node> params, Node *body);
	Node createIntNode(Token &t);
	Node createCompoundStmtNode(std::vector<Node> &stmts);
	Node createDeclNode(Node &var);
	Node createDeclNode(Node &var, std::vector<Node> &init);
	Node createGLoVarNode(Type &ty, std::string name);
	Node createLocVarNode(Type &ty, std::string name);
	// 两元操作符
	Node createBinOpNode(Type &ty, int kind, Node *left, Node *right);
	Node createUnaryNode(int kind, Node &node);

	Node createRetStmtNode(Node *n);
	Node createJumpNode(std::string label);

	Node createIfStmtNode(Node *cond, Node *then, Node *els);


	//
	

	bool isFuncDef();
	Node funcDef();
	std::vector<Node> param_list();
	Node param_decl();

	/**
	 * decl
	 */
	void declaration(std::vector<Node> &list, bool isGlo);
	void skip_parenthesis(int *count);
	Type declarator(Type &ty, std::string &name, std::vector<Node> &params, int deal_type);
	Type decl_spec_opt(int *sclass);
	Type decl_specifiers(int *rsclass);
	Type direct_decl_tail(Type &retty, std::vector<Node> &params);
	Type func_param_list(Type *basetype, std::vector<Node> &params);
	Type decl_array(Type &ty);
	int array_int_expr();
	Node func_body(Type &functype, std::string, std::vector<Node> &params);
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
	Node conditional_expr();
	Node do_cond_expr();
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
	std::vector<Node> argument_expr_list();
	Node primary_expr();

	Node var_or_func(Token &t);


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
	Label labels;

	std::string label_break;


	std::ofstream out;
	std::vector<std::string> _stk_quad;
	std::vector<std::string> _stk_incdec;
};
#endif // !_ZCC_PARSER_H
