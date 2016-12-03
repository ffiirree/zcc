#include "parser.h"
#include "error.h"

void Parser::declaration(std::vector<Node> &list, bool isGlo)
{
	int sclass = 0;
	Type baseType = decl_spec_opt(&sclass);               // �����Ļ�������
	if (next_is(';'))
		return;

	for (;;) {
		std::string name;
		std::vector<Node> params;

		Type ty = declarator(&baseType, name, params, DECL_BODY);  // ����
		ty.setStatic(sclass == K_STATIC);

		if(sclass == K_TYPEDEF){     //typedef ����

		}
		else if (ty.isStatic() && !isGlo) {   // �ֲ�static����

		}
		else if (params.size() != 0) {
			createFuncDecl(baseType, name, params);
		}
		else {
			Node var;
			if (isGlo) {
				var = createGLoVarNode(ty, name);
			}
			else {
				var = createLocVarNode(ty, name);
				pushQuadruple(name);
			}
				

			if (next_is('=')) {
				list.push_back(createDeclNode(var, decl_init(ty)));
				if (!isGlo)
					createQuadruple("=");
			}
			else if (sclass != K_EXTERN && ty.getType() != NODE_FUNC) {
				list.push_back(createDeclNode(var));
			}
		}
		if (next_is(';'))
			return;
		if (!next_is(','))
			error("';' or ',' are expected, but got not");
	}
}

//initializer
std::vector<Node> Parser::decl_init(Type &ty)
{
	std::vector<Node> list;
	if (is_keyword(lex.peek(), '{') || ty.getType() == STRING_) {
		init_list(list, ty, 0, false);
	}
	else {
		list.push_back(assignment_expr());
	}
	return list;
}


void  Parser::init_list(std::vector<Node> &r, Type &ty, int off, bool designated)
{
	expect('{');
	std::vector<Node> list;
	do {
		if (is_keyword(lex.peek(), '[') || is_keyword(lex.peek(), '.')) {
			r.push_back(designator_list());
			expect('=');
		}
		list = decl_init(ty);
		for (size_t i = 0; i < list.size(); ++i) {
			r.push_back(list.at(i));
		}
	} while (next_is(','));
	expect('}');
}

Node Parser::designator_list()
{
	if (next_is('[')) {
		Node r = conditional_expr();
		expect(']');
		return r;
	}
	else if (next_is('.')) {
		Node r;
		Token t = lex.next();
		if (t.getId() == ID)
			r = localenv->search(t.getSval());
		if (r.getKind() == 0)
			error("init list error");
		return r;
	}
}


/**
 * \\ int * var, var2;
 * \\ int *(*pt)(int a, int b)
 * declarator = ['*'] ID direct_declarator_tail
 *            | ['*'] '(' declarator ')' direct_declarator_tail
 *
 * \\ ֻ֧�ֶ�������,[]��[10]��ʽ
 * direct_declarator_tail = '[' ']' direct_declarator_tail
                          |'[' conditional_expr ']' direct_declarator_tail

                          | '(' param_type_list ')' direct_declarator_tail                                  # Function declarators (including prototypes)
                          | '(' [ID_list] ')' direct_declarator_tail                                        # Function declarators (including prototypes)
                          | empty
 */
Type Parser::conv2ptr(Type ty)
{
	Type r;
	Type *ptr = new Type(ty);
	r.type = PTR;
	r.ptr = ptr;
	r.size = 4;

	return r;
}

Type Parser::declarator(Type *ty, std::string &name, std::vector<Node> &params, int deal_type)
{
	// ָ�����
	if (next_is('*')) {
		return declarator(new Type(conv2ptr(*ty)), name, params, deal_type);
	}
	// int (*ptr)();
	if (next_is('(')) {
		Type *stub = new Type();
		Type t = declarator(stub, name, params, deal_type);
		expect(')');
		*stub = direct_decl_tail(ty, params, deal_type);
		return t;
	}

	Token t = lex.next();
	if (t.getType() == ID) {
		name = t.getSval();
		return direct_decl_tail(ty, params, deal_type);
	}
	return direct_decl_tail(ty, params, deal_type);
}


Type Parser::decl_spec_opt(int *sclass)
{
	if (is_type(lex.peek()))
		return decl_specifiers(sclass);

	error("missing type, defalut 'int'.");
	return Type(K_INT, 4, false);
}

