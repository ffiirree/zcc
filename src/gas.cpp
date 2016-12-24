#include "gen.h"
#include "error.h"

int Generate::gas_flo_load(const std::string &name, bool isChange)
{
    std::string _src;

    std::string fn = searchFLoat(name);
    if (!fn.empty()) {
        gas_tab(gas_fld(4, K_FLOAT) + "\t" + name);
		return K_FLOAT;
    }

	if (isNumber(name)) {
		gas_tab("fild	" + name);
		return K_FLOAT;
	}

    if (isFloatTemVar(name)) {
        TempVar temp = searchFloatTempvar(name);
        if (isChange)
            gas_tab("fxch");
        return temp.type;
    }
    else  if (isLocVar(name)) {
        Node var = searchLocvar(name);
        if (var.kind == NODE_GLO_VAR) {
            gas_tab(gas_fld(var.type.size_, var.type.type) + "\t" + "_" + var.varName);
            return var.type.type;
        }
        else if (var.kind == NODE_LOC_VAR) {
            gas_tab(gas_fld(var.type.size_, var.type.type) + "\t" + loc_var_val(var._off));
            return var.type.type;
        }
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
        _size = var.type.size_;
        _des = loc_var_val(var._off);
    }
    else {
        var = gloEnv->search(name);
        _size = var.type.size_;
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



void Generate::gas_def_int(const std::string &n, int size, int init, bool is_fir)
{
    gas_tab(".globl\t_" + n);
    if (is_fir) gas_tab(".data");
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
    if(vm_->use_) vm_->push_data("_" + n, init, size);
}

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


void Generate::gas_dec(const std::string &n, int size)
{
    out << "\t.comm\t";
    out << "_" + n;
    out << "," << size;
   // out << ", " << ((size / 2 < 3) ? size / 2 : 3);
    out << std::endl;
}

void Generate::gas_def_arr(Node &n, bool is_fir)
{
    gas_tab(".globl\t_" + n.varName);
    if (is_fir) gas("\t.data");
    gas("_" + n.varName + ":");

    size_t i = 0;
    for (; i < n.lvarinit.size(); ++i) {
        out << getTypeString(n.type) << n.lvarinit.at(i).int_val << std::endl;
    }
    out << "\t.space\t" << (n.type._all_len - i) * n.type.size_ << std::endl;
}

void Generate::gas_custom(Node &n, bool is_fir)
{
    gas_tab(".globl\t_" + n.varName);
    if (is_fir) gas("\t.data");
    gas("_" + n.varName + ":");

    int _initsize = 0;
    for (size_t i = 0; i < n.lvarinit.size(); ++i) {
        out << getTypeString(*n.type.fields.at(i)._type) << n.lvarinit.at(i).int_val << std::endl;
        _initsize += n.type.fields.at(i)._type->size_;
    }
    if(_initsize < n.type.size_)
        out << "\t.space\t" << n.type.size_ - _initsize << std::endl;
}

Type Generate::gas_load(const std::string &_q, const std::string &_reg)
{
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

    Node var;
    if (isLocVar(_q)) {
        var = searchLocvar(_q);
		if (var.type.type == K_FLOAT || var.type.type == K_DOUBLE) {
			gas_flo_load(_q, false);
			clearRegTemp(_q);
			return Type(var.type.type, var.type.size_, var.type.isUnsig);
		}
        else if (var.kind == NODE_GLO_VAR) {
            gas_ins(movXXl(var.type.size_, var.type.isUnsig), "_" + var.varName, _reg);
            setReg(_reg, _q);
            return Type(var.type.type, var.type.size_, var.type.isUnsig);
        }
        else if (var.kind == NODE_LOC_VAR) {
            gas_ins(movXXl(var.type.size_, var.type.isUnsig), loc_var_val(var._off), _reg);
            setReg(_reg, _q);
            return Type(var.type.type, var.type.size_, var.type.isUnsig);
        }
    }

	std::string _f = searchFLoat(_q);
	if (!_f.empty()) {
		gas_flo_load(_q, false);
		clearRegTemp(_q);
		return Type(K_FLOAT, 4, false);
	}

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
        if (_t.isUnsig || _t.type == K_FLOAT || _t.type == K_DOUBLE)
            gas_ins("ja", des);
		else 
            gas_ins("jg", des);
	}
	else if (op == "<") {
		if (_t.isUnsig || _t.type == K_FLOAT || _t.type == K_DOUBLE)
            gas_ins("jb", des);
		else
            gas_ins("jl", des);
	}
	else if (op == ">=") {
		if (_t.isUnsig || _t.type == K_FLOAT || _t.type == K_DOUBLE)
            gas_ins("jae", des);
		else
            gas_ins("jge", des);
	}
	else if (op == "<=") {
		if (_t.isUnsig || _t.type == K_FLOAT || _t.type == K_DOUBLE)
            gas_ins("jbe", des);
		else
			gas_ins("jle", des);
	}
    else if (op == "==")
        gas_ins("je", des);
    else if (op == "!=")
        gas_ins("jne", des);
}

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
    setReg(_reg, _n);

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
    setReg(_reg, _n);
	if (_t_save.type == K_FLOAT || _t_save.type == K_DOUBLE)
		_stk_float_temp_var.push_back(_t_save);
	else
		_stk_temp_var.push_back(_t_save);
}

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

void Generate::add_sub_with_ptr(std::vector<std::string> &_q, const std::string &op)
{
    Type _save, _t1, _t2;
    std::string _q1_reg, _q2_reg;

    _q1_reg = getEmptyReg();
    _t1 = gas_load(_q.at(1), _q1_reg);
    _q2_reg = getEmptyReg();
    _t2 = gas_load(_q.at(2), _q2_reg);

    //if (_t1.getType() == PTR &&_t2.getType() != PTR) {
    //    if (_q1_reg != "%eax") {
    //        
    //        gas_ins("movl", _q1_reg, "%eax");
    //    }
    //    gas_ins("imull", )
    //}

    gas_ins(op, _q1_reg, _q2_reg);

    temp_clear(_q.at(1), _q.at(2));
    temp_save(_q.at(3), _save, _q2_reg);
}

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


Type Generate::getStructFieldType(Node &var, std::string &_off)
{
    for (size_t i = 0; i < var.type.fields.size(); ++i) {
        if (_off == std::to_string(var.type.fields.at(i)._off)) {
            return *(var.type.fields.at(i)._type);
        }
    }
    error("%s do not have this field.", var.varName.c_str());
    return Type();
}

Type Generate::getPtrType(Node &var)
{
    return *(var.type.ptr);
}


void Generate::gas_ins(const std::string &_i, const std::string &_src, const std::string &_des)
{
    std::string ins = _i + "\t" + _src + ", " + _des;
    gas_tab(ins);
    vm_->push_back({ vm_->getInsByOp(_i), _src, _des , ins });
}


void Generate::gas_ins(const std::string &_i, const std::string &_des)
{
    std::string ins = _i + "\t" + _des;
    gas_tab(ins);
    vm_->push_back({ vm_->getInsByOp(_i), _des, ins });
}

void Generate::gas_call(const std::string &_des)
{
    std::string ins = "call\t_" + _des;
    gas_tab(ins);
    vm_->push_back({ vm_->getInsByOp("call"), "_" + _des , ins });
}

void Generate::gas_jxx_label(const std::string &_l)
{
    gas(_l + ":");
    vm_->setFuncAddr(_l);
}