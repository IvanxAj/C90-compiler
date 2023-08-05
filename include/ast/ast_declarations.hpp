#pragma once

#include "ast_node.hpp"

class Declarator
    : Node
{
// printing identifier is behaviour of functiondef not declarator -> pass up
public:
    Declarator(std::string _identifier) : identifier(_identifier) {}


    void compile(std::ostream& os, Context& context, int destReg) const override {
        os << ".text" << std::endl;
        os << ".globl " << identifier << std::endl;
        os << identifier << ":" << std::endl;
        os << "addi sp,sp,-16" << std::endl;
        os << "sw s0,12(sp)" << std::endl;
        os << "addi s0,sp,16" << std::endl;
    }

    // only declarator have identifier so getIdentifier method only for declarators
    const std::string& getIdentifier() const {
        return identifier;
    }

private:
    std::string identifier;
};
