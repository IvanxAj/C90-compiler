
#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <ostream>
#include <bit>

constexpr int STACK_ALIGNMENT = 16;

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
    _float,
    _double,
    _void,
    _unsigned,
    INVALID_TYPE = -1,
};

enum class HeapObjectTypes
{
    Basic,
    Array,
    String,
    Temp
};

struct HeapObject {
    HeapObjectTypes object_type;
    std::vector<std::string> properties;

    // default
    HeapObject() : object_type(HeapObjectTypes::Basic), properties({}) { }

    HeapObject(HeapObjectTypes _type, const std::vector<std::string>& _properties)
        : object_type(_type), properties(_properties) { }
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
    bool is_pointer;    // Default false

    Variable(): type(Specifier::_int), offset(0), is_pointer(false) {}

    Variable(Specifier _type, int _offset, bool _is_pointer = false)
        : type(_type), offset(_offset), is_pointer(_is_pointer) {}

};

struct LoopLabel {
    std::string start_label;
    std::string end_label;
};

struct Scope
{
    std::unordered_map<std::string, Variable> bindings;     // Track variables in scope
    std::unordered_map<std::string,int> enums;              // Track enums in scope

    void addLocalVar(const std::string& name, Specifier type, int offset, bool isPointer = false) {
        bindings[name] = Variable(type, offset, isPointer);
    }

    Variable getLocalVar(const std::string& name) const {
        auto it = bindings.find(name);
        return it == bindings.end() ? Variable(Specifier::INVALID_TYPE, -1) : it->second;
    }

    void addLocalEnum(const std::string& name, int value) {
        enums[name] = value;
    }

    bool enumExists(const std::string& name) const {
        return enums.find(name) != enums.end();
    }

    int getLocalEnum(const std::string& name) const {
        // would have already checked if enumExists in a certain scope, so should always be found
        return enums.at(name);
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
    int local_var_offset = -STACK_ALIGNMENT;     // track current local var offset
    int param_offset = -2 * STACK_ALIGNMENT;     // track current param offset
    // int param_offset_excess = 0;              // track params on previous stack frame

    // local context
    bool is_function_def = 0;
    std::string ret_label;
    Specifier current_func_type;

    std::vector<Scope> scopes;
    std::vector<LoopLabel> loopLabels;
    std::unordered_map<std::string, Specifier> functionReturnTypes;
    std::unordered_map<std::string, HeapObject> heapMemory;

    // Add a global scope on constructor
    Context() {
        scopes.push_back(Scope());
    }

    /* ----------------------------------HANDLE REGS------------------------------------------- */

    void useReg(int i) { usedRegs[i] = 1; }
    void freeReg(int i) { usedRegs[i] = 0; }

    int allocateReg(Specifier type) {
        // initialise to int start and end registers
        int start = 5;
        int end = 32;
        // change to float registers based on type
        if (type == Specifier::_float || type == Specifier::_double) {
            start = 37;
            end = 64;
        }
        for (int i = start; i < end; i++) {
            if (!usedRegs[i]) {
                useReg(i);
                return i;
            }
        }
        return -1;
    }

    std::string getMnemonic(int i) const {
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
        local_var_offset = -STACK_ALIGNMENT;
        param_offset = -2 * STACK_ALIGNMENT;
        // param_offset_excess = 0;
    }

    void saveFuncReturnType(const std::string& func_name, Specifier return_type) {
        functionReturnTypes[func_name] = return_type;
        current_func_type = return_type;
    }

    /* ----------------------------------HANDLE HEAP------------------------------------------- */

    void addDataBasedOnSpecifier(std::vector<std::string>& properties, Specifier type, double value) {
        switch (type) {
            case Specifier::_int:
                properties.push_back(".word " + std::to_string(static_cast<int>(value)));
                break;
            case Specifier::_float: {
                uint32_t int_bits = std::bit_cast<uint32_t>(static_cast<float>(value));
                properties.push_back(".word " + std::to_string(int_bits));
                break;
            }
            case Specifier::_double: {
                uint64_t bits = std::bit_cast<uint64_t>(static_cast<double>(value));
                int32_t low = static_cast<int32_t>(bits);
                int32_t high = static_cast<int32_t>(bits >> 32);
                properties.push_back(".word " + std::to_string(low));
                properties.push_back(".word " + std::to_string(high));
                break;
            }
            case Specifier::_char:
                properties.push_back(".byte " + std::to_string(static_cast<int>(value)));
                break;
            default:
                // Handle other specifiers or throw an error
                break;
        }
    }

    void addHeapObject(HeapObjectTypes type, const std::string& identifier, Specifier spec, int size, const std::vector<double>& data) {
        std::vector<std::string> properties;

        // Basic and Array types have some common properties
        if (type == HeapObjectTypes::Basic || type == HeapObjectTypes::Array) {
            properties.push_back(".globl " + identifier);
            properties.push_back(".size " + identifier + ", " + std::to_string(size));
            properties.push_back(identifier + ":");
        }

        if (type == HeapObjectTypes::Basic) {
            addDataBasedOnSpecifier(properties, spec, data[0]);

        } else if (type == HeapObjectTypes::Array) {
            // TODO: think through arrays fully
            for (size_t i = 0; i < data.size(); ++i) {
                addDataBasedOnSpecifier(properties, spec, data[i]);
            }
            int remainingSize = size - typeSizes.at(spec) * data.size();
            if (remainingSize > 0) {
                properties.push_back(".zero " + std::to_string(remainingSize));
            }

        } else if (type == HeapObjectTypes::Temp) {
            properties.push_back(identifier + ":");
            addDataBasedOnSpecifier(properties, spec, data[0]);
        }

        // Add the object to heapMemory
        heapMemory[identifier] = HeapObject(type, properties);
    }

    /* ----------------------------------HANDLE VARS------------------------------------------- */

    Variable getVar(const std::string& name) const {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            Variable var = it->getLocalVar(name);
            if (var.offset != -1) return var;
        }
        return Variable(Specifier::INVALID_TYPE, -1, false);
    }

