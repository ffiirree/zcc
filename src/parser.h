#ifndef _ZCC_PARSER_H
#define _ZCC_PARSER_H

#include "lex.h"

#define __IN_SCOPE__(localEnv, preEnv) do{ Env *old = preEnv; localEnv = new Env(old); old->setNext(localEnv);}while(0)
#define __OUT_SCOPE__(localEnv, _name) do{localEnv->setName(_name); localEnv = localEnv->pre(); }while(0)

class Env {
public:
	Env():Env(nullptr) {}
	Env(Env *p) :_pre(p), nodes() { }
	void push_back(Node &n);
	void pop_back() { nodes.pop_back(); }
	Node &back() { return nodes.back(); }
	Node &search(std::string &key);
	void set(std::string &_name, int ty, Node *_body);

	inline Env *pre() { return _pre; }
	inline std::vector<Env *> getNext() { return _next; }
	inline void setNext(Env *_n) { _next.push_back(_n); }
	inline void setName(const std::string &_n) { _name = _n; }
	inline std::string getName() { return _name; }
	inline int getNodesSize() {
		int r = 0;
		for (int i = 0; i < nodes.size(); ++i) {
			r += nodes.at(i).type.size;
		}
		return r;
	}
	size_t size() { return nodes.size(); }
	Node &at(size_t i) { return nodes.at(i); }

private:
	std::string _name;
	Env *_pre;
	std::vector<Env *> _next;
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

class BoolLabel{
public:
	BoolLabel() :_begin(), _true(), _false(), _next(){}
	BoolLabel(const BoolLabel &bl):_begin(bl._begin), _true(bl._true), _false(bl._false), _next(bl._next), _leaf(bl._leaf){}
	BoolLabel operator=(const BoolLabel &bl) { _begin = bl._begin; _true = bl._true, _false = bl._false; _next = bl._next;_leaf = bl._leaf; return *this; }

	std::string _begin;
	std::string _true;
	std::string _false;
	std::string _next;
	bool _leaf = false;
};

class Parser {
public:
	Parser(){}
	Parser(const std::string &filename) :lex(filename) { 
		globalenv = new Env(nullptr); 
		globalenv->setName(filename);
		createQuadFile();
		switch_case_label = newLabel("case");
	}
	Parser(Lex &l) :lex(l) { 
		globalenv = new Env(nullptr); 
		createQuadFile();
		switch_case_label = newLabel("case");
	}
	Parser(const Parser &p) = delete;
	Parser operator=(const Parser &p) = delete;

	std::vector<Node> trans_unit();
	std::string getQuadrupleFileName();
	Env *getGloEnv() { return globalenv; }
	Env *getLocEnv() { return localenv; }

private:
	
	std::string getQuadrupleFileName(std::string &filename);
	void createQuadFile();
	void generateIfGoto();
	void pushQuadruple(const std::string &name);
	void pushIncDec(const std::string &name);
	void createQuadruple(const std::string &op);
	void createBoolQuadruple(const std::string &op);
	void createBoolGenQuadruple(const std::string &op);
	void gotoLabel(const std::string &op);
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
	Node Parser::createFloatNode(Token &t);
	Node createCompoundStmtNode(std::vector<Node> &stmts);
	Node createDeclNode(Node &var);
	Node createDeclNode(Node &var, std::vector<Node> &init);
	Node createGLoVarNode(Type &ty, std::string name);
	Node createLocVarNode(Type &ty, std::string name);
	Node createFuncDeclParams(Type &ty);
	Node createFuncDecl(Type &ty, std::string & funcName, std::vector<Node> params);
	// 两元操作符
	Node createBinOpNode(Type &ty, int kind, Node *left, Node *right);
	Node createUnaryNode(int kind, Node &node);

	Node createRetStmtNode(Node *n);
	Node createJumpNode(std::string label);

	Node createIfStmtNode(Node *cond, Node *then, Node *els);


	//
	bool isFuncDef();
	Node funcDef();
	std::vector<Node> param_list(int deal_type);
	Node param_decl(int deal_type);

	/**
	 * decl
	 */
	void declaration(std::vector<Node> &list, bool isGlo);
	void skip_parenthesis(int *count);
	Type declarator(Type &ty, std::string &name, std::vector<Node> &params, int deal_type);
	Type decl_spec_opt(int *sclass);
	Type decl_specifiers(int *rsclass);
	Type direct_decl_tail(Type &retty, std::vector<Node> &params, int deal_type);
	Type func_param_list(Type *basetype, std::vector<Node> &params, int deal_type);
	Type decl_array(Type &ty);
	int array_int_expr();
	Node func_body(Type &functype, std::string, std::vector<Node> &params);
	std::vector<Node> initializer(Type &ty);
	std::vector<Node> decl_init(Type &ty);
	void init_list(std::vector<Node> &r, Type &ty, int off, bool designated);
	Node Parser::designator_list();

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
	Env *globalenv = nullptr;           // 全局
	Env *localenv = nullptr;            // 临时
	Env *funcCall = nullptr;            // 记录函数调用
	Label labels;

	std::string label_break;
	std::vector<std::string> _stk_if_goto;
	std::vector<std::string> _stk_if_goto_op;
	std::vector<std::string> _stk_if_goto_out;
	std::vector<std::string> _stk_ctl_bg_l;       // break ..con..
	std::vector<std::string> _stk_ctl_end_l;

	std::string switch_case_label;
	std::string switch_expr;

	std::ofstream out;
	std::vector<std::string> _stk_quad;
	std::vector<std::string> _stk_incdec;
	std::vector<BoolLabel> boolLabel;
};
#endif // !_ZCC_PARSER_H
