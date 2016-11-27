#include "parser.h"
#include "error.h"

void Parser::declaration(Env &env, bool isGlo)
{

}


Type Parser::declarator(Type &ty, std::string &name, std::vector<Node> params, int deal_type)
{
	if (next_is('(')) {

	}

	if (next_is('*')) {

	}

	Token t = lex.next();
	if (t.getType() == ID) {
		name = t.getSval();
		return direct_decl_tail(ty, params);
	}
	return direct_decl_tail(ty, params);
}


Type Parser::decl_spec_opt(int *sclass)
{
	if (is_type(lex.peek().getId()))
		return decl_specifiers(sclass);

	error("missing type, defalut 'int'.");
	return Type(K_INT, 4, false);
}

Type Parser::direct_decl_tail(Type retty, std::vector<Node> params)
{
	if (next_is('['))
		_log_("no array.");
	if (next_is('('))
		return func_param_list(retty, params);
	return retty;
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
			// 只能出现一次
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
			// 只能出现在声明的首位，且只能一次
			// Type qualifiers
		case K_CONST: _log_("no const."); break;
		case K_RESTRICT: _log_("no restrict."); break;
		case K_VOLATILE: _log_("no volatile."); break;

			// Function specifiers
		case K_INLINE: _log_("no inline."); break;

		default: break;
		}
	}

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

	return type;
}