    int getVarOffset(const std::string& name) const {
        Variable var = getVar(name);
        return var.offset;
    }

    Specifier getVarType(const std::string& name) const {
        Variable var = getVar(name);
        return var.type;
    }

    bool getIsPointer(const std::string& name) const {
        Variable var = getVar(name);
        return var.is_pointer;
    }

    void addVar(const std::string& name, Specifier type, bool is_pointer = false) {
        int var_size = typeSizes.at(type);
        // type of a pointer, is the type of variable it is pointing to - need to manually set 4
        if (is_pointer == true) var_size = 4;

        if (scopes.size() == 1) {
            // global variable
            scopes.back().addLocalVar(name, type, 0, is_pointer);
            return;
        } else {
            // local variable
            local_var_offset -= var_size;
            scopes.back().addLocalVar(name, type, local_var_offset, is_pointer);
            return;
        }
    }

    int addParam(const std::string& name, Specifier type, int param_index, bool is_pointer = false) {
        int param_size = typeSizes.at(type);
        if (is_pointer == true) param_size = 4;

        if (param_index < 8) {
            param_offset -= param_size;
            scopes.back().addLocalVar(name, type, param_offset, is_pointer);
            return param_offset;
        }

        // scopes.back().addLocalVar(name, type, param_offset_excess, is_pointer);
        // param_offset_excess += param_size;
        // 1 indicates that the param was not taken from register, and is already on the stack
        return 1;
    }

    void addArray(const std::string& array_name, Specifier type, int array_size) {
        int var_size = typeSizes.at(type);
        int total_array_size = array_size * var_size;

        if (scopes.size() == 1) {
            // global array
            scopes.back().addLocalVar(array_name, type, 0);
            return;
        } else {
            // local array
            scopes.back().addLocalVar(array_name, type, local_var_offset - var_size);
            local_var_offset -= total_array_size;
        }
    }

    void addEnum(const std::string& name, int value) {
        scopes.back().addLocalEnum(name, value);
    }

