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

#define temp_clear(_q1_, _q2_)  do {\
										pop_back_temp_stk(_q1_);\
										pop_back_temp_stk(_q2_);\
										pop_back_temp_stk(_q1_);\
							    }while(0);

#define loc_var_val(pos)      (std::to_string(pos) + "(%ebp)")
/**
 * 寄存器描述
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
 * 临时变量状态
 */
class TempVar {
public:
	TempVar() :_name() {  }
	TempVar(const std::string &_r) :_name(_r) {}
    TempVar(const std::string &_n, const std::string &_r) :_name(_n), _reg(_r) {  }
	TempVar(const std::string &_r, int _v) :_name(_r), _size(_v) {  }
	TempVar(const TempVar& r) :_name(r._name), _size(r._size),  _reg(r._reg), type(r.type){ }
    TempVar operator= (const TempVar &r) { _name = r._name; _size = r._size;  _reg = r._reg;type = r.type; return *this; }

    int type;
	std::string _name;             // 变量名
	int _size = 0;                 // 变量大小
    bool _isUnsig = false;
	std::string _reg;              // 是否放在了寄存器中
};

/**
 * 产生汇编代码
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
    void reg_init();
    void generate(std::vector<std::string> &_q);
    std::string getOutName();

    void getEnvSize(Env *_b, int &_size);
    int getFuncLocVarSize(Node &n);
    int getFuncCallSize(Node &n);
    void setLocEnv(const std::string &envName);


    // 控制指令
    std::string mov2stk(int size);
    std::string movXXl(int size, bool isz);
    std::string reg2stk(const std::string &_reg, int size);
    std::string mul(int size, bool isunsig);
    std::string gas_fld(int size, int _t);
    Type gas_fstp(const std::string &name);
    std::string getTypeString(Type _t);

    // 产生ASM
    inline void gas(const std::string &_s) { out << _s << std::endl; }
    inline void gas_ins(const std::string &_i, const std::string &_src, const std::string &_des) { out << "\t" + _i + "\t" + _src + ", " + _des << std::endl; }

    void const_str();
    void func_decl(Node &n);
    void gas_dec(const std::string &n, int size);                                    // 全局声明
    void gas_def_arr(Node &n, bool is_fir);                                          // 数组定义
    void gas_def_int(const std::string &n, int size, int init, bool is_fir);
    void gas_def_flo(const std::string &n, int size, const std::string &init, bool is_fir);
    void gas_custom(Node &n, bool is_fir);

    Type gas_load(const std::string &_q, const std::string &_reg);                   // 加载数据
    int gas_flo_load(const std::string &fl);
    Type getStructFieldType(Node &var, std::string &_off);

    void gas_jxx(const std::string &op, const std::string &des, Type &_t);
    void unlimited_binary_op(std::vector<std::string> &_q, const std::string &op);
    void shift_op(std::vector<std::string> &_q, const std::string &op);
	void genMulOrModAsm(std::vector<std::string> &_q);
    void genIncDec(const std::string &_obj, const std::string &op);

    void temp_save(const std::string &_n, int type, bool is_unsig = false, const std::string &_reg = "%st");
    void temp_save(const std::string &_n, Type &_t, const std::string &_reg);
    
    std::string Generate::getEmptyReg();
    void setReg(const std::string &_reg, const std::string &_var);
    std::string Generate::getReg(const std::string &_reg);
    void getReg(std::vector<std::string> &_q);
    bool isReg(const std::string &_t);
    void clearRegTemp(const std::string &var);

    std::string Generate::searchFLoat(const std::string &fl);
    LocVar &searchLocvar(const std::string &name);
    TempVar &searchTempvar(const std::string &name);
	TempVar &searchFloatTempvar(const std::string &name);

    bool isTempVar(const std::string &_t);
	bool isFloatTemVar(const std::string &_t);
    bool isLocVar(const std::string &_l);
    bool isEnumConst(const std::string &_l);

    void push_back_temp_stk(TempVar & tv, const std::string &reg);
    void pop_back_temp_stk(const std::string &var);

	Parser *parser;
	Env *gloEnv = nullptr;
	Env *locEnv = nullptr;

	std::vector<Reg> universReg;
    std::vector<Reg> float_reg;

	std::vector<Reg> segReg;
	File inf;
	std::ofstream out;
	std::string _infilename;
	bool is_main = false;
	std::vector<std::string> params;
	Node currentFunc;

	// 使用表达式栈来分配寄存器
	std::vector<TempVar> _stk_temp_var;
	std::vector<TempVar> _stk_float_temp_var;
    std::vector<TempVar> _stk_ret_;

	bool finit = true;                   // FPU是否初始化过
};

#endif // !__ZCC_GEN_H