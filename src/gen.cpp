#include<iomanip>
#include "gen.h"
#include "type.h"
#include "error.h"


Generate::Generate(Parser *p)
{
	parser = p;
	_infilename = p->getQuadrupleFileName();

	inf.open(_infilename);
	
	out.open(getOutName());
	if (!out.is_open())
		error("Open file failed.");

	reg_init();

	out << "\t.file\t\"" << _infilename << "\""<< std::endl;


	const_str();
	Env *gloenv = parser->getGloEnv();
	for (size_t i = 0; i < gloenv->size(); ++i) {
		Node n = gloenv->at(i);
		if (n.kind == NODE_GLO_VAR && n.params.empty())
			var_decl(n);
	}
}

/**
 * @berif 寄存器初始化
 */
void Generate::reg_init()
{
	// 通用寄存器 //32bits
	universReg.push_back(Reg("%eax"));
	universReg.push_back(Reg("%ebx"));
	universReg.push_back(Reg("%ecx"));
	universReg.push_back(Reg("%edx"));

	// 段寄存器 // 16bits
	segReg.push_back(Reg("%ecs"));
	segReg.push_back(Reg("%eds"));
	segReg.push_back(Reg("%ees"));
	segReg.push_back(Reg("%ess"));
}

/**
 * @berif 生成汇编代码
 */
void Generate::run()
{
	for (;;) {
		std::vector<std::string> quad = getQuad();

		if (quad.empty())
			break;
		generate(quad);
	}
	out << parser->newLabel("FE") << ":" << std::endl; // file end
	out << "\t.ident \"zcc 0.0.1\"" << std::endl;
}

/**
 * @berif 处理字符串常量
 */
void Generate::const_str()
{
	if (parser->getStrTbl().empty())
		return;

	out << "\t.section  .rdata,\"dr\"" << std::endl;
	std::vector<StrCard> strTbl = parser->getStrTbl();
	for (size_t i = strTbl.size(); i > 0; --i) {
		out << strTbl.back()._label << ":" << std::endl;
		out << "\t.ascii \"" << strTbl.back()._str << "\\0\"" << std::endl;
		strTbl.pop_back();
	}
}

/**
 * @berif 处理全局变量
 */
void Generate::var_decl(Node &n)
{
	// 如果初始化为0，则为声明
	if (n.lvarinit.empty()) {
		glo_var_decl(n);
	}
	// 否则为定义
	else {
		glo_var_define(n);
	}
}

/**
 * @breif 处理声明的全局变量
 */
void Generate::glo_var_decl(Node &n)
{
	out << "\t.comm\t" << "_" + n.varName << ", " << n.type.size << ", " << n.type.size/2 <<std::endl << std::endl;
}

/**
 * @berif 全局变量定义
 */
void Generate::glo_var_define(Node &n)
{
	out << "\t.globl\t" << "_" + n.varName << std::endl;
	out << "\t.data" << std::endl;
	if (n.type.getType() == ARRAY)
		out << "\t.align\t" << n.type.len * 4 << std::endl;
	else
		out << "\t.align\t" << n.type.size << std::endl;
	out << "_" + n.varName << ":" << std::endl;
	if (n.type.getType() == ARRAY) {
		size_t i = 0;
		for (; i < n.lvarinit.size(); ++i) {
			out << getTypeString(n) << n.lvarinit.at(i).int_val << std::endl;
		}
		out << "\t.space\t" << (n.type.len - i) * n.type.size << std::endl;
	}
	else
		out << getTypeString(n) << n.lvarinit.at(0).int_val << std::endl << std::endl;
}

/**
 * @获取节点的数据类型
 */
std::string Generate::getTypeString(Node &n)
{
	switch (n.kind) {
	case NODE_CHAR:
		return "\t.byte\t";
	case NODE_SHORT:
		return "\t.word\t";
	case NODE_LONG:
		return "\t.long\t";
	default:
		return "\t.long\t";
	}
}

