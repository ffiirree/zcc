#include "parser.h"
#include "error.h"

Node Parser::expr()
{
	return comma_expr();
}
Node Parser::expr_opt()
{
	return comma_expr();
}
Node Parser::bool_expr()
{
    Node r = comma_expr();
    if (isCondition && _stk_if_goto.empty()) {
        _stk_quad.push_back("0");
        createBoolGenQuadruple("!=");

        BoolLabel b_;
        b_._leaf = true;
        boolLabel.push_back(b_);
    }
    return r;
}

bool Parser::compute_bool_expr()
{
    bool r = false;

    isComputeBool = true;
    bool_expr();

    if (_stk_quad.size() > 1)
        error("bool_expr not res.");
    if (_stk_quad.size() == 1) {
        if (atoi(_stk_quad.back().c_str()) != 0)
            r = true;
    }


    isComputeBool = false;

    return r;
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
	Token t = ts_.next();
	
	if (is_keyword(t, '?')) {
        conditional_expr(node);
        ts_.next();
	}

	int cop = get_compound_assign_op(t); // * + * ..

	if (is_keyword(t, '=') || cop) {
		Node value = assignment_expr();
		Node _temp;
		if (cop) {
			_temp = binop(cop, *node, value);
			pushQuadruple((*node).name());
			std::string _1, _2;
			_1 = _stk_quad.back(); _stk_quad.pop_back();
			_2 = _stk_quad.back(); _stk_quad.pop_back();
			_stk_quad.push_back(_1);
			_stk_quad.push_back(_2);

			if (_temp.type.type == K_DOUBLE || _temp.type.type == K_FLOAT) {
				createQuadruple(get_compound_assign_op_signal(t)+ "f");
				createQuadruple("=f");
			}
			else {
				createQuadruple(get_compound_assign_op_signal(t));
				createQuadruple("=");
			}
		}
		else {
			_temp = value;
			if (_temp.type.type == K_DOUBLE || _temp.type.type == K_FLOAT)
				createQuadruple("=f");
			else
				createQuadruple("=");
		}
		Node *right = new Node(_temp);

		return createBinOpNode(node->getType(), '=', node, right);
	}
    ts_.back();
	return *node;
}

/**
 * conditional_expr = logical_OR_expr
 *                  | logical_OR_expr '?' expression ':' conditional_expr
 */
Node Parser::conditional_expr(Node *node)
{
    BoolLabel _to;
    std::string snext = newLabel("sn");

    _to._true = newLabel("tot");
    _to._false = newLabel("tof");
    boolLabel.back()._true = _to._true;
    boolLabel.back()._false = _to._false;
    generateIfGoto();
    out << _to._true << ":" << std::endl;

    Node *then = new Node(expr());
    out << "goto\t" << snext << std::endl;
    out << _to._false << ":" << std::endl;

    expect(':');
    Node *els = new Node(com_conditional_expr());

    out << snext << ":" << std::endl;

    return createIfStmtNode(node, then, els);
}

