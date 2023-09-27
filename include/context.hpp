
#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

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
    _float,
    _double,
    _unsigned,
    INVALID_TYPE = -1,
};

const std::unordered_map<Specifier, int> typeSizes = {
    {Specifier::_int,       4},
    {Specifier::_char,      1},
    {Specifier::_void,      0},
    {Specifier::_float,     4},
    {Specifier::_double,    8},
    {Specifier::_unsigned,  4}
};

struct Variable
{

    Specifier type;     // Type of variable - only support int for now
    int offset;         // Offset from frame pointer
    bool isPointer;     // Default false

    Variable(): type(Specifier::_int), offset(0), isPointer(false) {}

    Variable(Specifier _type, int _offset, bool _isPointer = false)
        : type(_type), offset(_offset), isPointer(_isPointer) {}

};

struct LoopLabel {
    std::string startLabel;
    std::string endLabel;
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

    std::array<bool, 64> usedRegs = {
        // base registers
        1, 1, 1, 1, 1,
        0, 0, 0,
        1, 1,
        1, 1, 1, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0,
        // float registers
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1,
        1, 1,
        1, 1, 0, 0, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0
    };

    // stack
    int local_var_offset = -16;     // track current local var offset
    int param_offset = -32;         // track current param offset
    int param_offset_excess = 0;    // track params on previous stack frame

    // local context
    bool isFunctionDef = 0;
    std::string ret_label;
    Specifier current_func_type;

    std::vector<Scope> scopes;
    std::vector<LoopLabel> loopLabels;
    std::unordered_map<std::string, Specifier> functionReturnTypes;
    // std::vector<std::pair<std::string, Specifier>> functionReturnTypes;

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

    int allocateFloatReg() {
        for (int i = 37; i < 64; i++) {
            if (!usedRegs[i]) {
                useReg(i);
                return i;
            }
        }
        return -1;
    }

    std::string getMnemonic(int i) {
        static const std::array<std::string, 64> regNames = {
            "zero", "ra", "sp", "gp", "tp",
            "t0", "t1", "t2",
            "s0", "s1",
            "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
            "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
            "t3", "t4", "t5", "t6",

            "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
            "fs0", "fs1",
            "fa0", "fa1",
            "fa2", "fa3", "fa4", "fa5", "fa6", "fa7",
            "fs2", "fs3", "fs4", "fs5", "fs6", "fs7","fs8", "fs9", "fs10", "fs11",
            "ft8", "ft9", "ft10", "ft11"
        };

        return regNames[i];
    }

    /* ----------------------------------HANDLE VARS------------------------------------------- */

    Variable getVar(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            Variable var = it->getLocalVar(name);
            if (var.offset != -1) return var;
        }
        return Variable(Specifier::INVALID_TYPE, -1);
    }

    int getVarOffset(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            int offset = it->getLocalVar(name).offset;
            if (offset != -1) {
                return offset;
            }
        }
        return -1;
    }

    Specifier getVarType(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            Specifier type = it->getLocalVar(name).type;
            if (type != Specifier::INVALID_TYPE) return type;
        }
        // will return INVALID _TYPE if variable doesnt exist anyway
        return Specifier::INVALID_TYPE;
    }

    int addVar(const std::string& name, Specifier type) {
        int var_size = typeSizes.at(type);
        local_var_offset -= var_size;
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

    void addArray(const std::string& array_name, Specifier type, int array_size) {
        int var_size = typeSizes.at(type);
        int total_array_size = array_size * var_size;

        scopes.back().addLocalVar(array_name, type, local_var_offset - var_size);

        local_var_offset -= total_array_size;
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
    }

    void saveFuncReturnType(const std::string& func_name, Specifier return_type) {
        functionReturnTypes[func_name] = return_type;
        current_func_type = return_type;
    }

    /* ----------------------------------HANDLE LOOPS------------------------------------------- */

    void addLabels(const std::string& start_label, const std::string& end_label) {
        loopLabels.push_back({start_label, end_label});
    }

    void popLabels() {
        if (!loopLabels.empty()) {
            loopLabels.pop_back();
        }
    }

    void updateStartLabel(const std::string& new_start_label) {
        if (!loopLabels.empty()) {
            loopLabels.back().startLabel = new_start_label;
        }
    }

    std::string getCurrentLoopStart() {
        if (!loopLabels.empty()) {
            return loopLabels.back().startLabel;
        }
        return "";
    }

    /*  Case statements add looplabel with empty end_label
        To allow break to keep working, we have to ignore this, and return
        first valid label - so we iterate backwards
    */
    std::string getCurrentLoopEnd() {
        for (auto it = loopLabels.rbegin(); it != loopLabels.rend(); ++it) {
            if (!it->endLabel.empty()) {
                return it->endLabel;
            }
        }
        return "";
    }

    /* ----------------------------------UTILITY------------------------------------------- */

    int calculateStackSize(int totalLocalVarBytes, int totalParamBytes) {
        int stack_size = 16;  // Minimum stack size

        // Calculate space needed for local variables
        // can set the local_var_offset here

        // allocate 4 bytes for floats - helps guarantee floats don't overwrite vars
        totalParamBytes += 8;

        if (totalLocalVarBytes > 0) {
            stack_size += 16 * std::ceil(static_cast<double>(totalLocalVarBytes) / 16.0);
        }

        // can set the param offset here
        param_offset = -stack_size;

        // Calculate space needed for parameters
        if (totalParamBytes > 0) {
            stack_size += 16 * std::ceil(static_cast<double>(totalParamBytes) / 16.0);
        }

        return stack_size;
    }

    std::string specifierToString(Specifier specifier) const {
        switch (specifier) {
            case Specifier::_int:       return "int";
            case Specifier::_char:      return "char";
            case Specifier::_void:      return "void";
            case Specifier::_float:     return "float";
            case Specifier::_double:    return "double";
            case Specifier::_unsigned:  return "unsigned";
            default:                    return "INVALID_TYPE";
        }
    }

    void debugScope() const {
        std::cerr << "------ DEBUGGING SCOPES ------" << std::endl;

        for (size_t i = 0; i < scopes.size(); ++i) {
            std::cerr << "Scope " << i + 1 << ":" << std::endl;
            for (const auto& binding : scopes[i].bindings) {
                std::cerr << "  Name: " << binding.first
                          << ", Type: " << specifierToString(binding.second.type)
                          << ", Offset: " << binding.second.offset << std::endl;
            }
            std::cerr << std::endl;
        }

        std::cerr << "-----------------------------" << std::endl;
    }

    inline std::string makeLabel (std::string label){
        static int unique = 0 ;
        return label + std::to_string(unique++);
    }
};

