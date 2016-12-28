#ifndef _ZCC_PARSER_H
#define _ZCC_PARSER_H

#include <iomanip>
#include <map>
#include "lex.h"

//#define _OVERLOAD_

#define __IN_SCOPE__(localEnv, preEnv, _name) do{ \
                                                   Env *old = preEnv; \
                                                   localEnv = new Env(old); \
                                                   old->setNext(localEnv); \
                                                   std::string _name_ = _name; \
                                                   localEnv->setName(_name_); \
                                                   gen_quad(".inscope", _name_);\
                                             }while(0)
#define __OUT_SCOPE__(localEnv) do{ localEnv = localEnv->pre(); gen_quad(".outscope"); }while(0)

#define _EN_CONDITION_()                isCondition = true
#define _DIS_CONDITION_()               isCondition = false
 
#define _GENQL_(q1)                     gen_quad(q1, ":")
#define _GENQ1_(q1)                     gen_quad(q1)
#define _GENQ2_(q1, q2)                 gen_quad(q1, q2)
#define _GENQ3_(q1, q2, q3)             gen_quad(q1, q2, q3)
#define _GENQ4_(q1, q2, q3, q4)         gen_quad(q1, q2, q3, q4)

/**
 * @berif scope/env
 */
class Env {
public:
    Env() :Env(nullptr) {}
    Env(Env *p) :pre_(p), nodes_() { }
    Env(const Env&) = delete;
    Env &operator=(const Env&) = delete;
    ~Env() = default;

    void push_back(Node &n);
    void pop_back() { nodes_.pop_back(); }
    Node &back() { return nodes_.back(); }
    Node &search(const std::string &key);

    void setFuncDef(Node &_def);

    inline Env *pre() { return pre_; }
    inline std::vector<Env *> getNext() { return next_; }
    inline void setNext(Env *_n) { next_.push_back(_n); }
    inline void setName(const std::string &_n) { name_ = _n; }
    inline std::string getName() { return name_; }
    size_t size() { return nodes_.size(); }
    Node &at(size_t i) { return nodes_.at(i); }
    int call_size_ = 0;

private:
    std::string name_;
    Env *pre_;
    std::vector<Env *> next_;
    std::vector<Node> nodes_;
};

/**
 * @class Label
 */
class Label {
public:
    Label() :Label(nullptr) {}
    Label(Env *p) : labels() { }
    Label(const Label&) = delete;
    Label &operator=(const Label&) = delete;
    ~Label() = default;

