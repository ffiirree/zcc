#include "parser.h"
#include "error.h"

#include<iomanip>
/**
 * @berif 一个翻译单元，一个文件
 * 
 */
std::vector<Node> Parser::trans_unit()
{
	std::vector<Node> list;
	for (;;) {
		Token t = lex.peek();

		if (t.getType() == K_EOF) {
			labels.cheak();
			_log_("End of file.");
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
	if (n.kind == NODE_GLO_VAR)
		_log_("Add glo var : %s.", n.varName.c_str());
	else if (n.kind == NODE_LOC_VAR)
		_log_("Add loc var : %s.", n.varName.c_str());
	else if (n.kind == NODE_FUNC)
		_log_("Add function name : %s.", n.funcName.c_str());


	if (n.kind == NODE_FUNC) {
		Node r = search(n.funcName);
		if (r.kind == NODE_FUNC_DECL) {
			set(n.funcName, NODE_FUNC, n.body);
			return;
		}
		else if (r.kind != 0) {
			error("function redefined.");
		}
	}

	if (n.kind == NODE_GLO_VAR) {
		Node r = search(n.varName);
		if (r.kind != 0)
			error("redefined var.");
	}

	nodes.push_back(n);
}
void Label::push_back(const std::string &_l) {

	// 如果添加过了
	for (size_t i = 0; i < labels.size(); ++i) {
		if (_l == labels.at(i)) {
			if (enLabels.at(i) == false) {
				enLabels.at(i) = true;
				return;
			}
			else {
				error("%s is existed.", _l.c_str());
				return;
			}
		}
	}

	// 从未添加过
	labels.push_back(_l);
	enLabels.push_back(true);
}

bool Label::cheak()
{
	for (size_t i = 0; i < labels.size(); ++i) {
		if (!enLabels.at(i))
			error("Label '%s' is undefined.", labels.at(i).c_str());
	}
	return true;
}


/**
 * @berif 检查是否是函数定义
 */
bool Parser::isFuncDef()
{
	int count = 0;
	Token t;

	// 返回类型
	do {
		t = lex.next();
		count++;
	} while (is_type(t) || t.getId() == '*');
	lex.back(); count--;

	for (;;) {
		t = lex.next();
		count++;
		if (is_keyword(t, '(')) {
			skip_parenthesis(&count);
			if (next_is('{')) {
				count++;
				goto _end;
			}
		}
		else if (t.getType() == ID) {
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
				lex.back();
			return false;
		}
	}

_end:
	for (int i = 0; i < count; ++i)
		lex.back();
	return true;
}

/**
 * @berif 函数定义
 */
Node Parser::funcDef()
{
	int current_class = 0;                                                  // static ...
	std::string funcName;                                                   // 函数名字
	std::vector<Node> params;

	__IN_SCOPE__(localenv, globalenv);

	Type *retty = new Type(decl_spec_opt(&current_class));                             // 获取函数的返回类型
	Type functype = declarator(retty, funcName, params, FUNC_BODY);         // 函数定义类型，函数描述
	if (functype.type == PTR) {
		error("Ptr not can be function.");
	}

	out << funcName << ":" << std::endl;

	functype.setStatic(current_class == K_STATIC);                          // 函数是否是static
	expect('{');
	Node r = func_body(functype, funcName, params);                         // 函数体

	__OUT_SCOPE__(localenv, funcName);
	out << ".end" << std::endl;
	return r;
}



Type Parser::func_param_list(Type *retty, std::vector<Node> &params, int deal_type)
{
	// foo()
	if (next_is(')')) {
		return Type(NODE_FUNC, retty, params);
	}
	// foo(void)
	else if (is_keyword(lex.peek(), K_VOID)) {
		lex.next();
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

std::vector<Node> Parser::param_list(int decl_type)
{
	std::vector<Node> list;
	list.push_back(param_decl(decl_type));

	while (next_is(','))
		list.push_back(param_decl(decl_type));
	return list;
}

Node Parser::param_decl(int decl_type)
{
	int sclass = 0;
	Type basety = decl_specifiers(&sclass);
	std::string paramname;
	std::vector<Node> list;
	Type type = declarator(&basety, paramname, list, NODE_PARAMS);

	if (decl_type == NODE_FUNC_DECL)
		return createFuncDeclParams(type);

	return createLocVarNode(type, paramname);
}


Node Parser::func_body(Type &functype, std::string name, std::vector<Node> &params)
{
	Node body = compound_stmt();
	return  createFuncNode(functype, name, params, &body);
}



Node &Env::search(std::string &key)
{
	Env *ptr = this;

	while (ptr) {
		for (size_t i = 0; i < ptr->nodes.size(); ++i) {
			if (key == ptr->nodes.at(i).varName || key == ptr->nodes.at(i).funcName)
				return ptr->nodes.at(i);
		}
		ptr = ptr->pre();
	}

	Node *r = new Node(NODE_NULL);

	return *r;
}

void Env::set(std::string &_name, int ty, Node *_body)
{
	Env *ptr = this;

	while (ptr) {
		for (size_t i = 0; i < ptr->nodes.size(); ++i) {
			if (_name == ptr->nodes.at(i).varName || _name == ptr->nodes.at(i).funcName) {
				ptr->nodes.at(i).kind = ty;
				ptr->nodes.at(i).body = _body;
			}
		}
		ptr = ptr->pre();
	}
}

/**
* 从当前作用域开始查找标识符
*/
Type Parser::get_type(std::string key)
{
	Type type;

	while (!localenv) {
		type = localenv->search(key).getType();
	}
	return type;
}
int Parser::get_compound_assign_op(Token &t)
{
	if (t.getType() != KEYWORD)
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

std::string Parser::get_compound_assign_op_signal(Token &t)
{
	if (t.getType() != KEYWORD)
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
	if (lex.next().getId() == id) 
		return true;
	lex.back();
	return false;
}

Node Parser::createIntNode(Token &t, int size, bool isch)
{
	if (isch) {
		_log_("Create char_ node.");
		Node node(NODE_CHAR);

		node.int_val = t.getCh();
		node.type = Type(K_CHAR, size, false);
		return node;
	}
	else {
		_log_("Create int_ node.");
		Node node(NODE_INT);

		node.int_val = atoi(t.getSval().c_str());
		node.type = Type(K_INT, size, false);
		return node;
	}
}


Node Parser::createIntNode(Type &ty, int val)
{
	_log_("Create int node.");
	Node node(NODE_INT);
	node.int_val = val;
	node.type = ty;
	return node;
}

Node Parser::createFloatNode(Type &ty, double val)
{
	_log_("Create float node.");
	Node node(NODE_DOUBLE);

	node.type = ty;
	node.float_val = val;
	return node;
}


Node Parser::createFloatNode(Token &t)
{
	_log_("Create float node.");
	Node node(NODE_DOUBLE);

	node.sval = t.getSval();
	node.type = Type(K_FLOAT, 4, false);
	return node;
}

Node Parser::createStrNode(Token &t)
{
	_log_("Create STR node.");
	Node node(NODE_STRING);

	node.sval = t.getSval();
	return node;
}

Node Parser::createFuncNode(Type &ty, std::string & funcName, std::vector<Node> params, Node *body)
{
	_log_("Create function node.");

	Node node(NODE_FUNC, ty);
	node.funcName = funcName;
	node.params = params;
	node.body = body;

	globalenv->push_back(node);

	return node;
}

Node Parser::createFuncDecl(Type &ty, std::string & funcName, std::vector<Node> params)
{
	_log_("Create function decl node.");

	Node node(NODE_FUNC_DECL, ty);
	node.funcName = funcName;
	node.params = params;

	globalenv->push_back(node);

	return node;
}



Node Parser::createCompoundStmtNode(std::vector<Node> &stmts)
{
	_log_("Create compound stmt node.");

	Node node(NODE_COMP_STMT);
	node.stmts = stmts;
	return node;
}
Node Parser::createDeclNode(Node &var)
{
	_log_("Create decl node.");

	Node node(NODE_DECL);
	node.decl_var = &var;
	return node;
}
Node Parser::createDeclNode(Node &var, std::vector<Node> &init)
{
	_log_("Create decl node with init.");

	Node node(NODE_DECL);
	node.decl_var = &var;
	node.decl_init = init;

	if (var.kind == NODE_GLO_VAR) {
		globalenv->back().lvarinit = init;
	}
	else if (var.kind == NODE_LOC_VAR) {
		localenv->back().lvarinit = init;
	}

	return node;
}

Node Parser::createGLoVarNode(Type &ty, std::string name)
{
	_log_("Create glo var node.");

	Node r(NODE_GLO_VAR, ty);
	r.varName = name;

	globalenv->push_back(r);

	return r;
}
Node Parser::createLocVarNode(Type &ty, std::string name)
{
	_log_("Create loc var node.");

	Node r(NODE_LOC_VAR, ty);
	r.varName = name;
	localenv->push_back(r);

	return r;
}

Node Parser::createFuncDeclParams(Type &ty)
{
	_log_("Create func dec param node.");

	Node r(NODE_DECL_PARAM, ty);

	return r;
}

Node Parser::createBinOpNode(Type &ty, int kind, Node *left, Node *right)
{
	_log_("Create binop %c node.", static_cast<char>(kind));

	Node r(kind, ty);
	r.left = left;
	r.right = right;
	return r;
}

Node Parser::createUnaryNode(int kind, Type &ty, Node &node)
{
	_log_("Create unary op %c node.", static_cast<char>(kind));

	Node r(kind);
	r.type = ty;
	r.operand = &node;
	return r;
}

Node Parser::createRetStmtNode(Node *n)
{
	_log_("Create return stmt node.");

	Node r(NODE_RETURN);
	r.retval = n;
	return r;
}

Node Parser::createJumpNode(std::string label)
{
	_log_("Create Jump node.");

	Node r(NODE_GOTO);
	r.label = label;
	r.newLabel = label;
	return r;
}

Node Parser::createIfStmtNode(Node *cond, Node *then, Node *els)
{
	_log_("Create if stmt.");

	Node r(NODE_IF_STMT);
	r.cond = cond;
	r.then = then;
	r.els = els;
	return r;
}


bool Parser::is_type(const Token &t)
{
	if (t.getType() != KEYWORD)
		return false;

	switch (t.getId())
	{
#define keyword(id, _, is) case id: return is;
		KEYWORD_MAP
#undef keyword
	default: return false;
	}
}

/**
* 检查是否是关键字id
*/
bool Parser::is_keyword(Token &t, int id)
{
	return (t.getType() == KEYWORD && t.getId() == id);
}

void Parser::skip_parenthesis(int *count)
{
	for (;;)
	{
		Token t = lex.next();
		(*count)++;

		if (t.getType() == K_EOF)
			error("error eof");

		if (is_keyword(t, '('))
			skip_parenthesis(count);

		if (is_keyword(t, ')'))
			break;
	}
}
void Parser::expect(int id)
{
	Token t = lex.next();
	if (t.getId() != id)
		error("expect '%c', but not is '%c'", id, t.getId());
}

bool Parser::is_inttype(Type &ty)
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
bool Parser::is_floattype(Type &ty)
{
	switch (ty.getType())
	{
	case K_FLOAT: case K_DOUBLE:
		return true;
	default: return false;
	}
}


bool Parser::is_arithtype(Type &ty)
{
	return is_inttype(ty) || is_floattype(ty);
}

std::string Parser::getQuadrupleFileName()
{
	std::string _fn = lex.getCurrentFile().getFileName();
	size_t _index_separator = 0;
	size_t _index_dot = 0;
	for (size_t i = 0; i < _fn.length(); ++i) {
		if (_fn.at(i) == '/' || _fn.at(i) == '\\')
			_index_separator = i + 1;
		if (_fn.at(i) == '.')
			_index_dot = i;
	}
	if (_index_dot <= _index_separator && _fn.length() > 0) _index_dot = _fn.length() - 1;
	
	std::string _rfn;
	for (size_t i = _index_separator; i < _index_dot; ++i)
		_rfn.push_back(_fn.at(i));

	_rfn.push_back('.');
	_rfn.push_back('q');
	_rfn.push_back('d');
	return _rfn;
}


std::string Parser::getQuadrupleFileName(std::string &filename)
{
	return filename;
}

void Parser::createQuadFile()
{
	out.open(getQuadrupleFileName(), std::ios::out | std::ios::binary);
	if (!out.is_open())
		error("Create file filed!");
}

std::string Parser::num2str(size_t num)
{
	std::string _mstr, _rstr;
	for (;num > 0;) {
		size_t m = num - 10 * (num / 10);
		num /= 10;
		_mstr.push_back(m + 48);
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
	_stk_quad.push_back(name);
}

void Parser::pushIncDec(const std::string &name)
{
	_stk_incdec.push_back(name);
}

void Parser::gotoLabel(const std::string &op)
{
	BoolLabel _b = boolLabel.back(); boolLabel.pop_back();
	if(op == "||")
		out << _b._false << ":" << std::endl;  // label(b1.false)
	else if(op == "&&")
		out << _b._true << ":" << std::endl;  // label(b1.false)
	else if(op == "if")
		out << _b._true << ":" << std::endl;  // label(b1.false)
}

void Parser::generateIfGoto()
{
	BoolLabel b, b1, b2;
	std::vector<std::string>  _temp;

	if (_stk_if_goto.size() == 1) {
		b = boolLabel.back();boolLabel.pop_back();
		_stk_if_goto_out.push_back(_stk_if_goto.back() + b._true);
		_stk_if_goto_out.push_back("goto " + b._false);
		_stk_if_goto.pop_back();
		goto _gen_end;
	}

	for (int i = _stk_if_goto.size(); i > 0; --i) {
		_temp.push_back(_stk_if_goto.back());_stk_if_goto.pop_back();
	}

	for (int i = _stk_if_goto_op.size(); i > 0; --i) {
		std::string op = _stk_if_goto_op.back(); _stk_if_goto_op.pop_back();
		b = boolLabel.back();boolLabel.pop_back();
		b2 = boolLabel.back();boolLabel.pop_back();
		b1 = boolLabel.back();boolLabel.pop_back();
		if (op == "||") {
			b1._true = b._true;
			b1._false = newLabel("orf");
			b2._true = b._true;
			b2._false = b._false;
		}
		else if (op == "&&") {
			b1._true = newLabel("andt");
			b1._false = b._false;
			b2._true = b._true;
			b2._false = b._false;
		}

		if (b1._leaf) {
			_stk_if_goto_out.push_back(_temp.back() + b1._true);
			_stk_if_goto_out.push_back("goto " + b1._false);
			_temp.pop_back();

			if (op == "||")
				_stk_if_goto_out.push_back(b1._false + ":");
			else if (op == "&&")
				_stk_if_goto_out.push_back(b1._true + ":");
		}

		if (b2._leaf) {
			_stk_if_goto_out.push_back(_temp.back() + b2._true);
			_stk_if_goto_out.push_back("goto " + b2._false);
			_temp.pop_back();
		}

		

		if(!b2._leaf)
			boolLabel.push_back(b2);

		if (!b1._leaf)
			boolLabel.push_back(b1);
	}

_gen_end:
	for (size_t i = 0; i < _stk_if_goto_out.size(); ++i) {
		out << _stk_if_goto_out.at(i) << std::endl; 
	}
	_stk_if_goto_out.clear();
}

void Parser::createBoolGenQuadruple(const std::string &op)
{
	BoolLabel _b;
	_b._leaf = true;
	//_b._true = newLabel("true");
	//_b._false = newLabel("false");
	boolLabel.push_back(_b);


	std::string str;
	std::string var1 = _stk_quad.back(); _stk_quad.pop_back();
	std::string var2 = _stk_quad.back(); _stk_quad.pop_back();

	str = "if\t" + var2 + " " + op + " " + var1 + "\tgoto ";
	_stk_if_goto.push_back(str);
}

void Parser::createBoolQuadruple(const std::string &op)
{
	BoolLabel b1, b2;
	if (boolLabel.empty())
		return;

	if (op == "||") {
		b1._true = boolLabel.back()._true;
		b1._false = newLabel("orf");
		b2._true = boolLabel.back()._true;
		b2._false = boolLabel.back()._false;

		boolLabel.push_back(b2);
		boolLabel.push_back(b1);
	}
	else if (op == "&&") {
		b1._true = newLabel("andt");
		b1._false = boolLabel.back()._false;
		b2._true = boolLabel.back()._true;
		b2._false = boolLabel.back()._false;

		boolLabel.push_back(b2);
		boolLabel.push_back(b1);
	}
}

std::string getReulst(std::string &v1, std::string &v2, const std::string &op)
{
	// 注意入栈出栈的顺序
	int _var1 = atoi(v2.c_str());
	int _var2 = atoi(v1.c_str());

	int r = 0;

	if (op == "+") {
		r = _var1 + _var2;
	}
	else if (op == "-") {
		r = _var1 - _var2;
	}
	else if (op == "*") {
		r = _var1 * _var2;
	}
	else if (op == "/") {
		r = _var1 / _var2;
	}
	else if (op == "%") {
		r = _var1 % _var2;
	}
	else if (op == "&") {
		r = _var1 & _var2;
	}
	else if (op == "|") {
		r = _var1 | _var2;
	}
	else if (op == "^") {
		r = _var1 ^ _var2;
	}

	return std::to_string(r);
}

void Parser::createUnaryQuadruple(const std::string &op)
{
	if (op == "++" || op == "--") {
		out << op + "\t" + _stk_quad.back() << std::endl;
		return;
	}

	std::string _out_str;
	_out_str = op + "\t" + _stk_quad.back(); _stk_quad.pop_back();

	std::string tempName = newLabel("uy");
	_out_str += "\t" + tempName;

	// 添加到生成四元式的栈中
	_stk_quad.push_back(tempName);

	out << _out_str << std::endl;
}

// + - * / % & | ^ 
// 不进行 
// 赋值运算和二元操作符
void Parser::createQuadruple(const std::string &op)
{
	std::string _out_str = op;
	
	if (op == "=") {
		_out_str += "\t" + _stk_quad.back(); _stk_quad.pop_back();
		_out_str += "\t" + _stk_quad.back(); _stk_quad.pop_back();
	}
	else {
		std::string v1, v2;
		
		v1 = _stk_quad.back(); _stk_quad.pop_back();
		v2 = _stk_quad.back(); _stk_quad.pop_back();

		// 如果两个参数都是常量那么，优化掉
		if (isNumber(v1) && isNumber(v2)) {
			_stk_quad.push_back(getReulst(v1, v2, op));
			return;
		}

		// 如果不全是常量
		_out_str += "\t" + v1;
		_out_str += "\t" + v2;

		std::string tempName = newLabel("var");
		_out_str += "\t" + tempName;

		// 添加到生成四元式的栈中
		_stk_quad.push_back(tempName);
	}

	out << _out_str << std::endl;
}

/**
 * @berif 生成函数调用四元式
 */
void Parser::createFuncQuad(std::vector<Node> &params)
{
	out << std::endl;
	for (size_t i = 0; i < params.size(); ++i) {
		out << "param " << _stk_quad.back() << std::endl; _stk_quad.pop_back();
	}

	Node fn = localenv->search(_stk_quad.back());
	_stk_quad.pop_back();

	// 检查参数个数
	if ((fn.kind != NODE_FUNC && fn.kind != NODE_FUNC_DECL) || (fn.params.size() != params.size()))
		error("func call parms size error.");

	for (size_t i = 0; i < fn.params.size(); ++i) {
		localenv->_call_size += fn.params.at(i).type.size;
	}

	out << std::left << std::setw(15) << "call" << std::left << std::setw(15) << fn.funcName  << fn.params.size() << std::endl;
	_stk_quad.push_back("%eax");
}

void Parser::createIncDec()
{
	for (;;) {
		if (_stk_incdec.empty())
			return;

		std::string op = _stk_incdec.back(); _stk_incdec.pop_back();
		std::string var = _stk_incdec.back(); _stk_incdec.pop_back();
		std::string label;
		if (op == "++") {
			label = newLabel("inc");
			out << std::left << std::setw(15) << "+";
		}
		else {
			label = newLabel("dec");
			out << std::left << std::setw(15) << "-";
		}

		out << std::left << std::setw(15) << var;
		out << std::left << std::setw(15) << "1";
		out << std::left << std::setw(15) << label << std::endl;
		out << std::left << std::setw(15) << "=";
		out << std::left << std::setw(15) << label;
		out << std::left << std::setw(15) << var << std::endl;
	}
}