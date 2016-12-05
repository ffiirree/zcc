#include<iomanip>
#include "gen.h"
#include "type.h"
#include "error.h"


Generate::Generate(Parser *p)
{
	parser = p;
	_infilename = p->getQuadrupleFileName();

	inf.open(_infilename);
	
	out.open(getOutName());
	if (!out.is_open())
		error("Open file failed.");

    // �Ĵ�����ʼ��
	reg_init();
}

/**
 * @berif �Ĵ�����ʼ��
 */
void Generate::reg_init()
{
	// ͨ�üĴ��� //32bits
	universReg.push_back(Reg("%eax"));
	universReg.push_back(Reg("%ebx"));
	universReg.push_back(Reg("%ecx"));
	universReg.push_back(Reg("%edx"));

    float_reg.push_back(Reg("st(7)"));
    float_reg.push_back(Reg("st(6)"));
    float_reg.push_back(Reg("st(5)"));
    float_reg.push_back(Reg("st(4)"));
    float_reg.push_back(Reg("st(3)"));
    float_reg.push_back(Reg("st(2)"));
    float_reg.push_back(Reg("st(1)"));
    float_reg.push_back(Reg("st(0)"));

	// �μĴ��� // 16bits
	segReg.push_back(Reg("%ecs"));
	segReg.push_back(Reg("%eds"));
	segReg.push_back(Reg("%ees"));
	segReg.push_back(Reg("%ess"));
}

/**
 * @berif ���ɻ�����
 */
void Generate::run()
{
    gas("\t.file\t\"" + _infilename + "\"");

    // �ַ�������
    const_str();

    // ȫ�ֱ���
    gloEnv = parser->getGloEnv();
    bool is_fir_var = true;
    for (size_t i = 0; i < gloEnv->size(); ++i) {
        Node n = gloEnv->at(i);
        if (n.kind == NODE_GLO_VAR && n.params.empty()) {
            if (n.lvarinit.empty()) {
                gas_dec(n.varName, n.type.size);
            }
            else {
                switch (n.type.type) {
                case ARRAY: gas_def_arr(n, is_fir_var); break;
                case K_FLOAT:
                case K_DOUBLE:gas_def_flo(n.varName, n.type.size, n.lvarinit.at(0).sval, is_fir_var);break;
                case K_TYPEDEF:
                case K_STRUCT: gas_custom(n, is_fir_var);break;
                case K_UNION: error("Unsupport union."); break;
                default:gas_def_int(n.varName, n.type.size, n.lvarinit.at(0).int_val, is_fir_var);break;
                }
            }

            is_fir_var = false;
        }
    }

    // ����
	for (;;) {
		std::vector<std::string> quad = getQuad();

		if (quad.empty())
			break;
		generate(quad);
	}

    gas(parser->newLabel("FE") + ":");

    // ������
    std::vector<std::string> floatConst = parser->getFloatConst();
    if (!floatConst.empty())
        gas_tab(".section .rdata,\"dr\"");

    for (size_t i = floatConst.size()/3; i > 0; --i) {
        std::string size = floatConst.back(); floatConst.pop_back();
        gas_label(floatConst.back()); floatConst.pop_back();

        if (size == "4") {
            gas_tab(".float " + floatConst.back()); floatConst.pop_back();
        }
        else if (size == "8") {
            gas_tab(".double " + floatConst.back()); floatConst.pop_back();
        }
        else {
            error("unknown flaot size.");
        }
    }

    // ��־
    
    gas("\t.ident \"zcc 0.0.1\"");
}

/**
 * @berif �����ַ�������
 */
void Generate::const_str()
{
	if (parser->getStrTbl().empty())
		return;

    gas("\t.section  .rdata,\"dr\"");
	std::vector<StrCard> strTbl = parser->getStrTbl();
	for (size_t i = strTbl.size(); i > 0; --i) {

        gas_label(strTbl.back()._label);

		out << "\t.ascii \"" << strTbl.back()._str << "\\0\"" << std::endl;
		strTbl.pop_back();
	}
}