/**
 * @berif 获取函数局部变量的大小，以此来分配栈空间的大小
 */
int Generate::getFuncLocVarSize(Node &n)
{
	int _rsize = -1;
	std::vector<Env *> gloEnv = parser->getGloEnv()->getNext();
	for (size_t i = 0; i < gloEnv.size(); ++i) {
		Env * next = gloEnv.at(i);
		if (n.funcName == next->getName()) {
			getEnvSize(next, _rsize);
		}
	}
	return _rsize;
}
/**
 * @berif 获取该作用域中的局部变量的大小，并建立局部变量在栈中的位置关系
 */
void Generate::getEnvSize(Env *_begin, int &_size)
{
	if (_begin == nullptr)
		return;

	bool is_params = false;
	int params_pos = 8;
	if (_size == -1) {
		is_params = true;
		_size = 0;
	}

	for (size_t i = 0; i < _begin->size(); ++i) {
		int t = _begin->at(i).type.size;
		
		// 保存局部变量
		locvar.push_back(Locvar(_begin->at(i).varName, t));

		// 处理参数在栈中的位置，栈底ebp
		if (is_params) {
			locvar.back()._pos = params_pos;
			params_pos += t;
		}
		else {
			_size += t;
			locvar.back()._pos = -_size;
		}

		// 变量的初值
		if (!_begin->at(i).lvarinit.empty()) 
			locvar.back()._lvalue = _begin->at(i).lvarinit;
	}
	std::vector<Env *> node = _begin->getNext();
	for (size_t i = 0; i < node.size(); ++i) {
		getEnvSize(node.at(i), _size);
	}
}

void getEnvCallSize(Env * _begin, int *size)
{
	if (_begin == nullptr)
		return;
	*size += _begin->_call_size;
	std::vector<Env *> node = _begin->getNext();
	for (size_t i = 0; i < node.size(); ++i) {
		getEnvCallSize(node.at(i), size);
	}
}

int Generate::getFuncCallSize(Node &n)
{
	int _rsize = 0;
	
	std::vector<Env *> gloEnv = parser->getGloEnv()->getNext();
	for (size_t i = 0; i < gloEnv.size(); ++i) {
		Env * next = gloEnv.at(i);
		// 找到该函数
		if (n.funcName == next->getName()) {
			getEnvCallSize(next, &_rsize);
			return _rsize;
		}
	}
	return 0;
}




void Generate::func_decl(Node &n)
{
	locvar.clear();
	int size = getFuncLocVarSize(n);            // 获取临时变量的 大小
	size += getFuncCallSize(n);

	if (n.funcName == "main") {
		out << "\t.def\t__main;\t.scl\t2;\t.type\t32;\t.endef" << std::endl;
		is_main = true;
	}
	out << "\t.text" << std::endl;
	out << "\t.globl\t" << "_" << n.funcName << std::endl;
	out << "\t.def\t" << "_" << n.funcName << ";\t.scl\t2;\t.type\t32;\t.endef" << std::endl;
	out << "_" + n.funcName << ":" << std::endl;
	out << "\t.cfi_startproc" << std::endl;
	out << "\tpushl	%ebp" << std::endl;
	out << "\t.cfi_def_cfa_offset 8" << std::endl;
	out << "\t.cfi_offset 5, -8" << std::endl;
	out << "\tmovl	%esp, %ebp" << std::endl;
	out << "\t.cfi_def_cfa_register 5" << std::endl;
	if (size > 0)
		out << "\tsubl\t$" << (size >= 16 ? size : 16) << ", %esp" << std::endl;
	if (is_main) {
		//out << "\tandl	$ - 16, %esp" << std::endl;  // 对齐，可以不要
		out << "\tcall	___main" << std::endl;
	}
}

