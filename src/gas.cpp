#include "gen.h"
#include "error.h"

/**
 * @加载浮点数到内存中
 */
int Generate::gas_flo_load(const std::string &name)
{
    std::string _src;

    // 如果是常量
    std::string fn = searchFLoat(name);
    if (!fn.empty()) {
        gas_tab(gas_fld(4, K_FLOAT) + "\t" + name);
		return K_FLOAT;
    }

	if (isNumber(name)) {
		gas_tab("fild	" + name);
		return K_FLOAT;
	}

    // 寄存器、局部和全局
    Node var = gloEnv->search(name);
    if (isFloatTemVar(name)) {
        TempVar temp = searchFloatTempvar(name);
        return temp.type;
    }
    else  if (isLocVar(name)) {
        var = searchLocvar(name);
        gas_tab(gas_fld(var.type.size, var.type.type) + "\t" + loc_var_val(var._off));
        return var.type.type;
    }
    else if (!var.varName.empty()) {
        gas_tab(gas_fld(var.type.size, var.type.type) + "\t" + "_" + var.varName);
		return var.type.type;
    }
	else {
		error("error float number.");
	}
    return 0;
}

Type Generate::gas_fstp(const std::string &name)
{
	Type _r;
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

    switch (var.type.type)
    {
    case K_CHAR:    _ins = "fistps"; break;
    case K_SHORT:   _ins = "fistps"; break;
    case K_INT:     _ins = "fistpl"; break;
    case K_LONG:    _ins = "fistpl"; break;

    case K_FLOAT:   _ins = "fstps"; break;
    case K_DOUBLE:  _ins = "fstpl"; break;

    default:        error("Error float size."); break;
    }
    gas_tab(_ins + "\t" + _des);

	return _r;
}

