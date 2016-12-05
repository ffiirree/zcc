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
	createIncDec();
	while (next_is(','))
	{
		Node expr = assignment_expr();
		createIncDec();
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

		//if (is_keyword(t, '=') || cop) {
		//	ensure_lvalue(*node);
		//}
		Node _temp;
		if (cop) {
			_temp = binop(cop, conv(*node), value);
			pushQuadruple((*node).varName);

			createQuadruple(get_compound_assign_op_signal(t));
			createQuadruple("=");
		}
		else {
			_temp = value;

			createQuadruple("=");
		}
		Node *right = new Node(_temp);

		return createBinOpNode(node->getType(), '=', node, right);
	}
	lex.back();
	return *node;
}

/**
 * conditional_expr = logical_OR_expr
 *                  | logical_OR_expr '?' expression ':' conditional_expr
 */
Node Parser::conditional_expr()
{
	Node *node = new Node(logical_or_expr());
	if (next_is('?')) {
		Node *then = new Node(expr());
		expect(':');
		Node *els = new Node(conditional_expr());

		return createIfStmtNode(node, then, els);
	}
	return *node;
}

/**
 * logical_OR_expr = logical_AND_expr {'||' logical_AND_expr}
 */
Node Parser::logical_or_expr()
{
	Node *node = new Node(logical_and_expr());
	while (next_is(OP_LOGOR)) {
		node = new Node(createBinOpNode(Type(K_INT, 4, false), OP_LOGOR, node, new Node(logical_and_expr())));
		boolLabel.push_back(BoolLabel());
		_stk_if_goto_op.push_back("||");
	}
	return *node;
}

Node Parser::logical_and_expr()
{
	Node *node = new Node(bit_or_expr());
	while (next_is(OP_LOGAND)) {
		node = new Node(createBinOpNode(Type(K_INT, 4, false), OP_LOGAND, node, new Node(bit_or_expr())));
		boolLabel.push_back(BoolLabel());
		_stk_if_goto_op.push_back("&&");
	}
	return *node;
}
Node Parser::bit_or_expr()
{
	Node *node = new Node(bit_xor_expr());
	while (next_is('|')) {
		node = new Node(binop('|', conv(*node), conv(bit_xor_expr())));
		createQuadruple("|");
	}
		
	return *node;
}
Node Parser::bit_xor_expr()
{
	Node *node = new Node(bit_and_expr());
	while (next_is('^')) {
		node = new Node(binop('^', conv(*node), conv(bit_and_expr())));
		createQuadruple("^");
	}
		
	return *node;
}
Node Parser::bit_and_expr()
{
	Node *node = new Node(equal_expr());
	while (next_is('&')) {
		node = new Node(binop('&', conv(*node), conv(equal_expr())));
		createQuadruple("&");
	}
		
	return *node;
}
Node Parser::equal_expr()
{
	Node *node = new Node(relational_expr());
	Node r;

	if (next_is(OP_EQ)) {
		r = binop(OP_EQ, conv(*node), conv(equal_expr()));
		createBoolGenQuadruple("==");
	}
	else if (next_is(OP_NE)) {
		r = binop(OP_NE, conv(*node), conv(equal_expr()));
		createBoolGenQuadruple("!=");
	}
	else {
		return *node;
	}
	r.setType(Type(K_INT, 4, false));
	return r;
}

Node Parser::relational_expr()
{
	Node *node = new Node(shift_expr());
	for (;;) {
		if (next_is('<')) {
			node = new Node(binop('<', conv(*node), conv(shift_expr())));
			createBoolGenQuadruple("<");
		}
			
		else if (next_is('>')) {
			node = new Node(binop('>', conv(*node), conv(shift_expr())));
			createBoolGenQuadruple(">");
		}
		else if (next_is(OP_LE)) {
			node = new Node(binop(OP_LE, conv(*node), conv(shift_expr())));
			createBoolGenQuadruple("<=");
		}
			
		else if (next_is(OP_GE)) {
			node = new Node(binop(OP_GE, conv(*node), conv(shift_expr())));
			createBoolGenQuadruple(">=");
		}
		else   
			return *node;
		node->setType(Type(K_INT, 4, false));
	}
}

Node Parser::shift_expr()
{
	Node *node = new Node(add_expr());
	for (;;) {
		int op;
		if (next_is(OP_SAL))
			op = OP_SAL;//<<
		else if (next_is(OP_SAR))
			op = node->getType().isUnsigned() ? OP_SHR : OP_SAR;
		else
			break;
		Node *right = new Node(add_expr());

		node = new Node(createBinOpNode(node->getType(), op, new Node(conv(*node)), new Node(conv(*right))));
		if(op == OP_SAL)
			createQuadruple("<<");
		else if(op == OP_SHR)
			createQuadruple(">>>");
		else if(op == OP_SAR)
			createQuadruple(">>");
	}
	return *node;
}

