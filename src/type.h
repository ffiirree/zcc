#ifndef _ZCC_TYPE_H
#define _ZCC_TYPE_H

#include <vector>
#include <string>
#include <map>
#include <set>

#define _CASE_3(x0,x1,x2) case x0: case x1: case x2
#define _CASE_6(x0,x1,x2,x3,x4,x5) _CASE_3(x0,x1,x2): _CASE_3(x3,x4,x5)
#define _CASE_12(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11) _CASE_6(x0,x1,x2,x3,x4,x5): _CASE_6(x6,x7,x8,x9,x10,x11)

#define case_0_7	_CASE_6('0', '1', '2', '3','4', '5'): case '6': case '7'
#define case_0_9    _CASE_6('0', '1', '2', '3','4', '5'): _CASE_3('6', '7', '8'): case '9'
#define case_a_f    _CASE_6('a','b','c','d','e','f')
#define case_A_F    _CASE_6('A','B','C','D','E','F')
#define case_a_z    _CASE_12('a','b','c','d','e','f','g','h','i','j','k','l'): _CASE_12('m','n','o','p','q','r','s','t','u','v','w','x'): case 'y': case'z'
#define case_A_Z    _CASE_12('A','B','C','D','E','F','G','H','I','J','K','L'): _CASE_12('M','N','O','P','Q','R','S','T','U','V','W','X'): case 'Y': case'Z'
#define case_op		_CASE_6('=', '+', '-', '*', '/', '%'):_CASE_6('&', '^', '|', '<' ,'>', '!'):case '~'

#define OP_MAP  op(OP_ARROW, "->")\
				op(OP_A_ADD, "+=")\
				op(OP_A_AND, "&=")\
				op(OP_A_DIV, "/=")\
				op(OP_A_MOD, "%=")\
				op(OP_A_MUL, "*=")\
				op(OP_A_OR, "|=")\
				op(OP_A_SAL, "<<=")\
				op(OP_A_SAR, ">>=")\
				op(OP_A_SUB, "-=")\
				op(OP_A_XOR, "^=")\
				op(OP_DEC, "--")\
				op(OP_EQ, "==")\
				op(OP_GE, ">=")\
				op(OP_INC, "++")\
				op(OP_LE, "<=")\
				op(OP_LOGAND, "&&")\
				op(OP_LOGOR, "||")\
				op(OP_NE, "!=")\
				op(OP_SAL, "<<")\
				op(OP_SAR, ">>")

#define KEYWORD_MAP keyword(K_AUTO, "auto", true)\
					keyword(K_BOOL, "_Bool", true)\
					\
					keyword(K_CHAR, "char", true)\
					keyword(K_SHORT, "short", true)\
					keyword(K_INT, "int", true)\
					keyword(K_LONG, "long", true)\
					keyword(K_FLOAT, "float", true)\
					keyword(K_DOUBLE, "double", true)\
					\
					keyword(K_BREAK, "break", false)\
					keyword(K_CASE, "case", false)\
					keyword(K_COMPLEX, "_Complex", true)\
					keyword(K_CONST, "const", true)\
					keyword(K_CONTINUE, "continue", false)\
					keyword(K_DEFAULT, "default", false)\
					keyword(K_DO, "do", false)\
					keyword(K_ELSE, "else", false)\
					keyword(K_ENUM, "enum", true)\
					keyword(K_EXTERN, "extern", true)\
					keyword(K_FOR, "for", false)\
					keyword(K_GOTO, "goto", false)\
					keyword(K_IF, "if", false)\
					keyword(K_IMAGINARY, "_Imaginary", true)\
					keyword(K_INLINE, "inline", true)\
					keyword(K_REGISTER, "register", true)\
					keyword(K_RESTRICT, "restrict", true)\
					keyword(K_RETURN, "return", false)\
					keyword(K_SIGNED, "signed", true)\
					keyword(K_SIZEOF, "sizeof", false)\
					keyword(K_STATIC, "static", true)\
					keyword(K_STRUCT, "struct", true)\
					keyword(K_SWITCH, "switch", false)\
					keyword(ELLIPSIS, "...", false)\
					keyword(K_TYPEDEF, "typedef", true)\
					keyword(K_UNION, "union", true)\
					keyword(K_UNSIGNED, "unsigned", true)\
					keyword(K_VOID, "void", true)\
					keyword(K_VOLATILE, "volatile", true)\
					keyword(K_WHILE, "while", false)



