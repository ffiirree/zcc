#include "gen.h"
#include "error.h"

#define _q_0_is(str)          (_q.at(0) == str)
#define _q1                    _q.at(1)
#define _q2                    _q.at(2)
#define _q3                    _q.at(3)


void Generate::getReg(std::vector<std::string> &_q)
{
    std::string _q1_reg;
    std::string _q2_reg;

    if (_q_0_is("+")) {
        add_or_sub_op(_q, "addl");
    }
    else if (_q_0_is("-")) {
        add_or_sub_op(_q, "subl");
    }
    else if (_q_0_is("&")) {
        unlimited_binary_op(_q, "andl");
    }
    else if (_q_0_is("|")) {
        unlimited_binary_op(_q, "orl");
    }
    else if (_q_0_is("^")) {
        unlimited_binary_op(_q, "xorl");
    }
    else if (_q_0_is(">>")) {
        shift_op(_q, "sarl");
    }
    else if (_q_0_is("<<")) {
        shift_op(_q, "sall");
    }
    else if (_q_0_is(">>>")) {
        shift_op(_q, "shrl");
    }
    else if (_q_0_is("++")) {
        genIncDec(_q1, "addl    $1, ");
    }
    else if (_q_0_is("--")) {
        genIncDec(_q1, "subl    $1, ");
    }
    else if (_q_0_is("~")) {
        genIncDec(_q1, "notl\t");
    }
    else if (_q_0_is("/")) {
        genMulOrModAsm(_q);
    }
    else if (_q_0_is("%")) {
        genMulOrModAsm(_q);
    }
    else if (_q_0_is("*")) {
        Type _t, _save;
        getReg("%eax");
        getReg("%edx");

        _t = gas_load(_q2, "%eax"); _save.type < _t.type ? _save = _t : true;
        _t = gas_load(_q1, "%edx"); _save.type < _t.type ? _save = _t : true;

        gas_ins(mul(4, _t.isUnsig), "%edx", "%eax");

        temp_clear(_q1, _q2);
        temp_save(_q3, _save, "%eax");
    }
    else if (_q_0_is("&U")) {
        getReg(std::string("%eax"));

        Node var;
        if (isLocVar(_q1)) {
            var = searchLocvar(_q1);
            if (var.kind_ == NODE_GLO_VAR) {
                gas_ins("movl", "$" + var.name(), "%eax");
            }
            else if (var.kind_ == NODE_LOC_VAR) {
                gas_ins("leal", loc_var_val(var.off_), "%eax");
            }
        }
        else {
            error("pre & var error");
        }

        temp_save(_q2, var.type_, "%eax");
    }
    else if (_q_0_is("*U")) {
        getReg("%eax");

        Node var;
        if (isLocVar(_q1)) {
            var = searchLocvar(_q1);
            if (var.kind_ == NODE_GLO_VAR) {
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), var.name(), "%eax");
            }
            else if (var.kind_ == NODE_LOC_VAR) {
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), loc_var_val(var.off_), "%eax");
            }
        }
        else {
            error("pre * var error");
        }
        gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), "0(%eax)", "%eax");

        temp_save(_q2, var.type_, "%eax");
    }
    else if (_q_0_is("*=")) {
        getReg("%eax");

        Node var;
        if (isLocVar(_q1)) {
            var = searchLocvar(_q1);
            if (var.kind_ == NODE_GLO_VAR) {
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), var.name(), "%eax");
            }
            else if (var.kind_ == NODE_LOC_VAR) {
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), loc_var_val(var.off_), "%eax");
            }
        }
        else {
            error("pre * var error");
        }
        gas_ins("leal", "0(%eax)", "%eax");

        temp_save(_q2, var.type_, "%eax");
    }
    // int a = str.b;
    // .  _off  str  .Lvar3
    else if (_q_0_is(".")) {
        getReg("%eax");

        Node var = searchLocvar(_q2);
        Type _ty = getStructFieldType(var, _q1);

        if (var.kind_ == NODE_GLO_VAR)
            gas_ins(movXXl(_ty.size_, _ty.isUnsig), var.name() + "+" + _q1, "%eax");
        else if (var.kind_ == NODE_LOC_VAR)
            gas_ins(movXXl(_ty.size_, _ty.isUnsig), std::to_string(var.off_ + atoi(_q1.c_str())) + "(%ebp)", "%eax");

        // 保存
        TempVar _temp(_q3, "%eax");
        _temp._size = _ty.size_;
        push_back_temp_stk(_temp, _temp._reg);
    }
    //
    else if (_q_0_is(".=")) {
        Node var = searchLocvar(_q2);
        Type _ty = getStructFieldType(var, _q1);
        gas_ins(mov2stk(_ty.size_), "$" + _q3, std::to_string(var.off_ + atoi(_q1.c_str())) + "(%ebp)");
    }
    // 取数组和结构体的某一元素的地址，用来赋值
    else if (_q_0_is(".&")) {
        getReg("%eax");
        Node var = searchLocvar(_q2);
        if (var.kind_ == NODE_GLO_VAR) {
            Type _ty = getStructFieldType(var, _q1);
            gas_ins(movXXl(_ty.size_, _ty.isUnsig), "$" + var.name() + "+" + _q1, "%eax");
        }
        else if (var.kind_ == NODE_LOC_VAR) {
            gas_ins("leal", std::to_string(var.off_ + atoi(_q1.c_str())) + "(%ebp)", "%eax");
        }

        // Save
        TempVar _temp(_q3, "%eax");
        _temp._size = var.type_.size_;
        push_back_temp_stk(_temp, _temp._reg);
    }
    else if (_q_0_is("[]")) {
        getReg("%eax");
        size_t temp_size = 0;

        if (isLocVar(_q2)) {
            Node var = searchLocvar(_q2);
            temp_size = var.type_.size_;
            if (var.kind_ == NODE_LOC_VAR) {

                if (var.getType().getType() == PTR) {
                    Type _ty = getPtrType(var);
                    
                    if (isNumber(_q1)) {
                        gas_ins("movl", loc_var_val(var.off_), "%eax");
                        gas_ins("addl", "$" + std::to_string(_ty.size_ * atoi(_q1.c_str())), "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                    else if (isEnumConst(_q1)) {
                        gas_ins("movl", loc_var_val(var.off_), "%eax");
                        gas_ins("addl", "$" + std::to_string(_ty.size_ * atoi(parser->searchEnum(_q1).c_str())), "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                    else if (isLocVar(_q1)) {
                        Node _loc = searchLocvar(_q1);
                        if (_loc.kind_ == NODE_GLO_VAR) {
                            getReg("%edx");
                            gas_ins("movl", loc_var_val(var.off_), "%eax");
                            gas_ins("movl", _loc.name(), "%edx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%edx");
                            gas_ins("addl", "%edx", "%eax");
                            gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                        }
                        else if (var.kind_ == NODE_LOC_VAR) {
                            getReg("%ecx");
                            gas_ins("movl", loc_var_val(var.off_), "%eax");
                            gas_ins("movl", loc_var_val(_loc.off_), "%ecx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%ecx");
                            gas_ins("addl", "%ecx", "%eax");
                            gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                        }
                    }
                    else if (isTempVar(_q1)) {
                        TempVar _loc = searchTempvar(_q1);
                        gas_ins("movl", loc_var_val(var.off_), "%eax");
                        gas_ins("imull", "$" + std::to_string(var.type_.size_), _loc._reg);
                        gas_ins("addl", _loc._reg, "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                    else {
                        error("error.");
                    }
                }
                else if (var.getType().getType() == ARRAY) {
                    Type _ty = var.getType();
                    if (isNumber(_q1))
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), std::to_string(var.off_ + var.type_.size_ * atoi(_q1.c_str())) + "(%ebp)", "%eax");
                    else if (isEnumConst(_q1))
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), std::to_string(var.off_ + var.type_.size_ * atoi(parser->searchEnum(_q1).c_str())) + "(%ebp)", "%eax");
                    else if (isLocVar(_q1)) {
                        Node _loc = searchLocvar(_q1);
                        if (_loc.kind_ == NODE_GLO_VAR) {
                            getReg("%edx");
                            gas_ins("leal", std::to_string(var.off_) + "(%ebp)", "%eax");
                            gas_ins("movl", _loc.name(), "%edx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%edx");
                            gas_ins("addl", "%edx", "%eax");
                            gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                        }
                        else if (var.kind_ == NODE_LOC_VAR) {
                            getReg("%ecx");
                            gas_ins("leal", std::to_string(var.off_) + "(%ebp)", "%eax");
                            gas_ins("movl", loc_var_val(_loc.off_), "%ecx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%ecx");
                            gas_ins("addl", "%ecx", "%eax");
                            gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                        }
                    }
                    else if (isTempVar(_q1)) {
                        TempVar _loc = searchTempvar(_q1);
                        gas_ins("leal", std::to_string(var.off_) + "(%ebp)", "%eax");
                        gas_ins("imull", "$" + std::to_string(var.type_.size_), _loc._reg);
                        gas_ins("addl", _loc._reg, "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                    else {
                        error("error.");
                    }
                }
            }
            else if (var.kind_ == NODE_GLO_VAR) {

                if (var.getType().getType() == PTR) {
                    Type _ty = getPtrType(var);
                    if (isNumber(_q1)) {
                        gas_ins("movl", var.name(), "%eax");
                        gas_ins("addl", "$" + std::to_string(_ty.size_ * atoi(_q1.c_str())), "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                    else if (isEnumConst(_q1)) {
                        gas_ins("movl", var.name(), "%eax");
                        gas_ins("addl", "$" + std::to_string(_ty.size_ * atoi(parser->searchEnum(_q1).c_str())), "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                    else if (isLocVar(_q1)) {
                        Node _loc = searchLocvar(_q1);
                        if (_loc.kind_ == NODE_GLO_VAR) {
                            getReg("%edx");
                            gas_ins("movl", var.name(), "%eax");
                            gas_ins("movl", _loc.name(), "%edx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%edx");
                            gas_ins("addl", "%edx", "%eax");
                            gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                        }
                        else if (var.kind_ == NODE_LOC_VAR) {
                            getReg("%ecx");
                            gas_ins("movl", var.name(), "%eax");
                            gas_ins("movl", loc_var_val(_loc.off_), "%ecx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%ecx");
                            gas_ins("addl", "%ecx", "%eax");
                            gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                        }
                    }
                    else if (isTempVar(_q1)) {
                        TempVar _loc = searchTempvar(_q1);
                        gas_ins("movl", var.name(), "%eax");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), _loc._reg);
                        gas_ins("addl", _loc._reg, "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                    else {
                        error("error.");
                    }
                }
                else if (var.getType().getType() == ARRAY) {
                    Type _ty = var.getType();

                    if (isNumber(_q1))
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), var.name() + "+" + std::to_string(var.type_.size_ * atoi(_q1.c_str())), "%eax");
                    else if (isEnumConst(_q1))
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), var.name() + "+" + std::to_string(var.type_.size_ * atoi(parser->searchEnum(_q1).c_str())), "%eax");
                    else if (isLocVar(_q1)) {
                        Node _loc = searchLocvar(_q1);
                        getReg("%ecx");
                        gas_ins("leal", var.name(), "%eax");
                        gas_ins("movl", loc_var_val(_loc.off_), "%ecx");
                        gas_ins("imull", "$" + std::to_string(var.type_.size_), "%ecx");
                        gas_ins("addl", "%ecx", "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                    else if (isTempVar(_q1)) {
                        TempVar _loc = searchTempvar(_q1);
                        gas_ins("leal", var.name(), "%eax");
                        gas_ins("imull", "$" + std::to_string(var.type_.size_), _loc._reg);
                        gas_ins("addl", _loc._reg, "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                    else {
                        Node _glo = gloEnv->search(_q1);
                        getReg("%edx");
                        gas_ins("leal", var.name(), "%eax");
                        gas_ins("movl", _glo.name(), "%edx");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), "%edx");
                        gas_ins("addl", "%edx", "%eax");
                        gas_ins(movXXl(_ty.size_, _ty.isUnsig), "(%eax)", "%eax");
                    }
                }

            }
        }

        TempVar _temp(_q3, "%eax");
        _temp._size = temp_size;
        push_back_temp_stk(_temp, _temp._reg);
    }
    else if (_q_0_is("[]=")) {
        Node var = searchLocvar(_q2);
        gas_ins(mov2stk(var.type_.size_), "$" + _q3, std::to_string(var.off_ + atoi(_q1.c_str())) + "(%ebp)");
    }

    else if (_q_0_is("[]&")) {
        getReg("%eax");

        Node var;
        if (isLocVar(_q2)) {
            var = searchLocvar(_q2);

            if (var.kind_ == NODE_LOC_VAR) {

                if (var.getType().getType() == PTR) {
                    Type _ty = getPtrType(var);
                    if (isNumber(_q1)) {
                        gas_ins("movl", loc_var_val(var.off_), "%eax");
                        gas_ins("addl", "$" + std::to_string(_ty.size_ * atoi(_q1.c_str())), "%eax");
                    }
                    else if (isEnumConst(_q1)) {
                        gas_ins("movl", loc_var_val(var.off_), "%eax");
                        gas_ins("addl", "$" + std::to_string(_ty.size_ * atoi(parser->searchEnum(_q1).c_str())), "%eax");
                    }
                    else if (isLocVar(_q1)) {
                        Node _loc = searchLocvar(_q1);
                        if (_loc.kind_ == NODE_GLO_VAR) {
                            getReg("%edx");
                            gas_ins("movl", loc_var_val(var.off_), "%eax");
                            gas_ins("movl", _loc.name(), "%edx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%edx");
                            gas_ins("addl", "%edx", "%eax");
                        }
                        else if (var.kind_ == NODE_LOC_VAR) {
                            getReg("%ecx");
                            gas_ins("movl", loc_var_val(var.off_), "%eax");
                            gas_ins("movl", loc_var_val(_loc.off_), "%ecx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%ecx");
                            gas_ins("addl", "%ecx", "%eax");
                        }
                    }
                    else if (isTempVar(_q1)) {
                        TempVar _loc = searchTempvar(_q1);
                        gas_ins("movl", loc_var_val(var.off_), "%eax");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), _loc._reg);
                        gas_ins("addl", _loc._reg, "%eax");
                    }
                    else {
                        error("error.");
                    }
                }
                else if (var.getType().getType() == ARRAY) {
                    Type _ty = var.getType();

                    if (isNumber(_q1))
                        gas_ins("leal", std::to_string(var.off_ + _ty.size_ * atoi(_q1.c_str())) + "(%ebp)", "%eax");
                    else if (isEnumConst(_q1))
                        gas_ins("leal", std::to_string(var.off_ + _ty.size_ * atoi(parser->searchEnum(_q1).c_str())) + "(%ebp)", "%eax");
                    else if (isLocVar(_q1)) {
                        Node _loc = searchLocvar(_q1);
                        getReg("%ecx");
                        gas_ins("leal", std::to_string(var.off_) + "(%ebp)", "%eax");
                        gas_ins("movl", loc_var_val(_loc.off_), "%ecx");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), "%ecx");
                        gas_ins("addl", "%ecx", "%eax");
                    }
                    else if (isTempVar(_q1)) {
                        TempVar _loc = searchTempvar(_q1);
                        gas_ins("leal", std::to_string(var.off_) + "(%ebp)", "%eax");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), _loc._reg);
                        gas_ins("addl", _loc._reg, "%eax");
                    }
                    else {
                        Node _glo = gloEnv->search(_q1);
                        getReg("%edx");
                        gas_ins("leal", std::to_string(var.off_) + "(%ebp)", "%eax");
                        gas_ins("movl", _glo.name(), "%edx");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), "%edx");
                        gas_ins("addl", "%edx", "%eax");
                    }
                }
            }
            else if (var.kind_ == NODE_GLO_VAR) {

                if (var.getType().getType() == PTR) {
                    Type _ty = getPtrType(var);
                    if (isNumber(_q1)) {
                        gas_ins("movl", var.name(), "%eax");
                        gas_ins("addl", "$" + std::to_string(_ty.size_ * atoi(_q1.c_str())), "%eax");
                    }
                    else if (isEnumConst(_q1)) {
                        gas_ins("movl", var.name(), "%eax");
                        gas_ins("addl", "$" + std::to_string(_ty.size_ * atoi(parser->searchEnum(_q1).c_str())), "%eax");
                    }
                    else if (isLocVar(_q1)) {
                        Node _loc = searchLocvar(_q1);
                        if (_loc.kind_ == NODE_GLO_VAR) {
                            getReg("%edx");
                            gas_ins("movl", var.name(), "%eax");
                            gas_ins("movl", _loc.name(), "%edx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%edx");
                            gas_ins("addl", "%edx", "%eax");
                        }
                        else if (var.kind_ == NODE_LOC_VAR) {
                            getReg("%ecx");
                            gas_ins("movl", var.name(), "%eax");
                            gas_ins("movl", loc_var_val(_loc.off_), "%ecx");
                            gas_ins("imull", "$" + std::to_string(var.type_.size_), "%ecx");
                            gas_ins("addl", "%ecx", "%eax");
                        }
                    }
                    else if (isTempVar(_q1)) {
                        TempVar _loc = searchTempvar(_q1);
                        gas_ins("movl", var.name(), "%eax");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), _loc._reg);
                        gas_ins("addl", _loc._reg, "%eax");
                    }
                    else {
                        error("error.");
                    }
                }
                else if (var.getType().getType() == ARRAY) {
                    Type _ty = var.getType();

                    if (isNumber(_q1))
                        gas_ins("leal", var.name() + "+" + std::to_string(_ty.size_ * atoi(_q1.c_str())), "%eax");
                    else if (isEnumConst(_q1))
                        gas_ins("leal", var.name() + "+" + std::to_string(_ty.size_ * atoi(parser->searchEnum(_q1).c_str())), "%eax");
                    else if (isLocVar(_q1)) {
                        Node _loc = searchLocvar(_q1);
                        getReg("%ecx");
                        gas_ins("leal", var.name(), "%eax");
                        gas_ins("movl", loc_var_val(_loc.off_), "%ecx");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), "%ecx");
                        gas_ins("addl", "%ecx", "%eax");
                    }
                    else if (isTempVar(_q1)) {
                        TempVar _loc = searchTempvar(_q1);
                        gas_ins("leal", var.name(), "%eax");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), _loc._reg);
                        gas_ins("addl", _loc._reg, "%eax");
                    }
                    else {
                        Node _glo = gloEnv->search(_q1);
                        getReg("%edx");
                        gas_ins("leal", var.name(), "%edx");
                        gas_ins("movl", _glo.name(), "%eax");
                        gas_ins("imull", "$" + std::to_string(_ty.size_), "%eax");
                        gas_ins("addl", "%edx", "%eax");
                    }
                }

            }
        }

        TempVar _temp(_q3, "%eax");
        _temp._size = var.type_.size_;
        push_back_temp_stk(_temp, _temp._reg);
    }
    else if (_q_0_is("->")) {
        getReg("%eax");

        Node var = searchLocvar(_q2);
        Type _ty = getPtrType(var);

        if (var.kind_ == NODE_GLO_VAR) {
            gas_ins("movl", "$" + var.name(), "%eax");
        }
        else if (var.kind_ == NODE_LOC_VAR) {
            gas_ins("movl", loc_var_val(var.off_), "%eax");
        }

        gas_ins("addl", "$" + _q1, "%eax");

        // 取值
        if (_ty.getType() == K_STRUCT) {
            Type _struct_ty;

            for (size_t i = 0; i < _ty.fields.size(); ++i) {
                if (_q1 == std::to_string(_ty.fields.at(i)._off)) {
                    _struct_ty = *(_ty.fields.at(i)._type);
                }
            }
            gas_ins(movXXl(_struct_ty.size_, _struct_ty.isUnsig), "(%eax)", "%eax");
        }


        // 保存
        TempVar _temp(_q3, "%eax");
        _temp._size = _ty.size_;
        push_back_temp_stk(_temp, _temp._reg);
    }
    else if (_q_0_is("->&")) {
        getReg("%eax");

        Node var = searchLocvar(_q2);
        Type _ty = getPtrType(var);

        if (var.kind_ == NODE_GLO_VAR) {
            gas_ins("movl", "$" + var.name(), "%eax");
        }
        else if (var.kind_ == NODE_LOC_VAR) {
            gas_ins("movl", loc_var_val(var.off_), "%eax");
        }

        gas_ins("addl", "$" + _q1, "%eax");

        TempVar _temp(_q3, "%eax");
        _temp._size = _ty.size_;
        push_back_temp_stk(_temp, _temp._reg);
    }
    // float
    else if (_q_0_is("=f") || _q_0_is("=d")) {
        gas_flo_load(_q1, false);
        gas_fstp(_q2);
        finit = false;
    }
    else if (_q_0_is("+f")) {
        int _save = 0, _t;

        _t = gas_flo_load(_q1, false); if(_t > _save ) _save = _t;
        _t = gas_flo_load(_q2, false); if(_t > _save ) _save = _t;
        gas_tab("faddp");

        finit = false;
        temp_save(_q3, _save);
    }
    else if (_q_0_is("-f")) {
        int _save = 0, _t;

        _t = gas_flo_load(_q1, false); if(_t > _save ) _save = _t;
        _t = gas_flo_load(_q2, true); if(_t > _save ) _save = _t;

        gas_tab("fsubp");

        finit = false;
        temp_save(_q3, _save);
    }
    else if (_q_0_is("*f")) {
        int _save = 0, _t;

        _t = gas_flo_load(_q1, false); if(_t > _save ) _save = _t;
        _t = gas_flo_load(_q2, false); if(_t > _save ) _save = _t;
        gas_tab("fmulp");

        finit = false;
        temp_save(_q3, _save);
    }
    else if (_q_0_is("/f")) {
        int _save = 0, _t;

        _t = gas_flo_load(_q1, false); if(_t > _save ) _save = _t;
        _t = gas_flo_load(_q2, true); if(_t > _save ) _save = _t;
        gas_tab("fdivp");

        finit = false;
        temp_save(_q3, _save);
    }
    else if (_q_0_is("+U")) {
        error("Not support operator.");
    }
    else if (_q_0_is("-U")) {
        error("Not support operator.");
    }
    else {
        error("Not support operator.");
    }
}
#undef _q_0_is
#undef _q1
#undef _q2
#undef _q3

