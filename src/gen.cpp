#include "gen.h"
#include "error.h"


Generate::Generate(Parser *p, VirtualMachine *vm) :vm_(vm)
{
    parser = p;
    _infilename = p->getQuadrupleFileName();

    inf.open(_infilename);

    out.open(getOutName());
    if (!out.is_open())
        error("Open file failed.");

    // init register
    reg_init();
    run();
}

void Generate::reg_init()
{
    //32bits register
    universReg.push_back({ "%eax" });
    universReg.push_back({ "%ebx" });
    universReg.push_back({ "%ecx" });
    universReg.push_back({ "%edx" });

    float_reg.push_back({ "st(7)" });
    float_reg.push_back({ "st(6)" });
    float_reg.push_back({ "st(5)" });
    float_reg.push_back({ "st(4)" });
    float_reg.push_back({ "st(3)" });
    float_reg.push_back({ "st(2)" });
    float_reg.push_back({ "st(1)" });
    float_reg.push_back({ "st(0)" });
}

void Generate::run()
{
    gas("\t.file\t\"" + _infilename + "\"");

    // string const
    const_str();

    // global var
    locEnv = gloEnv = parser->getGloEnv();
    bool is_fir_var = true;
    for (size_t i = 0; i < gloEnv->size(); ++i) {
        Node n = gloEnv->at(i);
        if (n.kind_ == NODE_GLO_VAR && n.params.empty()) {
            if (n.lvarinit_.empty()) {
                gas_dec(n.name(), n.type_.size_);
            }
            else {
                switch (n.type_.type) {
                case ARRAY: gas_def_arr(n, is_fir_var); break;
                case K_FLOAT:
                case K_DOUBLE:gas_def_flo(n.name(), n.type_.size_, n.lvarinit_.at(0)->sval_, is_fir_var);break;
                case K_TYPEDEF:
                case K_STRUCT: gas_custom(n, is_fir_var);break;
                case K_UNION: error("Unsupport union."); break;
                default:gas_def_int(n.name(), n.type_.size_, n.lvarinit_.at(0)->int_val, is_fir_var);break;
                }
            }

            is_fir_var = false;
        }
    }

    // function
    for (;;) {
        std::vector<std::string> quad = getQuad();

        if (quad.empty())
            break;
        generate(quad);
    }

    gas(parser->newLabel("FE") + ":");

    // float number
    std::vector<std::string> floatConst = parser->getFloatConst();
    if (!floatConst.empty())
        gas_tab(".section .rodata");

    for (size_t i = floatConst.size() / 3; i > 0; --i) {
        std::string size = floatConst.back(); floatConst.pop_back();
        gas_label(floatConst.back()); floatConst.pop_back();

        if (size == "4f") {
            gas_tab(".float " + floatConst.back()); floatConst.pop_back();
        }
        else if (size == "8f") {
            gas_tab(".double " + floatConst.back()); floatConst.pop_back();
        }
        else {
            error("unknown flaot size.");
        }
    }

    gas("\t.ident \"zcc 0.0.1\"");
}

void Generate::const_str()
{
    if (parser->getStrTbl().empty())
        return;

    gas("\t.section  .rodata");
    std::vector<StrCard> strTbl = parser->getStrTbl();
    for (size_t i = strTbl.size(); i > 0; --i) {

        gas_label(strTbl.back()._label);

        out << "\t.ascii \"" << strTbl.back()._str << "\\0\"" << std::endl;
        strTbl.pop_back();
    }
}

void Generate::func_decl(Node &n)
{
    int size = n.local_vars_stk_size_ + n.max_call_params_size_ * 2;

    gas_tab(".text");
    gas_glo(n.name());
    gas_label(n.name());
    if (vm_->use_) vm_->setFuncAddr(n.name());

    gas_tab(".cfi_startproc");
    gas_ins("pushl", "%ebp");
    gas_tab(".cfi_def_cfa_offset 8");
    gas_tab(".cfi_offset 5, -8");
    gas_ins("movl", "%esp", "%ebp");
    gas_tab(".cfi_def_cfa_register 5");

    if (size > 0) {
        int stk_size = ((size + 8 > 16) ? size + 8 : 16);
        gas_ins("subl", "$" + std::to_string(stk_size), "%esp");
    }
}

