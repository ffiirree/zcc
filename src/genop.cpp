#include "gen.h"
#include "error.h"



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
		unlimited_binary_op(_q, "addl");
	}
	else if (_q_0_is("-")) {
		unlimited_binary_op(_q, "subl");
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
            gas_ins("leal", loc_var_val(var._off), "%eax");
        }
        else {
            var = gloEnv->search(_q1);
            gas_ins("movl", "$_" + var.varName, "%eax");
        }

		temp_save(_q2, var.type, "%eax");
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

		temp_save(_q2, var.type, "%eax");
	}

	// 取数组和结构体中的值
    else if (_q_0_is(".")) {
        // 取其他类型的值还有问题，没有通过指针看出原始类型，需要修改
        getReg("%eax");

        int size = 0;
        bool is_unsig = 0;
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

                if(isNumber(_q1))
                    gas_ins(movXXl(size, is_unsig), std::to_string(var._off + var.type.size * atoi(_q1.c_str())) + "(%ebp)", "%eax");
                else if(isEnumConst(_q1))
                    gas_ins(movXXl(size, is_unsig), std::to_string(var._off + var.type.size * atoi(parser->searchEnum(_q1).c_str())) + "(%ebp)", "%eax");
                else if (isLocVar(_q1)) {
                    Node _loc = searchLocvar(_q1);
                    getReg("%ecx");
                    gas_ins("leal", std::to_string(var._off) + "(%ebp)", "%eax");
                    gas_ins("movl", loc_var_val(_loc._off), "%ecx");
                    gas_ins("imull", "$" + std::to_string(var.type.size), "%ecx");
                    gas_ins("addl", "%ecx", "%eax");
                    gas_ins(movXXl(size, is_unsig), "(%eax)", "%eax");
                }
                else if (isTempVar(_q1)) {
                    TempVar _loc = searchTempvar(_q1);
                    gas_ins("leal", std::to_string(var._off) + "(%ebp)", "%eax");
                    gas_ins("imull", "$" + std::to_string(var.type.size), _loc._reg);
                    gas_ins("addl", _loc._reg, "%eax");
                    gas_ins(movXXl(size, is_unsig), "(%eax)", "%eax");
                }
                else {
                    Node _glo = gloEnv->search(_q1);
                    getReg("%edx");
                    gas_ins("leal", std::to_string(var._off) + "(%ebp)", "%eax");
                    gas_ins("movl", "_" + _glo.varName, "%edx");
                    gas_ins("imull", "$" + std::to_string(var.type.size), "%edx");
                    gas_ins("addl", "%edx", "%eax");
                    gas_ins(movXXl(size, is_unsig), "(%eax)", "%eax");
                }
			} 
        }
        else {
            var = gloEnv->search(_q2);
			if (var.type.type == K_STRUCT || var.type.type == K_TYPEDEF) {
				for (size_t i = 0; i < var.type.fields.size(); ++i) {
					if (_q1 == std::to_string(var.type.fields.at(i)._off)) {
						size = var.type.fields.at(i)._type->size;
						is_unsig = var.type.fields.at(i)._type->isUnsig;
					}

				}
				gas_ins(movXXl(size, is_unsig), "_" + var.varName + "+" + _q1, "%eax");
			}
			else if (var.type.type == ARRAY) {
				size = var.type.size;
				is_unsig = var.type.isUnsig;

				if (isNumber(_q1))
					gas_ins(movXXl(size, is_unsig), "_" + var.varName + "+" + std::to_string(var.type.size * atoi(_q1.c_str())), "%eax");
				else if (isEnumConst(_q1))
					gas_ins(movXXl(size, is_unsig), "_" + var.varName + "+" + std::to_string(var.type.size * atoi(parser->searchEnum(_q1).c_str())), "%eax");
				else if (isLocVar(_q1)) {
					Node _loc = searchLocvar(_q1);
					getReg("%ecx");
					gas_ins("leal", "_" + var.varName, "%eax");
					gas_ins("movl", loc_var_val(_loc._off), "%ecx");
					gas_ins("imull", "$" + std::to_string(var.type.size), "%ecx");
					gas_ins("addl", "%ecx", "%eax");
					gas_ins(movXXl(size, is_unsig), "(%eax)", "%eax");
				}
				else if (isTempVar(_q1)) {
					TempVar _loc = searchTempvar(_q1);
					gas_ins("leal", "_" + var.varName, "%eax");
					gas_ins("imull", "$" + std::to_string(var.type.size), _loc._reg);
					gas_ins("addl", _loc._reg, "%eax");
					gas_ins(movXXl(size, is_unsig), "(%eax)", "%eax");
				}
				else {
					Node _glo = gloEnv->search(_q1);
					getReg("%edx");
					gas_ins("leal", "_" + var.varName, "%eax");
					gas_ins("movl", "_" + _glo.varName, "%edx");
					gas_ins("imull", "$" + std::to_string(var.type.size), "%edx");
					gas_ins("addl", "%edx", "%eax");
					gas_ins(movXXl(size, is_unsig), "(%eax)", "%eax");
				}
			}
           
        }

        // 保存
        TempVar _temp(_q3, "%eax");
        push_back_temp_stk(_temp, _temp._reg);
    }
	// 初始化时使用
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
	// 取数组和结构体的某一元素的地址，用来赋值
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
				if (isNumber(_q1))
					gas_ins("leal", std::to_string(var._off + var.type.size * atoi(_q1.c_str())) + "(%ebp)", "%eax");
				else if (isEnumConst(_q1))
					gas_ins("leal", std::to_string(var._off + var.type.size * atoi(parser->searchEnum(_q1).c_str())) + "(%ebp)", "%eax");
				else if (isLocVar(_q1)) {
					Node _loc = searchLocvar(_q1);
					getReg("%ecx");
					gas_ins("leal", std::to_string(var._off) + "(%ebp)", "%eax");
					gas_ins("movl", loc_var_val(_loc._off), "%ecx");
					gas_ins("imull", "$" + std::to_string(var.type.size), "%ecx");
					gas_ins("addl", "%ecx", "%eax");
				}
				else if (isTempVar(_q1)) {
					TempVar _loc = searchTempvar(_q1);
					gas_ins("leal", std::to_string(var._off) + "(%ebp)", "%eax");
					gas_ins("imull", "$" + std::to_string(var.type.size), _loc._reg);
					gas_ins("addl", _loc._reg, "%eax");
				}
				else {
					Node _glo = gloEnv->search(_q1);
					getReg("%edx");
					gas_ins("leal", std::to_string(var._off) + "(%ebp)", "%eax");
					gas_ins("movl", "_" + _glo.varName, "%edx");
					gas_ins("imull", "$" + std::to_string(var.type.size), "%edx");
					gas_ins("addl", "%edx", "%eax");
				}
			}
           
        }
        else {
            var = gloEnv->search(_q2);
			if (var.type.type == K_STRUCT || var.type.type == K_TYPEDEF) {
				for (size_t i = 0; i < var.type.fields.size(); ++i) {
					if (_q1 == std::to_string(var.type.fields.at(i)._off)) {
						size = var.type.fields.at(i)._type->size;
						is_unsig = var.type.fields.at(i)._type->isUnsig;
					}
				}
				gas_ins(movXXl(size, is_unsig), "$_" + var.varName + "+" + _q1, "%eax");
			}
			else if (var.type.type == ARRAY) {
				size = var.type.size;
				is_unsig = var.type.isUnsig;

				if (isNumber(_q1))
					gas_ins("leal", "_" + var.varName + "+" + std::to_string(var.type.size * atoi(_q1.c_str())), "%eax");
				else if (isEnumConst(_q1))
					gas_ins("leal", "_" + var.varName + "+" + std::to_string(var.type.size * atoi(parser->searchEnum(_q1).c_str())), "%eax");
				else if (isLocVar(_q1)) {
					Node _loc = searchLocvar(_q1);
					getReg("%ecx");
					gas_ins("leal", "_" + var.varName, "%eax");
					gas_ins("movl", loc_var_val(_loc._off), "%ecx");
					gas_ins("imull", "$" + std::to_string(var.type.size), "%ecx");
					gas_ins("addl", "%ecx", "%eax");
				}
				else if (isTempVar(_q1)) {
					TempVar _loc = searchTempvar(_q1);
					gas_ins("leal", "_" + var.varName, "%eax");
					gas_ins("imull", "$" + std::to_string(var.type.size), _loc._reg);
					gas_ins("addl", _loc._reg, "%eax");
				}
				else {
					Node _glo = gloEnv->search(_q1);
					getReg("%edx");
					gas_ins("leal", "_" + var.varName, "%edx");
					gas_ins("movl", "_" + _glo.varName, "%eax");
					gas_ins("imull", "$" + std::to_string(var.type.size), "%eax");
					gas_ins("addl", "%edx", "%eax");
				}
			}
        }

        // 保存
        TempVar _temp(_q3, "%eax");
        push_back_temp_stk(_temp, _temp._reg);
    }
    // 浮点运算
    else if (_q_0_is("=f") || _q_0_is("=d")) {
		gas_flo_load(_q1);
        gas_fstp(_q2);
		finit = false;
    }
    else if (_q_0_is("+f")) {
		int _save = 0, _t;

		_t = gas_flo_load(_q1); _t > _save ? _save = _t : true;
		_t = gas_flo_load(_q2); _t > _save ? _save = _t : true;
		gas_tab("faddp");

		finit = false;
		temp_save(_q3, _save);
    }
    else if (_q_0_is("-f")) {
		int _save = 0, _t;

		_t = gas_flo_load(_q2); _t > _save ? _save = _t : true;
		_t = gas_flo_load(_q1); _t > _save ? _save = _t : true;
		gas_ins("fsubr", "%st(1)", "%st(0)");

		finit = false;
		temp_save(_q3, _save);
    }
    else if (_q_0_is("*f")) {
		int _save = 0, _t;

		_t = gas_flo_load(_q1); _t > _save ? _save = _t : true;
		_t = gas_flo_load(_q2); _t > _save ? _save = _t : true;
		gas_tab("fmulp");

		finit = false;
		temp_save(_q3, _save);
    }
    else if (_q_0_is("/f")) {
		int _save = 0, _t;

		_t = gas_flo_load(_q1); _t > _save ? _save = _t : true;
		_t = gas_flo_load(_q2); _t > _save ? _save = _t : true;
		gas_tab("fdivp");

		finit = false;
		temp_save(_q3, _save);
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

/**
 * @berif 自增自减， ++ a
 */
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
        gas_tab("idivl   %ecx");
	}
    else if(isEnumConst(_q.at(1))) {
        getReg("%ecx");

		_t.create(K_INT, 4, false);

        gas_ins("movl", "$" + parser->searchEnum(_q.at(1)), "%ecx");
        gas_tab("idivl   %ecx");
    }
	else if (isLocVar(_q.at(1))) {
		LocVar _l = searchLocvar(_q.at(1));
		_t = _l.type;
        gas_tab("idivl\t" + loc_var_val(_l._off));
	}
	else if (isTempVar(_q.at(1))) {
		TempVar _temp = searchTempvar(_q.at(1));
		
		_t.create(_temp.type, _temp._size, _temp._isUnsig);

        gas_tab("idivl\t" + _temp._reg);
	}
    else {
        getReg("%ecx");
        Node var = gloEnv->search(_q.at(1));

		_t = var.type;

        gas_ins(movXXl(var.type.size, var.type.isUnsig), "_" + var.varName, "%ecx");
        gas_tab("idivl  %ecx");
    }

	_save.type < _t.type ? _save = _t : true;

	temp_clear(_q.at(1), _q.at(2));
	temp_save(_q.at(3), _save, "%eax");
}