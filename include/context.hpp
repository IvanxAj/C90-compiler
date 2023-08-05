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
    _void
};

struct variable
{

    int reg;            // Reg it is stored in
    int offset;         // Offset from frame pointer
    specifier type;     // Type of variable - only support int for now

};

struct stackFrame
{
    int offset;                                 // offset to previous stackframe
    std::map<std::string, variable> varMap;     // Track variables in scope

};


struct Context
{

    bool usedRegs[32] =
        {1, 1, 1, 1, 1,
         0, 0, 0,
         1, 1,
         1, 1, 1, 1, 1, 1, 1, 1,
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

    // enter + exit scope



};




// struct StackFrameContext
// {
//     // local variables - map variable name to location on stack
//     // std::map< std::string, location >

// };