void Generate::func_decl(Node &n)
{
	int size = getFuncLocVarSize(n);            // ��ȡ��ʱ������ ��С
	size += getFuncCallSize(n);

	if (n.funcName == "main") {
        gas_func_def("_main");
		is_main = true;
	}
    gas_tab(".text");
    gas_glo(n.funcName);
    gas_func_def(n.funcName);
    gas_label("_" + n.funcName);

    gas_tab(".cfi_startproc");
    gas_tab("pushl  %ebp");
    gas_tab(".cfi_def_cfa_offset 8");
    gas_tab(".cfi_offset 5, -8");
    gas_tab("movl	%esp, %ebp");
    gas_tab(".cfi_def_cfa_register 5");

	if (size > 0)
		out << "\tsubl\t$" << ((size + 8 > 16) ? size + 8 : 16) << ", %esp" << std::endl;
	if (is_main) {
        gas_call("__main");
	}
}

#define _q_0_is(str) (_q.at(0) == str)
void Generate::generate(std::vector<std::string> &_q)
{
	if (_q.size() == 2 && _q.at(1) == ":") {
		Node r = parser->getGloEnv()->search(_q.at(0));

		if (r.kind == NODE_FUNC) {
            setLocEnv(r.funcName);                    // ������Ϊ��ǰ����

			currentFunc = r;
			func_decl(r);
			return;
		}

        gas_label(_q.at(0));                          // �����������ǩ�����
	}
    // ÿ�����ʽ��Ҫ���ջ
    else if (_q_0_is("clr")) {
        for (size_t i = 0; i < universReg.size(); ++i)
            universReg.at(i)._var.clear();

        for (size_t i = _stk_temp_var.size(); i > 0; --i)
            _stk_temp_var.pop_back();
    }
	else if (_q_0_is("=")) {
        std::string _src, _des;
        int size = 0;

        // Ŀ�ĵ�
		LocVar var = searchLocvar(_q.at(2));
        if (var.varName.empty()) {
            var = gloEnv->search(_q.at(2));

            if (!var.varName.empty()) {
                size = var.type.size;
                _des = "_" + var.varName;
            }
            else if (isTempVar(_q.at(2))) {
                TempVar _tem = searchTempvar(_q.at(2));
                size = 4;
                _des = "(" + _tem._reg + ")";
            }
        }
        else {
            size = var.type.size;
            _des = loc_var_val(var._off);
        }

        // ��Ϊ��ʱ����
        TempVar _temp = searchTempvar(_q.at(1));
        if (!_temp._name.empty()) {
            gas_ins(mov2stk(size), reg2stk(_temp._reg, size), _des);
        }
		// ��һ������Ϊ����
		else if (isNumber(_q.at(1))) {
            gas_ins(mov2stk(size), "$" + _q.at(1), _des);
		}
	}
	else if (_q_0_is("if")) {
		// ע���ջ����ջ��˳��
		std::string _q1 = _q.at(3);
		std::string _q2 = _q.at(1);

		std::string _q1_reg;
		std::string _q2_reg;

        _q1_reg = gas_load(_q1);
		_q2_reg = gas_load(_q2);

        gas_ins("cmpl", _q1_reg, _q2_reg);
        gas_jxx(_q.at(2), _q.at(5));

		// �Ƚ���ʱ�����ͳ�����ջ
		pop_back_temp_stk(_q1);
		pop_back_temp_stk(_q2);
	}
	else if (_q_0_is("goto")) {
        gas_jmp(_q.at(1));
	}
	else if (_q_0_is("param")) {
		params.push_back(_q.at(1));
	}
	else if (_q_0_is("call")) {
		std::string funcName = _q.at(1);
		Node func = parser->getGloEnv()->search(funcName);

        std::string _src, _des;

		int pos = 0;
		for (size_t i = 0; i < func.params.size(); ++i) {
			size_t param_size = func.params.at(i).type.size;

            // Ŀ�ĵ�ַ
            _des = std::to_string(pos) + "(%esp)"; pos += param_size;

            if (func.params.at(i).getType().getType() == PTR) {
                gas_ins(mov2stk(4), "$" + params.back(), _des);
                params.pop_back();
                continue;
            }

			std::string _out_str;
			LocVar _loc = searchLocvar(params.back());
			if (!_loc.varName.empty()) {
                getReg("%eax");
                gas_ins(movXXl(_loc.type.size, _loc.type.isUnsig), std::to_string(_loc._off) + "(%ebp)", "%eax");
                _src = reg2stk("%eax", param_size);
			}
			else if(isNumber(params.back())){
                _src = "$" + params.back();
			}
			else if(isTempVar(params.back())){
				TempVar _te = searchTempvar(params.back());
                _src = reg2stk(_te._reg, param_size);
			}
            else {
                Node var = gloEnv->search(params.back());
                gas_ins(movXXl(var.type.size, var.type.isUnsig), "_" + var.varName, "%eax");
                _src = reg2stk("%eax", param_size);
            }

            gas_ins(mov2stk(param_size), _src, _des);
			params.pop_back();
		}
        gas_call(_q.at(1));

        if (_q.size() == 4) {
            setReg("%eax", _q.at(3));
            TempVar var(_q.at(3), "%eax");
            _stk_temp_var.push_back(var);
        }
	}
	else if (_q_0_is("ret")) {
		if (is_main) is_main = false;         // �˳�������

		std::string _src;
		// ��ȡ������������
		int size = currentFunc.type.retType->getSize();

		// ���ұ���
		LocVar ret = searchLocvar(_q.at(1));

		if (ret.varName.empty())
            _src = "$" + _q.at(1);
		else
            _src = std::to_string(ret._off) + "(%ebp)";

        if (isNumber(_q.at(1))) {
            gas_ins(movXXl(4, false), "$" + _q.at(1), "%eax");
        }
        else if (isTempVar(_q.at(1))) {
            TempVar var = searchTempvar(_q.at(1));
            gas_ins("movl", var._reg, "%eax");
        }
        else if (isLocVar(_q.at(1))) {
            LocVar var = searchLocvar(_q.at(1));
            _src = loc_var_val(var._off);
            gas_ins(movXXl(var.type.size, var.type.isUnsig), _src, "%eax");
        }
        else {
            Node var = gloEnv->search(_q.at(1));
            gas_ins(movXXl(var.type.size, var.type.isUnsig), "_" + var.varName, "%eax");
        }

        gas_tab("leave");
        gas_tab(".cfi_restore 5");
        gas_tab(".cfi_def_cfa 4, 4");
        gas_tab("ret");
	}
	else if (_q_0_is(".end")) {
        gas_tab(".cfi_endproc");
	}
	// op
	else{
		getReg(_q);
	}
}
#undef _q_0_is
//void genAsm()

