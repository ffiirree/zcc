#include "gen.h"
#include "error.h"

/**
 * @加载浮点数到内存中
 */
std::string Generate::gas_flo_load(const std::string &name)
{
    std::string _src;
    // 如果是常量
    std::string fn = searchFLoat(name);
    if (!fn.empty()) {
        gas_tab(gas_fld(4) + "\t" + name);
        return std::string();
    }

    // 寄存器、局部和全局
    Node var = gloEnv->search(name);
    if (!var.varName.empty()) {
        gas_tab(gas_fld(var.type.size) + "\t" + "_" + var.varName);
    }
    else if (isTempVar(name)) {
        TempVar var = searchTempvar(name);
        error("error");
    }
    else {
        var = searchLocvar(name);
        gas_tab(gas_fld(var.type.size) + "\t" + loc_var_val(var._off));
    }
    return std::string();
}

std::string Generate::gas_fstp(const std::string &name)
{
    int  _size = 0;
    std::string _ins, _des;
    Node var;
    
    if (isLocVar(name)) {
        var = searchLocvar(name);
        _size = var.type.size;
        _des = loc_var_val(var._off);
    }
    else {
        var = gloEnv->search(name);
        _size = var.type.size;
        _des = "_" + var.varName;
    }

    if (_size == 4)
        _ins = "fstps";
    else if (_size == 8)
        _ins = "fstpl";
    else
        error("Error float size.");

    gas_tab(_ins + "\t" + _des);

    return std::string();
}

std::string Generate::gas_fld(int size)
{
    if (size == 4)
        return "flds";
    else if (size == 8)
        return "fldl";
    else
        error("Error float size.");
}

std::string Generate::searchFLoat(const std::string &fl)
{
    std::vector<std::string> floatConst = parser->getFloatConst();
    for (size_t i = 0; i < floatConst.size(); ++i) {
        if (floatConst.at(i) == fl) {
            return floatConst.at(i - 1);
        }
    }
    return std::string();
}

/**
 * @berif 整形全局变量的定义
 */
void Generate::gas_def_int(const std::string &n, int size, int init, bool is_fir)
{
    gas_tab(".globl\t_" + n);
    if (is_fir) gas_tab(".data");
    //if (size > 1) gas_tab(".align\t" + std::to_string(size));
    gas("_" + n + ":");
    switch (size)
    {
    case 1: gas_tab(".byte\t" + std::to_string(init)); break;
    case 2: gas_tab(".word\t" + std::to_string(init)); break;
    case 4: gas_tab(".long\t" + std::to_string(init)); break;
    default:
        error("Error data size.");
        break;
    }
}

/**
 * @berif 浮点型全局变量的定义
 */
void Generate::gas_def_flo(const std::string &n, int size, const std::string &init, bool is_fir)
{
    gas_tab(".globl\t_" + n);
    if (is_fir) gas_tab(".data");
    gas_tab(".align\t" + std::to_string(size));
    gas("_" + n + ":");
    switch (size)
    {
    case 4: gas_tab(".float\t" + init); break;
    case 8: gas_tab(".double\t" + init);break;
    default:
        error("Error data size.");
        break;
    }
}


/**
 * 声明
 */
void Generate::gas_dec(const std::string &n, int size)
{
    out << "\t.comm\t";
    out << "_" + n;
    out << "," << size;
   // out << ", " << ((size / 2 < 3) ? size / 2 : 3);
    out << std::endl;
}

/**
 * @berif 数组定义
 */
void Generate::gas_def_arr(Node &n, bool is_fir)
{
    gas_tab(".globl\t_" + n.varName);
    if (is_fir) gas("\t.data");
    gas("_" + n.varName + ":");

    size_t i = 0;
    for (; i < n.lvarinit.size(); ++i) {
        out << getTypeString(n.type) << n.lvarinit.at(i).int_val << std::endl;
    }
    out << "\t.space\t" << (n.type._all_len - i) * n.type.size << std::endl;
}

void Generate::gas_custom(Node &n, bool is_fir)
{
    gas_tab(".globl\t_" + n.varName);
    if (is_fir) gas("\t.data");
    gas("_" + n.varName + ":");

    int _initsize = 0;
    for (size_t i = 0; i < n.lvarinit.size(); ++i) {
        out << getTypeString(*n.type.fields.at(i)._type) << n.lvarinit.at(i).int_val << std::endl;
        _initsize += n.type.fields.at(i)._type->size;
    }
    if(_initsize < n.type.size)
        out << "\t.space\t" << n.type.size - _initsize << std::endl;
}
/**
 * 加载数据到寄存器
 */
std::string  Generate::gas_load(const std::string &_q)
{
    std::string _des = getEmptyReg();
    gas_load(_q, _des);
    return _des;
}

/**
 * 加载数据到寄存器
 * @ret 是否为有符号数
 */
bool Generate::gas_load(const std::string &_q, const std::string &_reg)
{
    // 加载立即数
    if (isNumber(_q)) {
        gas_ins("movl", "$" + _q, _reg);
        setReg(_reg, _q);
        return false;
    }

    // 加载局部变量
    Node var;
    if (isLocVar(_q)) {
        var = searchLocvar(_q);
        gas_ins(movXXl(var.type.size, var.type.isUnsig), loc_var_val(var._off), _reg);
        setReg(_reg, _q);
        return var.type.isUnsig;
    }

    // 加载全局变量
    var = gloEnv->search(_q);
    if (!var.varName.empty()) {
        gas_ins(movXXl(var.type.size, var.type.isUnsig), "_" + var.varName, _reg);
        setReg(_reg, _q);
        return var.type.isUnsig;
    }

    // 加载临时变量
    if (isTempVar(_q)) {
        TempVar var = searchTempvar(_q);
        gas_ins("movl", var._reg, _reg);
        clearRegTemp(_q);
        setReg(_reg, _q);
        return var._isUnsig;
    }
    else {
        error("unknown data.");
        return false;
    }
}

void Generate::gas_jxx(const std::string &op, const std::string &des)
{
    if (op == ">")
        gas_tab(("jg\t" + des));
    else if (op == "<")
        gas_tab(("jl\t" + des));
    else if (op == ">=")
        gas_tab(("jge\t" + des));
    else if (op == "<=")
        gas_tab(("jle\t" + des));
    else if (op == "==")
        gas_tab(("je\t" + des));
    else if (op == "!=")
        gas_tab(("jne\t" + des));
}