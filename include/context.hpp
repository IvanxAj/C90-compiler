
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

enum class Specifier
{
    _int,
    _char,
    _void,
    _unsigned,
    INVALID_TYPE = -1,
};

const std::unordered_map<Specifier, int> typeSizes = {
    {Specifier::_int,   4},
    {Specifier::_char,  1},
    {Specifier::_void,  0},
    {Specifier::_unsigned, 4}
};

struct Variable
{

    Specifier type;     // Type of variable - only support int for now
    int offset;         // Offset from frame pointer

    Variable(): type(Specifier::_int), offset(0) {}

    Variable(Specifier _type, int _offset)
        : type(_type), offset(_offset) {}

};

struct Scope
{
    std::unordered_map<std::string, Variable> bindings;     // Track variables in scope

    void addLocalVar(const std::string& name, Specifier type, int offset) {
        bindings[name] = Variable(type, offset);
    }

    Variable getLocalVar(const std::string& name) const {
        auto it = bindings.find(name);
        return it == bindings.end() ? Variable(Specifier::INVALID_TYPE, -1) : it->second;
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
    int param_offset_excess = 0;    // track params on previous stack frame
    int frame_size = 32;

    // flags
    bool isFunctionDef = 0;
    std::string ret_label;

    std::vector<Scope> scopes;
    std::vector<std::pair<std::string, std::string>> loopLabels;

    // Add a global scope on constructor
    Context() {
        scopes.push_back(Scope());
    }

    /* ----------------------------------HANDLE REGS------------------------------------------- */

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

    /* ----------------------------------HANDLE VARS------------------------------------------- */

    int getVarOffset(const std::string& name) {
        for (int i = scopes.size() - 1; i >= 0; --i) {
            int offset = scopes[i].getLocalVar(name).offset;
            if (offset != -1) {
                return offset;
            }
        }
        return -1;
    }

    Specifier getVarType(const std::string& name) {
        for (int i = scopes.size() - 1; i >= 0; --i) {
            Specifier type = scopes[i].getLocalVar(name).type;
            return type;
        }
        // will return INVALID _TYPE if variable doesnt exist anyway
        return Specifier::INVALID_TYPE;
    }

    int addVar(const std::string& name, Specifier type) {
        int varSize = typeSizes.at(type);
        local_var_offset -= varSize;
        scopes.back().addLocalVar(name, type, local_var_offset);
        return local_var_offset;
    }

    int addParam(const std::string& name, Specifier type, int param_index) {
        int param_size = typeSizes.at(type);
        if (param_index < 8) {
            param_offset -= param_size;
            scopes.back().addLocalVar(name, type, param_offset);
            return param_offset;
        }

        scopes.back().addLocalVar(name, type, param_offset_excess);
        param_offset_excess += param_size;
        // 1 indicates that the param was not taken from register
        return 1;
    }

    /* ----------------------------------HANDLE SCOPES------------------------------------------- */

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

    void resetOffsets() {
        local_var_offset = -16;
        param_offset = -32;
        param_offset_excess = 0;
        frame_size = 32;
    }

    /* ----------------------------------HANDLE LOOPS------------------------------------------- */

    void newLoop(const std::string& start_label, const std::string& end_label) {
        loopLabels.push_back({start_label, end_label});
    }

    void exitLoop() {
        if (!loopLabels.empty()) {
            loopLabels.pop_back();
        }
    }

    std::string getCurrentLoopStart() {
        if (!loopLabels.empty()) {
            return loopLabels.back().first;
        }
        return "";
    }

    std::string getCurrentLoopEnd() {
        if (!loopLabels.empty()) {
            return loopLabels.back().second;
        }
        return "";
    }

    /* ----------------------------------UTILITY------------------------------------------- */

    int calculateStackSize(int totalLocalVarBytes, int totalParamBytes) {
        int stackSize = 16;  // Minimum stack size

        // Calculate space needed for local variables
        // can set the local_var_offset here
        if (totalLocalVarBytes > 0) {
            stackSize += 16 * std::ceil(static_cast<double>(totalLocalVarBytes) / 16.0);
        }

        // can set the param offset here
        param_offset = -stackSize;

        // Calculate space needed for parameters
        if (totalParamBytes > 0) {
            stackSize += 16 * std::ceil(static_cast<double>(totalParamBytes) / 16.0);
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
                    case Specifier::_int: type = "int"; break;
                    case Specifier::_char: type = "char"; break;
                    default: type = "Unknown"; break;
                }

                std::cerr << "  Variable: " << name << ", Type: " << type << ", Offset: " << offset << std::endl;
            }
        }
    }

    inline std::string makeLabel (std::string label){
        static int unique = 0 ;
        return label + std::to_string(unique++);
    }
};

