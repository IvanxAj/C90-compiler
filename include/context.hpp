
#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <cmath>

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

enum Specifier
{
    _int,
    _char,
    _void
};

// defines the size of all types - currently on int and void
const std::array<int, 3> typeSizes = {4, 1, 0};

struct Variable
{

    Specifier type;     // Type of variable - only support int for now
    int offset;         // Offset from frame pointer

    Variable(): type(_int), offset(0) {}

    Variable(Specifier _type, int _offset)
        : type(_type), offset(_offset) {}

};

struct Scope
{
    std::unordered_map<std::string, Variable> bindings;     // Track variables in scope

    void addLocalVar(const std::string& name, Specifier type, int offset) {
        bindings[name] = Variable(type, offset);
    }

    int getLocalVar(const std::string& name) const {
        auto it = bindings.find(name);
        return it == bindings.end() ? -1 : it->second.offset;
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
    int local_var_offset = -16;     // track current local var offset
    int param_offset = -32;         // track current param offset
    int frame_size = 32;

    std::vector<Scope> scopes;

    // Add a global scope on constructor
    Context() {
        scopes.push_back(Scope());
    }

    void useReg(int i) { usedRegs[i] = 1; }
    void freeReg(int i) { usedRegs[i] = 0; }

    int allocateReg() {
        for (int i = 5; i < 32; i++) {
            if (!usedRegs[i]) {
                useReg(i);
                return i;
            }
        }
        return -1;
    }

    int getVar(const std::string& name) {
        for (int i = scopes.size() - 1; i >= 0; --i) {
            int offset = scopes[i].getLocalVar(name);
            if (offset != -1) {
                return offset;
            }
        }
        return -1;
    }

    int addVar(const std::string& name, Specifier type) {
        int varSize = typeSizes[type];
        local_var_offset -= varSize;
        scopes.back().addLocalVar(name, type, local_var_offset);
        return local_var_offset;
    }

    // Create a new scope by adding a new stack frame - defaulted
    void newScope() {
        scopes.emplace_back();
    }

    // Delete the last scope by removing the last stack frame
    void popScope() {
        if (!scopes.empty()) {
            scopes.pop_back();
        }
    }

    int calculateStackSize(int totalLocalVarBytes, int totalParamBytes) {
        int stackSize = 16;  // Minimum stack size

        // Calculate space needed for local variables
        if (totalLocalVarBytes > 0) {
            stackSize += 16 * std::ceil(static_cast<double>(totalLocalVarBytes) / 16.0);
            // can set the local_var_offset here
        }

        // Calculate space needed for parameters
        if (totalParamBytes > 0) {
            stackSize += 16 * std::ceil(static_cast<double>(totalParamBytes) / 16.0);
            // can set the param offset here
        }

        return stackSize;
    }

    void debugScope() {
        if (scopes.empty()) {
            std::cerr << "No current scope to debug." << std::endl;
            return;
        }

        std::cerr << "Debugging all scopes:" << std::endl;

        for (size_t i = 0; i < scopes.size(); ++i) {
            const Scope& currentScope = scopes[i];
            std::cerr << "Scope " << i << ":" << std::endl;

            for (const auto& entry : currentScope.bindings) {
                std::string name = entry.first;
                Variable var = entry.second;
                int offset = var.offset;
                std::string type;

                switch (var.type) {
                    case _int: type = "int"; break;
                    case _char: type = "char"; break;
                    default: type = "Unknown"; break;
                }

                std::cerr << "  Variable: " << name << ", Type: " << type << ", Offset: " << offset << std::endl;
            }
        }
    }
};

