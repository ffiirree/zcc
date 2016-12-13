#ifndef _ZVM_ZVM_H
#define _ZVM_ZVM_H

#include <map>
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
    exitvm,
    cmpl,
    jg,
    jl,
    jge,
    jle,
    jmp,
    je,
    jne,
};

class AsmIns {
public:
    AsmIns(Instruction optor, const std::string &ins) : operator_(optor), lstr_(), rstr_(), ins_(ins) {}
    AsmIns(Instruction optor, const std::string operand, const std::string &ins) : operator_(optor), lstr_(operand), rstr_(), ins_(ins) {}
    AsmIns(Instruction optor, const std::string lopand, const std::string ropand, const std::string &ins) : operator_(optor), lstr_(lopand), rstr_(ropand), ins_(ins) {}

    AsmIns(const AsmIns &ai) { copying(ai); }
    AsmIns operator=(const AsmIns &ai) { copying(ai); return *this; }
    ~AsmIns() = default;

    

public:
    Instruction operator_;           // 操作符

    void *loperand_ = nullptr;       // 左操作数
    void *roperand_ = nullptr;       // 右操作数

    std::string ins_;                // 汇编指令,DEBUG
    std::string lstr_;               // 左操作数
    std::string rstr_;               // 右操作数
private:
    void copying(const AsmIns&ai);

    
};

struct Data {
    std::string name_;
    int addr = 0;
};

class VirtualMachine {
public:
    VirtualMachine(bool use = false, bool debug = false);
    VirtualMachine(const VirtualMachine &vm) = delete;
    VirtualMachine operator=(const VirtualMachine &vm) = delete;
    ~VirtualMachine() = default;

    void create(Parser *p);
    void setFuncAddr(const std::string &fn);
    void link();
    void run();
    void decode(AsmIns &ai);

    bool isImmediate(const std::string &name);
    int getImmediate(const std::string &name);
    void push_back(const AsmIns &ai) { text_.push_back(ai); }
    void push_data(const std::string name, int val, int size);
    Instruction getInsByOp(const std::string &name);
    void *getOperandAddr(const std::string &name);
    int getRegValByName(const std::string &name);

    static int eax;
    static int ebx;
    static int ecx;
    static int edx;

    static int esp;
    static int ebp;

    bool use_ = false;

private:
    Parser *parser_;

    std::vector<AsmIns> text_;
    size_t pc = 0;

    bool debug_ = false;
    

    std::map<std::string, int> data_;
    int heap_ptr_ = 0;

    char heap_[64];
    int stk_[24];

    bool zf = false;
    bool sf = false;
};



#endif // !_ZVM_ZVM_H