/**
 * \ 32bits: 通用寄存器
 *    eax 主累加器
 *    ebx 累加器和基址寄存器
 *    ecx 累加器和计数器
 *    edx 累加器和I/O地址寄存器
 *
 * \ 32bits: 指针和变址寄存器
 *    esp 堆栈指针
 *    ebp 基址指针
 *    esi 源变址寄存器
 *    edi 目标变址寄存器
 *
 * \ 16bits: cs ss ds es fs gs
 * \ 
 * \ movl 4B
 * \ movw 2B
 * \ movb 1B
 */
void Generate::generate(std::vector<std::string> &_q)
{
#define _q_0_is(str) (_q.at(0) == str)
	// 标签直接输出
	if (_q.size() == 2 && _q.at(1) == ":") {
		Node r = parser->getGloEnv()->search(_q.at(0));

		if (r.kind == NODE_FUNC) {
			currentFunc = r;
			func_decl(r);
			return;
		}

		out << _q.at(0) << ":" << std::endl;
	}
	else if (_q_0_is("=")) {
		// 赋值中第二个一般为局部变量或全局变量
		Locvar var = searchLocvar(_q.at(2));
		Locvar tvar = searchTempvar(_q.at(1));

		std::string _out;

		if (_q.at(1) == "%eax") {
			_out = "%eax";
			goto _ass_end;
		}
		// 第一个参数为数字
		if (isNumber(_q.at(1))) {
			_out = "$" + _q.at(1);
			goto _ass_end;
		}
		// 或为临时变量
		
		if (!tvar._var.empty()) {
			_out = tvar._reg;
		}

_ass_end:
		if (var._size == 1) {
			out << "\tmovb\t" << _out + ", "  + std::to_string(var._pos) << "(%ebp)" << std::endl;
			return;
		}
		else if (var._size == 2) {
			out << "\tmovw\t" << _out + ", "  + std::to_string(var._pos) << "(%ebp)" << std::endl;
			return;
		}
		else if (var._size == 4) {
			out << "\tmovl\t" << _out + ", "  + std::to_string(var._pos) << "(%ebp)" << std::endl;
			return;
		}
	}
	else if (_q_0_is("if")) {
		// 注意出栈和入栈的顺序
		std::string _q1 = _q.at(3);
		std::string _q2 = _q.at(1);

		std::string _q1_reg;
		std::string _q2_reg;

		_q1_reg = getQuadReg(_q1);
		_q2_reg = getQuadReg(_q2);

		out << "\tcmpl\t" << _q1_reg + ", " + _q2_reg << std::endl;

		if (_q.at(2) == ">")
			out << "\tjg\t" << _q.at(5) << std::endl;
		else if(_q.at(2) == "<")
			out << "\tjl\t" << _q.at(5) << std::endl;
		else if (_q.at(2) == ">=")
			out << "\tjge\t" << _q.at(5) << std::endl;
		else if (_q.at(2) == "<=")
			out << "\tjle\t" << _q.at(5) << std::endl;
		else if (_q.at(2) == "==")
			out << "\tje\t" << _q.at(5) << std::endl;
		else if (_q.at(2) == "!=")
			out << "\tjne\t" << _q.at(5) << std::endl;

		// 清理立即数
		clearRegConst();

		// 先将临时变量和常数出栈
		pop_back_temp_stk(_q1);
		pop_back_temp_stk(_q2);
	}
	else if (_q_0_is("goto")) {
		out << "\tjmp\t" << _q.at(1) << std::endl;
	}
	else if (_q_0_is("param")) {
		params.push_back(_q.at(1));
	}
	else if (_q_0_is("call")) {
		std::string funcName = _q.at(1);

		Node func = parser->getGloEnv()->search(funcName);

		int pos = 0;
		for (size_t i = 0; i < func.params.size(); ++i) {
			size_t param_size = func.params.at(i).type.size;

			std::string _out_str;
			Locvar _loc = searchLocvar(params.back());
			if (!_loc._var.empty()) {
				getReg(std::string("%eax"));
				out << "\tmovl\t" + std::to_string(_loc._pos) + "(%ebp), %eax" << std::endl;
				_out_str = "%eax";
			}
			else if(isNumber(params.back())){
				_out_str = "$" + params.back();
			}
			else if(isTempVar(params.back())){
				Locvar _te = searchTempvar(params.back());
				_out_str = _te._reg;
			}

			if (param_size == 1) {
				out << "\tmovb\t" << _out_str << ", " << pos<< "(%esp)" << std::endl;
				pos += 1;
			}
			else if(param_size == 2){
				out << "\tmovw\t" << _out_str << ", " << pos << "(%esp)" << std::endl;
				pos += 2;
			}
			else if (param_size == 4) {
				if(func.params.at(i).getType().getType() == PTR)
					out << "\tmovl\t$" << params.back() << ", " << pos << "(%esp)" << std::endl;
				else 
					out << "\tmovl\t" << _out_str << ", " << pos << "(%esp)" << std::endl;
				pos += 4;
			}
			params.pop_back();
		}

		out << "\tcall\t" << "_" + _q.at(1) << std::endl;
	}
	else if (_q_0_is("ret")) {
		if (is_main) is_main = false;
		std::string _out;
		// 获取函数返回类型
		int size = currentFunc.type.retType->getSize();

		// 查找变量
		// !ERROR 还有可能是全局变量
		Locvar ret = searchLocvar(_q.at(1));

		if (ret._var.empty())
			_out = "$" + _q.at(1);
		else
			_out = std::to_string(ret._pos) + "(%ebp)";
		switch (size) {
		case 1: out << "\tmovb\t" << _out << ", %eax" << std::endl; break;
		case 2: out << "\tmovw\t" << _out << ", %eax" << std::endl; break;
		case 4: out << "\tmovl\t" << _out << ", %eax" << std::endl; break;
		}

		out << "\tleave" << std::endl;
		out << "\t.cfi_restore 5" << std::endl;
		out << "\t.cfi_def_cfa 4, 4" << std::endl;
		out << "\tret" << std::endl;
	}
	else if (_q_0_is(".end")) {
		out << "\t.cfi_endproc" << std::endl << std::endl;
	}
	// op
	else{
		getReg(_q);
	}
}
#undef _q_0_is
//void genAsm()