Type Parser::direct_decl_tail(Type *retty, std::vector<Node> &params, int decl_type)
{
	// ֻ֧�ֶ�������
	if (next_is('[')) {
		int _len = 0;
		if (next_is(']')) {
			_len = -1;
		}
		else {
			_len = array_int_expr();
			expect(']');
		}

		Token tok = lex.peek();
		Type t = direct_decl_tail(retty, std::vector<Node>(), decl_type);
		if(t.len != 0)
			_len = _len * t.len;

		if (t.getType() == NODE_FUNC)
			error("array of functions");
		return Type(ARRAY, retty->size, _len);
	}

	// ��������ţ���Ϊ����
	if (next_is('(')) {
		if (decl_type == DECL_BODY)
			decl_type = NODE_FUNC_DECL;
		return func_param_list(retty, params, decl_type);
	}

	return *retty;
}


int Parser::array_int_expr()
{
	Node r = conditional_expr();
	if (r.kind == NODE_INT || r.kind == NODE_CHAR || r.kind == NODE_INT) {
		return r.int_val;
	}
	return 0;
}


Type Parser::decl_specifiers(int *rsclass)
{
	int current_class = 0;
	Token t;
	Type type;

	enum { k_zero = 0, k_void = 0x01, k_bool, k_char, k_int, k_float, k_double } kind = k_zero;
	enum { size_zero = 0, k_short = 0x02, k_long = 0x04 } size = size_zero;
	enum { sig_zero = 0, k_signed = 0x01, k_unsigned } issigned = sig_zero;

	for (;;) {
		t = lex.next();
		if (t.getType() == K_EOF)
			error("error");

		if (kind == 0 && t.getType()== ID) {
			type = get_type(t.getSval());
		}

		if (t.getType()!= KEYWORD) {
			break;
		}
		switch (t.getId())
		{
			// Type specifiers
#define type_spec_cheak(cheak, val, type) do{if(cheak) error("error %s specifier.", type); else cheak = val;}while(0)
			// ֻ�ܳ���һ��
		case K_VOID:     type_spec_cheak(kind, k_void, "void");break;
		case K_CHAR:     type_spec_cheak(kind, k_char, "char");break;
		case K_INT:      type_spec_cheak(kind, k_int, "int");break;
		case K_FLOAT:    type_spec_cheak(kind, k_float, "float");break;
		case K_DOUBLE:   type_spec_cheak(kind, k_double, "double");break;
		case K_BOOL:     type_spec_cheak(kind, k_bool, "bool"); break;

		case K_SHORT:    type_spec_cheak(size, k_short, "short"); break;
		case K_LONG:     type_spec_cheak(size, k_long, "long");break;

		case K_SIGNED:   type_spec_cheak(issigned, k_signed, "signed");break;
		case K_UNSIGNED: type_spec_cheak(issigned, k_unsigned, "unsigned"); break;

		case K_TYPEDEF:  type_spec_cheak(current_class, K_TYPEDEF, "typedef");break;
		case K_EXTERN:   type_spec_cheak(current_class, K_EXTERN, "extern");break;
		case K_STATIC:   type_spec_cheak(current_class, K_STATIC, "static");break;
		case K_AUTO:     type_spec_cheak(current_class, K_AUTO, "auto");break;
		case K_REGISTER: type_spec_cheak(current_class, K_REGISTER, "register");break;
#undef type_spec_cheak

		case K_COMPLEX: // do something ..
		case K_IMAGINARY: // do something ..


		case K_ENUM: _log_("no enum."); break;
		case K_STRUCT: _log_("no struct."); break;
		case K_UNION: _log_("no union."); break;

			// Storage-class specifiers
			// ֻ�ܳ�������������λ����ֻ��һ��
			// Type qualifiers
		case K_CONST: _log_("no const."); break;
		case K_RESTRICT: _log_("no restrict."); break;
		case K_VOLATILE: _log_("no volatile."); break;

			// Function specifiers
		case K_INLINE: _log_("no inline."); break;

		default: 
			goto _end;
			break;
		}
	}
_end:
	if (current_class)
		*rsclass = current_class;

	if (kind) {
		switch (kind)
		{
		case k_void: type.create(K_VOID, 0, false); break;
		case k_bool: type.create(K_BOOL, 1, false); break;
		case k_char: type.create(K_CHAR, 1, issigned == k_signed); break;
		case k_int: type.create(K_INT, 4, issigned == k_signed); break;
		case k_float: type.create(K_FLOAT, 4, false); break;
		case k_double: type.create(K_DOUBLE, 8, false); break;
		default: break;
		}
	}
	if (size) {
		switch (size)
		{
		case k_short: type.create(K_SHORT, size, issigned == k_signed); break;
		case k_long: type.create(K_LONG, size, issigned == k_signed); break;
		default: break;
		}
	}

	lex.back();

	return type;
}


std::vector<Node> Parser::initializer(Type &ty)
{
	std::vector<Node> list;
	return list;
}