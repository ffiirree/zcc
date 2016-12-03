#include "gen.h"
#include "error.h"



#define _q_0_is(str) (_q.at(0) == str)
/**
* @berif �Ĵ�������
*/
void Generate::getReg(std::vector<std::string> &_q)
{
	// ע���ջ����ջ��˳��
	std::string _q1 = _q.at(1);
	std::string _q2;
	std::string _q3;

	if (_q.size() > 2) {
		_q2 = _q.at(2);
		if (_q.size() > 3)
			_q3 = _q.at(3);
	}

	std::string _q1_reg;
	std::string _q2_reg;
	std::string _q3_reg;

	// ����ǰ���������Ĵ����У���������
	//   1.����������Ǿֲ�������ȫ�ֱ�����movl->
	//   2.���򣬵�����Ϊ��ǰ����Ĵ���
	// 
	// ��ʱ���������ڼĴ�����
	// �ֲ�������������󵯳�
	// addl S,D D = D + S
	if (_q_0_is("+")) {
		_q1_reg = getQuadReg(_q1);
		_q2_reg = getQuadReg(_q2);
		out << "\taddl\t" << _q1_reg + ", " + _q2_reg << std::endl;

		saveAndClear(_q1, _q2, _q3, _q2_reg);
	}
	//subl S,D D = D �C S
	else if (_q_0_is("-")) {
		_q1_reg = getQuadReg(_q1);
		_q2_reg = getQuadReg(_q2);
		out << "\tsubl\t" << _q1_reg + ", " + _q2_reg << std::endl;

		saveAndClear(_q1, _q2, _q3, _q2_reg);
	}
	else if (_q_0_is("*")) {
		char _q1_ty = getVarType(_q1), _q2_ty = getVarType(_q2);

		// ��֤eaxΪ��
		getReg("%eax");

		if (_q1_ty == 'l') {
			/*setReg(_q1_reg, _q1);��Ϊ����ʱ������Ҫ*/
			LocVar _loc = searchLocvar(_q1);
			out << "\tmovl\t" + std::to_string(_loc._off) + "(%ebp), %eax" << std::endl;

			if (_q2_ty == 'n')  // imull n, eax, eax
				out << "\timull\t$" + _q2 + ", %eax, %eax" << std::endl;
			else if (_q2_ty == 'l') { // imull (%ebp), eax
				LocVar _q2_v = searchLocvar(_q2);
				out << "\timull\t" + std::to_string(_q2_v._off) + "(%ebp), %eax" << std::endl;
			}
			else if (_q2_ty == 't') { // imull ebx, eax
				TempVar _t = searchTempvar(_q2);
				out << "\timull\t" << _t._reg << ", %eax" << std::endl;
				pop_back_temp_stk(_q2);
			}
		}
		else if (_q1_ty == 'n') {
			if (_q2_ty == 'l') {
				LocVar _loc = searchLocvar(_q2);
				out << "\tmovl\t" + std::to_string(_loc._off) + "(%ebp), %eax" << std::endl;
				out << "\timull\t$" + _q1 + ", %eax, %eax" << std::endl;
			}
			else if (_q2_ty == 't') {
				TempVar _t = searchTempvar(_q2);
				out << "\timull\t$" + _q1 + "," + _t._reg + ", %eax" << std::endl;
				pop_back_temp_stk(_q2);
			}
		}
		else if (_q1_ty == 't') {
			TempVar _q1_t = searchTempvar(_q1);
			if (_q2_ty == 'n')
				out << "\timull\t$" + _q2 + "," + _q1_t._reg + ", %eax" << std::endl;
			else if (_q2_ty == 'l') { // imull ebx, eax
				LocVar _l = searchLocvar(_q2);
				out << "\tmovl\t" + std::to_string(_l._off) + "(%ebp), %eax" << std::endl;

				out << "\timull\t" + _q1_t._reg + ", %eax" << std::endl;
			}
			else if (_q2_ty == 't') {
				TempVar q2_t = searchTempvar(_q2);

				out << "\tmovl\t" << q2_t._reg << ", %eax" << std::endl;
				out << "\timull\t" << _q1_t._reg << ", %eax" << std::endl;
			}
			pop_back_temp_stk(_q2);
			pop_back_temp_stk(_q1);
			pop_back_temp_stk(_q2);
		}
		// �����eax
		if (isLocVar(_q3)) {
			LocVar _loc = searchLocvar(_q3);
			out << "\tmovl\teax, " + std::to_string(_loc._off) + "(%ebp)" << std::endl;
		}
		else {
			TempVar _t;
			_t._name = _q3;
			_t._reg = "%eax";
			push_back_temp_stk(_t, _t._reg);
		}
	}
	else if (_q_0_is("/")) {
		genMulOrModAsm(_q);
	}
	else if (_q_0_is("%")) {
		genMulOrModAsm(_q);
	}
	else if (_q_0_is("&")) {
		_q1_reg = getQuadReg(_q1);
		_q2_reg = getQuadReg(_q2);
		out << "\tandl\t" << _q1_reg + ", " + _q2_reg << std::endl;

		saveAndClear(_q1, _q2, _q3, _q2_reg);
	}
	else if (_q_0_is("|")) {
		_q1_reg = getQuadReg(_q1);
		_q2_reg = getQuadReg(_q2);
		out << "\torl\t" << _q1_reg + ", " + _q2_reg << std::endl;

		saveAndClear(_q1, _q2, _q3, _q2_reg);
	}
	// xorl S, D   D = D ^ S ����һ���ڼĴ���
	else if (_q_0_is("^")) {
		_q1_reg = getQuadReg(_q1);
		_q2_reg = getQuadReg(_q2);
		out << "\txorl\t" << _q1_reg + ", " + _q2_reg << std::endl;

		saveAndClear(_q1, _q2, _q3, _q2_reg);
	}
	else if (_q_0_is(">>")) {
		getReg("%eax");
		getReg("%ecx");

		std::string _out_str;

		if (isNumber(_q1)) {
			_out_str = "$" + _q1;
		}
		else if (isLocVar(_q1)) {
			LocVar _q1_v = searchLocvar(_q1);
			out << "\tmovl\t" + std::to_string(_q1_v._off) + "(%ebp), %ecx" << std::endl;
			_out_str = "%cl";
		}
		else if (isTempVar(_q1)) {
			TempVar _temp = searchTempvar(_q1);
			out << "\tmovl\t" << _temp._reg << ", %ecx" << std::endl;
			_out_str = "%cl";
		}

		if (isNumber(_q2)) {
			out << "\tmovl\t$" << _q2 << ", %eax" << std::endl;
		}
		else if (isLocVar(_q2)) {
			LocVar _q2_v = searchLocvar(_q2);
			out << "\tmovl\t" + std::to_string(_q2_v._off) + "(%ebp), %eax" << std::endl;
		}
		else if (isTempVar(_q2)) {
			TempVar _temp = searchTempvar(_q2);
			out << "\tmovl\t" << _temp._reg << ", %eax" << std::endl;
		}
		out << "\tsarl\t" << _out_str + ", %eax" << std::endl;
		saveAndClear(_q1, _q2, _q3, "%eax");
	}
	else if (_q_0_is("<<")) {
		getReg(std::string("%eax"));
		getReg(std::string("%ecx"));

		std::string _out_str;

		if (isNumber(_q1)) {
			_out_str = "$" + _q1;
		}
		else if (isLocVar(_q1)) {
			LocVar _q1_v = searchLocvar(_q1);
			out << "\tmovl\t" + std::to_string(_q1_v._off) + "(%ebp), %ecx" << std::endl;
			_out_str = "%cl";
		}
		else if (isTempVar(_q1)) {
			TempVar _temp = searchTempvar(_q1);
			out << "\tmovl\t" << _temp._reg << ", %ecx" << std::endl;
			_out_str = "%cl";
		}


		if (isNumber(_q2)) {
			out << "\tmovl\t$" << _q2 << ", %eax" << std::endl;
		}
		else if (isLocVar(_q2)) {
            LocVar _q2_v = searchLocvar(_q2);
			out << "\tmovl\t" + std::to_string(_q2_v._off) + "(%ebp), %eax" << std::endl;
		}
		else if (isTempVar(_q2)) {
			TempVar _temp = searchTempvar(_q2);
			out << "\tmovl\t" << _temp._reg << ", %eax" << std::endl;
		}

		out << "\tsall\t" << _out_str + ", %eax" << std::endl;
		saveAndClear(_q1, _q2, _q3, "%eax");
	}
	else if (_q_0_is(">>>")) {
		getReg(std::string("%eax"));
		getReg(std::string("%ecx"));

		std::string _out_str;

		if (isNumber(_q1)) {
			_out_str = "$" + _q1;
		}
		else if (isLocVar(_q1)) {
            LocVar _q1_v = searchLocvar(_q1);
			out << "\tmovl\t" + std::to_string(_q1_v._off) + "(%ebp), %ecx" << std::endl;
			_out_str = "%cl";
		}
		else if (isTempVar(_q1)) {
			TempVar _temp = searchTempvar(_q1);
			out << "\tmovl\t" << _temp._reg << ", %ecx" << std::endl;
			_out_str = "%cl";
		}

		if (isNumber(_q2)) {
			out << "\tmovl\t$" << _q2 << ", %eax" << std::endl;
		}
		else if (isLocVar(_q2)) {
			LocVar _q2_v = searchLocvar(_q2);
			out << "\tmovl\t" + std::to_string(_q2_v._off) + "(%ebp), %eax" << std::endl;
		}
		else if (isTempVar(_q2)) {
			TempVar _temp = searchTempvar(_q2);
			out << "\tmovl\t" << _temp._reg << ", %eax" << std::endl;
		}

		out << "\tshrl\t" << _out_str + ", %eax" << std::endl;
		saveAndClear(_q1, _q2, _q3, std::string("%eax"));
	}
	else if (_q_0_is("&U")) {
		// ֻ����ȫ�ֱ�����ֲ�����
		LocVar _lv = searchLocvar(_q1);
		if (_lv.varName.empty())
			error("& addr error.");
		getReg(std::string("%eax"));
		out << "\tleal\t" + std::to_string(_lv._off) + "(%ebp), %eax" << std::endl;

		LocVar _var = searchLocvar(_q2);
		if (!_var.varName.empty()) {
			out << "\tmovl\t%eax, " + std::to_string(_var._off) + "(%ebp)" << std::endl;
		}
		else {
			// �����ڽ���Ķ��ǵ�һ��
			TempVar _temp;
			_temp._name = _q2;

			_temp._reg = "%eax";
			push_back_temp_stk(_temp, _temp._reg);
		}
	}
	else if (_q_0_is("*U")) {
		getReg("%eax");
		LocVar _v = searchLocvar(_q1);
		out << "\tmovl\t" + std::to_string(_v._off) + "(%ebp), %eax" << std::endl;
		out << "\tmovl\t(%eax), %eax" << std::endl;

		// �����ڽ���Ķ��ǵ�һ��
		TempVar _temp;
		_temp._name = _q2;
		_temp._reg = "%eax";
		push_back_temp_stk(_temp, _temp._reg);
	}
	else if (_q_0_is("+U")) {
		// Ϊ�ֲ���ȫ�ֱ���
		error("Not support operator.");
	}
	else if (_q_0_is("-U")) {
		error("Not support operator.");
	}
	else if (_q_0_is("++")) {
		//ֻ���Ǿֲ����������ȫ�ֱ���
		LocVar _loc = searchLocvar(_q1);
		out << "\taddl\t$1, " + std::to_string(_loc._off) + "(%ebp)" << std::endl;
	}
	else if (_q_0_is("--")) {
		//ֻ���Ǿֲ����������ȫ�ֱ���
		LocVar _loc = searchLocvar(_q1);
		out << "\tsubl\t$1, " + std::to_string(_loc._off) + "(%ebp)" << std::endl;
	}
	else if (_q_0_is("~")) {
		//ֻ���Ǿֲ����������ȫ�ֱ���
		LocVar _loc = searchLocvar(_q1);
		if(!_loc.varName.empty()){
			out << "\tnotl\t$1, " + std::to_string(_loc._off) + "(%ebp)" << std::endl;
			return;
		}
		
		TempVar _tem = searchTempvar(_q1);
		if(!_tem._name.empty()){
			out << "\tnotl\t" << _tem._reg << std::endl;
		}
	}
	else if (_q_0_is("[]")) {
		error("Not support operator.");
	}
	else {
		error("Not support operator.");
	}
}
#undef _q_0_is