    void push_back(const std::string &_l);
    void push_back_un(const std::string &_l) {
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

/**
 * @class BoolLabel
 */
class BoolLabel {
public:
    BoolLabel() :true_(), false_() {}
    BoolLabel(BoolLabel *pre) : true_(), false_(), pre_(pre) {  }
    BoolLabel(const BoolLabel &bl) :
        true_(bl.true_),
        false_(bl.false_),
        leaf_(bl.leaf_),
        trueList_(bl.trueList_),
        falseList_(bl.falseList_),
        nextList_(bl.nextList_),
        pre_(bl.pre_) {
    }
    BoolLabel &operator=(const BoolLabel &bl) {
        true_ = bl.true_;
        false_ = bl.false_;
        leaf_ = bl.leaf_;
        trueList_ = bl.trueList_;
        falseList_ = bl.falseList_;
        nextList_ = bl.nextList_;
        pre_ = bl.pre_;
        return *this;
    }
    ~BoolLabel() = default;

    std::vector<int> *trueList_ = nullptr;
    std::vector<int> *falseList_ = nullptr;
    std::vector<int> *nextList_ = nullptr;
    BoolLabel *pre_ = nullptr;


    std::string true_;
    std::string false_;
    bool leaf_ = false;
};

class StrCard {
public:
    StrCard() :_str(), _label() {}
    StrCard(const std::string &_s, const std::string &_l) :_str(_s), _label(_l) { }
    StrCard(const StrCard &bl) :_str(bl._str), _label(bl._label) {}
    StrCard operator=(const StrCard &bl) { _str = bl._str; _label = bl._label; return *this; }

    std::string _str;
    std::string _label;
};

/**
 * @class Parser
 */
class Parser {
public:
    using Quadruple = std::tuple<std::string, std::string, std::string, std::string>;
public:
    Parser() = default;
    Parser(TokenSequence &ts) :ts_(ts), quadStk_() { }
    Parser(TokenSequence &ts, const std::string &_ofn) :ts_(ts), _of_name(_ofn + ".q"), quadStk_() {
        globalenv = new Env(nullptr);
        globalenv->setName(_of_name);
        createQuadFile();
        switch_case_label = newLabel("case");
        trans_unit();
    }
    Parser(const Parser &p) = delete;
    Parser &operator=(const Parser &p) = delete;
    ~Parser() = default;

    std::vector<Node> trans_unit();
    Env *getGloEnv() const { return globalenv; }
    Env *getLocEnv() const { return localenv; }
    std::vector<StrCard> getStrTbl() const { return const_string; }
    std::string newLabel(const std::string &_l);
    std::string getQuadrupleFileName() const { return _of_name; }
    inline std::vector<std::string> getFloatConst() const { return float_const; }
    std::string searchEnum(const std::string &key);
    bool compute_bool_expr();
    Node expr();

private:
    bool cheak_redefined(Env *_env, const std::string &_name);
    Type conv2ptr(Type ty);
    void createQuadFile();
    void pushQuadruple(const std::string &name);
    void pushIncDec(const std::string &name);
    void createQuadruple(const std::string &op);
    void createUnaryQuadruple(const std::string &op);
    void computeBoolExpr(const std::string &op);
    void createFuncQuad(std::vector<Node> &params);
    void createIncDec();
    void createRelOpQuad(const std::string &op);
    std::string num2str(size_t num);
    int str2int(std::string &str);


    bool next_is(int id);
    void expect(int id);
    bool is_keyword(const Token &t, int id);
    bool is_type(const Token &t);
    bool is_inttype(const Type &ty);
    bool is_floattype(const Type &ty);
    bool is_arithtype(const Type &ty);
    Type get_type(const std::string &key);
    int get_compound_assign_op(const Token &t);
    std::string get_compound_assign_op_signal(const Token &t);

    //
    Node createFuncNode(const Type &ty, const std::string & funcName, std::vector<Node> params, Node *body);
    Node createIntNode(const Token &t, int size, bool isch);
    Node createIntNode(const Type &ty, int val);
    Node createFloatNode(const Type &ty, double val);
    Node createFloatNode(const Token &t);
    Node createStrNode(const Token &t);
    Node createCompoundStmtNode(std::vector<Node> &stmts);
    Node createDeclNode(Node &var);
    Node createDeclNode(Node &var, std::vector<Node> init);
    Node createGLoVarNode(const Type &ty, const std::string &name);
    Node createLocVarNode(const Type &ty, const std::string &name);
    Node createFuncDeclParams(const Type &ty);
    Node createFuncDecl(const Type &ty, const std::string & funcName, const std::vector<Node> &params);
    Node createBinOpNode(const Type &ty, int kind, Node *left, Node *right);
    Node createUnaryNode(int kind, const Type &ty, Node &node);

    Node createRetStmtNode(Node *n);
    Node createJumpNode(const std::string &label);

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
    Type struct_def();
    Type enum_def();

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
    Node bool_expr();
	Node expr_opt();
	Node comma_expr();
	Node assignment_expr();
    Node conditional_expr(Node *node);
    Node com_conditional_expr();
	Node logical_or_expr();
	Node logical_and_expr(bool isLeft);
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
    Type getCustomType(const std::string &_n);
    

#ifdef _OVERLOAD_
    std::string getOverLoadName(const std::string &name, std::vector<Node> &_p);
#endif

    void ensure_inttype(Node &node);
    bool ensure_lvalue(const Node &node);
    Type usual_arith_conv(Type &t, Type &u);
    bool cheak_is_float(const Node &n);
    bool cheak_is_int_type(const Node &n);
    bool cheak_is_custom_type(const Node &n);

    /**
     * \ Generate Quadruples.
     */
    inline void gen_quad(const std::string &q1, 
        const std::string &q2 = std::string(), 
        const std::string &q3 = std::string(), 
        const std::string &q4 = std::string()) {
        quadStk_.push_back({ q1, q2, q3, q4 });
    }

    void out_quad();

    std::vector<int> *makelist(int index);
    std::vector<int> *merge(std::vector<int> *p1, std::vector<int> *p2);
    void backpatch(std::vector<int> *p, int index);
    std::vector<BoolLabel *> boolLabels_;

    /**
     * \ Storage quadruples for generating quad file.
     */
    std::vector<Quadruple> quadStk_;

    /**
     * \ Token Sequence from Lex.
     */
    TokenSequence ts_;

    /**
     * @berif Every scope has a symbol table(Env).
     * \ globalenv: Always point to global scope.
     * \ localenv: Always point to current scope.
     */
	Env *globalenv = nullptr;
	Env *localenv = nullptr;

	Label labels;

    /**
     * @berif constant
     * \ string
     * \ float 
     * \ enum
     */
	std::vector<StrCard> const_string;
    std::vector<std::string> float_const;
    std::map<std::string, std::string> enum_const;

	std::string label_break;
	std::vector<std::string> _stk_ctl_bg_l;
	std::vector<std::string> _stk_ctl_end_l;

	std::string switch_case_label;
	std::string switch_expr;

    std::string _of_name;

    /**
     * \ Output stream for quad file.
     */
	std::ofstream out;

	std::vector<std::string> quad_arg_stk_;
	std::vector<std::string> _stk_incdec;
    std::map<std::string, Type> custom_type_tbl;

    bool isCondition = false;
    bool isComputeBool = false;
};

/**
 * @berif Cheak whether the string is a number.
 */
inline bool isNumber(const std::string &str)
{
	if (str.empty())
		return false;

	if (!((str.at(0) >= '0' && str.at(0) <= '9')
		|| (str.at(0) == '-' || str.at(0) == '+')))
		return false;

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