std::string Generate::getQuadReg(const std::string &_q)
{
	std::string _reg;

	// 立即数直接加载到内存
	if (isNumber(_q)) {
		for (size_t i = 0; i < universReg.size(); ++i) {
			if (universReg.at(i)._var.empty()) {
				_reg = universReg.at(i)._reg;
				universReg.at(i)._var = _q;
				out << "\tmovl\t$" + _q + ", " << _reg << std::endl;
				return _reg;
			}
		}
	}
	else {
		// 局部变量不会保存在寄存器中
		Locvar _loc = searchLocvar(_q);
		if (!_loc._var.empty()) {
			for (size_t i = 0; i < universReg.size(); ++i) {
				if (universReg.at(i)._var.empty()) {
					_reg = universReg.at(i)._reg;
					universReg.at(i)._var = _q;
					out << "\tmovl\t" + std::to_string(_loc._pos) + "(%ebp), " << _reg << std::endl;
					return _reg;
				}
			}
		}

		// 在表达式栈中寻找，临时变量一定先作为结果出现，也就是出现过的一定在寄存器中
		Locvar _tem = searchTempvar(_q);
		if (_tem._var.empty()) {
			error("Temp var is not found.");
		}
		return _tem._reg;
	}
	return std::string();
}

/**
 * 如果寄存器里存储的是常量，删除
 */
void Generate::clearRegConst()
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (isNumber(universReg.at(i)._var))
			universReg.at(i)._var.clear();
	}
}

/**
 * var删除
 */
