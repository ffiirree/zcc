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

void AsmIns::copying(const AsmIns&ai)
{
    operator_ = ai.operator_;

    loperand_ = ai.loperand_;
    roperand_ = ai.roperand_;

    ins_ = ai.ins_;
    lstr_ = ai.lstr_;
    rstr_ = ai.rstr_;
}

VirtualMachine::VirtualMachine(bool use, bool debug) : use_(use), debug_(debug)
{
    // ջ
    ebp = (int)(stk_ + 24);
    esp = ebp;
    heap_ptr_ = (int)heap_;
    text_.push_back({ getInsByOp("call"), "_main", "call\t_main" });
    text_.push_back({ getInsByOp("exit"), "exit" });
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

void VirtualMachine::link()
{
    for (auto & ins : text_) {
        if (ins.operator_ == call || (ins.operator_ >= jg && ins.operator_ <= jne)) {
            std::map<std::string, int>::iterator iter = data_.find(ins.lstr_);
            if (iter != data_.end()) {
                ins.loperand_ = new int(iter->second);
            }
            else
                error("unknown label :%s.", ins.lstr_.c_str());
        }
    }
}

void VirtualMachine::decode(AsmIns &ai)
{
    if (ai.operator_ == call)
        return;
    if (ai.operator_ == leave)
        return;
    if (ai.operator_ == ret)
        return;
    if (ai.operator_ == exitvm)
        return;
    if (ai.operator_ >= jg && ai.operator_ <= jne)
        return;

    ai.loperand_ = getOperandAddr(ai.lstr_);
    if(!ai.rstr_.empty())
        ai.roperand_ = getOperandAddr(ai.rstr_);
}


void VirtualMachine::run()
{
    link();

    for (;;)
    {
        AsmIns asmIns = text_.at(pc++);
        decode(asmIns);
        int r = 0;

        switch (asmIns.operator_)
        {
        case movl:  _32sR = _32sL; break;
        case movb:  _16sR = _16sL; break;
        case movw:  _8sR = _8sL; break;
        case subl:  _32sR = _32sR - _32sL; break;
        case addl:  _32sR = _32sR + _32sL; break;
        case imull: _32sR = _32sR * _32sL; break;
        case idivl: eax = eax / _32REG; edx = eax % _32REG; break;
        case call:  esp -= 4; _32s(esp) = pc;   pc = _32REG;  break;
        case leave: esp = ebp;  ebp = _32s(esp); esp += 4; break;
        case ret:   pc = _32s(esp);  esp += 4;break;
        case pushl: esp -= 4; _32s(esp) = _32REG; break;
        case popl:
        case andl:  _32sR = _32sR & _32sL; break;
        case xorl:  _32sR = _32sR ^ _32sL; break;
        case orl:   _32sR = _32sR | _32sL; break;
        case sarl:  _32sR = _32sR >> _32sL; break;
        case sall:  _32sR = _32sR << _32sL; break;
        case shrl:  _32sR = _32sR >> _32sL; break;
        case notl:  _32sR = ~_32REG; break;
        case leal:  
        case exitvm:  std::cout << eax << std::endl; return;
        case cmpl:  r = _32sR - _32sL; 
            if (r > 0) { sf = true; zf = false; }
            else if (r == 0) { zf = true; sf = false; }
            else { sf = false; zf = false; }
        case je:    if (zf) pc = _32REG; break;
        case jne:   if (!zf) pc = _32REG; break;
        case jg:    if (sf) pc = _32REG; break;
        case jge:   if (sf || zf) pc = _32REG; break;
        case jl:    if (!sf && !zf) pc = _32REG; break;
        case jle:   if (!sf || zf) pc = _32REG; break;
        case jmp:   pc = _32REG; break;
        default:
            break;
        }

        if (debug_) {
_begin_debug_:
            std::cout << "#> ";
            std::string c1, c2;
            std::cin >> c1;

            if (c1 == "n") {
                std::cout << pc - 1 << ": " << text_.at(pc - 1).ins_ << std::endl << std::endl;
                std::cout << "eax=" << std::left << std::setw(10) << eax
                    << " ebx=" << std::left << std::setw(10) << ebx
                    << " ecx=" << std::left << std::setw(10) << ecx
                    << " edx=" << std::left << std::setw(10) << edx << std::endl;
                std::cout << "ebp=" << std::left << std::setw(10) << ebp << " esp="
                    << std::left << std::setw(10) << esp << std::endl << std::endl;
            }
            else if (c1 == "q") {
                return;
            }
            else if (c1 == "x") {
                std::cin >> c2;

                std::cout << c2 + ":" << *(int *)atoi(c2.c_str()) << std::endl;

            }
            else {
                std::cout << "Error : do not have ths order." << std::endl;
                goto _begin_debug_;
            }
            
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
    else if (name == "exit") return exitvm;
    else if (name == "cmpl") return cmpl;
    else if (name == "jg") return jg;
    else if (name == "jl") return jl;
    else if (name == "jge") return jge;
    else if (name == "jle") return jle;
    else if (name == "je") return je;
    else if (name == "jne") return jne;
    else if (name == "jmp") return jmp;
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
    std::map<std::string, int>::iterator iter;

    switch (name.at(0))
    {
    case '$':
        if (isImmediate(name))
            return new int(getImmediate(name));
    case '_':
        iter = data_.find(name);
        if (iter != data_.end())
            return (int *)iter->second;
        else
            error("%s is not exist.", name.c_str());
        break;

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

    default:
        break;
    }
}


void VirtualMachine::push_data(const std::string name, int val, int size)
{
    data_.insert(std::pair<std::string, int>(name, heap_ptr_));
    switch (size)
    {
    case 1: *(char *)heap_ptr_ = static_cast<char>(val);
    case 2: *(short *)heap_ptr_ = static_cast<short>(val);
    case 4: *(int *)heap_ptr_ = val;
    default:
        break;
    }
    heap_ptr_ += size;
}

void VirtualMachine::setFuncAddr(const std::string &fn)
{
    std::map <std::string, int>::iterator iter = data_.find(fn);
    if (iter != data_.end()) {
        iter->second = text_.size();
    }
    else {
        data_.insert(std::pair<std::string, int>(fn, text_.size()));
    }
}