#ifndef __ZCC_GEN_H
#define __ZCC_GEN_H


#include <iostream>
#include "file.h"
#include "parser.h"

#define st(x) do{x} while(0)

#define _text_ "\t.text"

#define gas_tab(str)          gas("\t" + std::string(str))
#define gas_glo(str)          gas("\t.globl\t_" + str);
#define gas_label(label)      gas(label+":")
#define gas_func_def(fn)      gas("\t.def\t_" + std::string(fn) + ";\t.scl\t2;\t.type\t32;\t.endef")

#define gas_jmp(des)          gas("\tjmp\t" + des);
#define gas_call(des)         gas("\tcall\t_" + std::string(des));



#define loc_var_val(pos)      (std::to_string(pos) + "(%ebp)")
/**
 * �Ĵ�������
 */
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

/**
 * ��ʱ����״̬
 */
class TempVar {
public:
	TempVar() :_name() {  }
    TempVar(const std::string &_n, const std::string &_r) :_name(_n), _reg(_r) {  }
	TempVar(const std::string &_r) :_name(_r) {}
	TempVar(const std::string &_r, int _v) :_name(_r), _size(_v) {  }
	TempVar(const TempVar& r) :_name(r._name), _size(r._size),  _reg(r._reg), type(r.type){ }
    TempVar operator= (const TempVar &r) { _name = r._name; _size = r._size;  _reg = r._reg;type = r.type; return *this; }

    int type;
	std::string _name;             // ������
	int _size = 0;                 // ������С
    bool _isUnsig = false;
	std::string _reg;              // �Ƿ�����˼Ĵ�����
};

/**
 * ����������
 */
using LocVar = Node;
class Generate{
public:
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
    void genIncDec(const std::string &_obj, const std::string &op);

    std::string mov2stk(int size);
    std::string movXXl(int size, bool isz);
    std::string reg2reg(int size);
    std::string reg2stk(const std::string &_reg, int size);
    std::string mul(int size, bool isunsig);

    inline void gas(const std::string &_s) { out << _s << std::endl; }
    inline void gas_ins(const std::string &_i, const std::string &_src, const std::string &_des) { out << "\t" + _i + "\t" + _src + ", " + _des << std::endl; }
    void gas_def_int(const std::string &n, int size, int init, bool is_fir);
    void gas_def_flo(const std::string &n, int size, const std::string &init, bool is_fir);
    void gas_def_arr(Node &n, bool is_fir);
    void gas_dec(const std::string &n, int size);
    void gas_jxx(const std::string &op, const std::string &des);
    std::string  gas_load(const std::string &_q);
    bool gas_load(const std::string &_q, const std::string &_reg);

    std::string getSrc(std::string &_n);
    
    std::string Generate::getEmptyReg();
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
	void var_decl(Node &n, bool is_fir);
	// ���ɺ����Ļ�࣬��������Ҫ��
	void func_decl(Node &n);
	// ��ȡ�����������оֲ������Ĵ�С�������������õ�size
	void getEnvSize(Env *_b, int &_size);
	// ��ȡ�����ֲ������Ĵ�С������size
	int getFuncLocVarSize(Node &n);
	// ��ȡ�������õĴ�С
	int getFuncCallSize(Node &n);
	// ��ȡ�ֲ�����
    LocVar &searchLocvar(const std::string &name);
    void setLocEnv(const std::string &envName);
    TempVar &searchTempvar(const std::string &name);
    void envUp2DownSearch(Env * _env, const std::string &name, LocVar &var, bool *isfind);
	std::string Generate::getReg(const std::string &_reg);
	void setReg(const std::string &_reg, const std::string &_var);
	void setRegConst(std::string &_reg);
	void getReg(std::vector<std::string> &_q);
	void glo_var_decl(Node &n);
	void glo_var_define(Node &n, bool is_fir);

	Parser *parser;
	Env *gloEnv = nullptr;
	Env *locEnv = nullptr;

	std::vector<Reg> universReg;

	void clearRegTemp(const std::string &var);
	std::vector<Reg> segReg;
	File inf;
	std::ofstream out;
	std::string _infilename;
	bool is_main = false;
	std::vector<std::string> params;
	Node currentFunc;

	// ʹ�ñ��ʽջ������Ĵ���
	std::vector<TempVar> _stk_temp_var;
	void push_back_temp_stk(TempVar & tv, const std::string &reg);
	void pop_back_temp_stk(const std::string &var);

	bool isTempVar(const std::string &_t);
	bool isLocVar(const std::string &_l);
	char getVarType(std::string &_v);
};

#endif // !__ZCC_GEN_H