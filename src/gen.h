#ifndef __ZCC_GEN_H
#define __ZCC_GEN_H
#include "file.h"
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

class Generate{
public:
	Generate(const std::string &filename);
	Generate(const Generate &) = delete;
	Generate operator= (const Generate &) = delete;
	~Generate() { out.close(); }


	std::vector<std::string> getQuad();
	void getReg(std::vector<std::string> &_q);

private:
	std::string getOutName();

	std::vector<Reg> universReg;
	std::vector<Reg> segReg;
	File inf;
	std::ofstream out;
	std::string _infilename;
};

#endif // !__ZCC_GEN_H