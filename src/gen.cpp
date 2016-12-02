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
 * @berif �Ĵ�����ʼ��
 */
void Generate::reg_init()
{
	// ͨ�üĴ��� //32bits
	universReg.push_back(Reg("%eax"));
	universReg.push_back(Reg("%ebx"));
	universReg.push_back(Reg("%ecx"));
	universReg.push_back(Reg("%edx"));

	// �μĴ��� // 16bits
	segReg.push_back(Reg("%ecs"));
	segReg.push_back(Reg("%eds"));
	segReg.push_back(Reg("%ees"));
	segReg.push_back(Reg("%ess"));
}

/**
 * @berif ���ɻ�����
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
 * @berif �����ַ�������
 */
void Generate::const_str()
{
	out << "\t.section  .rdata,\"dr\"" << std::endl;
	std::vector<StrCard> strTbl = parser->getStrTbl();
	for (size_t i = strTbl.size(); i > 0; --i) {
		out << strTbl.back()._label << ":" << std::endl;
		out << "\t.ascii \"" << strTbl.back()._str << "\\0\"" << std::endl;
		strTbl.pop_back();
	}
}

/**
 * @berif ����ȫ�ֱ���
 */
void Generate::var_decl(Node &n)
{
	// �����ʼ��Ϊ0����Ϊ����
	if (n.lvarinit.empty()) {
		glo_var_decl(n);
	}
	// ����Ϊ����
	else {
		glo_var_define(n);
	}
}

/**
 * @breif ����������ȫ�ֱ���
 */
void Generate::glo_var_decl(Node &n)
{
	out << "\t.comm\t" << "_" + n.varName << ", " << n.type.size << ", " << n.type.size/2 <<std::endl << std::endl;
}

/**
 * @berif ȫ�ֱ�������
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
 * @��ȡ�ڵ����������
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
 * @berif ��ȡ�����ֲ������Ĵ�С���Դ�������ջ�ռ�Ĵ�С
 */
