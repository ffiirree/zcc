#include "parser.h"
#include "error.h"

typedef struct Test {
    int a;
}Tsdf;

void Parser::declaration(std::vector<Node> &list, bool isGlo)
{
	int sclass = 0;
	Type baseType = decl_spec_opt(&sclass);               // 声明的基本类型
	if (next_is(';'))
		return;

	for (;;) {
		std::string name;
		std::vector<Node> params;

		Type ty = declarator(&baseType, name, params, DECL_BODY);  // 定义
		ty.setStatic(sclass == K_STATIC);

		if(sclass == K_TYPEDEF){                  //typedef 定义
            baseType.type = K_TYPEDEF;
            custom_type_tbl.insert(std::pair<std::string, Type>(name, baseType));
		}
		else if (ty.isStatic() && !isGlo) {       // 局部static变量
            error("Do not support static.");
		}
		else if (params.size() != 0) {
#ifdef _OVERLOAD_
			name = getOverLoadName(name, params);
#endif // _OVERLOAD_

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
                if (!isGlo) {
                    if(var.type.type == K_FLOAT)
                        createQuadruple("=f");
                    else if(var.type.type == K_DOUBLE)
                        createQuadruple("=d");
                    else if (var.type.type == K_STRUCT || var.type.type == K_TYPEDEF) {
                        // 需要全部初始化
                        int _off = 0;
                        for (size_t i = ty.fields.size(); i > 0; --i) {
                            _stk_quad.push_back(var.varName);
                            _stk_quad.push_back(std::to_string(ty.fields.at(i - 1)._off));
                            createQuadruple(".=");
                        }
                    }
                    else
                        createQuadruple("=");
                }
					
			}
			else if (sclass != K_EXTERN && ty.getType() != NODE_FUNC) {
				list.push_back(createDeclNode(var));
			}
		}
        if (next_is(';')) {
            return;
        }
			
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
 * \\ 只支持定长数组,[]和[10]形式
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
	// 指针变量
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

	Token t = lex.peek();
	if (t.getType() == ID) {
		lex.next();
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
	// 只支持定长数组
	if (next_is('[')) {
		decl_array(retty);
        Type r(ARRAY, retty->_all_len, retty->len);
        r.size = retty->size;
        return r;
	}

	// 如果是括号，则为函数
	if (next_is('(')) {
		if (decl_type == DECL_BODY)
			decl_type = NODE_FUNC_DECL;
		return func_param_list(retty, params, decl_type);
	}

	return *retty;
}

void Parser::decl_array(Type *ty)
{
	int _d_len = 0;
	int _all_len = 1;
	do {
		_d_len = array_int_expr();
		expect(']');
		if (_d_len) {
			ty->len.push_back(_d_len);
			_all_len *= _d_len;
		}		
	} while (next_is('['));
	ty->_all_len = _all_len;
}

int Parser::array_int_expr()
{
	Node r = conditional_expr();
	
	if (r.kind == NODE_INT || r.kind == NODE_CHAR || r.kind == NODE_INT) {
		_stk_quad.pop_back();
		return r.int_val;
	}
	return 0;
}



Type Parser::decl_specifiers(int *rsclass)
{
	int current_class = 0;
	Token t;
	Type type;

	enum { k_zero = 0, k_void = 0x01, k_bool, k_char, k_short, k_int, k_long, k_float, k_double } kind = k_zero;
    bool is_unsig = false;
    Type custom_type;

	for (;;) {
		t = lex.next();
		if (t.getType() == K_EOF)
			error("error");

		if (kind == 0 && t.getType()== ID && !custom_type.type) {
            custom_type = getCustomType(t.getSval());
            lex.next();
            if (custom_type.type == 0)
                error("Undefined type: ", t.getSval());
		}

		if (t.getType()!= KEYWORD) {
			break;
		}
		switch (t.getId())
		{
			// Type specifiers
#define type_spec_cheak(cheak, val, _t) do{if(cheak) error("error %s specifier.", _t); else cheak = val;}while(0)
			// 只能出现一次
		case K_VOID:     type_spec_cheak(kind, k_void, "void");break;
		case K_CHAR:     type_spec_cheak(kind, k_char, "char");break;
        case K_SHORT:    type_spec_cheak(kind, k_short, "short"); break;
		case K_INT:      type_spec_cheak(kind, k_int, "int");break;
        case K_LONG:     type_spec_cheak(kind, k_long, "long");break;
		case K_FLOAT:    type_spec_cheak(kind, k_float, "float");break;
		case K_DOUBLE:   type_spec_cheak(kind, k_double, "double");break;
		case K_BOOL:     type_spec_cheak(kind, k_bool, "bool"); break;
		

		case K_SIGNED:   type_spec_cheak(is_unsig, false, "signed");break;
		case K_UNSIGNED: type_spec_cheak(is_unsig, true, "unsigned"); break;

		case K_TYPEDEF:  type_spec_cheak(current_class, K_TYPEDEF, "typedef");break;
		case K_EXTERN:   type_spec_cheak(current_class, K_EXTERN, "extern");break;
		case K_STATIC:   type_spec_cheak(current_class, K_STATIC, "static");break;
		case K_AUTO:     type_spec_cheak(current_class, K_AUTO, "auto");break;
		case K_REGISTER: type_spec_cheak(current_class, K_REGISTER, "register");break;


		case K_COMPLEX: // do something ..
		case K_IMAGINARY: // do something ..


		case K_ENUM: _log_("no enum."); break;
        case K_STRUCT: if (custom_type.type != 0) error("error struct specifier."); custom_type = struct_def(); break;
		case K_UNION: _log_("no union."); break;

#undef type_spec_cheak
			// Storage-class specifiers
			// 只能出现在声明的首位，且只能一次
			// Type qualifiers
		case K_CONST: _log_("no const."); break;
		case K_RESTRICT: _log_("no restrict."); break;
		case K_VOLATILE: _log_("no volatile."); break;

			// Function specifiers
		case K_INLINE: _log_("no inline."); break;
		case ELLIPSIS: 
			type.create(ELLIPSIS, 0, false);
			break;

		default: 
			goto _end;
			break;
		}
	}
_end:
    lex.back();

    

	if (current_class)
		*rsclass = current_class;

    if (custom_type.type != 0)
        return custom_type;

	if (kind) {
		switch (kind)
		{
		case k_void: type.create(K_VOID, 0, false); break;
		case k_bool: type.create(K_BOOL, 1, false); break;
		case k_char: type.create(K_CHAR, 1, is_unsig); break;
        case k_short: type.create(K_SHORT, 2, is_unsig);break;
		case k_int: type.create(K_INT, 4, is_unsig); break;
        case k_long: type.create(K_LONG, 4, is_unsig); break;
		case k_float: type.create(K_FLOAT, 4, false); break;
		case k_double: type.create(K_DOUBLE, 8, false); break;
		default: break;
		}
	}
	return type;
}


std::vector<Node> Parser::initializer(Type &ty)
{
	std::vector<Node> list;
	return list;
}

Type Parser::struct_def()
{
    Type r;
    std::string _new_type_name;
    r.type = K_STRUCT;

    Token t = lex.peek();
    if (t.getType() != ID) {
        error("error define struct");
    }
    t = lex.next();
    _new_type_name = t.getSval();

    Type *_ft;
    std::string _fn;
    int _fo = 0;
    
    if (!next_is('{')) {
        lex.back();
        return Type();
    }
        

    int sclass = 0;
    while (lex.peek().getId() != '}' && lex.peek().getType() != K_EOF) {
        _ft = new Type(decl_spec_opt(&sclass));

        if (lex.peek().getType() != ID)
            error("expect a identier.");
        _fn = lex.next().getSval();
        expect(';');

        r.fields.push_back(Field(_fn, _ft, _fo));
        _fo += _ft->size;
    }
    r.size = _fo;
    expect('}');

    custom_type_tbl.insert(std::pair<std::string, Type>(_new_type_name, r));

    return r;
}