    bool isEnum(const std::string& name) const {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            if (it->enumExists(name)) return true;
        }
        return false;
    }

    int getEnumValue(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            if (it->enumExists(name)) return it->getLocalEnum(name);
        }
        // should always have returned a value - only run after isEnum
        return -1;
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
            loopLabels.back().start_label = new_start_label;
        }
    }

    std::string getCurrentLoopStart() const {
        if (!loopLabels.empty()) {
            return loopLabels.back().start_label;
        }
        return "";
    }

    /*  Case statements add looplabel with empty end_label
        To allow break to keep working, we have to ignore this, and return
        first valid label - so we iterate backwards
    */
    std::string getCurrentLoopEnd() const {
        for (auto it = loopLabels.rbegin(); it != loopLabels.rend(); ++it) {
            if (!it->end_label.empty()) {
                return it->end_label;
            }
        }
        return "";
    }

    int calculateStackSize(int totalLocalVarBytes, int totalParamBytes) {
        int stack_size = STACK_ALIGNMENT;  // Minimum stack size

        // Calculate space needed for local variables
        if (totalLocalVarBytes > 0) {
            stack_size += STACK_ALIGNMENT * std::ceil(static_cast<double>(totalLocalVarBytes) / STACK_ALIGNMENT);
        }

        param_offset = -stack_size;

        // Calculate space needed for parameters
        if (totalParamBytes > 0) {
            stack_size += STACK_ALIGNMENT * std::ceil(static_cast<double>(totalParamBytes) / STACK_ALIGNMENT);
        }

        return stack_size;
    }

    /* ----------------------------------UTILITY------------------------------------------- */

    // lw destReg, offset(baseReg) for input type
    void loadInstruction(std::ostream& os, Specifier type, int dest_reg, int offset, int address_reg = 8 ) const {
        // address_reg defaults to s0
        switch(type) {
            case Specifier::_int:
                os << "lw " << getMnemonic(dest_reg) << ", " << offset << "(" << getMnemonic(address_reg) << ") " << std::endl;
                break;
            case Specifier::_float:
                os << "flw " << getMnemonic(dest_reg) << ", " << offset << "(" << getMnemonic(address_reg) << ") " << std::endl;
                break;
            case Specifier::_double:
                os << "fld " << getMnemonic(dest_reg) << ", " << offset << "(" << getMnemonic(address_reg) << ") " << std::endl;
                break;
            default:
                std::cerr << "Load Instruction with unknown type" << std::endl;
                exit(1);
            }
    }

    // sw destReg, offset(baseReg) for input type
    void storeInstruction(std::ostream& os, Specifier type, int dest_reg, int offset, int address_reg = 8 ) const {
        // address_reg defaults to s0
        switch(type) {
            case(Specifier::_int):
                os << "sw " << getMnemonic(dest_reg) << ", " << offset << "(" << getMnemonic(address_reg) << ") " << std::endl;
                break;
            case(Specifier::_float):
                os << "fsw " << getMnemonic(dest_reg) << ", " << offset << "(" << getMnemonic(address_reg) << ") " << std::endl;
                break;
            case(Specifier::_double):
                os << "fsd " << getMnemonic(dest_reg) << ", " << offset << "(" << getMnemonic(address_reg) << ") " << std::endl;
                break;
            default:
                std::cerr << "Store Instruction with unknown type" << std::endl;
                exit(1);
            }
    }

    inline std::string makeLabel (std::string label){
        static int unique = 0 ;
        return label + std::to_string(unique++);
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

    void printVariableInfo(const std::string& var_name, const Variable& var) const {
        std::cerr << "Variable: " << var_name
                << " | Type: " << specifierToString(var.type)
                << " | Offset: " << var.offset
                << " | Is Pointer: " << (var.is_pointer ? "Yes" : "No")
                << std::endl;
    }

    void debugScope() const {
        std::cerr << "------ DEBUGGING SCOPES ------" << std::endl;

        // for (const auto& pair : heapMemory) {
        //     const auto& identifier = pair.first;
        //     const auto& object = pair.second;

        //     std::cerr << identifier << ":" << std::endl;
        //     for (const auto& property : object.properties) {
        //         std::cerr << "    " << property << std::endl;
        //     }
        //     std::cerr << std::endl;
        // }

        for (size_t i = 0; i < scopes.size(); ++i) {
            std::cerr << "Scope " << i << ":" << std::endl;

            // Output variables
            std::cerr << "  Variables:" << std::endl;
            for (const auto& [var_name, var] : scopes[i].bindings) {
                printVariableInfo(var_name, var);
            }

            // Output enums
            std::cerr << "  Enums:" << std::endl;
            for (const auto& [enum_name, enum_value] : scopes[i].enums) {
                std::cerr << "    " << enum_name << " = " << enum_value << std::endl;
            }

            std::cerr << std::endl;
        }

        std::cerr << "-----------------------------" << std::endl;
    }

};