enum {
	KEYWORD = 128, K_EOF,
#define keyword(key, name, _) key,
	KEYWORD_MAP
#undef keyword
};


enum {
	ID = 180, CHAR_, STRING_, INTEGER, FLOAT, FUNC_BODY, COMPARISON, SECTION, UNIT_ROOT,
	VAR, DECL, INIT, BIN_OP, UNARY_OP, STMT, TERN_OP, LABEL, COM_STMT, STRUCT_REF,
	ARRAY,
	PTR,
	COMPOUND_STMT, 
	DECL_BODY,
	FUNC_DECL,
	CONV, // ����ת��
	OP_PRE_INC,
	OP_PRE_DEC,
	OP_POST_INC,
	OP_POST_DEC,
	CAST,
	// Only in CPP
	MIN_CPP_TOKEN,
	TNEWLINE,
	TSPACE,
	TMACRO_PARAM,
    DS, // '##'

#define op(ty, _) ty,
	OP_MAP
#undef op
	OP_SHR, // �߼�����
	OP_SHL, // �߼����ƣ�ͬ��������
	OP_A_SHR,
	OP_A_SHL,
};


std::string getOnlyFileName(const std::string &_fn);
using HideSet = std::set<std::string>;

class Pos {
public:
	Pos() :line(1), cols(1) {  }
	Pos(int _line, int _cols) :line(_line), cols(_cols) {  }
	Pos(const Pos &p) : line(p.line), cols(p.cols) {  }
	Pos operator=(const Pos &p) { line = p.line; cols = p.cols; return (*this); }

	int line;
	int cols;
};
inline bool operator==(const Pos &p1, const Pos &p2) { return p1.line == p2.line && p1.cols == p2.cols; }

/**
 * �ʷ���Ԫ
 */
class Token {
public:
	Token() :type(K_EOF), pos(), counter(0), id(0) {}
	Token(int _type, int _id) :type(_type), counter(0), id(_id) {  }
	Token(int ty, const std::string &_sval) : type(ty), counter(0), sval(_sval) {  }
	Token(int ty, char _c) : type(ty), pos(), counter(0), ch(_c) {  }
	~Token() { if (type == ID || type == STRING_ || type == INTEGER || type == FLOAT)sval.~basic_string(); }

    Token(const Token &t) :type(t.type), pos(t.pos), counter(t.counter), _hs(t._hs), isfol(t.isfol)  { copyUnion(t); }
    Token operator=(const Token &t) { type = t.type; pos = t.pos; counter = t.counter; _hs = t._hs;isfol = t.isfol; copyUnion(t); return (*this); }

	inline int getType() const { return type; }
	inline Pos getPos() const { return pos; }
	inline void setPos(const Pos &_p) { pos = _p; }
	inline int getCounter() const { return counter; }
	inline void setCounter(int _c) { counter = _c; }
	inline int getId() const { return id; }
	inline std::string getSval() const { return sval; }
	inline int getCh() const { return ch; }

    std::string to_string() const;

private:
	void copyUnion(const Token &t);

	int type;
	Pos pos;
	int counter;

    HideSet *_hs = nullptr;
    bool isfol = false;           // first of line
	union
	{
		int id;                  // KEYWORD
		std::string sval;        // STRING 
		int ch;                  // CHAR_
	};

};
std::ostream &operator<<(std::ostream & os, const Token & t);
bool operator==(const Token &t1, const Token &t2);
bool operator!=(const Token &t1, const Token &t2);

class Node;
class Type;

class Field {
public:
    Field() {}
    Field(const std::string &name) :_name(name) {}
    Field(const std::string &name, Type *_t, int _o) : _name(name), _type(_t), _off(_o){}
    Field(const Field &f) : _name(f._name), _type(f._type), _off(f._off) {}
    Field operator=(const Field &f) { _name = f._name;  _type = f._type; _off = f._off; return *this; }


    std::string _name;
    Type *_type = nullptr;
    int _off = 0;
};

/**
 * �ڵ������
 */
