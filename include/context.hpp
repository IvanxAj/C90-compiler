#pragma once

#include <map>
#include <string>
#include <vector>

/*
REGISTERS RISC-V
name  |  ABI      |   meaning
      |  mnemonic |
-------------------------------------
$x0     $zero       Immutable zero
$x1     $ra         Return address
$x2     $sp         Stack pointer
$x3     $gp         Global pointer (??)
$x4     $tp         Thread pointer (??)
$x5-7   $t0-t2      Temp registers
$x8-9   $s0-s1      Frame pointer s0 (otherwise callee-saved registers)
$x10-17 $a0-a7      Argument registers
$x18-27 $s2-s11     Callee-saved registers
$28-31  $t3-t6      Temp registers

*/

enum specifier
{
    _int,
    _char,
    _void
};

// defines the size of all types - currently on int and void
const std::array<int, 3> typeSizes = {4, 1, 0};

struct Variable
{

    specifier type;     // Type of variable - only support int for now
    int offset;         // Offset from frame pointer

    Variable(): type(_int), offset(0) {}

    Variable(specifier _type, int _offset)
        : type(_type), offset(_offset) {}

};

struct stackFrame
{
    std::map<std::string, Variable> bindings;     // Track variables in scope
    int local_var_offset = -16;                   // track current local var offset

    // void addVar(std::string& _name, specifier _type, int _offset) {
    //     bindings[_name] = Variable(_type, _offset);
    // }

    // take in offset or keep internal? - what if I need to allocate more memory
    // consider moving internally
    void addVar(std::string& _name, specifier _type) {
        int varSize = typeSizes[_type];
        local_var_offset -= varSize;
        bindings[_name] = Variable(_type, local_var_offset);
    }

};

struct Context
{
    std::string getMnemonic(int i) {
        static const std::array<std::string, 32> regNames = {
            "zero", "ra", "sp", "gp", "tp",
            "t0", "t1", "t2",
            "s0", "s1",
            "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
            "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
            "t3", "t4", "t5", "t6"
        };

        return regNames[i];
    }

    bool usedRegs[32] =
        {1, 1, 1, 1, 1,
         0, 0, 0,
         1, 1,
         1, 1, 1, 0, 0, 1, 1, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         0, 0, 0, 0};

    // stack
    std::vector<stackFrame> stack;

    void useReg(int i) { usedRegs[i] = 1; }
    void freeReg(int i) { usedRegs[i] = 0; }

    int allocate()
    {
        for (int i = 5; i < 32; i++)
        {
            if (!usedRegs[i])
            {
                useReg(i);
                return i;
            }
        }
        return -1;
    }

};

