#include<iomanip>
#include "parser.h"
#include "error.h"

std::vector<Node*> Parser::trans_unit()
{
    std::vector<Node*> list;
    for (;;) {
        Token t = ts_.peek();
        if (t.getType() == T_EOF) {
            labels.cheak();
            out_quad();
            out.close();
            return list;
        }

        if (isFuncDef())
            list.push_back(funcDef());
        else
            declaration(list, true);
    }
    return list;
}

void Env::push_back(Node &n) {
    if (n.kind_ == NODE_FUNC) {
        Node r = search(n.name());
        if (r.kind_ == NODE_FUNC_DECL) {
            setFuncDef(n);
            return;
        }
        else if (r.kind_ != 0) {
            error("Function redefined: " + n.name());
        }
    }

    nodes_.push_back(n);
}

void Label::push_back(const std::string &label) {

    for (size_t i = 0; i < labels.size(); ++i) {
        if (label == labels.at(i)) {
            if (enLabels.at(i) == false) {
                enLabels.at(i) = true;
                return;
            }
            else {
                error(label + " is existed");
                return;
            }
        }
    }

    labels.push_back(label);
    enLabels.push_back(true);
}

bool Label::cheak()
{
    for (size_t i = 0; i < labels.size(); ++i) {
        if (!enLabels.at(i))
            error("Label '" + labels.at(i) + "' is undefined");
    }
    return true;
}

bool Parser::isFuncDef()
{
    int count = 0;
    Token t;

    do {
        t = ts_.next();
        count++;
    } while (is_type(t) || t.getId() == '*');
    ts_.back(); count--;

    for (;;) {
        t = ts_.next();
        count++;
        if (is_keyword(t, '(')) {
            skip_parenthesis(&count);
            if (next_is('{')) {
                count++;
                goto _end;
            }
        }
        else if (t.getType() == T_IDENTIFIER) {
            if (next_is('(')) {
                count++;
                skip_parenthesis(&count);
                if (next_is('{')) {
                    count++;
                    goto _end;
                }
            }
        }
        else {
            for (int i = 0; i < count; ++i)
                ts_.back();
            return false;
        }
    }

_end:
    for (int i = 0; i < count; ++i)
        ts_.back();
    return true;
}

Node *Parser::funcDef()
{
    int current_class = 0;                                                  // static ...
    std::string funcName;
    std::vector<Node*> params;
    __IN_SCOPE__(localenv, globalenv, newLabel("fun"));

    Type *retty = new Type(decl_spec_opt(&current_class));
    Type functype = declarator(retty, funcName, params, FUNC_BODY);
    if (functype.type == PTR) {
        errorp(ts_.getPos(), "Ptr not can be function.");
    }

#if defined(_OVERLOAD_)
    funcName = getOverLoadName(funcName, params);
#endif

#ifdef WIN32
    _GENQL_("_" + funcName);
#elif defined(__linux__)
    out << funcName << ":" << std::endl;
#endif

    functype.setStatic(current_class == K_STATIC);
    expect('{');

    __IN_FUNC_DEF__(retty);
    Node *r = func_body(functype, funcName, params);
    __OUT_FUNC_DEF__();

    __OUT_SCOPE__(localenv);
    _GENQ1_(".end");
    return r;
}



Type Parser::func_param_list(Type *retty, std::vector<Node *> &params, int deal_type)
{
    // foo()
    if (next_is(')')) {
        return Type(NODE_FUNC, retty, params);
    }
    // foo(void)
    else if (is_keyword(ts_.peek(), K_VOID)) {
        ts_.next();
        expect(')');
        return Type(NODE_FUNC, retty, params);
    }
    // foo(int x, int y)
    else {
        params = param_list(deal_type);
        expect(')');
        return Type(NODE_FUNC, retty, params);
    }
}

std::vector<Node *> Parser::param_list(int decl_type)
{
    std::vector<Node*> list;

    __BEGIN_PARAMS__();

    list.push_back(param_decl(decl_type));

    while (next_is(','))
        list.push_back(param_decl(decl_type));

    __END_PARAMS__();
    return list;
}