Node Parser::add_expr()
{
	Node *node = new Node(multi_expr());
	for (;;) {
		if (next_is('+')) {
            Node *right = new Node(multi_expr());
			node = new Node(binop('+', conv(*node), conv(*right)));
            if (node->type.type == K_FLOAT || node->type.type == K_DOUBLE
                || right->type.type == K_FLOAT || right->type.type == K_DOUBLE)
                createQuadruple("+f");
            else
                createQuadruple("+");
		}
		else if (next_is('-')) {
            Node *right = new Node(multi_expr());
			node = new Node(binop('-', conv(*node), conv(*right)));
            if (node->type.type == K_FLOAT || node->type.type == K_DOUBLE
                || right->type.type == K_FLOAT || right->type.type == K_DOUBLE)
                createQuadruple("-f");
            else
                createQuadruple("-");
		}
		else    
			return *node;
	}
}

Node Parser::multi_expr()
{
	Node *node = new Node(cast_expr());
	for (;;) {
		if (next_is('*')) {
            Node *right = new Node(cast_expr());
			node = new Node(binop('*', conv(*node), conv(*right)));
            if (node->type.type == K_FLOAT || node->type.type == K_DOUBLE
                || right->type.type == K_FLOAT || right->type.type == K_DOUBLE)
                createQuadruple("*f");
            else
                createQuadruple("*");
		}
		else if (next_is('/')) {
            Node *right = new Node(cast_expr());
			node = new Node(binop('/', conv(*node), conv(*right)));
			
            if (node->type.type == K_FLOAT || node->type.type == K_DOUBLE
                || right->type.type == K_FLOAT || right->type.type == K_DOUBLE)
                createQuadruple("/f");
            else
                createQuadruple("/");
		}
		else if (next_is('%')) {
			node = new Node(binop('%', conv(*node), conv(cast_expr())));
			createQuadruple("%");
		}
		else    
			return *node;
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
}
lex.back();
return unary_expr();
}

Node Parser::unary_expr()
{
    Token tok = lex.next();
    if (tok.getType() == KEYWORD) {
        Node r;
        switch (tok.getId()) {
        case K_SIZEOF:
            createUnaryQuadruple("sizeof");
            return sizeof_operand();
        case OP_INC:
            r = unary_incdec(OP_PRE_INC);
            createUnaryQuadruple("++");
            return r;
        case OP_DEC:
            r = unary_incdec(OP_PRE_DEC);
            createUnaryQuadruple("--");
            return r;

        case '&':
            r = unary_addr();
            createUnaryQuadruple("&U");
            return r;

        case '*':
            r = unary_deref(tok);
            createUnaryQuadruple("*U");
            return r;

            // 不完全
        case '+':
            r = cast_expr();
            createUnaryQuadruple("+U");
            return r;

            // 不完全
        case '-':
            r = unary_minus();
            createUnaryQuadruple("-U");
            return r;

        case '~':
            r = unary_bitnot(tok);
            createUnaryQuadruple("~");
            return r;

        case '!':
            r = unary_lognot();
			//boolLabel.push_back(BoolLabel());
			//_stk_if_goto_op.push_back("!");
            return r;
        }
    }
    lex.back();
    return postfix_expr();
}

Node Parser::unary_minus()
{
    Node *expr = new Node(cast_expr());

    if (is_inttype(expr->type))
        return binop('-', conv(createIntNode(expr->type, 0)), conv(*expr));

    return binop('-', createFloatNode(expr->type, 0.0), *expr);
}

Node Parser::postfix_expr()
{
    Node *node = new Node(primary_expr());
    return postfix_expr_tail(*node);
}


std::vector<Node> Parser::argument_expr_list()
{
    std::vector<Node> list;
    list.push_back(assignment_expr());
    while (next_is(','))
        list.push_back(assignment_expr());
    return list;
}

Node Parser::postfix_expr_tail(Node &node)
{
    for (;;) {
        if (next_is('(')) {

            std::vector<Node> parms = argument_expr_list();
            expect(')');
            createFuncQuad(parms);
        }
        if (next_is('[')) {
			expr();
			expect(']');
			if (lex.peek().getId() == '=')
				createQuadruple(".&");
			else
				createQuadruple(".");
        }
        if (next_is('.')) {
            Token t = lex.next();
            int _off = 0;
            for (size_t i = 0; i < node.type.fields.size(); ++i){
                if (t.getSval() == node.type.fields.at(i)._name)
                    _off = node.type.fields.at(i)._off;
            }
            _stk_quad.push_back(std::to_string(_off));
            
            if(lex.peek().getId() == '=')
                createQuadruple(".&");
            else 
                createQuadruple(".");
		}
		if (next_is(OP_ARROW)) {
            error("Unspport '->'");
		}
		Token tok = lex.peek();
		// 后置++/--
		if (next_is(OP_INC) || next_is(OP_DEC)) {
			ensure_lvalue(node);
			int op = is_keyword(tok, OP_INC) ? OP_POST_INC : OP_POST_DEC;

			pushIncDec(_stk_quad.back());
			if(op == OP_POST_DEC)
				pushIncDec("--");
			else 
				pushIncDec("++");

			return createUnaryNode(op, node.type, node);
		}
		return node;
	}
}

