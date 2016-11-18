#ifndef _ZCC_TYPE_H
#define _ZCC_TYPE_H

#define _CASE_3(x0,x1,x2) case x0: case x1: case x2
#define _CASE_6(x0,x1,x2,x3,x4,x5) _CASE_3(x0,x1,x2): _CASE_3(x3,x4,x5)
#define _CASE_12(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11) _CASE_6(x0,x1,x2,x3,x4,x5): _CASE_6(x6,x7,x8,x9,x10,x11)

#define case_0_7	_CASE_6('0', '1', '2', '3','4', '5'): case '6': case '7'
#define case_0_9    _CASE_6('0', '1', '2', '3','4', '5'): _CASE_3('6', '7', '8'): case '9'
#define case_a_f    _CASE_6('a','b','c','d','e','f')
#define case_A_F    _CASE_6('A','B','C','D','E','F')
#define case_a_z    _CASE_12('a','b','c','d','e','f','g','h','i','j','k','l'): _CASE_12('m','n','o','p','q','r','s','t','u','v','w','x'): case 'y': case'z'
#define case_A_Z    _CASE_12('A','B','C','D','E','F','G','H','I','J','K','L'): _CASE_12('M','N','O','P','Q','R','S','T','U','V','W','X'): case 'Y': case'Z'


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
					keyword(K_BREAK, "break", false)\
					keyword(K_CASE, "case", false)\
					keyword(K_CHAR, "char", true)\
					keyword(K_COMPLEX, "_Complex", true)\
					keyword(K_CONST, "const", true)\
					keyword(K_CONTINUE, "continue", false)\
					keyword(K_DEFAULT, "default", false)\
					keyword(K_DO, "do", false)\
					keyword(K_DOUBLE, "double", true)\
					keyword(K_ELSE, "else", false)\
					keyword(K_ENUM, "enum", true)\
					keyword(K_EXTERN, "extern", true)\
					keyword(K_FLOAT, "float", true)\
					keyword(K_FOR, "for", false)\
					keyword(K_GOTO, "goto", false)\
					keyword(K_IF, "if", false)\
					keyword(K_IMAGINARY, "_Imaginary", true)\
					keyword(K_INLINE, "inline", true)\
					keyword(K_INT, "int", true)\
					keyword(K_LONG, "long", true)\
					keyword(K_REGISTER, "register", true)\
					keyword(K_RESTRICT, "restrict", true)\
					keyword(K_RETURN, "return", false)\
					keyword(K_SHORT, "short", true)\
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
	ID = 180, CHAR_, STRING_, INTEGER, FLOAT,
	FUNC, COMPARISON, SECTION, UNIT_ROOT,

#define op(ty, _) ty,
	OP_MAP
#undef op
};

#endif // !_ZCC_TYPE_H