Node *Parser::param_decl(int decl_type)
{
    int sclass = 0;
    Type basety = decl_specifiers(&sclass);
    std::string paramname;
    std::vector<Node *> list;
    Type type = declarator(&basety, paramname, list, NODE_PARAMS);

    if (decl_type == NODE_FUNC_DECL)
        return createFuncDeclParams(type);

    return createLocVarNode(type, paramname);
}


Node *Parser::func_body(Type &functype, std::string name, std::vector<Node *> &params)
{
    Node *body = compound_stmt();
    if (noReturnValue) {
        if (functype.retType->type == K_VOID)
            _GENQ2_("ret", "0");
        else 
            errorp(ts_.getPos(), "Function need a return statement");
    }
    
    return  createFuncNode(functype, name, params, body);
}

Node &Env::search(const std::string &key)
{
    Env *ptr = this;

    while (ptr) {
        for (size_t i = 0; i < ptr->nodes_.size(); ++i) {
            if (key == ptr->nodes_.at(i).name() || key == ptr->nodes_.at(i).name())
                return ptr->nodes_.at(i);
        }
        ptr = ptr->pre();
    }

    Node *r = new Node(NODE_NULL);

    return *r;
}

void Env::setFuncDef(Node &_def)
{
    Env *ptr = this;

    while (ptr) {
        for (size_t i = 0; i < ptr->nodes_.size(); ++i) {
            if (_def.name() == ptr->nodes_.at(i).name()) {
                ptr->nodes_.at(i) = _def;
            }
        }
        ptr = ptr->pre();
    }
}

Type Parser::get_type(const std::string &key)
{
    Type type;

    while (!localenv) {
        type = localenv->search(key).getType();
    }
    return type;
}
int Parser::get_compound_assign_op(const Token &t)
{
    if (t.getType() != T_KEYWORD)
        return 0;
    switch (t.getId()) {
    case OP_A_ADD: return '+';
    case OP_A_SUB: return '-';
    case OP_A_MUL: return '*';
    case OP_A_DIV: return '/';
    case OP_A_MOD: return '%';
    case OP_A_AND: return '&';
    case OP_A_OR:  return '|';
    case OP_A_XOR: return '^';
    case OP_A_SAL: return OP_SAL;
    case OP_A_SAR: return OP_SAR;
    default: return 0;
    }
}

std::string Parser::get_compound_assign_op_signal(const Token &t)
{
    if (t.getType() != T_KEYWORD)
        return 0;
    switch (t.getId()) {
    case OP_A_ADD: return "+";
    case OP_A_SUB: return "-";
    case OP_A_MUL: return "*";
    case OP_A_DIV: return "/";
    case OP_A_MOD: return "%";
    case OP_A_AND: return "&";
    case OP_A_OR:  return "|";
    case OP_A_XOR: return "^";
    case OP_A_SAL: return "<<";
    case OP_A_SAR: return ">>";
    default: return 0;
    }
}

bool Parser::next_is(int id)
{
    if (ts_.next().getId() == id)
        return true;
    ts_.back();
    return false;
}

Node *Parser::createIntNode(const Token &t, int size, bool isch)
{
    if (isch) {
        Node *node = new Node(NODE_CHAR);

        node->int_val = t.getCh();
        node->type_ = Type(K_CHAR, size, false);
        return node;
    }
    else {
        Node *node = new Node(NODE_INT);

        node->int_val = atoi(t.getSval().c_str());
        node->type_ = Type(K_INT, size, false);
        return node;
    }
}


Node *Parser::createIntNode(const Type &ty, int val)
{
    Node *node = new Node(NODE_INT);
    node->int_val = val;
    node->type_ = ty;
    return node;
}

Node *Parser::createFloatNode(const Type &ty, double val)
{
    Node *node = new Node(NODE_DOUBLE);

    node->type_ = ty;
    node->float_val = val;
    return node;
}


Node *Parser::createFloatNode(const Token &t)
{
    Node *node = new Node(NODE_DOUBLE);

    node->sval_ = t.getSval();
    node->type_ = Type(K_FLOAT, 4, false);
    return node;
}

