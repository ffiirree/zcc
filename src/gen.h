#ifndef __ZCC_GEN_H
#define __ZCC_GEN_H
#include "file.h"
#include "parser.h"
#include <iostream>

class Reg {
public:
	Reg(const std::string &_r):_reg(_r), _var(){}
	Reg(const std::string &_r, const std::string &_v) :_reg(_r), _var(_v) {  }
	Reg(const Reg& r) :_reg(r._reg), _var(r._var) {}
	Reg operator= (const Reg &r) { _reg = r._reg; _var = r._var; return *this; }
	std::string _reg;
	std::string _var;
};

class Locvar {
public:
	Locvar() :_var() {  }
	Locvar(const std::string &_r) :_var(_r) {}
	Locvar(const std::string &_r, int _v) :_var(_r), _size(_v) {  }
	Locvar(const Locvar& r) :_var(r._var), _size(r._size), _pos(r._pos), _lvalue(r._lvalue), _is_param(r._is_param) { }
	Locvar operator= (const Locvar &r) { _var = r._var; _size = r._size; _pos = r._pos; _lvalue = r._lvalue;_is_param = r._is_param; return *this; }
	std::string _var;
	int _size = 0;
	bool _is_param = false;
	std::vector<Node> _lvalue;
	int _pos = 0;
};

class Generate{
public:
	Generate(Parser *parser);
	Generate(const Generate &) = delete;
	Generate operator= (const Generate &) = delete;
	~Generate() { out.close(); }


	std::vector<std::string> getQuad();
	void getReg(std::vector<std::string> &_q);

private:
	std::string getOutName();
	void reg_init();
	void var_decl(Node &n);
	void func_decl(Node &n);
	void getEnvSize(Env *_b, int &_size);
	int Generate::getFuncLocVarSize(Node &n);
	Locvar search(const std::string &name)
	{
		for (int i = 0; i < locvar.size(); ++i) {
			if (locvar.at(i)._var == name) {
				return locvar.at(i);
			}
		}
		return Locvar();
	}


	void glo_var_decl(Node &n);
	void glo_var_define(Node &n);

	Parser *parser;

	std::vector<Locvar> locvar;

	std::vector<Reg> universReg;
	std::vector<Reg> segReg;
	File inf;
	std::ofstream out;
	std::string _infilename;
};

#endif // !__ZCC_GEN_H