#define _q_0_is(str) (_q.at(0) == str)
void Generate::generate(std::vector<std::string> &_q)
{
    if (_q.size() == 2 && _q.at(1) == ":") {
        Node r = parser->getGloEnv()->search(_q.at(0));

        if (r.kind_ == NODE_FUNC) {
            currentFunc = r;
            func_decl(r);
            return;
        }

        gas_jxx_label(_q.at(0));
    }
    else if (_q_0_is(".inscope")) {
        setLocEnv(_q.at(1));
    }
    else if (_q_0_is(".outscope")) {
        if (locEnv->pre() != nullptr)
            locEnv = locEnv->pre();
    }
    else if (_q_0_is("clr")) {
        for (size_t i = 0; i < universReg.size(); ++i)
            universReg.at(i)._var.clear();

        for (size_t i = _stk_temp_var.size(); i > 0; --i)
            _stk_temp_var.pop_back();

        // FPU
        if (!finit) {
            gas_tab("finit");
            _stk_float_temp_var.clear();
            finit = true;
        }
    }
    else if (_q_0_is("=")) {
        std::string _src, _des;
        int _des_size = 0;

        if (isLocVar(_q.at(2))) {
            LocVar var = searchLocvar(_q.at(2));

            if (var.kind_ == NODE_GLO_VAR) {
                if (var.type_.type == K_FLOAT || var.type_.type == K_DOUBLE) {
                    gas_fstp(var.name());
                    return;
                }

                if (!var.name().empty()) {
                    _des_size = var.type_.size_;
                    _des = var.name();
                }
            }
            else if (var.kind_ == NODE_LOC_VAR) {
                if (var.type_.type == K_FLOAT || var.type_.type == K_DOUBLE) {
                    gas_fstp(var.name());
                    return;
                }
                else {
                    _des_size = var.type_.size_;
                    _des = loc_var_val(var.off_);
                }
            }
        }
        else if (isTempVar(_q.at(2))) {
            TempVar _tem = searchTempvar(_q.at(2));
            _des_size = 4;
            _des = "(" + _tem._reg + ")";
        }

        TempVar _temp = searchTempvar(_q.at(1));
        if (!_temp._name.empty()) {
            gas_ins(mov2stk(_des_size), reg2stk(_temp._reg, _des_size), _des);
        }
        else if (isNumber(_q.at(1))) {
            gas_ins(mov2stk(_des_size), "$" + _q.at(1), _des);
        }
        else if (isEnumConst(_q.at(1))) {
            gas_ins(mov2stk(_des_size), "$" + parser->searchEnum(_q.at(1)), _des);
        }
        else if (isLocVar(_q.at(1))) {
            Node var = searchLocvar(_q.at(1));

            if (var.kind_ == NODE_GLO_VAR) {
                std::string _reg = getEmptyReg();
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), var.name(), _reg);
                gas_ins(mov2stk(_des_size), reg2stk(_reg, _des_size), _des);
            }
            else if (var.kind_ == NODE_LOC_VAR) {
                std::string _reg = getEmptyReg();
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), loc_var_val(var.off_), _reg);
                gas_ins(mov2stk(_des_size), reg2stk(_reg, _des_size), _des);
            }
        }
    }
    else if (_q_0_is("if")) {
        Type _t1, _t2;
        std::string _q1 = _q.at(3);
        std::string _q2 = _q.at(1);

        std::string _q1_reg;
        std::string _q2_reg;

        _q1_reg = getEmptyReg();
        _t1 = gas_load(_q1, _q1_reg);
        _q2_reg = getEmptyReg();
        _t2 = gas_load(_q2, _q2_reg); _t1.type < _t2.type ? _t1 = _t2 : true;

        if (_t1.type == K_FLOAT || _t1.type == K_DOUBLE) {
            if (isFloatTemVar(_q2))
                gas_tab("fxch	%st(1)");
            gas_tab("fucompp");
            getReg("%eax");
            gas_tab("fnstsw	%ax");
            gas_tab("sahf");
            gas_jxx(_q.at(2), _q.at(5), _t1);
        }
        else {
            gas_ins("cmpl", _q1_reg, _q2_reg);
            gas_jxx(_q.at(2), _q.at(5), _t1);
        }
        temp_clear(_q1, _q2);
    }
    else if (_q_0_is("goto")) {
        gas_ins("jmp", _q.at(1));
    }
    else if (_q_0_is("param")) {

        params.push_back(std::tuple<std::string, int, int>(_q.at(1) , atoi(_q.at(2).c_str()), atoi(_q.at(3).c_str())));
    }
    else if (_q_0_is("call")) {
        std::string funcName = _q.at(1);
        Node func = parser->getGloEnv()->search(funcName);

        std::string _src, _des;

        int pos = 0;
        for (size_t i = params.size(); i > 0; --i) {
            size_t param_size = std::get<2>(params.at(i-1));

            _des = std::to_string(pos) + "(%esp)"; pos += param_size;

            if (std::get<1>(params.at(i-1)) == PTR) {
                gas_ins(mov2stk(4), "$" + std::get<0>(params.at(i - 1)), _des);
                continue;
            }

            std::string _out_str;

            if (isLocVar(std::get<0>(params.at(i - 1)))) {
                LocVar var = searchLocvar(std::get<0>(params.at(i - 1)));
                if (var.kind_ == NODE_GLO_VAR) {
                    gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), var.name(), "%eax");
                    _src = reg2stk("%eax", param_size);
                }
                else if (var.kind_ == NODE_LOC_VAR) {
                    getReg("%eax");
                    if (var.type_.type == K_FLOAT || var.type_.type == K_DOUBLE) {
                        gas_tab(gas_fld(var.type_.size_, var.type_.type) + "\t" + std::to_string(var.off_) + "(%ebp)");
                        gas_tab("fstpl\t" + _des);
                        continue;
                    }

                    gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), std::to_string(var.off_) + "(%ebp)", "%eax");
                    _src = reg2stk("%eax", param_size);
                }

            }
            else if (isNumber(std::get<0>(params.at(i - 1)))) {
                _src = "$" + std::get<0>(params.at(i - 1));
            }
            else if (isEnumConst(std::get<0>(params.at(i - 1)))) {
                _src = "$" + parser->searchEnum(std::get<0>(params.at(i - 1)));
            }
            else if (isTempVar(std::get<0>(params.at(i - 1)))) {
                TempVar _te = searchTempvar(std::get<0>(params.at(i - 1)));
                _src = reg2stk(_te._reg, _te._size);
            }

            gas_ins(mov2stk(param_size), _src, _des);
        }
        gas_call(_q.at(1));
        params.clear();

        if (_q.size() == 4) {
            setReg("%eax", _q.at(3));
            TempVar var(_q.at(3), "%eax");
            if (func.getKind() == NODE_FUNC) {
                var.type = func.type_.retType->type;
                var._isUnsig = func.type_.retType->isUnsig;
                var._size = func.type_.retType->size_;
            }
            else if (func.getKind() == NODE_FUNC_DECL) {
                var.type = func.type_.type;
                var._isUnsig = func.type_.isUnsig;
                var._size = func.type_.size_;
            }
            _stk_temp_var.push_back(var);
        }
    }
    else if (_q_0_is("ret")) {
        if (_q.size() > 1) {
            std::string _src;
            int size = currentFunc.type_.retType->getSize();

            LocVar ret = searchLocvar(_q.at(1));

            if (ret.name().empty())
                _src = "$" + _q.at(1);
            else
                _src = std::to_string(ret.off_) + "(%ebp)";

            if (isNumber(_q.at(1))) {
                gas_ins(movXXl(4, false), "$" + _q.at(1), "%eax");
            }
            else if (isEnumConst(_q.at(1))) {
                gas_ins(movXXl(4, false), "$" + parser->searchEnum(_q.at(1)), "%eax");
            }
            else if (isTempVar(_q.at(1))) {
                TempVar var = searchTempvar(_q.at(1));
                gas_ins("movl", var._reg, "%eax");
            }
            else if (isLocVar(_q.at(1))) {
                LocVar var = searchLocvar(_q.at(1));

                if (var.kind_ == NODE_GLO_VAR) {
                    gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), var.name(), "%eax");
                }
                else if (var.kind_ == NODE_LOC_VAR) {
                    _src = loc_var_val(var.off_);
                    gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), _src, "%eax");
                }
            }
       }

        gas_tab("leave");
        if (vm_->use_) vm_->push_back({ vm_->getInsByOp("leave"), "leave" });
        gas_tab(".cfi_restore 5");
        gas_tab(".cfi_def_cfa 4, 4");
        gas_tab("ret");
        if (vm_->use_) vm_->push_back({ vm_->getInsByOp("ret"), "ret" });
    }
    else if (_q_0_is(".end")) {
        gas_tab(".cfi_endproc");
    }
    // op
    else {
        getReg(_q);
    }
}
#undef _q_0_is
//void genAsm()

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
    return std::string();               // for warning
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

