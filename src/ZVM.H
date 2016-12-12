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
    AsmIns(Instruction optor, void * operand) : operator_(optor), loperand_(operand), roperand_(nullptr), ins_(){}
    AsmIns(Instruction optor, void * operand, const std::string &ins) : operator_(optor), loperand_(operand), roperand_(nullptr), ins_(ins) {}
    AsmIns(Instruction optor, void * lopand, void * ropand) : operator_(optor), loperand_(lopand), roperand_(ropand), ins_(){}
    AsmIns(Instruction optor, void * lopand, void * ropand, const std::string &ins) : operator_(optor), loperand_(lopand), roperand_(ropand), ins_(ins) {}
    AsmIns(const AsmIns &ai) : operator_(ai.operator_), loperand_(ai.loperand_), roperand_(ai.roperand_), ins_(ai.ins_){}
    AsmIns operator=(const AsmIns &ai) { operator_ = ai.operator_; loperand_ = ai.loperand_; roperand_ = ai.roperand_; ins_ = ai.ins_; return *this; }
    ~AsmIns() = default;

    Instruction operator_;
    void * loperand_;
    void * roperand_;

    std::string ins_;
};

class VirtualMachine {
public:
    VirtualMachine(bool debug = false);
    VirtualMachine(const VirtualMachine &vm) = delete;
    VirtualMachine operator=(const VirtualMachine &vm) = delete;
    ~VirtualMachine() = default;

    void create(Parser *p);
    void run();

    bool isImmediate(const std::string &name);
    int getImmediate(const std::string &name);
    void push_back(const AsmIns &ai) { text_.push_back(ai); }
    Instruction getInsByOp(const std::string &name);
    void *getOperandAddr(const std::string &name);
    int getRegValByName(const std::string &name);

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