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

/**
 * \ Operator
 */
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

/**
 * \ Keyword
 */
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

/**
 * \ Virtual Machine Instructions
 */
#define VM_INS vminsmap("mov",    mov)\
                vminsmap("movl",  movl)\
                vminsmap("movb",  movb)\
                vminsmap("movw",  movw)\
                vminsmap("subl",  subl)\
                vminsmap("addl",  addl)\
                vminsmap("imull", imull)\
                vminsmap("idivl", idivl)\
                vminsmap("call",  call)\
                vminsmap("leave", leave)\
                vminsmap("ret",   ret)\
                vminsmap("pushl", pushl)\
                vminsmap("popl",  popl)\
                vminsmap("andl",  andl)\
                vminsmap("xorl",  xorl)\
                vminsmap("orl",   orl)\
                vminsmap("sarl",  sarl)\
                vminsmap("sall",  sall)\
                vminsmap("shrl",  shrl)\
                vminsmap("notl",  notl)\
                vminsmap("leal",  leal)\
                vminsmap("exit",  exitvm)\
                vminsmap("cmpl",  cmpl)\
                vminsmap("jg",    jg)\
                vminsmap("jl",    jl)\
                vminsmap("jge",   jge)\
                vminsmap("jle",   jle)\
                vminsmap("jmp",   jmp)\
                vminsmap("je",    je)\
                vminsmap("jne",   jne)\
                vminsmap("ja",    ja)\
                vminsmap("jb",    jb)\
                vminsmap("jae",   jae)\
                vminsmap("jbe",   jbe)


enum TokenKind {
    T_KEYWORD = 180, T_IDENTIFIER, T_CHAR, T_STRING, T_FLOAT, T_INTEGER,
    T_NEWLINE, T_SPACE, T_OP, T_EOF,

    //keyword
#define keyword(key, name, _) key,
    KEYWORD_MAP
#undef keyword

    // operator
#define op(ty, _) ty,
    OP_MAP
#undef op
    OP_SHR,
    OP_SHL,
    OP_A_SHR,
    OP_A_SHL,
    OP_PRE_INC,
    OP_PRE_DEC,
    OP_POST_INC,
    OP_POST_DEC,
    CONV,
    CAST,

    // preprocessor,
    OP_BACKLASH, 
    OP_DS,                // '##'
};



enum {
    FUNC_BODY = 265, BIN_OP, UNARY_OP, COM_STMT,
    ARRAY,
    PTR,
    DECL_BODY,
    FUNC_DECL,
};


std::string getOnlyFileName(const std::string &_fn);
using HideSet = std::set<std::string>;

class Pos {
public:
    Pos() :line(1), cols(1) {  }
    Pos(int _line, int _cols) :line(_line), cols(_cols) {  }
    Pos(const Pos &p) : line(p.line), cols(p.cols) {  }
    Pos &operator=(const Pos &p) { line = p.line; cols = p.cols; return (*this); }
    ~Pos() = default;

    int line;
    int cols;
};

/**
 * @class Token
 */
class Token {
public:
    Token() :kind_(T_EOF), pos_(), id_(0) { }
    Token(int id) :kind_(T_KEYWORD), id_(id) { }
    Token(int kind, const std::string &sval) : kind_(kind), sval_(sval) { }
    Token(int kind, char ch) : kind_(kind), pos_(), ch_(ch) { }
    Token(const Token &t) { copying(t); }
    Token &operator=(const Token &t) { copying(t); return (*this); }
    ~Token() { if (kind_ == T_IDENTIFIER || kind_ == T_STRING || kind_ == T_INTEGER || kind_ == T_FLOAT)sval_.~basic_string(); }

    inline int getType() const { return kind_; }
    inline Pos getPos() const { return pos_; }
    inline void setPos(const Pos &_p) { pos_ = _p; }
    inline int getId() const { return id_; }
    inline std::string getSval() const { return sval_; }
    inline int getCh() const { return ch_; }

    std::string toString() const;
    void setBOL() { isbol_ = true; }
    bool isBOL() { return isbol_; }

    bool needExpand();

    int kind_ = 0;
    Pos pos_;
    bool isbol_ = false;
    HideSet *hs_ = nullptr;

    union
    {
        int id_;                  // which keyword or operator
        std::string sval_;        // STRING value
        int ch_;                  // CHAR_ value
    };

private:
    void copyUnion(const Token &t);
    void copying(const Token &t);
};
std::ostream &operator<<(std::ostream & os, const Token & t);

