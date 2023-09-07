#include "ast/ast_init_declarator.hpp"

// Constructors

Init_Declarator::Init_Declarator(BaseDeclarator* _declarator, BaseExpression* _initialiser)
    : declarator(_declarator), initialiser(_initialiser) {}

Init_Declarator::Init_Declarator(BaseDeclarator* _declarator)
    : declarator(_declarator), initialiser(nullptr) {}

Init_Declarator::~Init_Declarator() {
    delete declarator;
    delete initialiser;
}

const std::string& Init_Declarator::getIdentifier() {
    return declarator->getIdentifier();
}


void Init_Declarator::compile(std::ostream& os, Context& context, int destReg) const {

    if (initialiser !=  nullptr) {
        std::string var_name = declarator->getIdentifier();

        int reg = context.allocateReg();
        context.useReg(reg);
        initialiser->compile(os, context, reg);

        int var_offset = context.getVar(var_name);
        os << "sw " << context.getMnemonic(reg) << ", " << var_offset << "(s0)" << std::endl;

        context.freeReg(reg);
    }
}