int Generate::getFuncLocVarSize(Node &n)
{
	int _rsize = 0;
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
 * @berif ��ȡ���������еľֲ������Ĵ�С���������ֲ�������ջ�е�λ�ù�ϵ
 */
void Generate::getEnvSize(Env *_begin, int &_size)
{
	if (_begin == nullptr)
		return;

	for (size_t i = 0; i < _begin->size(); ++i) {
		int t = _begin->at(i).type.size;
		
		// ����ֲ�����
		locvar.push_back(Locvar(_begin->at(i).varName, t));

		// ���������ջ�е�λ�ã�ջ��ebp
		_size += t;
		locvar.back()._pos = -_size;

		// �����ĳ�ֵ
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
		// �ҵ��ú���
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
	int size = getFuncLocVarSize(n);            // ��ȡ��ʱ������ ��С
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
		//out << "\tandl	$ - 16, %esp" << std::endl;  // ���룬���Բ�Ҫ
		out << "\tcall	___main" << std::endl;
	}
}

/**
 * \ 32bits: ͨ�üĴ���
 *    eax ���ۼ���
 *    ebx �ۼ����ͻ�ַ�Ĵ���
 *    ecx �ۼ����ͼ�����
 *    edx �ۼ�����I/O��ַ�Ĵ���
 *
 * \ 32bits: ָ��ͱ�ַ�Ĵ���
 *    esp ��ջָ��
 *    ebp ��ַָ��
 *    esi Դ��ַ�Ĵ���
 *    edi Ŀ���ַ�Ĵ���
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
	// ��ǩֱ�����
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
		// ��ֵ�еڶ���һ��Ϊ�ֲ�������ȫ�ֱ���
		Locvar var = searchLocvar(_q.at(2));
		Locvar tvar = searchTempvar(_q.at(1));

		std::string _out;

		// ��һ������Ϊ����
		if (isNumber(_q.at(1))) {
			_out = "$" + _q.at(1);
			goto _ass_end;
		}
		// ��Ϊ��ʱ����
		
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
		// ע���ջ����ջ��˳��
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

		// ����������
		clearRegConst();

		// �Ƚ���ʱ�����ͳ�����ջ
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

		for (size_t i = 0; i < func.params.size(); ++i) {
			int pos = 0;
			size_t param_size = func.params.at(i).type.size;

			if (param_size == 1) {
				out << "\tmovb\t" << params.back() << ", " << pos<< "(%esp)" << std::endl;
				pos += 1;
			}
			else if(param_size == 2){
				out << "\tmovw\t" << params.back() << ", " << pos << "(%esp)" << std::endl;
				pos += 2;
			}
			else if (param_size == 4) {
				if(func.params.at(i).getType().getType() == PTR)
					out << "\tmovl\t$" << params.back() << ", " << pos << "(%esp)" << std::endl;
				else 
					out << "\tmovl\t" << params.back() << ", " << pos << "(%esp)" << std::endl;
				pos += 4;
			}
		}

		out << "\tcall\t" << "_" + _q.at(1) << std::endl;
	}
	else if (_q_0_is("ret")) {
		if (is_main) is_main = false;
		std::string _out;
		// ��ȡ������������
		int size = currentFunc.type.retType->getSize();

		// ���ұ���
		// !ERROR ���п�����ȫ�ֱ���
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
		out << "\t.cfi_endproc" << std::endl << std::endl;
	}
	else if(_q_0_is("+")){
		getReg(_q);
	}
	else if (_q_0_is("-")) {
		getReg(_q);
	}
	else if (_q_0_is("*")) {
		getReg(_q);
	}
	else if (_q_0_is("/")) {
		getReg(_q);
	}
	else if (_q_0_is("%")) {
		getReg(_q);
	}
	else if (_q_0_is("&")) {
		getReg(_q);
	}
	else if (_q_0_is("|")) {
		getReg(_q);
	}
	else if (_q_0_is("^")) {
		getReg(_q);
	}
	else if (_q_0_is(">>")) {
		getReg(_q);
	}
	else if (_q_0_is("<<")) {
		getReg(_q);
	}
}

/**
 * @ ��ʱ������ջ�����󶨼Ĵ���
 */
inline void Generate::push_back_temp_stk(Locvar & tv, std::string &reg)
{
	setReg(reg, tv._var);
	_stk_temp_var.push_back(tv);
}

/**
 * ��ʱ������ջ�������Ĵ���
 */
inline void Generate::pop_back_temp_stk(std::string &var)
{
	clearRegTemp(var);

	if (isNumber(var))
		return;
	if (_stk_temp_var.empty())
		return;
	
	if (_stk_temp_var.back()._var == var) 
		_stk_temp_var.pop_back();
}

/**
 * @berif �Ĵ�������
 */
void Generate::getReg(std::vector<std::string> &_q)
{
	// ע���ջ����ջ��˳��
	std::string _q1 = _q.at(2);
	std::string _q2 = _q.at(1);
	std::string _q3 = _q.at(3);

	std::string _q1_reg;
	std::string _q2_reg;
	std::string _q3_reg;

	// ����ǰ���������Ĵ����У���������
	//   1.����������Ǿֲ�������ȫ�ֱ�����movl->
	//   2.���򣬵�����Ϊ��ǰ����Ĵ���
	// 
	// ��ʱ���������ڼĴ�����
	// �ֲ�������������󵯳�
	if (_q_0_is("+")) {
		_q1_reg = getQuadReg(_q1);
		_q2_reg = getQuadReg(_q2);
		out << "\taddl\t" << _q1_reg + ", " + _q2_reg << std::endl;

		// ����������
		clearRegConst();

		// �Ƚ���ʱ�����ͳ�����ջ
		pop_back_temp_stk(_q1);
		pop_back_temp_stk(_q2);

		Locvar _var = searchLocvar(_q3);
		if (!_var._var.empty()) {
			out << "\tmovl\t" + _q2_reg + ", " + std::to_string(_var._pos) + "(%ebp)" << std::endl;
		}
		else {
			Locvar _temp;
			_temp._var = _q3;
			_temp._is_temp = true;
			_temp._reg = _q2_reg;
			push_back_temp_stk(_temp, _q2_reg);
		}
	}
	else if (_q_0_is("-")) {
		_q1_reg = getQuadReg(_q1);
		_q2_reg = getQuadReg(_q2);
		out << "\tsubl\t" << _q1_reg + ", " + _q2_reg << std::endl;

		// ����������
		clearRegConst();

		// �Ƚ���ʱ�����ͳ�����ջ
		pop_back_temp_stk(_q1);
		pop_back_temp_stk(_q2);

		Locvar _var = searchLocvar(_q3);
		if (!_var._var.empty()) {
			out << "\tmovl\t" + _q2_reg + ", " + std::to_string(_var._pos) + "(%ebp)" << std::endl;
		}
		else {
			Locvar _temp;
			_temp._var = _q3;
			_temp._is_temp = true;
			_temp._reg = _q2_reg;
			push_back_temp_stk(_temp, _q2_reg);
		}
	}
}

void Generate::clearRegConst()
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (isNumber(universReg.at(i)._var))
			universReg.at(i)._var.clear();
	}
}

void Generate::clearRegTemp(std::string &var)
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (universReg.at(i)._var == var) {
			universReg.at(i)._var.clear();
		}
	}
}


std::string Generate::getQuadReg(std::string &_q)
{
	std::string _reg;

	// ������ֱ�Ӽ��ص��ڴ�
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
		// �ֲ��������ᱣ���ڼĴ�����
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

		// �ڱ��ʽջ��Ѱ�ң���ʱ����һ������Ϊ������֣�Ҳ���ǳ��ֹ���һ���ڼĴ�����
		Locvar _tem = searchTempvar(_q);
		if (_tem._var.empty()) {
			error("Temp var is not found.");
		}
		return _tem._reg;
	}
}


void Generate::setRegConst(std::string &_reg)
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (universReg.at(i)._reg == _reg) {
			universReg.at(i).is_const = true;
		}
	}
}
void Generate::setReg(std::string &_reg, std::string &_var)
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (universReg.at(i)._reg == _reg) {
			universReg.at(i)._var = _var;
		}
	}
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