/**
 * ��ջ�д洢
 */
std::string Generate::mov2stk(int size)
{
    switch (size) {
    case 1:return "movb";
    case 2:return "movw";
    case 4:return "movl";
    default:
        error("Var size error.");
        return std::string();
    }
}

/**
 * ��Ĵ����д�ţ�ȫ������Ϊ4�ֽ�
 */
std::string Generate::movXXl(int size, bool isz)
{
    switch (size) {
    case 1:if (isz) return "movzbl";else return "movsbl";
    case 2:if (isz) return "movzwl";else return "movswl";
    case 4:return "movl";
    default:
        error("Var size error.");
        return std::string();
    }
}


std::string Generate::reg2stk(const std::string &_reg, int size)
{
    std::string _r;
    _r.push_back('%');
    switch (size) {
    case 1: _r.push_back(_reg.at(2)); _r.push_back('l'); return _r;
    case 2: _r.push_back(_reg.at(2)); _r.push_back(_reg.at(3)); return _r;
    case 4: return _reg;
    }
}

std::string Generate::mul(int size, bool isunsig)
{
    std::string _r;
    _r.push_back('\t');
    if (!isunsig)
        _r.push_back('i');
    switch (size) {
    case 1: _r += "mulb"; return _r;
    case 2: _r += "mulw"; return _r;
    case 4: _r = "imull"; return _r;
    default:
        error("Var size error.");
        return std::string();
    }
}

/**
 * varɾ��
 */
void Generate::clearRegTemp(const std::string &var)
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (universReg.at(i)._var == var) {
			universReg.at(i)._var.clear();
		}
	}
}


/**
 * ���Ĵ���_reg����Ϊ_var
 */