/**
* @berif 自增自减， ++ a
*/
void Generate::genIncDec(const std::string &_obj, const std::string &op)
{
    std::string _des = getEmptyReg();

    if (isLocVar(_obj)) {
        Node var = searchLocvar(_obj);

        if (var.type_.getType() == PTR) {
            if (var.kind_ == NODE_GLO_VAR) {
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), var.name(), _des);
                if (op.at(0) == 'a')
                    gas_ins("addl", "$" + std::to_string(var.type_.ptr->size_), _des);
                else
                    gas_ins("subl", "$" + std::to_string(var.type_.ptr->size_), _des);
                gas_ins(mov2stk(var.type_.size_), reg2stk(_des, var.type_.size_), var.name());
            }
            else if (var.kind_ == NODE_LOC_VAR) {
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), loc_var_val(var.off_), _des);
                if (op.at(0) == 'a')
                    gas_ins("addl", "$" + std::to_string(var.type_.ptr->size_), _des);
                else
                    gas_ins("subl", "$" + std::to_string(var.type_.ptr->size_), _des);
                gas_ins(mov2stk(var.type_.size_), reg2stk(_des, var.type_.size_), loc_var_val(var.off_));
            }
        }
        else {
            if (var.kind_ == NODE_GLO_VAR) {
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), var.name(), _des);
                gas_tab(op + _des);
                gas_ins(mov2stk(var.type_.size_), reg2stk(_des, var.type_.size_), var.name());
            }
            else if (var.kind_ == NODE_LOC_VAR) {
                gas_ins(movXXl(var.type_.size_, var.type_.isUnsig), loc_var_val(var.off_), _des);
                gas_tab(op + _des);
                gas_ins(mov2stk(var.type_.size_), reg2stk(_des, var.type_.size_), loc_var_val(var.off_));
            }
        }
    }
    else if (isTempVar(_obj)) {
        TempVar _tem = searchTempvar(_obj);
        gas_tab(op + _tem._reg);
    }
}

