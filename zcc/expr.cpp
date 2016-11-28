#include "parser.h"
#include <cassert>
#include "error.h"

Node Parser::expr()
{
	return comma_expr();
}
Node Parser::expr_opt()
{
	return comma_expr();
}

/**
 * expression = assignment_expr {',' assignment_expr}
 */
Node Parser::comma_expr()
{
	Node node = assignment_expr();
	while (next_is(','))
	{
		Node expr = assignment_expr();
		node = createBinOpNode(expr.getType(), ',', &node, &expr);
	}
	return node;
}
/**
 * assignment_expr = conditional_expr
 *                 | unary_expr assignment_operator assignment_expr
 * assignment_operator = '=' | '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '&=' | 'ˆ=' | '|='
 */
Node Parser::assignment_expr()
{
	Node *node = new Node(logical_or_expr());
	Token t = lex.next();
	
	if (is_keyword(t, '?')) {
		// do..
	}

	int cop = get_compound_assign_op(t); // * + * ..

	// 如果是赋值运算符
	if (is_keyword(t, '=') || cop) {
		// 右侧值
		Node value = assignment_expr();

		if (is_keyword(t, '=') || cop) {
			ensure_lvalue(*node);
		}
		Node *right = new Node(cop ? binop(cop, conv(*node), value) : value);

		// 数学运算中的隐式类型转换
		//if(is_arithtype(node->getType() && node->getType().getType() != right->getType().getType()))
		//	right = ast_conv(node->ty, right);

		return createBinOpNode(node->getType(), '=', node, right);
	}
	lex.back();
	return *node;
}

/**
 * conditional_expr = logical_OR_expr
 *                  | logical_OR_expr '?' expression ':' conditional_expr
 */
Node Parser::conditional_expr(Node *node)
{
	Node r;
	return r;
}

/**
 * logical_OR_expr = logical_AND_expr {'||' logical_AND_expr}
 */
Node Parser::logical_or_expr()
{
	Node *node = new Node(logical_and_expr());
	while (next_is(OP_LOGOR))
		*node = createBinOpNode(Type(K_INT, 4, false), OP_LOGOR, node, new Node(logical_and_expr()));
	return *node;
}

Node Parser::logical_and_expr()
{
	Node *node = new Node(bit_or_expr());
	while (next_is(OP_LOGAND))
		*node = createBinOpNode(Type(K_INT, 4, false), OP_LOGAND, node, new Node(bit_or_expr()));
	return *node;
}
Node Parser::bit_or_expr()
{
	Node *node = new Node(bit_xor_expr());
	while (next_is('|'))
		*node = binop('|', conv(*node), conv(bit_xor_expr()));
	return *node;
}
Node Parser::bit_xor_expr()
{
	Node *node = new Node(bit_and_expr());
	while (next_is('^'))
		*node = binop('^', conv(*node), conv(bit_and_expr()));
	return *node;
}
Node Parser::bit_and_expr()
{
	Node *node = new Node(equal_expr());
	while (next_is('&'))
		*node = binop('&', conv(*node), conv(equal_expr()));
	return *node;
}
Node Parser::equal_expr()
{
	Node *node = new Node(relational_expr());
	Node r;

	if (next_is(OP_EQ))
		r = binop(OP_EQ, conv(*node), conv(equal_expr()));
	else if (next_is(OP_NE)) {
		r = binop(OP_NE, conv(*node), conv(equal_expr()));
	}
	else {
		return *node;
	}
	r.setType(Type(K_INT, 4, false));
	return r;
}
Node Parser::equal_expr_tail()
{
	Node r;
	return r;
}
Node Parser::relational_expr()
{
	Node *node = new Node(shift_expr());
	for (;;) {
		if (next_is('<'))   *node = binop('<', conv(*node), conv(shift_expr()));
		else if (next_is('>'))   *node = binop('<', conv(shift_expr()), conv(*node));
		else if (next_is(OP_LE)) *node = binop(OP_LE, conv(*node), conv(shift_expr()));
		else if (next_is(OP_GE)) *node = binop(OP_LE, conv(shift_expr()), conv(*node));
		else    return *node;
		node->setType(Type(K_INT, 4, false));
	}
}

Node Parser::shift_expr()
{
	Node *node = new Node(add_expr());
	for (;;) {
		int op;
		if (next_is(OP_SAL))
			op = OP_SAL;
		else if (next_is(OP_SAR))
			op = node->getType().isSigned() ? OP_SHR : OP_SAR;
		else
			break;
		Node *right = new Node(add_expr());
		ensure_inttype(*node);
		ensure_inttype(*right);
		*node = createBinOpNode(node->getType(), op, new Node(conv(*node)), new Node(conv(*right)));
	}
	return *node;
}

Node Parser::add_expr()
{
	Node *node = new Node(multi_expr());
	for (;;) {
		if (next_is('+')) *node = binop('+', conv(*node), conv(multi_expr()));
		else if (next_is('-')) *node = binop('-', conv(*node), conv(multi_expr()));
		else    return *node;
	}
}

Node Parser::multi_expr()
{
	Node *node = new Node(cast_expr());
	for (;;) {
		if (next_is('*'))      *node = binop('*', conv(*node), conv(cast_expr()));
		else if (next_is('/')) *node = binop('/', conv(*node), conv(cast_expr()));
		else if (next_is('%')) *node = binop('%', conv(*node), conv(cast_expr()));
		else    return *node;
	}
}

/**
 * cast_expr = unary_expr
 *           | '(' type ')' cast_expr
 */
