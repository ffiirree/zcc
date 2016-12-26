#include<iomanip>
#include "parser.h"
#include "error.h"

Node Parser::statement()
{
    Token t = ts_.next();
    if (t.getType() == T_KEYWORD) {
        switch (t.getId())
        {
        case '{': return compound_stmt();
        case K_IF: return if_stmt();
        case K_FOR:return for_stmt();
        case K_WHILE:return while_stmt();
        case K_DO:return do_stmt();
        case K_RETURN: return return_stmt();
        case K_SWITCH: return switch_stmt();
        case K_CASE: return case_stmt();
        case K_DEFAULT: return default_stmt();
        case K_BREAK: return break_stmt();
        case K_CONTINUE:return continue_stmt();
        case K_GOTO: return goto_stmt();
        }
    }

    if (t.getType() == T_IDENTIFIER && next_is(':')) {
        // label
        labels.push_back(t.getSval());
        _GENQL_(t.getSval());
        return Node();
    }
    ts_.back();

    Node r = expr_opt();
    expect(';');

    return r;
}

Node Parser::compound_stmt()
{
    __IN_SCOPE__(localenv, localenv, newLabel("Env"));

    std::vector<Node> list;

    for (;;) {
        if (next_is('}')) break;
        decl_or_stmt(list);
    }

    __OUT_SCOPE__(localenv);
    return createCompoundStmtNode(list);
}

void Parser::decl_or_stmt(std::vector<Node> &list)
{
    if (ts_.peek().getId() == T_EOF)
        error("premature end of input");

    if (is_type(ts_.peek())) {
        declaration(list, false);
    }
    else {
        list.push_back(statement());
    }
    _GENQ1_("clr");
}

Node Parser::if_stmt()
{
    std::string nextLabel = newLabel("n");

    expect('(');
    _EN_CONDITION_();
    Node *cond = new Node(bool_expr());          // B.code
    _DIS_CONDITION_();
    expect(')');

    BoolLabel *B = boolLabels_.back(); boolLabels_.pop_back();
    _GENQL_(newLabel("t"));
    backpatch(B->trueList_, quadStk_.size());
    _GENQ1_("clr");
    Node *then = new Node(statement());     // S1.code

    if (next_is(K_ELSE)) {
        _GENQ2_("goto", nextLabel);
        _GENQL_(newLabel("f"));
        backpatch(B->falseList_, quadStk_.size());

        Node *els = new Node(statement());
        _GENQL_(nextLabel);

        return createIfStmtNode(cond, then, els);
    }

    _GENQL_(nextLabel);
    backpatch(B->falseList_, quadStk_.size());

    return createIfStmtNode(cond, then, nullptr);
}

Node Parser::while_stmt()
{
    std::string beginLabel = newLabel("wb");

    _GENQL_(beginLabel);                       // Label(begin)
    expect('(');
    _EN_CONDITION_();
    Node node = bool_expr();
    _DIS_CONDITION_();
    expect(')');
    _GENQ1_("clr");

    BoolLabel *B = boolLabels_.back(); boolLabels_.pop_back();
    _GENQL_(newLabel("wt"));                 // Label(B.true)
    backpatch(B->trueList_, quadStk_.size());

    Node body = statement();                 // S1.code

    _GENQ2_("goto", beginLabel);
    _GENQL_(newLabel("n"));
    backpatch(B->falseList_, quadStk_.size());

    std::vector<Node> list;
    return createCompoundStmtNode(list);
}


Node Parser::do_stmt()
{
    int M = 0;

    _GENQL_(newLabel("db"));
    M = quadStk_.size();
    Node *r = new Node(statement());
    expect(K_WHILE);
    expect('(');
    _EN_CONDITION_();
    Node *_b = new Node(bool_expr());
    _DIS_CONDITION_();
    expect(')');
    expect(';');
    BoolLabel *B = boolLabels_.back(); boolLabels_.pop_back();

    backpatch(B->trueList_, M);

    _GENQL_(newLabel("sn"));
    backpatch(B->falseList_, quadStk_.size());

    return *r;
}

Node Parser::switch_stmt()
{
    std::string _next = newLabel("swn");

    _stk_ctl_end_l.push_back(_next);

    expect('(');
    Node r = expr();
    if (r.type.getType() != K_INT && r.type.getType() != K_CHAR
        && r.type.getType() != K_SHORT && r.type.getType() != K_LONG)
        error("Switch only integer!");

    if (r.kind == NODE_INT || r.kind == NODE_CHAR || r.kind == NODE_SHORT || r.kind == NODE_LONG)
        switch_expr = std::to_string(r.int_val);
    else if (r.kind == NODE_LOC_VAR || NODE_GLO_VAR)
        switch_expr = r.name();
    else
        switch_expr = _stk_quad.back();
    expect(')');

    expect('{');
    compound_stmt();

    _GENQL_(_next);
    _stk_ctl_end_l.pop_back();
    return Node();
}

Node Parser::for_stmt()
{
    BoolLabel *B = nullptr;
    int M = 0;

    std::string _next = newLabel("forn");
    std::string _begin = newLabel("forb");
    std::string _exp3 = newLabel("fe3");


    expect('(');
    __IN_SCOPE__(localenv, localenv, newLabel("for"));
    if (is_type(ts_.peek())) {
        std::vector<Node> list;
        declaration(list, false);
    }
    else if (is_keyword(ts_.peek(), ';')) {
        expect(';');
    }
    else {
        expr();
        expect(';');
    }

    _GENQL_(_begin);
    M = quadStk_.size();

    if (is_keyword(ts_.peek(), ';')) {
        expect(';');
    }
    else {
        _EN_CONDITION_();
        bool_expr();
        _DIS_CONDITION_();
        expect(';');
        B = boolLabels_.back(); boolLabels_.pop_back();
    }
    
    _GENQ1_("clr");
    _GENQL_(_exp3);
    if (is_keyword(ts_.peek(), ')')) {
        expect(')');
    }
    else {
        expr();
        expect(')');
    }
    _GENQ1_("clr"); 
    _GENQ2_("goto", _begin); 

    _GENQL_(newLabel("ft"));
    backpatch(B->trueList_, quadStk_.size());

    statement();
    __OUT_SCOPE__(localenv);

    _GENQ2_("goto", _exp3);
    _GENQL_(_next);
    backpatch(B->falseList_, quadStk_.size());

    return Node();
}


Node Parser::goto_stmt()
{
    Token t = ts_.next();
    labels.push_back_un(t.getSval());
    expect(';');

    _GENQ2_("goto", t.getSval());

    return Node();
}

Node Parser::continue_stmt()
{
    _GENQ2_("goto", _stk_ctl_bg_l.back());
    return  Node();
}

Node Parser::return_stmt()
{
    Node *retval = new Node(expr_opt());
    expect(';');

    _GENQ2_("ret", _stk_quad.back());

    return createRetStmtNode(retval);
}


Node Parser::case_stmt()
{
    int val = com_conditional_expr().int_val;
    expect(':');

    _GENQ4_("if", switch_expr + " != " + std::to_string(val), "goto", switch_case_label);
    statement();

    _GENQL_(switch_case_label);
    switch_case_label = newLabel("case");

    return Node();
}
Node Parser::default_stmt()
{
    expect(':');
    statement();
    return Node();
}

Node Parser::break_stmt()
{
    _GENQ2_("goto", _stk_ctl_end_l.back());
    expect(';');
    return createJumpNode(label_break);
}