Node *Parser::createStrNode(const Token &t)
{
    Node *node = new Node(NODE_STRING);
    node->type_.type = PTR;
    node->type_.size_ = 4;
    node->type_.ptr = new Type(K_CHAR, 1, false);

    node->sval_ = t.getSval();
    return node;
}

Node *Parser::createFuncNode(const Type &ty, const std::string & funcName, std::vector<Node *> params, Node *body)
{
    Node *node = new Node(NODE_FUNC, ty);
    node->setName(funcName);
    node->params = params;
    node->body = body;

    node->local_vars_stk_size_ = localVarsSize_;
    node->params_stk_size_ = paramsSize_;
    node->max_call_params_size_ = maxCallSize_;

    globalenv->push_back(*node);
    return node;
}

Node *Parser::createFuncDecl(const Type &ty, const std::string & funcName, const std::vector<Node *> &params)
{
    Node *node = new Node(NODE_FUNC_DECL, ty);
    node->setName(funcName);
    node->params = params;

    globalenv->push_back(*node);

    return node;
}



Node *Parser::createCompoundStmtNode(std::vector<Node *> &stmts)
{
    Node *node = new Node(NODE_COMP_STMT);
    node->stmts = stmts;
    return node;
}
Node *Parser::createDeclNode(Node &var)
{
    Node *node = new Node(NODE_DECL);
    node->decl_var = &var;
    return node;
}
Node *Parser::createDeclNode(Node &var, std::vector<Node *> init)
{
    Node *node = new Node(NODE_DECL);
    node->decl_var = &var;
    node->decl_init = init;

    if (var.kind_ == NODE_GLO_VAR) {
        globalenv->back().lvarinit_ = init;
    }
    else if (var.kind_ == NODE_LOC_VAR) {
        localenv->back().lvarinit_ = init;
    }

    return node;
}

Node *Parser::createGLoVarNode(const Type &ty, const std::string &name)
{
    Node *r = new Node(NODE_GLO_VAR, ty);
    r->setName(name);

    if (cheak_redefined(globalenv, r->name()))
        errorp(ts_.getPos(), "redefined global variable : " + r->name());

    globalenv->push_back(*r);

    return r;
}
Node *Parser::createLocVarNode(const Type &ty, const std::string &name)
{
    Node *r = new Node(NODE_LOC_VAR, ty);
    r->setName(name);

    if (cheak_redefined(localenv, r->name()))
        errorp(ts_.getPos(), "redefined local variable : " + r->name());

    if (isFunction) {
        if (ty.type == ARRAY) {
            localVarsSize_ += ty.size_ *ty._all_len;
        }
        else {
            localVarsSize_ += ty.size_;
        }
        r->off_ = -localVarsSize_;
    }
    else if (isParams) {
        r->off_ = paramsSize_;
        paramsSize_ += ty.size_;
    }

    localenv->push_back(*r);
    return r;
}

Node *Parser::createFuncDeclParams(const Type &ty)
{
    Node *r = new Node(NODE_DECL_PARAM, ty);
    return r;
}

Node *Parser::createBinOpNode(const Type &ty, int kind, Node *left, Node *right)
{
    Node *r = new Node(kind, ty);
    r->left_ = left;
    r->right_ = right;
    return r;
}

Node *Parser::createUnaryNode(int kind, const Type &ty, Node *node)
{
    Node *r = new Node(kind);
    r->type_ = ty;
    r->operand_ = node;
    return r;
}

Node *Parser::createRetStmtNode(Node *n)
{
    Node *r = new Node(NODE_RETURN);
    r->retval = n;
    return r;
}

Node *Parser::createJumpNode(const std::string &label)
{
    Node *r = new Node(NODE_GOTO);
    r->label = label;
    r->newLabel = label;
    return r;
}

Node *Parser::createIfStmtNode(Node *cond, Node *then, Node *els)
{
    Node *r = new Node(NODE_IF_STMT);
    r->cond = cond;
    r->then = then;
    r->els = els;
    return r;
}