Node Parser::primary_expr()
{
	Token tok = lex.next();
	std::string strl;
    std::string Lfloat;

	if (is_keyword(tok, '(')) {
		Node r = expr();
		expect(')');
		return r;
	}

	switch (tok.getType()) {
		
		// 如果是ID， 则可能为变量或函数调用
	case ID:
		return var_or_func(tok);

		// 常量
	case INTEGER:
		pushQuadruple(tok.getSval());
		return createIntNode(tok, 4, false);

	case FLOAT:
        Lfloat = newLabel("f");
        float_const.push_back(tok.getSval());
        float_const.push_back(Lfloat);
        float_const.push_back("4");
		pushQuadruple(Lfloat);
		return createFloatNode(tok);

	case CHAR_:
		pushQuadruple(std::to_string(tok.getCh()));
		return createIntNode(tok, 1, true);

	case STRING_:
		strl = newLabel("LSTR");
		pushQuadruple(strl);
		const_string.push_back(StrCard(tok.getSval(), strl));
		return createStrNode(tok);

	case KEYWORD:
		lex.back();
		return NULL;

	default:
		error("internal error: unknown token kind");
	}
}

Node Parser::var_or_func(Token &t)
{
	Node r = localenv->search(t.getSval());

	if (r.kind == NODE_GLO_VAR || r.kind == NODE_LOC_VAR)
		pushQuadruple(r.varName);
#ifdef _OVERLOAD_
	else 
		pushQuadruple(t.getSval());
#elif
	else if (r.kind == NODE_FUNC || r.kind == NODE_FUNC_DECL)
		pushQuadruple(t.getSval());
	if (r.kind == NODE_NULL)
		error("undefined var : %s！", t.getSval().c_str());
#endif // _OVERLOAD_

	return r;
}


Node Parser::wrap(Type &t, Node &node) {
	if (t.getType() == node.type.getType() && t.isUnsigned() == t.isUnsigned())
		return node;
	return createUnaryNode(CONV, node.type, node);
}

Node Parser::binop(int op, Node &lhs, Node &rhs)
{
	if (lhs.type.getType() == '*' && rhs.type.getType() == '*') {
		if (op == '-')
			return createBinOpNode(Type(K_LONG, 8, false), '-', &lhs, &rhs);
		return createBinOpNode(Type(K_INT, 4, false), '-', &lhs, &rhs);
	}

	if(lhs.type.getType() == '*')
		return createBinOpNode(lhs.type, op, &lhs, &rhs);
	if(rhs.type.getType() == '*')
		return createBinOpNode(rhs.type, op, &lhs, &rhs);

	Type r = usual_arith_conv(lhs.type, rhs.type);

	return createBinOpNode(r, op, new Node(wrap(r, lhs)), new Node(wrap(r, rhs)));
}

// 常规算术转换规则
Type Parser::usual_arith_conv(Type &t, Type &u)
{
	if (t.getType() < u.getType()) {
		Type tmp = t;
		t = u;
		u = tmp;
	}
	if (is_floattype(t))
		return t;
	if (t.getSize() > u.getSize())
		return t;

	if (t.isUnsigned() == u.isUnsigned())
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
	return node;
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
	Node *node = new Node(unary_expr());
	
	return createUnaryNode(ty, node->type, *node);
}

Node Parser::unary_addr()
{
	Node *operand = new Node(cast_expr());

	return createUnaryNode(NODE_ADDR, conv2ptr(operand->type), *operand);
}


Node Parser::unary_deref(Token &t)
{
	Node *operand = new Node(conv(cast_expr()));
	if (operand->type.getType() != PTR)
		error("pointer type expected, but got");
	if (operand->type.getType() == NODE_FUNC)
		return *operand;
	return createUnaryNode(NODE_DEREF, *(operand->type.ptr), *operand);
}





Node Parser::unary_bitnot(Token &t)
{
	Node *expr = new Node(cast_expr());
	*expr = conv(*expr);

	if (!is_inttype(expr->type))
		error("invalid use of ~");
	return createUnaryNode('~', expr->type, *expr);
}
Node Parser::unary_lognot()
{
	Node *operand = new Node(cast_expr());
	*operand = conv(*operand);
	return createUnaryNode('!', Type(K_INT, 4, false), *operand);
}