class Node;
class Type;

class Field {
public:
    Field() {}
    Field(const std::string &name) :_name(name) {}
    Field(const std::string &name, Type *_t, int _o) : _name(name), _type(_t), _off(_o) {}
    Field(const Field &f) : _name(f._name), _type(f._type), _off(f._off) {}
    Field &operator=(const Field &f) { _name = f._name;  _type = f._type; _off = f._off; return *this; }
    ~Field() = default;

    std::string _name;
    Type *_type = nullptr;
    int _off = 0;
};

/**
 * @class Type
 */
class Type {
public:
    Type() {}
    Type(int ty) :type(ty) {}
    Type(int ty, int _s, std::vector<int> _l) :type(ty), _all_len(_s), len(_l) {}
    Type(int ty, int s, bool isunsig) :type(ty), size_(s), isUnsig(isunsig), len(0), fields(), params() { }
    Type(int ty, Type *ret, std::vector<Node*> _params) : type(ty), retType(ret), params(_params) { }

    Type(const Type &t) { coping(t); }
    Type &operator=(const Type &t) { coping(t); return *this; }
    ~Type() = default;

    inline Type create(int ty, int s, bool isuns) { type = ty, size_ = s, isUnsig = isuns; return *this; }

    inline int getType() const { return type; }
    inline bool isUnsigned() { return isUnsig; }
    inline bool isStatic() { return isSta; }
    inline void setStatic(bool is) { isSta = is; }
    inline int getSize() const { return size_; }
    inline void setUnsig(bool isunsig) { isUnsig = isunsig; }

    int type = 0;
    int size_ = 0;

    bool isUnsig = false;
    bool isSta = false;

    // 
    Type *ptr = nullptr;

    // array length
    int _all_len = 0;
    std::vector<int> len;

    // struct or union
    std::vector<Field> fields;
    bool is_struct = true;

    //function
    Type *retType = nullptr;
    std::vector<Node*> params;

private:
    void coping(const Type &t);
};


enum NodeKind {
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
    Node(int k) :kind_(k) { }
    Node(int k, const Type &ty) :kind_(k), type_(ty) { }
    Node(int k, const Type &ty, long val) :kind_(k), type_(ty), int_val(val) { }
    Node(int k, const Type &ty, double val) :kind_(k), type_(ty), float_val(val) { }

    Node(const Node &n) { copying(n); }
    Node &operator=(const Node &n) { copying(n); return *this; }
    ~Node() = default;

    inline int getKind() const { return kind_; }
    inline Type getType() const { return type_; }
    inline void setType(Type ty) { type_ = ty; }

    std::string name() const {
        if (kind_ == NODE_GLO_VAR || kind_ == NODE_LOC_VAR) return varName;
        if (kind_ == NODE_FUNC || kind_ == NODE_FUNC_DECL) return funcName;

        return std::string();
    }
    
#if defined(WIN32)
    inline void setFuncName(const std::string &name) { funcName = "_" + name; }
    inline void setVarName(const std::string &name) { varName = "_" + name; }
#elif defined(__linux__)
    inline void setFuncName(const std::string &name) { funcName = name; }
    inline void setVarName(const std::string &name) { varName = name; }
#endif

public:
    int kind_ = NODE_NULL;
    Type type_;

	/**
     * \ Char short int long
     */
    long int_val = 0;

	/**
     * \ float or double
     */
    double float_val = 0.0;

	/**
     * \ string
     */
    std::string sval_;

	/**
     * \ Local/global variable
     */
    // private: std::string varName;
    int off_ = 0;
    std::vector<Node*> lvarinit_;

	/**
     * \ 
     */
    Node *left_ = nullptr;
    Node *right_ = nullptr;

	/**
     * \ unary op
     */
    Node *operand_ = nullptr;

	/**
     * \function define and function declartion
     */
    // private: std::string funcName;
    std::vector<Node*> params;
    Node *body = nullptr;


	// declartion
    Node *decl_var = nullptr;
    std::vector<Node*> decl_init;

	// Initializer
    Node *init_val = nullptr;
    int init_off = 0;

	// If statement or ternary operator
    Node *cond = nullptr;
    Node *then = nullptr;
    Node *els = nullptr;

    // Goto and label
    std::string label;
    std::string newLabel;

    // return stmt
    Node *retval = nullptr;

    // Compound statement
    std::vector<Node *> stmts;
private:
    void copying(const Node &n);
    std::string funcName;
    std::string varName;
};

#endif // !_ZCC_TYPE_H