bool Parser::is_type(const Token &t)
{
    if (t.getType() == T_IDENTIFIER)
        return getCustomType(t.getSval()).type != 0;

    if (t.getType() != T_KEYWORD)
        return false;

    switch (t.getId())
    {
#define keyword(id, _, is) case id: return is;
        KEYWORD_MAP
#undef keyword
    default: return false;
    }
}

bool Parser::is_keyword(const Token &t, int id)
{
    return (t.getType() == T_KEYWORD && t.getId() == id);
}

void Parser::skip_parenthesis(int *count)
{
    for (;;)
    {
        Token t = ts_.next();
        (*count)++;

        if (t.getType() == T_EOF)
            errorp(ts_.getPos(), "error eof");

        if (is_keyword(t, '('))
            skip_parenthesis(count);

        if (is_keyword(t, ')'))
            break;
    }
}
void Parser::expect(int id)
{
    Token t = ts_.next();
    if (t.getId() != id)
        errorp(ts_.getPos(), std::string("expect '") + (char)id + "', but not is '" + (char)t.getId() + "'");
}

bool Parser::is_inttype(const Type &ty)
{
    switch (ty.getType())
    {
    case K_BOOL: case K_SHORT:case K_CHAR: case K_INT:
    case K_LONG:
        return true;
    default:
        return false;
    }
}

bool Parser::is_floattype(const Type &ty)
{
    switch (ty.getType())
    {
    case K_FLOAT: case K_DOUBLE:
        return true;
    default: return false;
    }
}


bool Parser::is_arithtype(const Type &ty)
{
    return is_inttype(ty) || is_floattype(ty);
}

void Parser::createQuadFile()
{
    out.open(_of_name, std::ios::out | std::ios::binary);
    if (!out.is_open())
        error("Create file filed!");
}

std::string Parser::num2str(size_t num)
{
    std::string _mstr, _rstr;
    for (;num > 0;) {
        size_t m = num - 10 * (num / 10);
        num /= 10;
        _mstr.push_back(static_cast<char>(m + 48));
    }
    for (int i = _mstr.length() - 1; i >= 0; --i)
        _rstr.push_back(_mstr.at(i));

    return _rstr;
}

int Parser::str2int(std::string &str)
{
    int r = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        r = r * 10 + str.at(i) - 48;
    }
    return r;
}

std::string Parser::newLabel(const std::string &_l)
{
    static size_t counter = 1;
    std::string _rstr = ".L" + _l + num2str(counter++);
    return _rstr;
}


void Parser::pushQuadruple(const std::string &name)
{
    quad_arg_stk_.push_back(name);
}

std::string getReulst(std::string &v1, std::string &v2, const std::string &op)
{
    int _var1 = atoi(v2.c_str());
    int _var2 = atoi(v1.c_str());

    int r = 0;

    if (op == "+") r = _var1 + _var2;
    else if (op == "-") r = _var1 - _var2;
    else if (op == "*") r = _var1 * _var2;
    else if (op == "/") r = _var1 / _var2;
    else if (op == "%") r = _var1 % _var2;
    else if (op == "&") r = _var1 & _var2;
    else if (op == "|") r = _var1 | _var2;
    else if (op == "^") r = _var1 ^ _var2;

    return std::to_string(r);
}

void Parser::createUnaryQuadruple(const std::string &op)
{
    if (op == "++" || op == "--") {
        _GENQ2_(op, quad_arg_stk_.back());
        return;
    }

    if ((op == "-U" || op == "+U")) {

        // float constant
        std::string num = quad_arg_stk_.back();
        for (size_t i = 0;i < float_const.size(); ++i) {
            if (num == float_const.at(i)) {
                float_const.at(i - 1) = op.at(0) + float_const.at(i - 1);
                return;
            }
        }
        // integer constant
        if (isNumber(quad_arg_stk_.back())) {
            quad_arg_stk_.pop_back();
            num = "-" + num;
            quad_arg_stk_.push_back(num);
            return;
        }


        // other variable
        std::string lastarg = quad_arg_stk_.back();
        quad_arg_stk_.pop_back();
        quad_arg_stk_.push_back("0");
        quad_arg_stk_.push_back(lastarg);
        if (op == "-U")
            createQuadruple("-");
        else
            createQuadruple("+");
        return;
    }

    if (op == "~" && isNumber(quad_arg_stk_.back())) {
        int _n = atoi(quad_arg_stk_.back().c_str()); quad_arg_stk_.pop_back();
        _n = ~_n;
        quad_arg_stk_.push_back(std::to_string(_n));
        return;
    }

    std::string tempName = newLabel("uy");
    _GENQ3_(op, quad_arg_stk_.back(), tempName);

    quad_arg_stk_.pop_back();
    quad_arg_stk_.push_back(tempName);
}