class Type {
public:
	Type() {}
    Type(int ty) :type(ty) {}
	Type(int ty, int _s, std::vector<int> _l) :type(ty), _all_len(_s), len(_l) {}
	Type(int ty, int s, bool isunsig) :type(ty), size(s), isUnsig(isunsig), len(0), fields(), params(){ }
	Type(int ty, Type *ret, std::vector<Node> _params) : type(ty), retType(ret), params(_params) { }

    Type(const Type &t) { coping(t); }
    inline Type operator=(const Type &t) { coping(t); return *this; }

	inline Type create(int ty, int s, bool isuns) { type = ty, size = s, isUnsig = isuns; return *this; }

	inline int getType() const { return type; }
	inline bool isUnsigned() { return isUnsig; }
	inline bool isStatic() { return isSta; }
	inline void setStatic(bool is) { isSta = is; }
	inline int getSize() const { return size; }
	inline void setUnsig(bool isunsig) { isUnsig = isunsig; }

	int type = 0;
	int size = 0;

	bool isUnsig = false;
	bool isSta = false;

	// ָ��������飬�б������������
	Type *ptr = nullptr;

	// array length
	int _all_len = 0;
	std::vector<int> len;

    // struct or union
    std::vector<Field> fields;      // ÿ����Ա�����֣� ���ͣ� ƫ��
    bool is_struct = true;                       // struct or union

	//function
	Type *retType= nullptr;
	std::vector<Node> params;

private:
    inline void coping(const Type &t);
};

enum NodeKind{
	NODE_NULL,
	NODE_CHAR,
	NODE_INT,
	NODE_LONG,
	NODE_SHORT,

	NODE_FLOAT,
	NODE_DOUBLE,
	NODE_STRING,

	NODE_GLO_VAR,
	NODE_LOC_VAR,

	NODE_BINOP,
	NODE_UOP,
	NODE_FUNC,
	NODE_DECL,
	NODE_INIT,
	NODE_IF_STMT,
	NODE_OR_TOP,
	NODE_GOTO,
	NODE_LABEL,
	NODE_RETURN,
	NODE_COMP_STMT,
	NODE_STRUCT_REF,

	NODE_PARAMS,
	NODE_FUNC_DECL,
	NODE_DECL_PARAM,
	NODE_ADDR,
	NODE_DEREF
};


/**
 * AST's Node
 */
class Node;
class Node {
public:
	Node() : Node(NODE_NULL) { }
	Node(int k) :kind(k) { }
	Node(int k, Type &ty) :kind(k), type(ty) { }
	Node(int k, Type &ty, long val) :kind(k), type(ty), int_val(val) { }
	Node(int k, Type &ty, double val) :kind(k), type(ty), float_val(val) { }

	Node(const Node &n);
	Node operator=(const Node &n);
	~Node() { }

	inline int getKind() const { return kind; }
	inline Type getType() const { return type; }
	inline void setType(Type &ty) { type = ty; }

public:
	int kind = NODE_NULL;
	Type type;

	// Char, int, or long
	long int_val = 0;

	// Float or double
	double float_val = 0.0;

	// String
	std::string sval;

	// Local/global variable
	struct {
		std::string varName;

		// local
		int _off = 0;                 // �ֲ�������ջ�е�ƫ��
		std::vector<Node> lvarinit;

		// global
		std::string glabel;
	};

	// ��Ԫ�����
	Node *left = nullptr;             // ��ڵ�
	Node *right = nullptr;            // �ҽڵ�

	// һԪ�����
	Node *operand = nullptr;

	// �������������
	std::string funcName;           // ������
	std::vector<Node> params;       // ��������
	Node *body = nullptr;           // ������


	// ����
	struct {
		Node *decl_var = nullptr;
		std::vector<Node> decl_init;
	};

	// Initializer
	struct {
		Node *init_val = nullptr;
		int init_off = 0;
		Type to_type;
	};

	// If statement or ternary operator
	struct {
		Node *cond = nullptr;
		Node *then = nullptr;
		Node *els = nullptr;
	};

	// Goto and label
	struct {
		std::string label;
		std::string newLabel;
	};

	// return stmt
	Node *retval = nullptr;

	// Compound statement
	std::vector<Node> stmts;

    // struct

private:
	inline void copying(const Node &n);
};

#endif // !_ZCC_TYPE_H
