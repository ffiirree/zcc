#ifndef _ZCC_PARSER_H
#define _ZCC_PARSER_H

#include <map>
#include "lex.h"

#define _OVERLOAD_

#define __IN_SCOPE__(localEnv, preEnv, _name) do{ \
                                                   Env *old = preEnv; \
                                                   localEnv = new Env(old); \
                                                   old->setNext(localEnv); \
                                                   std::string _name_ = _name; \
                                                   localEnv->setName(_name_); \
                                                   out << ".inscope\t" << _name_ << std::endl; \
                                             }while(0)
#define __OUT_SCOPE__(localEnv) do{ localEnv = localEnv->pre(); out << ".outscope" << std::endl; }while(0)

class Env {
public:
	Env():Env(nullptr) {}
	Env(Env *p) :_pre(p), nodes() { }
	void push_back(Node &n);
	void pop_back() { nodes.pop_back(); }
	Node &back() { return nodes.back(); }
	Node &search(const std::string &key);
   
    void setFuncDef(Node &_def);

	inline Env *pre() { return _pre; }
	inline std::vector<Env *> getNext() { return _next; }
	inline void setNext(Env *_n) { _next.push_back(_n); }
	inline void setName(const std::string &_n) { _name = _n; }
	inline std::string getName() { return _name; }
	size_t size() { return nodes.size(); }
	Node &at(size_t i) { return nodes.at(i); }
	int _call_size = 0;
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
		for (size_t i = 0; i < labels.size(); ++i) {
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

class StrCard {
public:
	StrCard() :_str(), _label(){}
	StrCard(const std::string &_s, const std::string &_l) :_str(_s), _label(_l) { }
	StrCard(const StrCard &bl) :_str(bl._str), _label(bl._label) {}
	StrCard operator=(const StrCard &bl) { _str = bl._str; _label = bl._label; return *this; }

	std::string _str;
	std::string _label;
};

/**
 * @ Parser,语法分析和中间代码生成
 */
class Parser {
public:
	Parser(){}
    // 构造函数，为了预处理器分析表达式的使用
    Parser(Lex &l) :lex(l) { }

	Parser(Lex &l, const std::string &_ofn) :lex(l), _of_name(_ofn + ".q") { 
		globalenv = new Env(nullptr); 
        globalenv->setName(_of_name);
		createQuadFile();
		switch_case_label = newLabel("case");
        trans_unit();
	}
	Parser(const Parser &p) = delete;
	Parser operator=(const Parser &p) = delete;

	std::vector<Node> trans_unit();
	Env *getGloEnv() { return globalenv; }
	Env *getLocEnv() { return localenv; }
	std::vector<StrCard> getStrTbl() { return const_string; }
	std::string newLabel(const std::string &_l);
    std::string getQuadrupleFileName() { return _of_name; }
    inline std::vector<std::string> getFloatConst() { return float_const; }
    std::string searchEnum(const std::string &key);

    Node expr();       // 允许预处理器使用
private:
    bool cheak_redefined(Env *_env, const std::string &_name);
	Type conv2ptr(Type ty);
	void createQuadFile();
	void generateIfGoto();
	void pushQuadruple(const std::string &name);
	void pushIncDec(const std::string &name);
	void createQuadruple(const std::string &op);
	void createUnaryQuadruple(const std::string &op);
	void createBoolQuadruple(const std::string &op);
	void createBoolGenQuadruple(const std::string &op);
	void gotoLabel(const std::string &op);
	void createFuncQuad(std::vector<Node> &params);
	void createIncDec();
	std::string num2str(size_t num);
	int str2int(std::string &str);
	

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
	Node createIntNode(Token &t, int size, bool isch);
	Node createIntNode(Type &ty, int val);
	Node createFloatNode(Type &ty, double val);
	Node createFloatNode(Token &t);
	Node createStrNode(Token &t);
	Node createCompoundStmtNode(std::vector<Node> &stmts);
	Node createDeclNode(Node &var);
	Node createDeclNode(Node &var, std::vector<Node> &init);
	Node createGLoVarNode(Type &ty, std::string name);
	Node createLocVarNode(Type &ty, std::string name);
	Node createFuncDeclParams(Type &ty);
	Node createFuncDecl(Type &ty, std::string & funcName, std::vector<Node> params);
	// 两元操作符
	Node createBinOpNode(Type &ty, int kind, Node *left, Node *right);
	Node createUnaryNode(int kind, Type &ty, Node &node);

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
	Type declarator(Type *ty, std::string &name, std::vector<Node> &params, int deal_type);
	Type decl_spec_opt(int *sclass);
	Type decl_specifiers(int *rsclass);
	Type direct_decl_tail(Type *retty, std::vector<Node> &params, int deal_type);
	Type func_param_list(Type *basetype, std::vector<Node> &params, int deal_type);
	void decl_array(Type *ty);
	int array_int_expr();
	Node func_body(Type &functype, std::string, std::vector<Node> &params);
	std::vector<Node> initializer(Type &ty);
	std::vector<Node> decl_init(Type &ty);
	void init_list(std::vector<Node> &r, Type &ty, int off, bool designated);
	Node designator_list();

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
	Node break_stmt();

	/**
	 * expr
	 */
	//Node expr();
	Node expr_opt();
	Node comma_expr();
	Node assignment_expr();
    Node conditional_expr();
	Node logical_or_expr();
	Node logical_and_expr();
	Node bit_or_expr();
	Node bit_xor_expr();
	Node bit_and_expr();
	Node equal_expr();
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
	Node unary_addr();
	Node unary_deref(Token &t);
	Node unary_minus();
	Node unary_bitnot(Token &t);
	Node unary_lognot();

	Node wrap(Type &t, Node &node);

	void ensure_inttype(Node &node);

	// 检查
	bool ensure_lvalue(const Node &node);
	Type usual_arith_conv(Type &t, Type &u);


	Lex lex;
	Env *globalenv = nullptr;              // 全局
	Env *localenv = nullptr;               // 临时
	Env *funcCall = nullptr;               // 记录函数调用
	Label labels;                          // 源程序中的Label
	std::vector<StrCard> const_string;     // 字符串常量
    std::vector<std::string> float_const;  // 浮点数常量
    //std::vector<std::string> enum_const;   // 枚举常量表
    std::map<std::string, std::string> enum_const;

	std::string label_break;
	std::vector<std::string> _stk_if_goto;
	std::vector<std::string> _stk_if_goto_op;
	std::vector<std::string> _stk_if_goto_out;
	std::vector<std::string> _stk_ctl_bg_l;       // break ..con..
	std::vector<std::string> _stk_ctl_end_l;

	std::string switch_case_label;
	std::string switch_expr;

    std::string _of_name;
	std::ofstream out;
	std::vector<std::string> _stk_quad;
	std::vector<std::string> _stk_incdec;
	std::vector<BoolLabel> boolLabel;
    std::map<std::string, Type> custom_type_tbl;
    Type getCustomType(const std::string &_n);
    Type struct_def();
    Type enum_def();
#ifdef _OVERLOAD_
	std::string getOverLoadName(const std::string &name, std::vector<Node> &_p);
#endif

    bool cheak_is_float(const Node &n);
    bool cheak_is_int_type(const Node &n);
    bool cheak_is_custom_type(const Node &n);
};

/**
 * @berif 语法分析过程中，浮点常量已经被剔除，所以只有整数
 */
inline bool isNumber(const std::string &str)
{
	if (str.empty())
		return false;

	if (!((str.at(0) >= '0' && str.at(0) <= '9')
		|| (str.at(0) == '-' || str.at(0) == '+')))
		return false;

	// 且浮点数
	bool _has_dot = false;
	for (size_t i = 1; i < str.size();++i) {
		if (!((str.at(i) >= '0' && str.at(i) <= '9') || (str.at(i) == '.' && _has_dot == false)))
			return false;
		if (str.at(i) == '.')
			_has_dot = true;
	}
	return true;
}

#endif // !_ZCC_PARSER_H
