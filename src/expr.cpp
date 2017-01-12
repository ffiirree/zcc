#include "parser.h"
#include "error.h"

Node *Parser::expression()
{
    return comma_expr();
}
Node *Parser::expr_opt()
{
    return comma_expr();
}
Node *Parser::bool_expr()
{
    Node *r = comma_expr();

    if (isCondition && boolLabels_.empty()) {
        BoolLabel *B = new BoolLabel();
        std::string val = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
        
        B->trueList_ = makelist(quadStk_.size());
        B->falseList_ = makelist(quadStk_.size() + 1);
        _GENQ3_("if", val + " != 0", "goto");
        _GENQ1_("goto");
        boolLabels_.push_back(B);
    }
    return r;
}

bool Parser::compute_bool_expr()
{
    bool r = false;

    isComputeBool = true;
    bool_expr();

    if (quad_arg_stk_.size() > 1)
        error("bool_expr not res.");
    if (quad_arg_stk_.size() == 1) {
        if (atoi(quad_arg_stk_.back().c_str()) != 0)
            r = true;
    }


    isComputeBool = false;

    return r;
}

Node *Parser::comma_expr()
{
    Node *left = assignment_expr();
    createIncDec();
    while (next_is(','))
    {
        Node *right = assignment_expr();
        createIncDec();
        left = createBinOpNode(right->getType(), ',', left, right);
    }
    return left;
}

Node *Parser::assignment_expr()
{
    Node *node = logical_or_expr();
    Token t = ts_.next();

    if (is_keyword(t, '?')) {
        conditional_expr(node);
        ts_.next();
    }

    int cop = get_compound_assign_op(t); // * + * ..

    if (is_keyword(t, '=') || cop) {
        Node *value = assignment_expr();
        Node *temp;
        if (cop) {
            temp = binop(cop, node, value);
            pushQuadruple((*node).name());
            std::string _1, _2;
            _1 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
            _2 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
            quad_arg_stk_.push_back(_1);
            quad_arg_stk_.push_back(_2);

            if (temp->type_.type == K_DOUBLE || temp->type_.type == K_FLOAT) {
                createQuadruple(get_compound_assign_op_signal(t) + "f");
                createQuadruple("=f");
            }
            else {
                createQuadruple(get_compound_assign_op_signal(t));
                createQuadruple("=");
            }
        }
        else {
            temp = value;
            if (temp->type_.type == K_DOUBLE || temp->type_.type == K_FLOAT)
                createQuadruple("=f");
            else
                createQuadruple("=");
        }
        Node *right = temp;

        return createBinOpNode(node->getType(), '=', node, right);
    }
    ts_.back();
    return node;
}

Node *Parser::conditional_expr(Node *node)
{
    BoolLabel *B = boolLabels_.back(); boolLabels_.pop_back();
    std::string snext = newLabel("sn");

    _GENQL_(newLabel("t"));
    backpatch(B->trueList_, quadStk_.size());

    Node *then = expression();
    _GENQ2_("goto", snext);
    _GENQL_(newLabel("f"));
    backpatch(B->falseList_, quadStk_.size());

    expect(':');
    Node *els = com_conditional_expr();

    _GENQL_(snext);
    return createIfStmtNode(node, then, els);
}

Node *Parser::com_conditional_expr()
{
    Node *node = logical_or_expr();

    if (next_is('?')) {
        std::string snext = newLabel("sn");
        BoolLabel *B = boolLabels_.back(); boolLabels_.pop_back();

        _GENQL_(newLabel("t"));
        backpatch(B->trueList_, quadStk_.size());

        Node *then = expression();
        _GENQ2_("goto", snext);
        _GENQL_(newLabel("f"));
        backpatch(B->falseList_, quadStk_.size());

        expect(':');
        Node *els = com_conditional_expr();

        _GENQL_(snext);
        return createIfStmtNode(node, then, els);
    }
    return node;
}

Node *Parser::logical_or_expr()
{
    Node *node = logical_and_expr(true);
    while (next_is(OP_LOGOR)) {
        if (!isComputeBool) {
            BoolLabel *B = new BoolLabel();
            _GENQL_(newLabel("B"));
            BoolLabel *B1 = boolLabels_.back(); boolLabels_.pop_back();
            backpatch(B1->falseList_, quadStk_.size());
            node = createBinOpNode(Type(K_INT, 4, false), OP_LOGOR, node, logical_and_expr(false));

            BoolLabel *B2 = boolLabels_.back(); boolLabels_.pop_back();

            boolLabels_.push_back(B);

            B->trueList_ = merge(B1->trueList_, B2->trueList_);
            B->falseList_ = B2->falseList_;
        }
        else if (isComputeBool) {
            node = createBinOpNode(Type(K_INT, 4, false), OP_LOGOR, node, logical_and_expr(false));
            computeBoolExpr("||");
        }
    }

    return node;
}