void Generate::clearRegTemp(const std::string &var)
{
    for (size_t i = 0; i < universReg.size(); ++i) {
        if (universReg.at(i)._var == var) {
            universReg.at(i)._var.clear();
        }
    }
}

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

    // 如果指定寄存器不为空, 调整寄存器
    TempVar &_tem = searchTempvar(_var);
    for (size_t i = 0; i < universReg.size(); ++i) {
        if (_reg != universReg.at(i)._reg && universReg.at(i)._var.empty()) {
            universReg.at(i)._var = _var;
            _tem._reg = universReg.at(i)._reg;

            gas_ins("movl", _reg, _tem._reg);

            return _reg;
        }
    }

    // failed
    return std::string();
}

void Generate::push_back_temp_stk(TempVar & tv, const std::string &reg)
{
    setReg(reg, tv._name);
    _stk_temp_var.push_back(tv);
}

void Generate::pop_back_temp_stk(const std::string &var)
{
    clearRegTemp(var);

    if (isNumber(var))
        return;
    if (isEnumConst(var))
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
bool Generate::isFloatTemVar(const std::string &_t)
{
    TempVar _var = searchFloatTempvar(_t);
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
    if (!var.name().empty())
        return true;

    return false;
}
bool Generate::isEnumConst(const std::string &_l)
{
    std::string enu = parser->searchEnum(_l);
    if (!enu.empty())
        return true;
    return false;
}

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
    _infilename.pop_back();
    _infilename.push_back('s');
    return _infilename;
}

void Generate::setLocEnv(const std::string &envName)
{
    std::vector<Env*> ptr = locEnv->getNext();
    for (size_t i = 0; i < ptr.size(); ++i) {
        if (ptr.at(i)->getName() == envName) {
            locEnv = ptr.at(i);
            return;
        }
    }
    error("Not find scope : " + envName);
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

TempVar &Generate::searchFloatTempvar(const std::string &name)
{
    for (size_t i = 0; i < _stk_float_temp_var.size(); ++i) {
        if (_stk_float_temp_var.at(i)._name == name) {
            return _stk_float_temp_var.at(i);
        }
    }
    TempVar *var = new TempVar();
    return *var;
}

LocVar &Generate::searchLocvar(const std::string &name)
{
    return locEnv->search(name);
}

