#include "gen.h"
#include "error.h"

#define _operation_(oper)   do{ \
                                _q1_reg = gas_load(_q1);\
                                _q2_reg = gas_load(_q2);\
                                gas_ins(oper, _q1_reg, _q2_reg);\
                                saveAndClear(_q1, _q2, _q3, _q2_reg);\
                            }while(0)

#define _op_shift_(op)     do{\
                                getReg("%edx");\
                                getReg("%ecx");\
                                gas_load(_q1, "%ecx");\
                                gas_load(_q2, "%edx");\
                                gas_ins(op, "%cl", "%edx");\
                                saveAndClear(_q1, _q2, _q3, "%edx");\
                            }while(0)


#define _q_0_is(str)          (_q.at(0) == str)
#define _q1                    _q.at(1)
#define _q2                    _q.at(2)
#define _q3                    _q.at(3)
/**
 * @berif 寄存器分配
 */
void Generate::getReg(std::vector<std::string> &_q)
{
	std::string _q1_reg;
	std::string _q2_reg;

	if (_q_0_is("+")) {
        _operation_("addl");
	}
	else if (_q_0_is("-")) {
        _operation_("subl");
	}
	else if (_q_0_is("*")) {
        getReg("%eax");
        getReg("%edx");

        bool isunsigned = gas_load(_q2, "%eax") | gas_load(_q1, "%edx");
        gas_ins(mul(4, isunsigned), "%edx", "%eax");

        saveAndClear(_q1, _q2, _q3, "%eax");
	}
	else if (_q_0_is("/")) {
		genMulOrModAsm(_q);
	}
	else if (_q_0_is("%")) {
		genMulOrModAsm(_q);
	}
	else if (_q_0_is("&")) {
        _operation_("andl");
	}
	else if (_q_0_is("|")) {
        _operation_("orl");
	}
	else if (_q_0_is("^")) {
        _operation_("xorl");
	}
	else if (_q_0_is(">>")) {
        _op_shift_("sarl");
	}
	else if (_q_0_is("<<")) {
        _op_shift_("sall");
	}
	else if (_q_0_is(">>>")) {
        _op_shift_("shrl");
	}
	else if (_q_0_is("&U")) {
        getReg(std::string("%eax"));

        Node var;
        if (isLocVar(_q1)) {
            var = searchLocvar(_q1);
            gas_ins("leal", loc_var_val(var._off), "%eax");
        }
        else {
            var = gloEnv->search(_q1);
            gas_ins("movl", "$_" + var.varName, "%eax");
        }

        // 保存
        TempVar _temp(_q2, "%eax");
        push_back_temp_stk(_temp, _temp._reg);
	}
	else if (_q_0_is("*U")) {
        // 取其他类型的值还有问题，没有通过指针看出原始类型，需要修改
		getReg("%eax");
        
        Node var;
        if (isLocVar(_q1)) {
            var = searchLocvar(_q1);
            gas_ins(movXXl(var.type.size, var.type.isUnsig), loc_var_val(var._off), "%eax");
        }
        else {
            var = gloEnv->search(_q1);
            gas_ins(movXXl(var.type.size, var.type.isUnsig), "_" + var.varName, "%eax");
        }
        gas_ins(movXXl(var.type.size, var.type.isUnsig), "0(%eax)", "%eax");

		// 保存
        TempVar _temp(_q2, "%eax");
		push_back_temp_stk(_temp, _temp._reg);
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
    else if (_q_0_is(".")) {
        // 取其他类型的值还有问题，没有通过指针看出原始类型，需要修改
        getReg("%eax");

        int size = 0;
        int is_unsig = 0;
        Node var;
        if (isLocVar(_q2)) {
            var = searchLocvar(_q2);
			if (var.type.type == K_STRUCT || var.type.type == K_TYPEDEF) {
				for (size_t i = 0; i < var.type.fields.size(); ++i) {
					if (_q1 == std::to_string(var.type.fields.at(i)._off)) {
						size = var.type.fields.at(i)._type->size;
						is_unsig = var.type.fields.at(i)._type->isUnsig;
						gas_ins(movXXl(size, is_unsig), std::to_string(var._off + atoi(_q1.c_str())) + "(%ebp)", "%eax");
					}
				}
			}
			else if (var.type.type == ARRAY) {
				size = var.type.size;
				is_unsig = var.type.isUnsig;
				gas_ins(movXXl(size, is_unsig), std::to_string(var._off + var.type.size * atoi(_q1.c_str())) + "(%ebp)", "%eax");
			} 
        }
        else {
            var = gloEnv->search(_q2);
            for (size_t i = 0; i < var.type.fields.size(); ++i) {
                if (_q1 == std::to_string(var.type.fields.at(i)._off)) {
                    size = var.type.fields.at(i)._type->size;
                    is_unsig = var.type.fields.at(i)._type->isUnsig;
                }
            }
            gas_ins(movXXl(size, is_unsig), "_" + var.varName + "+" + _q1, "%eax");
        }
        //gas_ins(movXXl(size, is_unsig), _q1 + "(%eax)", "%eax");

        // 保存
        TempVar _temp(_q3, "%eax");
        push_back_temp_stk(_temp, _temp._reg);
    }
    else if (_q_0_is(".=")) {
        int size = 0;
        int is_unsig = 0;
        Node var = searchLocvar(_q2);
		if (var.type.type == K_STRUCT || var.type.type == K_TYPEDEF) {
			for (size_t i = 0; i < var.type.fields.size(); ++i) {
				if (_q1 == std::to_string(var.type.fields.at(i)._off)) {
					size = var.type.fields.at(i)._type->size;
					is_unsig = var.type.fields.at(i)._type->isUnsig;
				}
			}
		}
		else if(var.type.type ==ARRAY) {
			size = var.type.size;
			is_unsig = var.type.isUnsig;
		}
        gas_ins(mov2stk(size), "$" + _q3, std::to_string(var._off + atoi(_q1.c_str())) + "(%ebp)");
    }
    else if (_q_0_is(".&")) {
        // 取其他类型的值还有问题，没有通过指针看出原始类型，需要修改
        getReg("%eax");

        int size = 0;
        int is_unsig = 0;
        Node var;
        if (isLocVar(_q2)) {
			var = searchLocvar(_q2);
			if (var.type.type == K_STRUCT || var.type.type == K_TYPEDEF) {
				for (size_t i = 0; i < var.type.fields.size(); ++i) {
					if (_q1 == std::to_string(var.type.fields.at(i)._off)) {
						size = var.type.fields.at(i)._type->size;
						is_unsig = var.type.fields.at(i)._type->isUnsig;
						gas_ins("leal", std::to_string(var._off + atoi(_q1.c_str())) + "(%ebp)", "%eax");
					}
				}
			}
			else if (var.type.type == ARRAY) {
				size = var.type.size;
				is_unsig = var.type.isUnsig;
				gas_ins("leal", std::to_string(var._off + atoi(_q1.c_str()) * var.type.size) + "(%ebp)", "%eax");
			}
           
        }
        else {
            var = gloEnv->search(_q2);
            for (size_t i = 0; i < var.type.fields.size(); ++i) {
                if (_q1 == std::to_string(var.type.fields.at(i)._off)) {
                    size = var.type.fields.at(i)._type->size;
                    is_unsig = var.type.fields.at(i)._type->isUnsig;
                }
            }
            gas_ins(movXXl(size, is_unsig), "$_" + var.varName + "+" + _q1, "%eax");
        }
        //gas_ins(movXXl(size, is_unsig), _q1 + "(%eax)", "%eax");

        // 保存
        TempVar _temp(_q3, "%eax");
        push_back_temp_stk(_temp, _temp._reg);
    }
    // 浮点运算
    else if (_q_0_is("=f")) {
        gas_flo_load(_q1);
        gas_fstp(_q2);
    }
    else if (_q_0_is("+f")) {
        gas_flo_load(_q1);
        gas_flo_load(_q2);
        //gas_tab("fadds");
        //gas_fstp(_q3);
        error("Not support operator: '-f' ");
    }
    else if (_q_0_is("-f")) {
        error("Not support operator: '-f' ");
    }
    else if (_q_0_is("*f")) {
        error("Not support operator: '*f' ");
    }
    else if (_q_0_is("/f")) {
        error("Not support operator: '/f' ");
    }
    // 不支持的运算
	else if (_q_0_is("[]")) {
		error("Not support operator.");
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

void Generate::genIncDec(const std::string &_obj, const std::string &op)
{
    std::string _des = getEmptyReg();
    LocVar _loc = searchLocvar(_obj);
    if (!_loc.varName.empty()) {
        gas_ins(movXXl(_loc.type.size, _loc.type.isUnsig), loc_var_val(_loc._off), _des);
        gas_tab(op + _des);
        gas_ins(mov2stk(_loc.type.size), reg2stk(_des, _loc.type.size), loc_var_val(_loc._off));
    }
    else if (isTempVar(_obj)) {
        TempVar _tem = searchTempvar(_obj);
        gas_tab(op + _tem._reg);
    }
    else {
        Node var = gloEnv->search(_obj);
        gas_ins(movXXl(var.type.size, var.type.isUnsig), "_" + var.varName, _des);
        gas_tab(op + _des);
        gas_ins(mov2stk(var.type.size), reg2stk(_des, var.type.size), "_" + var.varName);
    }
}

void Generate::genMulOrModAsm(std::vector<std::string> &_q)
{
	std::string _save_reg;                // 保存结果
	if (_q.at(0) == "/") 
        _save_reg = "%eax";
	else 
        _save_reg = "%edx";

	getReg("%eax");
	getReg("%edx");

    gas_load(_q.at(2), "%eax");
    gas_tab("cltd");                 // edx + eax ,扩充为64bits

	if (isNumber(_q.at(1))) {
        getReg("%ecx");
        gas_ins("movl", "$" + _q.at(1), "%ecx");
        gas_tab("idivl   %ecx");
	}
	else if (isLocVar(_q.at(1))) {
		LocVar _l = searchLocvar(_q.at(1));
        gas_tab("idivl\t" + loc_var_val(_l._off));
	}
	else if (isTempVar(_q.at(1))) {
		TempVar _t = searchTempvar(_q.at(1));
        gas_tab("idivl\t" + _t._reg);
	}
    else {
        getReg("%ecx");
        Node var = gloEnv->search(_q.at(1));
        gas_ins(movXXl(var.type.size, var.type.isUnsig), "_" + var.varName, "%ecx");
        gas_tab("idivl  %ecx");
    }

	saveAndClear(_q.at(1), _q.at(2), _q.at(3), _save_reg);
}


/**
 * 运算之后寄存器的清理和数据的保存
 */
void Generate::saveAndClear(std::string &q1, std::string &q2, std::string &q3, const std::string &_reg)
{
	// 先将临时变量和常数出栈
	pop_back_temp_stk(q1);
	pop_back_temp_stk(q2);
	pop_back_temp_stk(q1);

	LocVar _loc = searchLocvar(q3);
	if (!_loc.varName.empty()) {
        gas_ins(movXXl(_loc.type.size, _loc.type.isUnsig), reg2stk(_reg, _loc.type.size), loc_var_val(_loc._off));
	}
	else {
		// 出现在结果的都是第一次
		TempVar _temp;
		_temp._name = q3;
		_temp._reg = _reg;
		push_back_temp_stk(_temp, _reg);
	}
}