void Generate::setReg(const std::string &_reg, const std::string &_var)
{
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (universReg.at(i)._reg == _reg) {
			universReg.at(i)._var = _var;
		}
	}
}

std::string Generate::getEmptyReg()
{
    for (size_t i = 0; i < universReg.size(); ++i) {
        if (universReg.at(i)._var.empty())
            return  universReg.at(i)._reg;
    }

    error("reg overflow");
    return std::string();
}

/**
 * ��ȡָ��_reg�Ĵ�������ʱ�Ĵ���Ϊ��
 */
std::string Generate::getReg(const std::string &_reg)
{
	std::string _var;
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (_reg == universReg.at(i)._reg) {
			if (universReg.at(i)._var.empty())
				return _reg;
			else {
				_var = universReg.at(i)._var;
				universReg.at(i)._var.clear();
				break;
			}
		}
	}

	// ���ָ���Ĵ�����Ϊ��, �����Ĵ���
	TempVar _tem = searchTempvar(_var);
	for (size_t i = 0; i < universReg.size(); ++i) {
		if (_reg != universReg.at(i)._reg && universReg.at(i)._var.empty()) {
			universReg.at(i)._var = _var;
			_tem._reg = universReg.at(i)._reg;
			TempVar _pus = _tem;
			_stk_temp_var.pop_back();
			_stk_temp_var.push_back(_pus);

            gas_ins("movl", _reg, _tem._reg);

			return _reg;
		}
	}

	// ����ʧ��
	return std::string();
}


/**
 * @ ��ʱ������ջ�����󶨼Ĵ���
 */
void Generate::push_back_temp_stk(TempVar & tv,const std::string &reg)
{
	setReg(reg, tv._name);
	_stk_temp_var.push_back(tv);
}

/**
 * ��ʱ������ջ�������Ĵ���
 */
void Generate::pop_back_temp_stk(const std::string &var)
{
	clearRegTemp(var);

	if (isNumber(var))
		return;
	if (_stk_temp_var.empty())
		return;

	if (_stk_temp_var.back()._name == var)
		_stk_temp_var.pop_back();
}

bool Generate::isTempVar(const std::string &_t)
{
	TempVar _var = searchTempvar(_t);
	if (!_var._name.empty()) {
		return true;
	}
	return false;
}

bool Generate::isReg(const std::string &_t)
{
    for (size_t i = 0; i < universReg.size(); ++i) {
        if (universReg.at(i)._reg == _t)
            return true;
    }
    return false;
}
bool Generate::isLocVar(const std::string &_l)
{
	LocVar var = searchLocvar(_l);
	if (!var.varName.empty())
		return true;
    
    return false;
}

char Generate::getVarType(std::string &_v)
{
    if (isNumber(_v)) return 'n';
    else if (isLocVar(_v)) return 'l';
    else if (isTempVar(_v)) return 't';
    return 0;
}

/**
 * @��ȡ�ڵ����������
 */
std::string Generate::getTypeString(Type _t)
{
    switch (_t.type) {
    case K_CHAR:
        return "\t.byte\t";
    case K_SHORT:
        return "\t.word\t";
    case K_LONG:
    case K_INT:
        return "\t.long\t";
    case K_FLOAT:
        return "\t.float\t";
    case K_DOUBLE:
        return "\t.double\t";
    default:
        return "\t.long\t";
    }
}

/**
* @berif ��ȡ�����ֲ������Ĵ�С���Դ�������ջ�ռ�Ĵ�С
*/
int Generate::getFuncLocVarSize(Node &n)
{
    int _rsize = -1;
    std::vector<Env *> gloEnv = parser->getGloEnv()->getNext();
    for (size_t i = 0; i < gloEnv.size(); ++i) {
        Env * next = gloEnv.at(i);
        if (n.funcName == next->getName()) {
            getEnvSize(next, _rsize);
        }
    }
    return _rsize;
}
/**
* @berif ��ȡ���������еľֲ������Ĵ�С���������ֲ�������ջ�е�λ�ù�ϵ
*/
void Generate::getEnvSize(Env *_begin, int &_size)
{
    if (_begin == nullptr)
        return;

    bool is_params = false;
    int params_pos = 8;
    if (_size == -1) {
        is_params = true;
        _size = 0;
    }

    for (size_t i = 0; i < _begin->size(); ++i) {
        int _var_size = 0;

        if (_begin->at(i).type.getType() == ARRAY)
            _var_size = _begin->at(i).type.size * _begin->at(i).type._all_len;
        else
            _var_size = _begin->at(i).type.size;

        // ���������ջ�е�λ�ã�ջ��ebp
        if (is_params) {
            _begin->at(i)._off = params_pos;
            params_pos += _var_size;
        }
        else {
            _size += _var_size;
            _begin->at(i)._off = -_size;
        }
    }
    std::vector<Env *> node = _begin->getNext();
    for (size_t i = 0; i < node.size(); ++i) {
        getEnvSize(node.at(i), _size);
    }
}

