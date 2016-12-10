#ifndef _ZVM_ZVM_H
#define _ZVM_ZVM_H

#include <vector>
#include "PARSER.H"

enum Instruction {
    movl, movb, movw,
    subl, addl,
    imull, idivl, call,
    leave, ret, pushl,
    popl,

    andl,
    xorl,
    orl,
    sarl,
    sall,
    shrl,
    notl,
    leal,
};

class AsmIns {
public:
    AsmIns(Instruction optor, void * operand) : operator_(optor), loperand_(operand), roperand_(nullptr) {}
    AsmIns(Instruction optor, void * lopand, void * ropand) : operator_(optor), loperand_(lopand), roperand_(ropand) {}
    AsmIns(const AsmIns &ai) : operator_(ai.operator_), loperand_(ai.loperand_), roperand_(ai.roperand_) {}
    AsmIns operator=(const AsmIns &ai) { operator_ = ai.operator_; loperand_ = ai.loperand_; roperand_ = ai.roperand_; return *this; }
    ~AsmIns() = default;

    Instruction operator_;
    void * loperand_;
    void * roperand_;
};

class VirtualMachine {
public:
    VirtualMachine(bool debug = false);
    VirtualMachine(const VirtualMachine &vm) = delete;
    VirtualMachine operator=(const VirtualMachine &vm) = delete;
    ~VirtualMachine() = default;

    void create(Parser *p);
    void run();

    void push_back(const AsmIns &ai) { text_.push_back(ai); }

    static int eax;
    static int ebx;
    static int ecx;
    static int edx;

    static int esp;
    static int ebp;

    void *getRegByName(const std::string &name)
    {
        std::string _off;
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
        else {
            size_t i = 0;
            for (; i < name.size(); ++i) {
                if (name.at(i) != '(')
                    _off.push_back(name.at(i));
                else
                    break;
            }
            switch (name.at(i + 3))
            {
            case 'a': return &eax + atoi(_off.c_str());
            case 'b': 
                if(name.at(i + 4) == 'x')
                    return (void *)(ebx + atoi(_off.c_str()));
                else 
                    return (void *)(ebp + atoi(_off.c_str()));
            case 'c': return (void *)(ecx + atoi(_off.c_str()));
            case 'd': return (void *)(edx + atoi(_off.c_str()));
            case 's': return (void *)(esp + atoi(_off.c_str()));
            default:
                break;
            }
        }
    }

    Instruction getInsByOp(const std::string &name)
    {
        if (name == "subl") return subl;
        else if (name == "addl") return addl;
        else if (name == "andl") return andl;
        else if (name == "xorl") return xorl;
        else if (name == "orl") return orl;
    }

private:
    Parser *parser_;

    std::vector<void *> data_;
    std::vector<AsmIns> text_;
    size_t pc = 0;

    bool debug_ = false;

    int stk_[24];
};



#endif // !_ZVM_ZVM_H