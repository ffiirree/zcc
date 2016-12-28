#include "parser.h"
#include "error.h"

void Parser::declaration(std::vector<Node> &list, bool isGlo)
{
    int sclass = 0;
    Type baseType = decl_spec_opt(&sclass);
    if (next_is(';'))
        return;

    for (;;) {
        std::string name;
        std::vector<Node> params;

        Type ty = declarator(&baseType, name, params, DECL_BODY);
        ty.setStatic(sclass == K_STATIC);

        if (sclass == K_TYPEDEF) {
            baseType.type = K_TYPEDEF;
            custom_type_tbl.insert(std::pair<std::string, Type>(name, baseType));
        }
        else if (ty.isStatic() && !isGlo) {
            errorp(ts_.getPos(), "Do not support static.");
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

#if defined(WIN32)
                pushQuadruple("_" + name);
#elif defined(__linux__)
                pushQuadruple(name);
#endif
            }

            if (next_is('=')) {
                list.push_back(createDeclNode(var, decl_init(ty)));

                if (var.type.type == K_FLOAT || var.type.type == K_DOUBLE) {
                    std::string _i2f = quad_arg_stk_.back();
                    if (isNumber(_i2f)) {
                        quad_arg_stk_.pop_back();
                        float_const.push_back(_i2f);
                        _i2f = newLabel("f");
                        float_const.push_back(_i2f);
                        float_const.push_back("4f");
                        quad_arg_stk_.push_back(_i2f);
                    }
                }


                if (!isGlo) {
                    if (var.type.type == K_FLOAT)
                        createQuadruple("=f");
                    else if (var.type.type == K_DOUBLE)
                        createQuadruple("=d");
                    else if (var.type.type == ARRAY) {
                        int _off = 0;

                        std::vector<std::string> arr_init;
                        for (int i = 0; i < var.type._all_len; ++i) {
                            std::string init_val = quad_arg_stk_.back();

                            if (init_val != var.name()) {
                                arr_init.push_back(init_val);
                                quad_arg_stk_.pop_back();
                            }
                            else {
                                quad_arg_stk_.pop_back();
                                break;
                            }
                        }
                        if (!arr_init.empty())
                            for (size_t i = 0;i < arr_init.size(); ++i) {
                                quad_arg_stk_.push_back(arr_init.at(arr_init.size() - i - 1));
                                quad_arg_stk_.push_back(var.name());
                                quad_arg_stk_.push_back(std::to_string(i * var.type.size_));
                                createQuadruple("[]=");
                            }
                    }
                    else if (var.type.type == K_STRUCT || var.type.type == K_TYPEDEF) {
                        int _off = 0;
                        for (size_t i = ty.fields.size(); i > 0; --i) {
                            quad_arg_stk_.push_back(var.name());
                            quad_arg_stk_.push_back(std::to_string(ty.fields.at(i - 1)._off));
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
            errorp(ts_.getPos(), "';' or ',' are expected, but got not");
    }
}

//initializer
std::vector<Node> Parser::decl_init(Type &ty)
{
    std::vector<Node> list;
    if (is_keyword(ts_.peek(), '{') || ty.getType() == T_STRING) {
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
        if (is_keyword(ts_.peek(), '[') || is_keyword(ts_.peek(), '.')) {
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
        Node r = com_conditional_expr();
        expect(']');
        return r;
    }
    else if (next_is('.')) {
        Node r;
        Token t = ts_.next();
        if (t.getId() == T_IDENTIFIER)
            r = localenv->search(t.getSval());
        if (r.getKind() == 0)
            errorp(ts_.getPos(), "init list error");
        return r;
    }
    return Node();                       // for warning 
}


/**
* \\ int * var, var2;
* \\ int *(*pt)(int a, int b)
* declarator = ['*'] ID direct_declarator_tail
*            | ['*'] '(' declarator ')' direct_declarator_tail
*
* \\
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
    r.size_ = 4;

    return r;
}

Type Parser::declarator(Type *ty, std::string &name, std::vector<Node> &params, int deal_type)
{
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

    Token t = ts_.peek();
    if (t.getType() == T_IDENTIFIER) {
        ts_.next();
        name = t.getSval();
        return direct_decl_tail(ty, params, deal_type);
    }
    return direct_decl_tail(ty, params, deal_type);
}


Type Parser::decl_spec_opt(int *sclass)
{
    if (is_type(ts_.peek()))
        return decl_specifiers(sclass);

    warnp(ts_.getPos(), "missing type, defalut 'int'.");
    return Type(K_INT, 4, false);
}

Type Parser::direct_decl_tail(Type *retty, std::vector<Node> &params, int decl_type)
{
    if (next_is('[')) {
        decl_array(retty);
        Type r(ARRAY, retty->_all_len, retty->len);
        r.size_ = retty->size_;
        return r;
    }

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
    Node r = com_conditional_expr();

    if (r.kind == NODE_INT || r.kind == NODE_CHAR || r.kind == NODE_INT) {
        quad_arg_stk_.pop_back();
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
        t = ts_.next();
        if (t.getType() == T_EOF)
            errorp(ts_.getPos(), "end of file.");

        if (kind == 0 && t.getType() == T_IDENTIFIER && !custom_type.type) {
            custom_type = getCustomType(t.getSval());
            ts_.next();
            if (custom_type.type == 0)
                errorp(ts_.getPos(), "Undefined type: " + t.getSval());
        }

        if (t.getType() != T_KEYWORD) {
            break;
        }
        switch (t.getId())
        {
            // Type specifiers
#define type_spec_cheak(cheak, val, _t) do{if(cheak) errorp(ts_.getPos(), "error " + std::string(_t) +" specifier"); else cheak = val;}while(0)
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


        case K_ENUM: custom_type = enum_def(); break;
        case K_STRUCT: if (custom_type.type != 0) errorp(ts_.getPos(), "error struct specifier."); custom_type = struct_def(); break;
        case K_UNION: errorp(ts_.getPos(), "no union."); break;
#undef type_spec_cheak

        case K_CONST: errorp(ts_.getPos(), "no const."); break;
        case K_RESTRICT: errorp(ts_.getPos(), "no restrict."); break;
        case K_VOLATILE: errorp(ts_.getPos(), "no volatile."); break;

            // Function specifiers
        case K_INLINE: errorp(ts_.getPos(), "no inline."); break;
        case ELLIPSIS:
            type.create(ELLIPSIS, 0, false);
            break;

        default:
            goto _end;
            break;
        }
    }
_end:
    ts_.back();

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

Type Parser::enum_def()
{
    if (ts_.peek().getType() == T_IDENTIFIER)
        ts_.next();

    expect('{');

    Token t;
    int _val = 0;
    std::string _v;
    std::string _n;
    do {
        t = ts_.next();
        if (t.getType() == T_IDENTIFIER) {
            _n = t.getSval();
            Node n = globalenv->search(_n);
            if (!n.name().empty())
                errorp(ts_.getPos(), "redefined var : " + _n);
            if (!searchEnum(_n).empty())
                errorp(ts_.getPos(), "redefined var :" + _n);

            if (next_is('=')) {
                t = ts_.next();

                if (t.getType() == T_INTEGER) {
                    if (atoi(t.getSval().c_str()) < _val)
                        errorp(ts_.getPos(), "Inc val.");
                    _val = atoi(t.getSval().c_str()) + 1;
                    _v = t.getSval();
                }
                else {
                    errorp(ts_.getPos(), "need interger.");
                }

            }
            else {
                _v = std::to_string(_val);
                _val++;
            }
        }
        else
            errorp(ts_.getPos(), "Ench member need a name.");

        enum_const.insert(std::pair<std::string, std::string>(_n, _v));
    } while (next_is(',') && ts_.peek().getId() != '}');
    expect('}');

    return Type(K_ENUM);
}

std::string Parser::searchEnum(const std::string &key)
{
    if (enum_const.empty())
        return std::string();
    std::map<std::string, std::string>::iterator iter = enum_const.find(key);
    if (iter != enum_const.end())
        return iter->second;
    return std::string();
}

Type Parser::struct_def()
{
    Type r;
    std::string _new_type_name;
    r.type = K_STRUCT;

    Token t = ts_.peek();
    if (t.getType() != T_IDENTIFIER) {
        errorp(ts_.getPos(), "Struct need a type name.");
    }
    t = ts_.next();
    _new_type_name = t.getSval();

    Type *_ft;
    std::string _fn;
    int _fo = 0;

    if (!next_is('{')) {
        ts_.back();
        return Type();
    }


    int sclass = 0;
    while (ts_.peek().getId() != '}' && ts_.peek().getType() != T_EOF) {
        _ft = new Type(decl_spec_opt(&sclass));

        if (ts_.peek().getType() != T_IDENTIFIER)
            errorp(ts_.getPos(), "expect a identier.");
        _fn = ts_.next().getSval();
        expect(';');

        r.fields.push_back(Field(_fn, _ft, _fo));
        _fo += _ft->size_;
    }
    r.size_ = _fo;
    expect('}');

    custom_type_tbl.insert(std::pair<std::string, Type>(_new_type_name, r));
    return r;
}