void Parser::computeBoolExpr(const std::string &op)
{
    if (quad_arg_stk_.size() < 2)
        error("Compute " + op + " operand < 2.");

    std::string var1 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
    std::string var2 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();

    bool res = false;
    int op1 = atoi(var2.c_str());
    int op2 = atoi(var1.c_str());

    if (op == ">") res = op1 > op2;
    else if (op == "<")  res = op1 < op2;
    else if (op == ">=") res = op1 >= op2;
    else if (op == "<=") res = op1 <= op2;
    else if (op == "==") res = op1 == op2;
    else if (op == "!=") res = op1 != op2;
    else if (op == "&&") res = op1 && op2;
    else if (op == "||") res = op1 || op2;

    if (res)
        quad_arg_stk_.push_back("1");
    else
        quad_arg_stk_.push_back("0");
}

void Parser::createRelOpQuad(const std::string &op)
{
    int off = 1;
    BoolLabel *B = new BoolLabel();
    boolLabels_.push_back(B);
    B->trueList_ = makelist(quadStk_.size());
    B->falseList_ = makelist(quadStk_.size() + off + incDecStk_.size());
    std::string E1 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
    std::string E2 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
    _GENQ3_("if", E2 + " " + op + " " + E1, "goto");
    createIncDec();
    _GENQ1_("goto");

}

// + - * / % & | ^ 
// +f -f *f /f
void Parser::createQuadruple(const std::string &op)
{
    std::string _out_str = op;

    if (op == "=") {
        std::string v1 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
        std::string v2 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
        _GENQ3_(op, v1, v2);
    }
    else if (op == "+f" || op == "-f" || op == "*f" || op == "/f") {
        std::string v1, v2;

        v1 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
        if (isNumber(v1)) {
            float_const.push_back(v1);
            v1 = newLabel("f");
            float_const.push_back(v1);
            float_const.push_back("4f");
        }
        v2 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
        if (isNumber(v2)) {
            float_const.push_back(v2);
            v2 = newLabel("f");
            float_const.push_back(v2);
            float_const.push_back("4f");
        }

        std::string tempName = newLabel("var");
        quad_arg_stk_.push_back(tempName);

        _GENQ4_(op, v1, v2, tempName);
    }
    else if (op == ".=" || op == "[]=") {
        std::string v1 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
        std::string v2 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
        std::string v3 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();

        _GENQ4_(op, v1, v2, v3);
    }
    else {
        std::string v1, v2;

        v1 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();
        v2 = quad_arg_stk_.back(); quad_arg_stk_.pop_back();

        if (isNumber(v1) && isNumber(v2)) {
            quad_arg_stk_.push_back(getReulst(v1, v2, op));
            return;
        }

        std::string tempName = newLabel("var");
        quad_arg_stk_.push_back(tempName);
        _GENQ4_(op, v1, v2, tempName);
    }
}


