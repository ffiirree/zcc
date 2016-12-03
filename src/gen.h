#ifndef __ZCC_GEN_H
#define __ZCC_GEN_H

#include "file.h"
#include "parser.h"
#include <iostream>

class Reg {
public:
	Reg(const std::string &_r):_reg(_r), _var(){}
	Reg(const std::string &_r, const std::string &_v) :_reg(_r), _var(_v) {  }
	Reg(const Reg& r) :_reg(r._reg), _var(r._var), is_const(r.is_const) {}
	Reg operator= (const Reg &r) { _reg = r._reg; _var = r._var; is_const = r.is_const; return *this; }
	std::string _reg;
	std::string _var;
	bool is_const = false;
};

class TempVar {
public:
	TempVar() :_name() {  }
	TempVar(const std::string &_r) :_name(_r) {}
	TempVar(const std::string &_r, int _v) :_name(_r), _size(_v) {  }
	TempVar(const TempVar& r) :_name(r._name), _size(r._size), _lvalue(r._lvalue), _is_unsig(r._is_unsig), _reg(r._reg) { }
	TempVar operator= (const TempVar &r) { _name = r._name; _size = r._size; _lvalue = r._lvalue;_is_unsig = r._is_unsig; _reg = r._reg; return *this; }

	std::string _name;             // 变量名
	int _size = 0;                 // 变量大小
	bool _is_unsig = false;        // 是否是lin
	std::vector<Node> _lvalue;     // 
	std::string _reg;              // 是否放在了寄存器中
};

class Generate{
public:
	using LocVar = Node;

	Generate(Parser *parser);
	Generate(const Generate &) = delete;
	Generate operator= (const Generate &) = delete;
	~Generate() { out.close(); }

	void run();
	std::vector<std::string> getQuad();

private:
	//
	void saveAndClear(std::string &_q1, std::string &_q2, std::string &_q3, const std::string &_reg);
	void genMulOrModAsm(std::vector<std::string> &_q);


	std::string getTypeString(Node &n);
	// 产生汇编代码
	void generate(std::vector<std::string> &_q);
	// 输出文件名称
	std::string getOutName();
	// 初始化寄存器
	void reg_init();
	// 字符串常量生成汇编代码
	void const_str();
	// 生成变量声明的汇编代码
	void var_decl(Node &n);
	// 生成函数的汇编，函数名需要改
	void func_decl(Node &n);
	// 获取该作用域所有局部变量的大小，包括函数调用的size
	void getEnvSize(Env *_b, int &_size);
	// 获取函数局部变量的大小，包括size
	int getFuncLocVarSize(Node &n);
	// 获取函数调用的大小
	int getFuncCallSize(Node &n);
	// 获取局部变量
    LocVar &searchLocvar(const std::string &name) { return locEnv->search(name); }
    void setLocEnv(const std::string &envName);
    TempVar &searchTempvar(const std::string &name);

	std::string Generate::getQuadReg(const std::string &_q1);
	std::string Generate::getReg(const std::string &_reg);
	void Generate::setReg(const std::string &_reg, std::string &_var);
	void setRegConst(std::string &_reg);
	void getReg(std::vector<std::string> &_q);
	void glo_var_decl(Node &n);
	void glo_var_define(Node &n);

	Parser *parser;
	Env *gloEnv = nullptr;
	Env *locEnv = nullptr;

	std::vector<Reg> universReg;
	void Generate::clearRegConst();

	void Generate::clearRegTemp(const std::string &var);
	std::vector<Reg> segReg;
	File inf;
	std::ofstream out;
	std::string _infilename;
	bool is_main = false;
	std::vector<std::string> params;
	Node currentFunc;

	// 使用表达式栈来分配寄存器
	std::vector<TempVar> _stk_temp_var;
	void push_back_temp_stk(TempVar & tv, const std::string &reg);
	void pop_back_temp_stk(const std::string &var);

	bool isTempVar(std::string &_t);
	bool isLocVar(std::string &_l);
	char getVarType(std::string &_v);
};

#endif // !__ZCC_GEN_H