Node *Parser::logical_and_expr(bool isLeft)
{
    Node *node = bit_or_expr();

    while (next_is(OP_LOGAND)) {
        if (!isComputeBool) {
            BoolLabel *B = new BoolLabel();

            _GENQL_(newLabel("B"));
            BoolLabel *B1 = boolLabels_.back(); boolLabels_.pop_back();
            backpatch(B1->trueList_, quadStk_.size());
            node = createBinOpNode(Type(K_INT, 4, false), OP_LOGAND, node, bit_or_expr());
            BoolLabel *B2 = boolLabels_.back(); boolLabels_.pop_back();

            boolLabels_.push_back(B);

            B->trueList_ = B2->trueList_;
            B->falseList_ = merge(B1->falseList_, B2->falseList_);
        }
        else if (isComputeBool) {
            node = createBinOpNode(Type(K_INT, 4, false), OP_LOGAND, node, bit_or_expr());

            computeBoolExpr("&&");
        }
    }


    return node;
}
Node *Parser::bit_or_expr()
{
    Node *node = bit_xor_expr();
    while (next_is('|')) {
        node = binop('|', node, bit_xor_expr());
        createQuadruple("|");
    }

    return node;
}
Node *Parser::bit_xor_expr()
{
    Node *node = bit_and_expr();
    while (next_is('^')) {
        node = binop('^', node, bit_and_expr());
        createQuadruple("^");
    }

    return node;
}
Node *Parser::bit_and_expr()
{
    Node *node = equal_expr();
    while (next_is('&')) {
        node = binop('&', node, equal_expr());
        createQuadruple("&");
    }

    return node;
}
Node *Parser::equal_expr()
{
    Node *node = relational_expr();
    Node *r;

    if (next_is(OP_EQ)) {
        r = binop(OP_EQ, node, equal_expr());

        if (!isComputeBool)
            createRelOpQuad("==");
        else
            computeBoolExpr("==");
    }
    else if (next_is(OP_NE)) {
        r = binop(OP_NE, node, equal_expr());

        if (!isComputeBool)
            createRelOpQuad("!=");
        else 
            computeBoolExpr("!=");
    }
    else {
        return node;
    }
    r->setType(Type(K_INT, 4, false));
    return r;
}

Node *Parser::relational_expr()
{
    Node *node = shift_expr();
    for (;;) {
        if (next_is('<')) {
            node = binop('<', node, shift_expr());

            if (!isComputeBool)
                createRelOpQuad("<");
            else 
                computeBoolExpr("<");
        }
        else if (next_is('>')) {
            node = binop('>', node, shift_expr());
            if (!isComputeBool)
                createRelOpQuad(">");
            else 
                computeBoolExpr(">");
        }
        else if (next_is(OP_LE)) {
            node = binop(OP_LE, node, shift_expr());
            if (!isComputeBool) 
                createRelOpQuad("<=");
            else 
                computeBoolExpr("<=");
        }
        else if (next_is(OP_GE)) {
            node = binop(OP_GE, node, shift_expr());
            if (!isComputeBool)
                createRelOpQuad(">=");
            else 
                computeBoolExpr(">=");
        }
        else
            return node;
        node->setType(Type(K_INT, 4, false));
    }
}

Node *Parser::shift_expr()
{
    Node *node = add_expr();
    for (;;) {
        int op;
        if (next_is(OP_SAL))
            op = OP_SAL;//<<
        else if (next_is(OP_SAR))
            op = node->getType().isUnsigned() ? OP_SHR : OP_SAR;
        else
            break;
        Node *right = add_expr();

        if (cheak_is_int_type(*node)) errorp(ts_.getPos(), "shift operator need a interger object.");
        if (cheak_is_int_type(*right)) errorp(ts_.getPos(), "shift operator need a interger object.");

        node = createBinOpNode(node->getType(), op, node, right);
        if (op == OP_SAL)
            createQuadruple("<<");
        else if (op == OP_SHR)
            createQuadruple(">>>");
        else if (op == OP_SAR)
            createQuadruple(">>");
    }
    return node;
}

Node *Parser::add_expr()
{
    Node *node = multi_expr();
    for (;;) {
        if (next_is('+')) {
            Node *right = multi_expr();
            node = binop('+', node, right);
            if (node->type_.type == K_FLOAT || node->type_.type == K_DOUBLE
                || right->type_.type == K_FLOAT || right->type_.type == K_DOUBLE)
                createQuadruple("+f");
            else
                createQuadruple("+");
        }
        else if (next_is('-')) {
            Node *right = multi_expr();
            node = binop('-', node, right);
            if (node->type_.type == K_FLOAT || node->type_.type == K_DOUBLE
                || right->type_.type == K_FLOAT || right->type_.type == K_DOUBLE)
                createQuadruple("-f");
            else
                createQuadruple("-");
        }
        else
            return node;
    }
}