void Parser::createFuncQuad(std::vector<Node *> &params)
{
    size_t callSize = 0;
    for (size_t i = 0; i < params.size(); ++i) {
        std::string type = std::to_string(params.at(params.size() - 1 - i)->getType().type);
        std::string size = std::to_string(params.at(params.size() - 1 - i)->getType().size_);
        _GENQ4_("param", quad_arg_stk_.back(), type, size);
        quad_arg_stk_.pop_back();
        callSize += params.at(i)->getType().size_;
    }

    std::string func_name;
    func_name = quad_arg_stk_.back();
#ifdef _OVERLOAD_
    func_name = getOverLoadName(func_name, params);
#endif // _OVERLOAD_

    Node fn = localenv->search(func_name);
    quad_arg_stk_.pop_back();

    for (size_t i = 0;i < fn.params.size(); ++i) {
        if (fn.params.at(i)->type_.getType() == ELLIPSIS) {
            goto _skip_cheak_params_num;
        }
    }
    if ((fn.kind_ != NODE_FUNC && fn.kind_ != NODE_FUNC_DECL) || (fn.params.size() != params.size()))
        errorp(ts_.getPos(), "func call parms size error.");

_skip_cheak_params_num:
    for (size_t i = 0; i < fn.params.size(); ++i) {
        localenv->call_size_ += fn.params.at(i)->type_.size_;
    }

    std::string ret_;
    if (fn.type_.type != K_VOID || fn.type_.retType != nullptr) {
        ret_ = newLabel("ret");
        quad_arg_stk_.push_back(ret_);
        _GENQ4_("call", fn.name(), std::to_string(params.size()), ret_);
    }
    else {
        _GENQ3_("call", fn.name(), std::to_string(params.size()));
    }
    if (maxCallSize_ < callSize)
        maxCallSize_ = callSize;
}

void Parser::createIncDec()
{
    for (;;) {
        if (incDecStk_.empty())
            return;

        _GENQ2_(incDecStk_.back().first, incDecStk_.back().second);
        incDecStk_.pop_back();
    }
}

Type Parser::getCustomType(const std::string &_n)
{
    std::map<std::string, Type>::iterator iter = custom_type_tbl.find(_n);
    if (!(iter == custom_type_tbl.end())) {
        return iter->second;
    }
    return Type();
}

#ifdef _OVERLOAD_
std::string Parser::getOverLoadName(const std::string &name, std::vector<Node> &_p)
{
    if (name == "main")
        return name;
    else if (name == "printf")
        return name;
    else if (name == "puts")
        return name;
    else if (name == "putchar")
        return name;

    std::string _name_r;
    _name_r += name + "@";

    for (size_t i = 0; i < _p.size(); ++i) {
        switch (_p.at(i).type.type)
        {
        case K_INT: _name_r += "i";break;
        case K_CHAR: _name_r += "c";break;
        case K_SHORT:_name_r += "s";break;
        case K_LONG: _name_r += "l";break;
        case K_FLOAT: _name_r += "f";break;
        case K_DOUBLE: _name_r += "d";break;
        case K_STRUCT:
        case K_TYPEDEF:
            errorp(lex.getPos(), "Unspport struct and typedef overload.");
            break;
        default:
            errorp(lex.getPos(), "Unspport type overload.");
            break;
        }
    }
    return _name_r;
}
#endif
std::vector<int> *Parser::makelist(int index)
{
    std::vector<int> *list = new std::vector<int>();
    list->push_back({ index });
    return list;
}

std::vector<int> *Parser::merge(
    std::vector<int> *p1, 
    std::vector<int> *p2)
{
    std::vector<int> *list = new std::vector<int>();
    
    list->insert(list->end(), p1->begin(), p1->end());
    list->insert(list->end(), p2->begin(), p2->end());
    return list;
}

void Parser::backpatch(std::vector<int> *p, int index)
{
    for (const auto &i : *p) {
        Quadruple &quad = quadStk_.at(i);
        if (std::get<0>(quad) == "if") {
            std::get<3>(quad) = std::get<0>(quadStk_.at(index-1));
        }
        else if (std::get<0>(quad) == "goto") {
            std::get<1>(quad) = std::get<0>(quadStk_.at(index- 1));
        }
    }
}

void Parser::out_quad() {
    for (const auto &q : quadStk_) {
        if (std::get<1>(q) == ":") {
            out << std::get<0>(q) << ":" << std::endl;
            continue;
        }
        out << "\t" << std::left << std::setw(7) << std::get<0>(q) << " "
            << std::left << std::setw(10) << std::get<1>(q) << " "
            << std::left << std::setw(10) << std::get<2>(q) << " "
            << std::get<3>(q)
            << std::endl;
    }
}