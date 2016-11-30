#include<iomanip>
#include "gen.h"
#include "type.h"
#include "error.h"

Generate::Generate(const std::string &filename) :_infilename(filename), inf(filename) {
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

	out.open(getOutName());
	if (!out.is_open())
		error("Open file failed!");
}

void Generate::getReg(std::vector<std::string> &_q)
{
	// 标签直接输出
	if (_q.size() == 2 && _q.at(1) == ":") {
		out << _q.at(0) << ":" << std::endl;
	}
	else if (_q.at(0) == "if") {

	}
	else if (_q.at(0) == "goto") {

	}
	else if (_q.at(0) == "call") {

	}
	else if (_q.at(0) == "leave") {
		out << "ret" << std::endl;
	}
	else {

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
		_rstr.push_back(_infilename.at(i));
		if (_infilename.at(i) == '.') 
			break;
	}
	return _rstr + ".s";
}