Node *Parser::multi_expr()
{
    Node *node = cast_expr();
    for (;;) {
        if (next_is('*')) {
            Node *right = cast_expr();
            node = binop('*', node, right);
            if (node->type_.type == K_FLOAT || node->type_.type == K_DOUBLE
                || right->type_.type == K_FLOAT || right->type_.type == K_DOUBLE)
                createQuadruple("*f");
            else
                createQuadruple("*");
        }
        else if (next_is('/')) {
            Node *right = cast_expr();
            node = binop('/', node, right);

            if (node->type_.type == K_FLOAT || node->type_.type == K_DOUBLE
                || right->type_.type == K_FLOAT || right->type_.type == K_DOUBLE)
                createQuadruple("/f");
            else
                createQuadruple("/");
        }
        else if (next_is('%')) {
            Node *right = cast_expr();

            if (!cheak_is_int_type(*node)) errorp(ts_.getPos(), "mod op need a interger object.");
            if (!cheak_is_int_type(*right)) errorp(ts_.getPos(), "mod op need a interger object.");

            node = binop('%', node, right);
            createQuadruple("%");
        }
        else
            return node;
    }
}

Node *Parser::cast_expr()
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

Node *Parser::unary_expr()
{
    Token tok = ts_.next();
    if (tok.getType() == T_KEYWORD) {
        Node *r = nullptr;
        Node *node = nullptr;

        switch (tok.getId()) {
        case K_SIZEOF:
            return sizeof_op();

        case OP_INC:
            node = unary_expr();
            createUnaryQuadruple("++");
            return createUnaryNode(OP_PRE_INC, node->type_, node);

        case OP_DEC:
            node = unary_expr();
            createUnaryQuadruple("--");
            return createUnaryNode(OP_PRE_DEC, node->type_, node);

        case '&':
            node = cast_expr();
            createUnaryQuadruple("&U");
            return createUnaryNode(NODE_ADDR, conv2ptr(node->type_), node);

        case '*':
            node = cast_expr();
            if (node->type_.getType() != PTR)
                errorp(ts_.getPos(), "pointer type expected.");
            if (node->type_.getType() == NODE_FUNC)
                r = node;
            r = createUnaryNode(NODE_DEREF, *(node->type_.ptr), node);

            if(ts_.peek().toString() == "=")
                createUnaryQuadruple("*=");
            else 
                createUnaryQuadruple("*U");
            return r;


        case '+':
            r = cast_expr();
            createUnaryQuadruple("+U");
            return r;

        case '-':
            node = cast_expr();

            if (is_inttype(node->type_)) {
                createUnaryQuadruple("-U");
                return binop('-', createIntNode(node->type_, 0), node);
            }

            createUnaryQuadruple("-U");
            return binop('-', createFloatNode(node->type_, 0.0), node);

        case '~':
            node = cast_expr();
            if (!is_inttype(node->type_))
                errorp(ts_.getPos(), "invalid use of '~'.");
            createUnaryQuadruple("~");
            return createUnaryNode('~', node->type_, node);

        case '!':
            return createUnaryNode('!', Type(K_INT, 4, false), cast_expr());
        }
    }
    ts_.back();
    return postfix_expr();
}

Node *Parser::postfix_expr()
{
    Node *node = primary_expr();
    return postfix_expr_tail(node);
}


std::vector<Node *> Parser::argument_expr_list()
{
    std::vector<Node*> list;
    list.push_back(assignment_expr());
    while (next_is(','))
        list.push_back(assignment_expr());
    return list;
}

