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

class Locvar {
public:
	Locvar() :_var() {  }
	Locvar(const std::string &_r) :_var(_r) {}
	Locvar(const std::string &_r, int _v) :_var(_r), _size(_v) {  }
	Locvar(const Locvar& r) :_var(r._var), _size(r._size), _pos(r._pos), _lvalue(r._lvalue), _is_temp(r._is_temp), _reg(r._reg) { }
	Locvar operator= (const Locvar &r) { _var = r._var; _size = r._size; _pos = r._pos; _lvalue = r._lvalue;_is_temp = r._is_temp; _reg = r._reg; return *this; }

	std::string _var;              // ������
	int _size = 0;                 // ������С
	bool _is_temp = false;        // �Ƿ��ǲ���
	std::vector<Node> _lvalue;     // 
	int _pos = 0;                  // ջ�е�λ�ã����ebp
	std::string _reg;              // �Ƿ�����˼Ĵ�����
};

class Generate{
public:
	Generate(Parser *parser);
	Generate(const Generate &) = delete;
	Generate operator= (const Generate &) = delete;
	~Generate() { out.close(); }

	void run();
	std::vector<std::string> getQuad();

private:
	std::string getTypeString(Node &n);
	// ����������
	void generate(std::vector<std::string> &_q);
	// ����ļ�����
	std::string getOutName();
	// ��ʼ���Ĵ���
	void reg_init();
	// �ַ����������ɻ�����
	void const_str();
	// ���ɱ��������Ļ�����
	void var_decl(Node &n);
	// ���ɺ����Ļ�࣬��������Ҫ��
	void func_decl(Node &n);
	// ��ȡ�����������оֲ������Ĵ�С�������������õ�size
	void getEnvSize(Env *_b, int &_size);
	// ��ȡ�����ֲ������Ĵ�С������size
	int getFuncLocVarSize(Node &n);
	// ��ȡ�������õĴ�С
	int getFuncCallSize(Node &n);
	// ��ȡ�ֲ�����
	Locvar &searchLocvar(const std::string &name)
	{
		for (size_t i = 0; i < locvar.size(); ++i) {
			if (locvar.at(i)._var == name) {
				return locvar.at(i);
			}
		}
		Locvar *var = new Locvar();
		return *var;
	}
	Locvar &searchTempvar(const std::string &name)
	{
		for (size_t i = 0; i < _stk_temp_var.size(); ++i) {
			if (_stk_temp_var.at(i)._var == name) {
				return _stk_temp_var.at(i);
			}
		}
		Locvar *var = new Locvar();
		return *var;
	}
	std::string Generate::getQuadReg(const std::string &_q1);
	std::string Generate::getReg(std::string &_reg);
	void Generate::setReg(std::string &_reg, std::string &_var);
	void setRegConst(std::string &_reg);
	void getReg(std::vector<std::string> &_q);
	void glo_var_decl(Node &n);
	void glo_var_define(Node &n);

	Parser *parser;

	std::vector<Locvar> locvar;

	std::vector<Reg> universReg;
	void Generate::clearRegConst();

	void Generate::clearRegTemp(std::string &var);
	std::vector<Reg> segReg;
	File inf;
	std::ofstream out;
	std::string _infilename;
	bool is_main = false;
	std::vector<std::string> params;
	Node currentFunc;

	// ʹ�ñ��ʽջ������Ĵ���
	std::vector<Locvar> _stk_temp_var;
	inline void push_back_temp_stk(Locvar & tv, std::string &reg);
	inline void pop_back_temp_stk(std::string &var);

	bool isTempVar(std::string &_t);
	bool isLocVar(std::string &_l);
	char getVarType(std::string &_v);
};

#endif // !__ZCC_GEN_H