#include "gen.h"
#include "error.h"

/**
 * @berif ����ȫ�ֱ����Ķ���
 */
void Generate::gas_def_int(const std::string &n, int size, int init, bool is_fir)
{
    gas("\t.globl\t_" + n);
    if (is_fir) gas("\t.data");
    if (size > 1) gas("\t.align\t" + std::to_string(size));
    gas("_" + n + ":");
    switch (size)
    {
    case 1: gas("\t.byte\t" + std::to_string(init)); break;
    case 2: gas("\t.word\t" + std::to_string(init)); break;
    case 4: gas("\t.long\t" + std::to_string(init)); break;
    default:
        error("Error data size.");
        break;
    }
}

/**
 * @berif ������ȫ�ֱ����Ķ���
 */
void Generate::gas_def_flo(const std::string &n, int size, const std::string &init, bool is_fir)
{
    gas("\t.globl\t_" + n);
    if (is_fir) gas("\t.data");
    gas("\t.align\t" + std::to_string(size));
    gas("_" + n + ":");
    switch (size)
    {
    case 4: gas("\t.float\t" + init); break;
    case 8: gas("\t.double\t" + init);break;
    default:
        error("Error data size.");
        break;
    }
}

/**
 * ����
 */
void Generate::gas_dec(const std::string &n, int size)
{
    out << "\t.comm\t";
    out << "_" + n;
    out << "," << size;
    out << ", " << ((size / 2 < 3) ? size / 2 : 3);
    out << std::endl;
}

/**
 * @berif ���鶨��
 */
void Generate::gas_def_arr(Node &n, bool is_fir)
{
    gas("\t.globl\t_" + n.varName);
    if (is_fir) gas("\t.data");
    gas("_" + n.varName + ":");

    size_t i = 0;
    for (; i < n.lvarinit.size(); ++i) {
        out << getTypeString(n) << n.lvarinit.at(i).int_val << std::endl;
    }
    out << "\t.space\t" << (n.type._all_len - i) * n.type.size << std::endl;
}

/**
 * �������ݵ��Ĵ���
 */
std::string  Generate::gas_load(const std::string &_q)
{
    std::string _des = getEmptyReg();
    gas_load(_q, _des);
    return _des;
}

/**
 * �������ݵ��Ĵ���
 * @ret �Ƿ�Ϊ�з�����
 */
bool Generate::gas_load(const std::string &_q, const std::string &_reg)
{
    // ����������
    if (isNumber(_q)) {
        gas_ins("movl", "$" + _q, _reg);
        setReg(_reg, _q);
        return false;
    }

    // ���ؾֲ�����
    Node var;
    if (isLocVar(_q)) {
        var = searchLocvar(_q);
        gas_ins(movXXl(var.type.size, var.type.isUnsig), loc_var_val(var._off), _reg);
        setReg(_reg, _q);
        return var.type.isUnsig;
    }

    // ����ȫ�ֱ���
    var = gloEnv->search(_q);
    if (!var.varName.empty()) {
        gas_ins(movXXl(var.type.size, var.type.isUnsig), "_" + var.varName, _reg);
        setReg(_reg, _q);
        return var.type.isUnsig;
    }

    // ������ʱ����
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