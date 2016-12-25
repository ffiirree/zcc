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
    BoolLabel _if;
    std::string snext = newLabel("sn");

    expect('(');
    _EN_CONDITION_();
    Node *cond = new Node(bool_expr());          // B.code
    _DIS_CONDITION_();
    expect(')');

    _if.true_ = newLabel("ift");
    _if.false_ = newLabel("iff");
    boolLabel.back().true_ = _if.true_;
    boolLabel.back().false_ = _if.false_;
    generateIfGoto();

    _GENQL_(_if.true_);
    _GENQ1_("clr");
    Node *then = new Node(statement());     // S1.code

    if (next_is(K_ELSE)) {
        _GENQ2_("goto", snext);
        _GENQL_(_if.false_);

        Node *els = new Node(statement());
        _GENQL_(snext);
        return createIfStmtNode(cond, then, els);
    }
    else {
        _GENQL_(_if.false_);
    }
    return createIfStmtNode(cond, then, nullptr);
}

Node Parser::while_stmt()
{
    BoolLabel _while;
    std::string _begin = newLabel("wb");   // begin = newLabel
    std::string _snext = newLabel("sn");

    // break �� continue
    _stk_ctl_bg_l.push_back(_begin);
    _stk_ctl_end_l.push_back(_snext);

    _while.true_ = newLabel("wt");         // B.true = newLabel
    _while.true_ = _snext;                // B.false = S.next

    std::string _s1next = _begin;          // S1.next = begin

    _GENQL_(_begin);     // Label(begin)

    expect('(');
    _EN_CONDITION_();
    Node node = bool_expr();
    _DIS_CONDITION_();
    expect(')');

    boolLabel.back().true_ = _while.true_;
    boolLabel.back().false_ = _while.false_;
    generateIfGoto();                      // B.code
    _GENQL_(_while.true_);            // Label(B.true)

    _GENQ1_("clr");

    Node body = statement();               // S1.code
    _GENQ2_("goto", _begin);  // gen(goto begin)
    _GENQL_(_snext);

    _stk_ctl_bg_l.pop_back();
    _stk_ctl_end_l.pop_back();

    std::vector<Node> list;
    return createCompoundStmtNode(list);
}


Node Parser::do_stmt()
{
    BoolLabel doLabel;
    std::string begin = newLabel("db");   // begin = newLabel
    std::string snext = newLabel("sn");

    _stk_ctl_bg_l.push_back(begin);
    _stk_ctl_end_l.push_back(snext);

    doLabel.true_ = begin;
    doLabel.false_ = snext;

    _GENQL_(begin);
    Node *r = new Node(statement());
    expect(K_WHILE);
    expect('(');
    _EN_CONDITION_();
    Node *_b = new Node(bool_expr());
    _DIS_CONDITION_();
    expect(')');
    expect(';');

    boolLabel.back().true_ = doLabel.true_;
    boolLabel.back().false_ = doLabel.false_;
    generateIfGoto();                      // B.code
    _GENQL_(snext);

    _stk_ctl_bg_l.pop_back();
    _stk_ctl_end_l.pop_back();

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
    std::string _next = newLabel("forn");
    std::string _begin = newLabel("forb");
    std::string _exp3 = newLabel("fe3");


    BoolLabel _for;

    _stk_ctl_bg_l.push_back(_begin);
    _stk_ctl_end_l.push_back(_next);

    _for.false_ = _next;
    _for.true_ = newLabel("fort");

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

    if (is_keyword(ts_.peek(), ';')) {
        expect(';');
    }
    else {
        _EN_CONDITION_();
        bool_expr();
        _DIS_CONDITION_();
        expect(';');
    }

    boolLabel.push_back(_for);
    generateIfGoto();
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
    _GENQL_(_for.true_);

    statement();
    __OUT_SCOPE__(localenv);

    _GENQ2_("goto", _exp3);
    _GENQL_(_next);

    _stk_ctl_bg_l.pop_back();
    _stk_ctl_end_l.pop_back();

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

    _GENQIF_(switch_expr + " != " + std::to_string(val), switch_case_label);
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