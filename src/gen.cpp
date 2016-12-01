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

	Env *gloenv = parser->getGloEnv();
	for (int i = 0; i < gloenv->size(); ++i) {
		Node n = gloenv->at(i);
		if (n.kind == NODE_GLO_VAR && n.params.empty())
			var_decl(n);
	}
}

void Generate::var_decl(Node &n)
{
	if (n.lvarinit.empty()) {
		glo_var_decl(n);
	}
	else {
		glo_var_define(n);
	}
}

void Generate::glo_var_decl(Node &n)
{
	out << "\t.comm\t" << "_" + n.varName << ", " << n.type.size << ", " << n.type.size/2 <<std::endl << std::endl;
}

std::string getTypeString(Node &n)
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

void Generate::glo_var_define(Node &n)
{
	out << "\t.globl\t" << "_" + n.varName << std::endl;
	out << "\t.data" << std::endl;
	if(n.type.getType() == ARRAY)
		out << "\t.align\t" << n.type.len * 4 << std::endl;
	else 
		out << "\t.align\t" << n.type.size << std::endl;
	out << "_" + n.varName << ":" << std::endl;
	if (n.type.getType() == ARRAY) {
		int i = 0;
		for (; i < n.lvarinit.size(); ++i) {
			out << getTypeString(n) << n.lvarinit.at(i).int_val << std::endl;
		}
		out << "\t.space\t" << (n.type.len - i) * n.type.size << std::endl;
	}
	else
		out << getTypeString(n) << n.lvarinit.at(0).int_val << std::endl << std::endl;

}

void Generate::reg_init()
{
	// 通用寄存器
	universReg.push_back(Reg("AX"));
	universReg.push_back(Reg("BX"));
	universReg.push_back(Reg("CX"));
	universReg.push_back(Reg("DX"));

	// 段寄存器
	segReg.push_back(Reg("CS"));
	segReg.push_back(Reg("DS"));
	segReg.push_back(Reg("ES"));
	segReg.push_back(Reg("SS"));
}
void Generate::getEnvSize(Env *_begin, int &_size)
{
	if (_begin == nullptr)
		return;

	bool params = false;
	int pp = 8;
	if (_size == -1) {
		params = true;
		_size = 0;
	}
		

	for (int i = 0; i < _begin->size(); ++i) {
		int t = _begin->at(i).type.size;
		
		locvar.push_back(Locvar(_begin->at(i).varName, t));

		if (params) {
			locvar.back()._pos = pp;
			pp += 4;
		}
		else {
			_size += t;
			locvar.back()._pos = -_size;
		}
			
		if (!_begin->at(i).lvarinit.empty()) 
			locvar.back()._lvalue = _begin->at(i).lvarinit;
	}
	std::vector<Env *> node = _begin->getNext();
	for (int i = 0; i < node.size(); ++i) {
		getEnvSize(node.at(i), _size);
	}
}
int Generate::getFuncLocVarSize(Node &n)
{
	int _rsize = -1;
	std::vector<Env *> gloEnv = parser->getGloEnv()->getNext();
	for (int i = 0; i < gloEnv.size(); ++i) {
		Env * next = gloEnv.at(i);
		if (n.funcName == next->getName()) {
			getEnvSize(next, _rsize);
		}
	}
	return _rsize;
}
int Generate::getFuncCallSize(Node &n)
{
	return 0;
}


void Generate::func_decl(Node &n)
{
	locvar.clear();
	int size = getFuncLocVarSize(n);            // 获取临时变量的 大小
	size += getFuncCallSize(n);

	if (n.funcName == "main") {
		out << "\t.def\t__main;\t.scl\t2;\t.type\t32;\t.endef" << std::endl;
	}
	out << "\t.globl\t" << "_" << n.funcName << std::endl;
	out << "\t.def\t" << "_" << n.funcName << ";\t.scl\t2;\t.type\t32;\t.endef" << std::endl;
	out << "_" + n.funcName << ":" << std::endl;
	out << "\t.cfi_startproc" << std::endl;
	out << "\tpushl	%ebp" << std::endl;
	out << "\t.cfi_def_cfa_offset 8" << std::endl;
	out << "\t.cfi_offset 5, -8" << std::endl;
	out << "\tmovl	%esp, %ebp" << std::endl;
	out << "\t.cfi_def_cfa_register 5" << std::endl;
	if (n.funcName == "main") {
		out << "\tandl	$ - 16, %esp" << std::endl;
		if (size > 0)
			out << "\tsubl\t$" << (size + 4 > 16 ? size + 4 : 16) << ", %esp" << std::endl;
		out << "\tcall	___main" << std::endl;

	}
	else {
		if (size > 0)
			out << "\tsubl\t$" << size << ", %esp" << std::endl;
	}
}

bool isNumber(std::string &str)
{
	for (int i = 0; i < str.size();++i) {
		if (!(str.at(i) >= '0' && str.at(i) <= '9'))
			return false;
	}
	return true;
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
void Generate::getReg(std::vector<std::string> &_q)
{
#define _q_0_is(str) (_q.at(0) == str)
	// 标签直接输出
	if (_q.size() == 2 && _q.at(1) == ":") {
		Node r = parser->getGloEnv()->search(_q.at(0));

		if (r.kind == NODE_FUNC) {
			func_decl(r);
		}
	}
	else if (_q_0_is("=")) {
		Locvar var = search(_q.at(2));
		if (isNumber(_q.at(1))) {
			if (var._size == 1) {
				out << "\tmovb\t$" << _q.at(1) << ", " << var._pos << "(%ebp)" << std::endl;
			}
			else if (var._size == 2) {
				out << "\tmovw\t$" << _q.at(1) << ", " << var._pos << "(%ebp)" << std::endl;
			}
			else if (var._size == 4) {
				out << "\tmovl\t$" << _q.at(1) << ", " << var._pos << "(%ebp)" << std::endl;
			}
		}
	}
	else if (_q_0_is("if")) {

	}
	else if (_q_0_is("goto")) {

	}
	else if (_q_0_is("call")) {

	}
	else if (_q_0_is("ret")) {
		out << "\tleave" << std::endl;
		out << "\t.cfi_restore 5" << std::endl;
		out << "\t.cfi_def_cfa 4, 4" << std::endl;
		out << "\tret" << std::endl;
		out << "\t.cfi_endproc" << std::endl << std::endl;
	}
	else if(_q_0_is("+")){
		
	}
	else if (_q_0_is("-")) {

	}
	else if (_q_0_is("*")) {

	}
	else if (_q_0_is("/")) {

	}
	else if (_q_0_is("%")) {

	}
	else if (_q_0_is("&")) {

	}
	else if (_q_0_is("|")) {

	}
	else if (_q_0_is("^")) {

	}
	else if (_q_0_is("||")) {

	}
	else if (_q_0_is("&&")) {

	}
	else if (_q_0_is(">")) {

	}
	else if (_q_0_is("<")) {

	}
	else if (_q_0_is(">=")) {

	}
	else if (_q_0_is("<=")) {

	}
	else if (_q_0_is("==")) {

	}
	else if (_q_0_is("!=")) {

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
	for (int i = 0; i < _infilename.length(); ++i) {
		if (_infilename.at(i) == '.') 
			break;
		_rstr.push_back(_infilename.at(i));
	}
	return _rstr + ".s";
}