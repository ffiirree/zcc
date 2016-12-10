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
    void *getRegByName(const std::string &name);
    Instruction getInsByOp(const std::string &name);

    static int eax;
    static int ebx;
    static int ecx;
    static int edx;

    static int esp;
    static int ebp;

private:
    Parser *parser_;

    std::vector<void *> data_;
    std::vector<AsmIns> text_;
    size_t pc = 0;

    bool debug_ = false;

    int stk_[24];
};



#endif // !_ZVM_ZVM_H