void getEnvCallSize(Env * _begin, int *size)
{
    if (_begin == nullptr)
        return;
    *size += _begin->_call_size;
    std::vector<Env *> node = _begin->getNext();
    for (size_t i = 0; i < node.size(); ++i) {
        getEnvCallSize(node.at(i), size);
    }
}

int Generate::getFuncCallSize(Node &n)
{
    int _rsize = 0;

    std::vector<Env *> gloEnv = parser->getGloEnv()->getNext();
    for (size_t i = 0; i < gloEnv.size(); ++i) {
        Env * next = gloEnv.at(i);
        // �ҵ��ú���
        if (n.funcName == next->getName()) {
            getEnvCallSize(next, &_rsize);
            return _rsize;
        }
    }
    return 0;
}
std::vector<std::string> Generate::getQuad()
{
    std::vector<std::string> _quad;
    std::string _name;
    bool _is_push = false;

#define _push_name() do{ if (!_is_push && !_name.empty()) {\
	_quad.push_back(_name);\
	_is_push = true;\
	_name.clear();\
	}}while(0)

    for (char c = inf.next();c != 0;c = inf.next()) {

        switch (c) {
        case ':':
            _push_name();
            _quad.push_back(":");
            break;

        case ' ':
            _push_name();
            break;

        case '\n':
            _push_name();
            if (_quad.empty())
                continue;
            return _quad;

        case_0_9:
        case_a_z:
        case_A_Z:
        case '.':
        case '_':
        case '[':
        case ']':
		case '@':
        case_op:
            _name.push_back(c);
            _is_push = false;
            break;

        default:
            _push_name();
            break;
        }
    }
#undef _push_name
    return _quad;
}

std::string Generate::getOutName()
{
    std::string _rstr;
    for (size_t i = 0; i < _infilename.length(); ++i) {
        if (_infilename.at(i) == '.')
            break;
        _rstr.push_back(_infilename.at(i));
    }
    return _rstr + ".s";
}

void Generate::setLocEnv(const std::string &envName) {
    for (size_t i = 0; i < gloEnv->getNext().size(); ++i) {
        if (gloEnv->getNext().at(i)->getName() == envName) {
            locEnv = gloEnv->getNext().at(i);
            return;
        }
    }
    locEnv = nullptr;
}

TempVar &Generate::searchTempvar(const std::string &name)
{
    for (size_t i = 0; i < _stk_temp_var.size(); ++i) {
        if (_stk_temp_var.at(i)._name == name) {
            return _stk_temp_var.at(i);
        }
    }
    TempVar *var = new TempVar();
    return *var;
}

LocVar &Generate::searchLocvar(const std::string &name)
{
    LocVar *var = new LocVar();
    bool isfind = false;
    envUp2DownSearch(locEnv, name, *var, &isfind);
    return *var;
}

/**
 * �����������
 */
void Generate::envUp2DownSearch(Env *_env, const std::string &name, LocVar &var, bool *isfind)
{
    if (!_env) return;

    for (size_t i = 0; i < _env->size(); ++i) {
        if (name == _env->at(i).varName) {
            var = _env->at(i);
            *isfind = true;
            return;
        }
    }

    std::vector<Env *> _env_vec = _env->getNext();
    for (size_t i = 0; i < _env_vec.size(); ++i) {
        envUp2DownSearch(_env_vec.at(i), name, var, isfind);
        if (*isfind) return;
    }
}