Node Parser::com_conditional_expr()
{
    BoolLabel _to;
    std::string snext = newLabel("sn");

	Node *node = new Node(logical_or_expr());
	if (next_is('?')) {

        _to._true = newLabel("tot");
        _to._false = newLabel("tof");
        boolLabel.back()._true = _to._true;
        boolLabel.back()._false = _to._false;
        generateIfGoto();
        out << _to._true << ":" << std::endl;

		Node *then = new Node(expr());
        out << "goto\t" << snext << std::endl;
        out << _to._false << ":" << std::endl;

		expect(':');
		Node *els = new Node(com_conditional_expr());

        out << snext << ":" << std::endl;

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
        if (isComputeBool) {
            if (_stk_quad.size() < 2)
                error("|| operand < 2.");

            std::string v1 = _stk_quad.back(); _stk_quad.pop_back();
            std::string v2 = _stk_quad.back(); _stk_quad.pop_back();
            int op1 = atoi(v1.c_str());
            int op2 = atoi(v2.c_str());
            if (op1 || op2)
                _stk_quad.push_back("1");
            else
                _stk_quad.push_back("0");
        }
        else 
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
        if (isComputeBool) {
            if (_stk_quad.size() < 2)
                error("|| operand < 2.");

            std::string v1 = _stk_quad.back(); _stk_quad.pop_back();
            std::string v2 = _stk_quad.back(); _stk_quad.pop_back();
            int op1 = atoi(v1.c_str());
            int op2 = atoi(v2.c_str());
            if (op1 && op2)
                _stk_quad.push_back("1");
            else
                _stk_quad.push_back("0");
        }
        else
		    _stk_if_goto_op.push_back("&&");
	}
	return *node;
}
Node Parser::bit_or_expr()
{
	Node *node = new Node(bit_xor_expr());
	while (next_is('|')) {
		node = new Node(binop('|', *node, *(new Node(bit_xor_expr()))));
		createQuadruple("|");
	}
		
	return *node;
}
Node Parser::bit_xor_expr()
{
	Node *node = new Node(bit_and_expr());
	while (next_is('^')) {
		node = new Node(binop('^', *node, *(new Node(bit_and_expr()))));
		createQuadruple("^");
	}
		
	return *node;
}
Node Parser::bit_and_expr()
{
	Node *node = new Node(equal_expr());
	while (next_is('&')) {
		node = new Node(binop('&', *node, *(new Node(equal_expr()))));
		createQuadruple("&");
	}
		
	return *node;
}
Node Parser::equal_expr()
{
	Node *node = new Node(relational_expr());
	Node r;

	if (next_is(OP_EQ)) {
		r = binop(OP_EQ, *node, *(new Node(equal_expr())));
		createBoolGenQuadruple("==");
	}
	else if (next_is(OP_NE)) {
		r = binop(OP_NE, *node, *(new Node(equal_expr())));
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
			node = new Node(binop('<', *node, *(new Node(shift_expr()))));
			createBoolGenQuadruple("<");
		}
			
		else if (next_is('>')) {
			node = new Node(binop('>', *node, *(new Node(shift_expr()))));
			createBoolGenQuadruple(">");
		}
		else if (next_is(OP_LE)) {
			node = new Node(binop(OP_LE, *node, *(new Node(shift_expr()))));
			createBoolGenQuadruple("<=");
		}
			
		else if (next_is(OP_GE)) {
			node = new Node(binop(OP_GE, *node, *(new Node(shift_expr()))));
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

        if (cheak_is_int_type(*node)) errorp(ts_.getPos(), "shift operator need a interger object.");
        if (cheak_is_int_type(*right)) errorp(ts_.getPos(), "shift operator need a interger object.");

		node = new Node(createBinOpNode(node->getType(), op, new Node(*node), new Node(*right)));
        if (op == OP_SAL) 
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
			node = new Node(binop('+', *node, *right));
            if (node->type.type == K_FLOAT || node->type.type == K_DOUBLE
                || right->type.type == K_FLOAT || right->type.type == K_DOUBLE)
                createQuadruple("+f");
            else
                createQuadruple("+");
		}
		else if (next_is('-')) {
            Node *right = new Node(multi_expr());
			node = new Node(binop('-', *node, *right));
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
			node = new Node(binop('*', *node, *right));
            if (node->type.type == K_FLOAT || node->type.type == K_DOUBLE
                || right->type.type == K_FLOAT || right->type.type == K_DOUBLE)
                createQuadruple("*f");
            else
                createQuadruple("*");
		}
		else if (next_is('/')) {
            Node *right = new Node(cast_expr());
			node = new Node(binop('/', *node, *right));
			
            if (node->type.type == K_FLOAT || node->type.type == K_DOUBLE
                || right->type.type == K_FLOAT || right->type.type == K_DOUBLE)
                createQuadruple("/f");
            else
                createQuadruple("/");
		}
		else if (next_is('%')) {
            Node *right = new Node(cast_expr());

            if (!cheak_is_int_type(*node)) errorp(ts_.getPos(), "mod op need a interger object.");
            if (!cheak_is_int_type(*right)) errorp(ts_.getPos(), "mod op need a interger object.");

			node = new Node(binop('%', *node, *right));
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
    if (ts_.test('(')) {
        if (is_type(ts_.peek2())) {
            ts_.next();
            ts_.next();
            expect(')');
        }
    }
    return unary_expr();
}

Node Parser::unary_expr()
{
    Token tok = ts_.next();
    if (tok.getType() == T_KEYWORD) {
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
            //if(lex.peek().toString() == "=")
            //    createUnaryQuadruple("*=");
            //else 
            //    createUnaryQuadruple("*U");

            createUnaryQuadruple("*U");
            return r;


        case '+':
            r = cast_expr();
            createUnaryQuadruple("+U");
            return r;

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
    ts_.back();
    return postfix_expr();
}

Node Parser::unary_minus()
{
    Node *expr = new Node(cast_expr());

    if (is_inttype(expr->type))
		return binop('-', *(new Node(createIntNode(expr->type, 0))), *expr);

	return binop('-', *(new Node(createFloatNode(expr->type, 0.0))), *expr);
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
            int counter = 0;
            do {
                expr();
                expect(']');
                counter++;
                for (size_t i = counter; i < node.type.len.size(); ++i) {
                    _stk_quad.push_back(std::to_string(node.type.len.at(i)));
                    createQuadruple("*");
                }
                if (counter > 1) {
                    if (node.type.len.size() > 1)
                        createQuadruple("+");
                }
            } while (next_is('['));

            

			if (ts_.peek().getId() == '=')
				createQuadruple("[]&");
			else
				createQuadruple("[]");
        }
        if (next_is('.')) {
            Token t = ts_.next();
            int _off = 0;
            for (size_t i = 0; i < node.type.fields.size(); ++i){
                if (t.getSval() == node.type.fields.at(i)._name)
                    _off = node.type.fields.at(i)._off;
            }
            _stk_quad.push_back(std::to_string(_off));
            
            if(ts_.peek().getId() == '=')
                createQuadruple(".&");
            else 
                createQuadruple(".");
		}
		if (next_is(OP_ARROW)) {
            Token t = ts_.next();
            int _off = 0;
            for (size_t i = 0; i < node.type.ptr->fields.size(); ++i) {
                if (t.getSval() == node.type.ptr->fields.at(i)._name)
                    _off = node.type.ptr->fields.at(i)._off;
            }
            _stk_quad.push_back(std::to_string(_off));

            if (ts_.peek().getId() == '=')
                createQuadruple("->&");
            else
                createQuadruple("->");
            
		}
		Token tok = ts_.peek();
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
	Token tok = ts_.next();
	std::string strl;
    std::string Lfloat;

	if (is_keyword(tok, '(')) {
		Node r = expr();
		expect(')');
		return r;
	}

	switch (tok.getType()) {

	case T_IDENTIFIER:
		return var_or_func(tok);

	case T_INTEGER:
		pushQuadruple(tok.getSval());
		return createIntNode(tok, 4, false);

	case T_FLOAT:
        Lfloat = newLabel("f");
		float_const.push_back(tok.getSval());
        float_const.push_back(Lfloat);
        float_const.push_back("4f");
		pushQuadruple(Lfloat);
		return createFloatNode(tok);

	case T_CHAR:
		pushQuadruple(std::to_string(tok.getCh()));
		return createIntNode(tok, 1, true);

	case T_STRING:
		strl = newLabel("LSTR");
		pushQuadruple(strl);
		const_string.push_back(StrCard(tok.getSval(), strl));
		return createStrNode(tok);

	case T_KEYWORD:
        ts_.back();
		return Node();

	default:
		errorp(ts_.getPos(), "internal error: unknown token kind");
	}
    return Node();       // for warning
}

Node Parser::var_or_func(Token &t)
{
	Node r;
#if defined(WIN32)
    r = localenv->search("_" + t.getSval());
#elif defined(__linux__)
    r = localenv->search(t.getSval());
#endif

	if (r.kind == NODE_GLO_VAR || r.kind == NODE_LOC_VAR)
		pushQuadruple(r.name());
#ifdef _OVERLOAD_
	else 
		pushQuadruple(r.name());
#else
	else if (r.kind == NODE_FUNC || r.kind == NODE_FUNC_DECL)
		pushQuadruple(r.name());

    std::map<std::string, std::string> ::iterator iter = enum_const.find(t.getSval());
    if (iter != enum_const.end())
        pushQuadruple(t.getSval());
    else if (r.kind == NODE_NULL)
        errorp(ts_.getPos(), "undefined var : " + t.getSval());
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
Type Parser::usual_arith_conv(Type &t, Type &u)
{
	if (t.getType() < u.getType()) {
		Type tmp = t;
		t = u;
		u = tmp;
	}
	if (is_floattype(t))
		return t;
	if (t.size_ > u.size_)
		return t;

	if (t.isUnsigned() == u.isUnsigned())
		return t;
	Type r = t;
	r.setUnsig(true);
	return r;
}


void Parser::ensure_inttype(Node &node) 
{
	if (!is_inttype(node.getType()))
        errorp(ts_.getPos(), "integer type expected");
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
	Node *operand = new Node(cast_expr());
	if (operand->type.getType() != PTR)
        errorp(ts_.getPos(), "pointer type expected.");
	if (operand->type.getType() == NODE_FUNC)
		return *operand;
	return createUnaryNode(NODE_DEREF, *(operand->type.ptr), *operand);
}

Node Parser::unary_bitnot(Token &t)
{
	Node *expr = new Node(cast_expr());

	if (!is_inttype(expr->type))
        errorp(ts_.getPos(), "invalid use of '~'.");
	return createUnaryNode('~', expr->type, *expr);
}
Node Parser::unary_lognot()
{
	Node *operand = new Node(cast_expr());
	return createUnaryNode('!', Type(K_INT, 4, false), *operand);
}