void Generate::genMulOrModAsm(std::vector<std::string> &_q)
{
	// ע���ջ����ջ��˳��
	std::string _q1 = _q.at(1);
	std::string _q2 = _q.at(2);
	std::string _q3 = _q.at(3);

	std::string _save_reg;                // ������
	if (_q1.at(0) == '/') _save_reg = "%eax";
	else _save_reg = "%edx";

	getReg("%eax");
	getReg("%edx");

	if (isLocVar(_q2)) {
		LocVar _l = searchLocvar(_q2);
		out << "\tmovl\t" + std::to_string(_l._off) + "(%ebp), %eax" << std::endl;
	}
	else if (isTempVar(_q2)) {
		TempVar _t = searchTempvar(_q2);
		out << "\tmovl\t" + _t._reg << ", %eax" << std::endl;
	}
	else if (isNumber(_q2)) {
		out << "\tmovl\t$" + _q2 + ", %eax" << std::endl;
	}

	out << "\tcltd" << std::endl;

	if (isNumber(_q1)) {
		out << "\tidivl\t$" + _q1;
	}
	else if (isLocVar(_q1)) {
		LocVar _l = searchLocvar(_q1);
		out << "\tidivl\t" + std::to_string(_l._off) + "(%ebp)" << std::endl;
	}
	else if (isTempVar(_q1)) {
		TempVar _t = searchTempvar(_q1);
		out << "\tidivl\t" + _t._reg << std::endl;
	}

	saveAndClear(_q1, _q2, _q3, _save_reg);
}

/**
 * ����֮��Ĵ�������������ݵı���
 */
void Generate::saveAndClear(std::string &_q1, std::string &_q2, std::string &_q3, const std::string &_reg)
{
	// ����������
	clearRegConst();

	// �Ƚ���ʱ�����ͳ�����ջ
	pop_back_temp_stk(_q1);
	pop_back_temp_stk(_q2);
	pop_back_temp_stk(_q1);

	LocVar _var = searchLocvar(_q3);
	if (!_var.varName.empty()) {
		out << "\tmovl\t" + _reg + ", " + std::to_string(_var._off) + "(%ebp)" << std::endl;
	}
	else {
		// �����ڽ���Ķ��ǵ�һ��
		TempVar _temp;
		_temp._name = _q3;
		_temp._reg = _reg;
		push_back_temp_stk(_temp, _reg);
	}
}