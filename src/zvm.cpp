#include <fstream>
#include <malloc.h>
#include<iomanip>
#include "zvm.h"
#include "FILE.H"
#include "ERROR.H"

int VirtualMachine::eax = 0;
int VirtualMachine::ebx = 0;
int VirtualMachine::ecx = 0;
int VirtualMachine::edx = 0;

int VirtualMachine::esp = 0;
int VirtualMachine::ebp = 0;

VirtualMachine::VirtualMachine(bool debug) :debug_(debug)
{
    // ջ
    ebp = (int)(stk_ + 23);
    esp = ebp;
}

void VirtualMachine::create(Parser *p)
{
    pc = 0;
    int esp = 0;
    int ebp = 0;
}


#define _ptr(_ty, _obj)     (*reinterpret_cast<_ty>(_obj))
#define _32u(_obj23u)       _ptr(unsigned int *, _obj23u)
#define _16u(_obj23u)       _ptr(unsigned short *, _obj23u)
#define _8u(_obj23u)        _ptr(unsigned char *, _obj23u)
#define _32s(_obj32s)       _ptr(signed int *, _obj32s)
#define _16s(_obj32s)       _ptr(signed short *, _obj32s)
#define _8s(_obj32s)        _ptr(signed char *, _obj32s)
#define _32sL               _32s(asmIns.loperand_)
#define _32sR               _32s(asmIns.roperand_)
#define _32REG              _32u(asmIns.loperand_)
#define _16sL               _16s(asmIns.loperand_)
#define _16sR               _16s(asmIns.roperand_)
#define _8sL                _8s(asmIns.loperand_)
#define _8sR                _8s(asmIns.roperand_)

void VirtualMachine::run()
{
    for (;pc < text_.size();)
    {
        AsmIns asmIns = text_.at(pc++);

        if (debug_) {
            std::cout << asmIns.ins_ << std::endl;
            std::cout << "eax=" << std::left << std::setw(10) << eax
                << " ebx=" << std::left << std::setw(10) << ebx
                << " ecx=" << std::left << std::setw(10) << ecx
                << " edx=" << std::left << std::setw(10) << edx << std::endl;
            std::cout << "ebp=" << std::left << std::setw(10) << ebp << " esp="
                << std::left << std::setw(10) << esp << std::endl << std::endl;
        }

        switch (asmIns.operator_)
        {
        case movl:  _32sR = _32sL; break;
        case movb:  _16sR = _16sL; break;
        case movw:  _8sR = _8sL; break;
        case subl:  _32sR = _32sR - _32sL; break;
        case addl:  _32sR = _32sR + _32sL; break;
        case imull: _32sR = _32sR * _32sL; break;
        case idivl: eax = eax / _32REG; edx = eax % _32REG; break;
        case call:
        case leave:
        case ret:
        case pushl: _32s(esp) = _32REG; esp -= 4; break;
        case popl:
        case andl:  _32sR = _32sR & _32sL; break;
        case xorl:  _32sR = _32sR ^ _32sL; break;
        case orl:   _32sR = _32sR | _32sL; break;
        case sarl:  _32sR = _32sR >> _32sL; break;
        case sall:  _32sR = _32sR << _32sL; break;
        case shrl:  _32sR = _32sR >> _32sL; break;
        case notl:  _32sR = ~_32REG; break;
        case leal:  
        default:
            break;
        }

        
    }
}



Instruction VirtualMachine::getInsByOp(const std::string &name)
{
    if (name == "movl") return movl;
    else if (name == "movb") return movb;
    else if (name == "movw") return movw;
    else if (name == "imull") return imull;
    else if (name == "idivl") return idivl;
    else if (name == "subl") return subl;
    else if (name == "addl") return addl;
    else if (name == "andl") return andl;
    else if (name == "xorl") return xorl;
    else if (name == "orl") return orl;
    else if (name == "call") return call;
    else if (name == "ret") return ret;
    else if (name == "leave") return leave;
    else if (name == "popl") return popl;
    else if (name == "pushl") return pushl;
    else if (name == "sarl") return sarl;
    else if (name == "sall") return sall;
    else if (name == "notl") return notl;
    else if (name == "leal") return leal;
    else
        error("unknown operator:%s.", name.c_str());
}

int VirtualMachine::getRegValByName(const std::string &name)
{
    if (name == "%eax")
        return eax;
    else if (name == "%ebx")
        return ebx;
    else if (name == "%ecx")
        return ecx;
    else if (name == "%edx")
        return edx;
    else if (name == "%ebp")
        return ebp;
    else if (name == "%esp")
        return esp;
    else
        error("unknown register.");
}
/**
 * @attention 浮点数不会出现在指令里面
 */
bool VirtualMachine::isImmediate(const std::string &name)
{
    for (size_t i = 0; i < name.size(); ++i) {
        if (name.at(i) != '$' && (name.at(i) > '9' || name.at(i) < '0'))
            return false;
    }
    return true;
}

int VirtualMachine::getImmediate(const std::string &name)
{
    std::string _imm;
    for (int i = 1; i < name.size(); ++i) {
        _imm.push_back(name.at(i));
    }

    return atoi(_imm.c_str());
}

void *VirtualMachine::getOperandAddr(const std::string &name)
{
    std::string _off, _reg;
    int i = 0;

    switch (name.at(0))
    {
    case '$':
        if (isImmediate(name))
            return new int(getImmediate(name));
            

    case '%':
        if (name == "%eax")
            return &eax;
        else if (name == "%ebx")
            return &ebx;
        else if (name == "%ecx")
            return &ecx;
        else if (name == "%edx")
            return &edx;
        else if (name == "%ebp")
            return &ebp;
        else if (name == "%esp")
            return &esp;
        else
            error("unknown register.");
        break;

    case '-':
    case '+':
    case '(':
    case_0_9:
        for (i = 0; name.at(i) != '('; ++i) {
            _off.push_back(name.at(i));
        }
        if (_off.empty())
            _off.push_back('0');

        for (; name.at(i) != ')'; ++i) {
            switch (name.at(i))
            {
            case_a_z:
            case '%':
                _reg.push_back(name.at(i));
            }
        }

        return (void *)(getRegValByName(_reg) + atoi(_off.c_str()));

    case '_':

    default:
        break;
    }
}

