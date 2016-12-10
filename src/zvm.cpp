#include <fstream>
#include <malloc.h>
#include<iomanip>
#include "zvm.h"
#include "FILE.H"

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

        if (debug_) {
            std::cout << "eax=" << std::left << std::setw(10) << eax
                << " ebx=" << std::left << std::setw(10) << ebx
                << " ecx=" << std::left << std::setw(10) << ecx
                << " edx=" << std::left << std::setw(10) << edx << std::endl;
            std::cout << "ebp=" << std::left << std::setw(10) << ebp << " esp="
                << std::left << std::setw(10) << esp << std::endl << std::endl;
        }
    }
}