Node *Parser::postfix_expr_tail(Node *node)
{
    for (;;) {
        if (next_is('(')) {

            std::vector<Node *> parms = argument_expr_list();
            expect(')');
            createFuncQuad(parms);
        }
        if (next_is('[')) {
            int counter = 0;
            do {
                expression();
                expect(']');
                counter++;
                for (size_t i = counter; i < node->type_.len.size(); ++i) {
                    quad_arg_stk_.push_back(std::to_string(node->type_.len.at(i)));
                    createQuadruple("*");
                }
                if (counter > 1) {
                    if (node->type_.len.size() > 1)
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
            for (size_t i = 0; i < node->type_.fields.size(); ++i) {
                if (t.getSval() == node->type_.fields.at(i)._name) {
                    _off = node->type_.fields.at(i)._off;
                    node->type_.size_ = node->type_.fields.at(i)._type->size_;
                }
                    
            }
            quad_arg_stk_.push_back(std::to_string(_off));

            if (ts_.peek().getId() == '=')
                createQuadruple(".&");
            else
                createQuadruple(".");
        }
        if (next_is(OP_ARROW)) {
            Token t = ts_.next();
            int _off = 0;
            for (size_t i = 0; i < node->type_.ptr->fields.size(); ++i) {
                if (t.getSval() == node->type_.ptr->fields.at(i)._name)
                    _off = node->type_.ptr->fields.at(i)._off;
            }
            quad_arg_stk_.push_back(std::to_string(_off));

            if (ts_.peek().getId() == '=')
                createQuadruple("->&");
            else
                createQuadruple("->");

        }
        Token tok = ts_.peek();
        if (next_is(OP_INC) || next_is(OP_DEC)) {
            ensure_lvalue(*node);
            int op = is_keyword(tok, OP_INC) ? OP_POST_INC : OP_POST_DEC;

            if (op == OP_POST_DEC)
                incDecStk_.push_back({ "--", quad_arg_stk_.back() });
            else
                incDecStk_.push_back({ "++", quad_arg_stk_.back() });

            return createUnaryNode(op, node->type_, node);
        }
        return node;
    }
}

Node *Parser::primary_expr()
{
    Token tok = ts_.next();
    std::string strl;
    std::string Lfloat;

    if (is_keyword(tok, '(')) {
        Node *r = expression();
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
        return nullptr;

    default:
        errorp(ts_.getPos(), "internal error: unknown token kind");
    }
    return nullptr;       // for warning
}

Node *Parser::var_or_func(Token &t)
{
    Node r;
#if defined(WIN32)
    r = localenv->search("_" + t.getSval());
#elif defined(__linux__)
    r = localenv->search(t.getSval());
#endif

    if (r.kind_ == NODE_GLO_VAR || r.kind_ == NODE_LOC_VAR)
        pushQuadruple(r.name());
#ifdef _OVERLOAD_
    else
        pushQuadruple(r.name());
#else
    else if (r.kind_ == NODE_FUNC || r.kind_ == NODE_FUNC_DECL)
        pushQuadruple(r.name());

    std::map<std::string, std::string> ::iterator iter = enum_const.find(t.getSval());
    if (iter != enum_const.end())
        pushQuadruple(t.getSval());
    else if (r.kind_ == NODE_NULL)
        errorp(ts_.getPos(), "undefined var : " + t.getSval());
#endif // _OVERLOAD_

    return new Node(r);
}


Node *Parser::wrap(Type &t, Node *node) {
    if (t.getType() == node->type_.getType() && t.isUnsigned() == t.isUnsigned())
        return node;
    return createUnaryNode(CONV, node->type_, node);
}

Node *Parser::binop(int op, Node *lhs, Node *rhs)
{
    if (lhs->type_.getType() == '*' && rhs->type_.getType() == '*') {
        if (op == '-')
            return createBinOpNode(Type(K_LONG, 8, false), '-', lhs, rhs);
        return createBinOpNode(Type(K_INT, 4, false), '-', lhs, rhs);
    }

    if (lhs->type_.getType() == '*')
        return createBinOpNode(lhs->type_, op, lhs, rhs);
    if (rhs->type_.getType() == '*')
        return createBinOpNode(rhs->type_, op, lhs, rhs);

    Type r = usual_arith_conv(lhs->type_, rhs->type_);

    return createBinOpNode(r, op, wrap(r, lhs), wrap(r, rhs));
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


Node *Parser::sizeof_op()
{
    Type ty(K_INT, 4, true);
    Node *r = new Node(NODE_INT, ty);
    int size = 0;
    
    if (next_is('('))
    {
        if (ts_.peek().kind_ == T_KEYWORD) {
            Type ty = decl_specifiers(new int);
            size = ty.size_;
        }
        else {
#if defined(WIN32)
            Node var = localenv->search("_" + ts_.next().sval_);
#elif(__linux__)
            Node var = localenv->search(ts_.next().sval_);
#endif
            size = var.type_.size_;
        }
        expect(')');
    }
    else {
        if (ts_.peek().kind_ != T_IDENTIFIER)
            errorp(ts_.getPos(), "expect a object.");

#if defined(WIN32)
        Node var = localenv->search("_" + ts_.next().sval_);
#elif(__linux__)
        Node var = localenv->search(ts_.next().sval_);
#endif
        size = var.type_.size_;
    }

    r->sval_ = std::to_string(size);
    quad_arg_stk_.push_back(r->sval_);
    return r;
}