void Generate::genMulOrModAsm(std::vector<std::string> &_q)
{
    Type _save, _t;
    std::string _save_reg;                // 保存结果
    if (_q.at(0) == "/")
        _save_reg = "%eax";
    else
        _save_reg = "%edx";

    getReg("%eax");
    getReg("%edx");

    // 除数放入eax
    _t = gas_load(_q.at(2), "%eax"); _save.type < _t.type ? _save = _t : true;
    gas_tab("cltd");                 // edx + eax ,扩充为64bits

                                     // 被除数
    if (isNumber(_q.at(1))) {
        getReg("%ecx");

        _t.create(K_INT, 4, false);

        gas_ins("movl", "$" + _q.at(1), "%ecx");
        gas_ins("idivl", "%ecx");
    }
    else if (isEnumConst(_q.at(1))) {
        getReg("%ecx");

        _t.create(K_INT, 4, false);

        gas_ins("movl", "$" + parser->searchEnum(_q.at(1)), "%ecx");
        gas_ins("idivl", "%ecx");
    }
    else if (isLocVar(_q.at(1))) {
        LocVar _l = searchLocvar(_q.at(1));

        if (_l.kind_ == NODE_GLO_VAR) {
            getReg("%ecx");
            gas_ins(movXXl(_l.type_.size_, _l.type_.isUnsig), _l.name(), "%ecx");
            gas_ins("idivl", "%ecx");
        }
        else if (_l.kind_ == NODE_LOC_VAR) {
            gas_ins("idivl", loc_var_val(_l.off_));
        }
        _t = _l.type_;
    }
    else if (isTempVar(_q.at(1))) {
        TempVar _temp = searchTempvar(_q.at(1));

        _t.create(_temp.type, _temp._size, _temp._isUnsig);

        gas_ins("idivl", _temp._reg);
    }

    _save.type < _t.type ? _save = _t : true;

    temp_clear(_q.at(1), _q.at(2));
    temp_save(_q.at(3), _save, _save_reg);
}