void Generate::clearRegTemp(std::string &var)
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (universReg.at(i)._var == var) {
			universReg.at(i)._var.clear();
		}
	}
}


/**
 * 标记寄存器存的是立即数
 */
void Generate::setRegConst(std::string &_reg)
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (universReg.at(i)._reg == _reg) {
			universReg.at(i).is_const = true;
		}
	}
}
/**
 * 将寄存器_reg设置为_var
 */
void Generate::setReg(std::string &_reg, std::string &_var)
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (universReg.at(i)._reg == _reg) {
			universReg.at(i)._var = _var;
		}
	}
}

/**
 * 获取_reg寄存器，此时寄存器为空
 */
std::string Generate::getReg(std::string &_reg)
{
	std::string _var;
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (_reg == universReg.at(i)._reg) {
			if (universReg.at(i)._var.empty())
				return _reg;
			else {
				_var = universReg.at(i)._var;
				universReg.at(i)._var.clear();
				break;
			}
		}
	}

	// 如果指定寄存器不为空, 调整寄存器
	Locvar _tem = searchTempvar(_var);
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (_reg != universReg.at(i)._reg && universReg.at(i)._var.empty()) {
			universReg.at(i)._var = _var;
			_tem._reg = universReg.at(i)._reg;
			Locvar _pus = _tem;
			_stk_temp_var.pop_back();
			_stk_temp_var.push_back(_pus);
			out << "\tmovl\t" + _reg + ", " + _tem._reg << std::endl;
			return _reg;
		}
	}

	// 调整失败
	return std::string();
}


/**
* @ 临时变量进栈，并绑定寄存器
*/
void Generate::push_back_temp_stk(Locvar & tv, std::string &reg)
{
	setReg(reg, tv._var);
	_stk_temp_var.push_back(tv);
}

/**
* 临时变量出栈，并解绑寄存器
*/
void Generate::pop_back_temp_stk(std::string &var)
{
	clearRegTemp(var);

	if (isNumber(var))
		return;
	if (_stk_temp_var.empty())
		return;

	if (_stk_temp_var.back()._var == var)
		_stk_temp_var.pop_back();
}

bool Generate::isTempVar(std::string &_t)
{
	Locvar _var = searchTempvar(_t);
	if (!_var._var.empty()) {
		return true;
	}
	return false;
}
bool Generate::isLocVar(std::string &_l)
{
	Locvar _var = searchLocvar(_l);
	if (!_var._var.empty()) {
		return true;
	}
	return false;
}

char Generate::getVarType(std::string &_v)
{
	if (isNumber(_v)) return 'n';
	else if (isLocVar(_v)) return 'l';
	else if (isTempVar(_v)) return 't';
	return 0;
}


std::vector<std::string> Generate::getQuad()
{
	std::vector<std::string> _quad;
	std::string _name;
	bool _is_push = false;

#define _push_name() do{ if (!_is_push && !_name.empty()) {\
	_quad.push_back(_name);\
	_is_push = true;\
	_name.clear();\
	}}while(0)

	for (char c  = inf.next();c != 0;c = inf.next()) {
		
		switch (c) {
		case ':':
			_push_name();
			_quad.push_back(":");
			break;

		case ' ':
			_push_name();
			break;

		case '\n':
			_push_name();
			if (_quad.empty())
				continue;
			return _quad;

		case_0_9:
		case_a_z:
		case_A_Z:
		case '.':
		case '_':
		case_op:
			_name.push_back(c);
			_is_push = false;
			break;

		default:
			_push_name();
			break;
		}
	}
#undef _push_name
	return _quad;
}

std::string Generate::getOutName()
{
	std::string _rstr;
	for (size_t i = 0; i < _infilename.length(); ++i) {
		if (_infilename.at(i) == '.') 
			break;
		_rstr.push_back(_infilename.at(i));
	}
	return _rstr + ".s";
}