Node Parser::cast_expr()
{
	Token tok = lex.next();
	if (is_keyword(tok, '(') && is_type(lex.peek())) {
		/*Type ty = cast_type();
		expect(')');
		return createUnaryNode(CAST, ty, cast_expr());*/
	}
	lex.back();
	return unary_expr();
}
Node Parser::unary_expr()
{
	Token tok = lex.next();
	if (tok.getType() == KEYWORD) {
		switch (tok.getId()) {
		case K_SIZEOF: return sizeof_operand();
		case OP_INC: return unary_incdec(OP_PRE_INC);
		case OP_DEC: return unary_incdec(OP_PRE_DEC);
		case OP_LOGAND: return label_addr(tok);
		case '&': return unary_addr();
		case '*': return unary_deref(tok);
		case '+': return cast_expr();
		case '-': return unary_minus();
		case '~': return unary_bitnot(tok);
		case '!': return unary_lognot();
		}
	}
	lex.back();
	return postfix_expr();
}


Node Parser::postfix_expr()
{
	Node *node = new Node(primary_expr());
	return postfix_expr_tail(*node);
}

Node Parser::postfix_expr_tail(Node &node)
{
	for (;;) {
		if (next_is('(')) {
			//Token tok = lex.peek();
			//node = conv(node);
			//Type t = node.getType();
			//if (t.getType() != '*' || t.ptr->kind != FUNC)
			//	errort(tok, "function expected, but got %s", node2s(node));
			//node = read_funcall(node);
			//continue;
		}
		if (next_is('[')) {
			//node = read_subscript_expr(node);
			//continue;
		}
		if (next_is('.')) {
			/*node = read_struct_field(node);
			continue;*/
		}
		if (next_is(OP_ARROW)) {
			/*if (node->ty->kind != KIND_PTR)
				error("pointer type expected, but got %s %s",
					ty2s(node->ty), node2s(node));
			node = ast_uop(AST_DEREF, node->ty->ptr, node);
			node = read_struct_field(node);
			continue;*/
		}
		Token tok = lex.peek();
		if (next_is(OP_INC) || next_is(OP_DEC)) {
			ensure_lvalue(node);
			int op = is_keyword(tok, OP_INC) ? OP_POST_INC : OP_POST_DEC;
			return createUnaryNode(op, node.getType(), node);
		}
		return node;
	}
}

Node Parser::var_or_func(Token &t)
{
	Node r = localenv->search(t.getSval());
	if (r.kind == NODE_NULL)
		error("undefined var : %s！", t.getSval());

	return r;
}


Node Parser::primary_expr()
{
	Token tok = lex.next();

	if (is_keyword(tok, '(')) {
		Node r = expr();
		expect(')');
		return r;
	}

	switch (tok.getType()) {

		// 如果是ID， 则可能为变量或函数调用
	case ID:
		return var_or_func(tok);
		return NULL;

		// 常量
	case INTEGER:
		return createIntNode(tok);

	case FLOAT:
		return NULL;

	case CHAR_:
		//return ast_inttype(char_type(tok->enc), tok->c);
		return NULL;

	case STRING_:
		//return ast_string(tok->enc, tok->sval, tok->slen);
		return NULL;

	case KEYWORD:
		lex.back();
		return NULL;
	default:
		error("internal error: unknown token kind");
	}
}

Node Parser::wrap(Type &t, Node &node) {
	if (t.getType() == node.getType().getType() && t.isSigned() == t.isSigned())
		return node;
	return createUnaryNode(CONV, t, node);
}

Node Parser::binop(int op, Node &lhs, Node &rhs)
{
	if (lhs.getType().getType() == '*' && rhs.getType().getType() == '*') {
		if (op == '-')
			return createBinOpNode(Type(K_LONG, 8, false), '-', &lhs, &rhs);
		return createBinOpNode(Type(K_INT, 4, false), '-', &lhs, &rhs);
	}

	if(lhs.getType().getType() == '*')
		return createBinOpNode(lhs.getType(), op, &lhs, &rhs);
	if(rhs.getType().getType() == '*')
		return createBinOpNode(rhs.getType(), op, &lhs, &rhs);

	// 检查
	assert(is_arithtype(lhs.getType()));
	assert(is_arithtype(rhs.getType()));
	Type r = usual_arith_conv(lhs.getType(), rhs.getType());

	return createBinOpNode(r, op, new Node(wrap(r, lhs)), new Node(wrap(r, rhs)));
}

// 常规算术转换规则
Type Parser::usual_arith_conv(Type &t, Type &u)
{
	assert(is_arithtype(t));
	assert(is_arithtype(u));

	if (t.getType() < u.getType()) {
		// Make t the larger type
		Type tmp = t;
		t = u;
		u = tmp;
	}
	if (is_floattype(t))
		return t;
	assert(is_inttype(t) && t.getSize() >= 4);
	assert(is_inttype(u) && u.getSize() >= 4);
	if (t.getSize() > u.getSize())
		return t;
	assert(t.getSize() == u.getSize());
	if (t.isSigned() == u.isSigned())
		return t;
	Type r = t;
	r.setUnsig(true);
	return r;
}

/**
 * 类型转换
 */
Node Parser::conv(Node &node)
{
	Node r;
	return r;
}

void Parser::ensure_inttype(Node &node) 
{
	if (!is_inttype(node.getType()))
		error("integer type expected, but got");
}


Node Parser::sizeof_operand()
{
	Node r;
	return r;
}
Node Parser::unary_incdec(int ty)
{
	Node r;
	return r;
}

Node Parser::label_addr(Token &t)
{
	Node r;
	return r;
}
Node Parser::unary_addr()
{
	Node r;
	return r;
}
Node Parser::unary_deref(Token &t)
{
	Node r;
	return r;
}
Node Parser::unary_minus()
{
	Node r;
	return r;
}
Node Parser::unary_bitnot(Token &t)
{
	Node r;
	return r;
}
Node Parser::unary_lognot()
{
	Node r;
	return r;
}