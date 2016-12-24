#ifndef _ZVM_ZVM_H
#define _ZVM_ZVM_H

#include <map>
#include <vector>
#include "parser.h"
#include "type.h"

enum Instruction {
    INS_NULL,
#define vminsmap(_, ins) ins,
    VM_INS
#undef vminsmap
};

/**
 * @berif AsmIns
 */
class AsmIns {
public:
    AsmIns(Instruction optor, const std::string &ins) : operator_(optor), lstr_(), rstr_(), ins_(ins) {}
    AsmIns(Instruction optor, const std::string operand, const std::string &ins) : operator_(optor), lstr_(operand), rstr_(), ins_(ins) {}
    AsmIns(Instruction optor, const std::string lopand, const std::string ropand, const std::string &ins) : operator_(optor), lstr_(lopand), rstr_(ropand), ins_(ins) {}

    AsmIns(const AsmIns &ai) { copying(ai); }
    AsmIns &operator=(const AsmIns &ai) { copying(ai); return *this; }
    ~AsmIns() = default;

public:
    Instruction operator_;

    void *loperand_ = nullptr;
    void *roperand_ = nullptr;

    std::string ins_;
    std::string lstr_;
    std::string rstr_;
private:
    void copying(const AsmIns&ai);
};


/**
 * @berif VirtualMachine
 */
class VirtualMachine {
public:
    VirtualMachine(bool use = false, bool debug = false);
    VirtualMachine(const VirtualMachine &vm) = delete;
    VirtualMachine &operator=(const VirtualMachine &vm) = delete;
    ~VirtualMachine() = default;

    void create(Parser *p);
    void setFuncAddr(const std::string &fn);
    void link();
    void run();
    void decode(AsmIns &ai);

    bool isImmediate(const std::string &name);
    int getImmediate(const std::string &name);
    void push_back(const AsmIns &ai) { if(use_) text_.push_back(ai); }
    void push_data(const std::string name, int val, int size);
    Instruction getInsByOp(const std::string &name);
    void *getOperandAddr(const std::string &name);
    int getRegValByName(const std::string &name);

    /**
     * @berif Virtual register
     */
    static int eax;
    static int ebx;
    static int ecx;
    static int edx;

    static int esp;
    static int ebp;

    bool use_ = false;

private:
    Parser *parser_;

    /**
     * \ Whether the vm is running in the debug model.
     */
    bool debug_ = false;

    static std::vector<std::pair<std::string, Instruction>> instructions_;

    /**
     * @berif code segment
     */
    std::vector<AsmIns> text_;
    std::map<std::string, int> data_;
    size_t pc = 0;

    int heap_ptr_ = 0;
    char heap_[64];
    int stk_[24];

    /**
     * \ flags
     */
    bool zf = false;
    bool sf = false;
};



#endif // !_ZVM_ZVM_H