std::string Generate::gas_fld(int size, int _t)
{
    switch (_t) {
    case K_INT:
    case K_LONG:
    case K_CHAR:
    case K_SHORT:   return "fildl"; break;
    case K_FLOAT:   return "flds"; break;
    case K_DOUBLE:  return "fldl"; break;
    default: error("Unspport type.");
    }
    return std::string();
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

/**
 * 自定义数据类型结构体
 */
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
 * @ret 是否为有符号数
 */
Type Generate::gas_load(const std::string &_q, const std::string &_reg)
{
    // 加载立即数
    if (isNumber(_q)) {
        gas_ins("movl", "$" + _q, _reg);
        setReg(_reg, _q);
        return Type(K_INT, 4, false);
    }

    if (isEnumConst(_q)) {
        gas_ins("movl", "$" + parser->searchEnum(_q), _reg);
        setReg(_reg, parser->searchEnum(_q));
        return Type(K_INT, 4, false);
    }

    // 加载局部变量
    Node var;
    if (isLocVar(_q)) {
        var = searchLocvar(_q);
		// 加载浮点数
		if (var.type.type == K_FLOAT || var.type.type == K_DOUBLE) {
			gas_flo_load(_q);
			// 没有使用通用，释放掉
			clearRegTemp(_q);
			return Type(var.type.type, var.type.size, var.type.isUnsig);
		}

        gas_ins(movXXl(var.type.size, var.type.isUnsig), loc_var_val(var._off), _reg);
        setReg(_reg, _q);
        return Type(var.type.type, var.type.size, var.type.isUnsig);
    }

    // 加载全局变量
    var = gloEnv->search(_q);
    if (!var.varName.empty()) {
		// 加载浮点数
		if (var.type.type == K_FLOAT || var.type.type == K_DOUBLE) {
			gas_flo_load(_q);
			// 没有使用通用，释放掉
			clearRegTemp(_q);
			return Type(var.type.type, var.type.size, var.type.isUnsig);
		}

        gas_ins(movXXl(var.type.size, var.type.isUnsig), "_" + var.varName, _reg);
        setReg(_reg, _q);
		return Type(var.type.type, var.type.size, var.type.isUnsig);
    }

	std::string _f = searchFLoat(_q);
	if (!_f.empty()) {
		gas_flo_load(_q);
		clearRegTemp(_q);
		return Type(K_FLOAT, 4, false);
	}

    // 加载临时变量
    if (isTempVar(_q)) {
        TempVar var = searchTempvar(_q);
        gas_ins("movl", var._reg, _reg);
        clearRegTemp(_q);
        setReg(_reg, _q);
		return Type(var.type, var._size, var._isUnsig);
    }
	else if (isFloatTemVar(_q)) {
		TempVar  f = searchFLoat(_q);
		return Type(f.type, f._size, f._isUnsig);
	}
    else {
        error("unknown data.");
        return false;
    }
}

void Generate::gas_jxx(const std::string &op, const std::string &des, Type &_t)
{
	if (op == ">") {
		if(_t.isUnsig || _t.type == K_FLOAT || _t.type == K_DOUBLE)
			gas_tab(("ja\t" + des));
		else 
			gas_tab(("jg\t" + des));
	}
	else if (op == "<") {
		if (_t.isUnsig || _t.type == K_FLOAT || _t.type == K_DOUBLE)
			gas_tab(("jb\t" + des));
		else
			gas_tab(("jl\t" + des));
	}
	else if (op == ">=") {
		if (_t.isUnsig || _t.type == K_FLOAT || _t.type == K_DOUBLE)
			gas_tab(("jae\t" + des));
		else
			gas_tab(("jge\t" + des));
	}
	else if (op == "<=") {
		if (_t.isUnsig || _t.type == K_FLOAT || _t.type == K_DOUBLE)
			gas_tab(("jbe\t" + des));
		else
			gas_tab(("jle\t" + des));
	}
    else if (op == "==")
        gas_tab(("je\t" + des));
    else if (op == "!=")
        gas_tab(("jne\t" + des));
}

/**
 * @berif 保存临时变量
 */
void Generate::temp_save(const std::string &_n, int type, bool is_unsig, const std::string &_reg)
{
	TempVar _t_save(_n);
	_t_save._isUnsig = is_unsig;
	_t_save.type = type;
	switch (type) {
	case K_CHAR:	_t_save._size = 1; break;
	case K_SHORT:	_t_save._size = 2; break;
	case K_INT:		_t_save._size = 4; break;
	case K_LONG:	_t_save._size = 4; break;
	case K_FLOAT:	_t_save._size = 4; break;
	case K_DOUBLE:	_t_save._size = 8; break;
	default:		_t_save._size = 0; break;
	}
	_t_save._reg = _reg;

	if (type == K_FLOAT || type == K_DOUBLE)
		_stk_float_temp_var.push_back(_t_save);
	else
		_stk_temp_var.push_back(_t_save);
}

void Generate::temp_save(const std::string &_n, Type &_t, const std::string &_reg)
{
	TempVar _t_save(_n);
	_t_save._isUnsig = _t.isUnsig;
	_t_save.type = _t.type;
	switch (_t_save.type) {
	case K_CHAR:	_t_save._size = 1; break;
	case K_SHORT:	_t_save._size = 2; break;
	case K_INT:		_t_save._size = 4; break;
	case K_LONG:	_t_save._size = 4; break;
	case K_FLOAT:	_t_save._size = 4; break;
	case K_DOUBLE:	_t_save._size = 8; break;
	default:		_t_save._size = 0; break;
	}
	_t_save._reg = _reg;
	if (_t_save.type == K_FLOAT || _t_save.type == K_DOUBLE)
		_stk_float_temp_var.push_back(_t_save);
	else
		_stk_temp_var.push_back(_t_save);
}

/**
 * @berif 无限制(寄存器等)二元操作符
 */
void Generate::unlimited_binary_op(std::vector<std::string> &_q, const std::string &op)
{
	Type _save, _t;
	std::string _q1_reg, _q2_reg;

	_q1_reg = getEmptyReg();
	_t = gas_load(_q.at(1), _q1_reg); _save.type < _t.type ? _save = _t : true;
	_q2_reg = getEmptyReg();
	_t = gas_load(_q.at(2), _q2_reg); _save.type < _t.type ? _save = _t : true;

	gas_ins(op, _q1_reg, _q2_reg);

	temp_clear(_q.at(1), _q.at(2));
	temp_save(_q.at(3), _save, _q2_reg);
}

/**
 * @berif 移位操作符
 */
void Generate::shift_op(std::vector<std::string> &_q, const std::string &op)
{
	Type _save, _t;

	getReg("%edx");
	getReg("%ecx");

	_t = gas_load(_q.at(1), "%ecx"); _save.type < _t.type ? _save = _t : true;
	_t = gas_load(_q.at(2), "%edx"); _save.type < _t.type ? _save = _t : true;
	gas_ins(op, "%cl", "%edx");

	temp_clear(_q.at(1), _q.at(2));
	temp_save(_q.at(3), _save, "%edx");
}

/**
 * @berif 获取变量的地址，指针+偏移
 */
void Generate::gas_addr(Node &_var, const std::string &_off, std::string &_des_reg)
{
    int size = _var.type.size;
    bool is_unsig = _var.type.isUnsig;

    if (isNumber(_off))
        gas_ins(movXXl(size, is_unsig), std::to_string(_var._off + _var.type.size * atoi(_off.c_str())) + "(%ebp)", "%eax");
    else if (isEnumConst(_off))
        gas_ins(movXXl(size, is_unsig), std::to_string(_var._off + _var.type.size * atoi(parser->searchEnum(_off).c_str())) + "(%ebp)", "%eax");
    else if (isLocVar(_off)) {
        Node _loc = searchLocvar(_off);
        getReg("%ecx");
        gas_ins("leal", std::to_string(_var._off) + "(%ebp)", "%eax");
        gas_ins("movl", loc_var_val(_loc._off), "%ecx");
        gas_ins("imull", "$" + std::to_string(_var.type.size), "%ecx");
        gas_ins("addl", "%ecx", "%eax");
        gas_ins(movXXl(size, is_unsig), "(%eax)", "%eax");
    }
    else if (isTempVar(_off)) {
        TempVar _loc = searchTempvar(_off);
        gas_ins("leal", std::to_string(_var._off) + "(%ebp)", "%eax");
        gas_ins("imull", "$" + std::to_string(_var.type.size), _loc._reg);
        gas_ins("addl", _loc._reg, "%eax");
        gas_ins(movXXl(size, is_unsig), "(%eax)", "%eax");
    }
    else {
        Node _glo = gloEnv->search(_off);
        getReg("%edx");
        gas_ins("leal", std::to_string(_var._off) + "(%ebp)", "%eax");
        gas_ins("movl", "_" + _glo.varName, "%edx");
        gas_ins("imull", "$" + std::to_string(_var.type.size), "%edx");
        gas_ins("addl", "%edx", "%eax");
        gas_ins(movXXl(size, is_unsig), "(%eax)", "%eax");
    }
}