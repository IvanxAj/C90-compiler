#include "ast/ast_init_declarator.hpp"

// Constructors

Init_Declarator::Init_Declarator(BaseDeclaration* _declarator, BaseExpression* _initialiser)
    : declarator(_declarator), initialiser(_initialiser) {}

Init_Declarator::Init_Declarator(BaseDeclaration* _declarator)
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
        Specifier type = context.getVarType(var_name);

        int init_reg;
        std::string store_instruction;

        switch(type) {
            case Specifier::_int:
                init_reg = context.allocateReg();
                store_instruction = "sw ";
                break;
            case Specifier::_float:
                init_reg = context.allocateFloatReg();
                store_instruction = "fsw ";
                break;
            case Specifier::_double:
                init_reg = context.allocateFloatReg();
                store_instruction = "fsd ";
                break;
            default:
                std::cerr << "Initialiser: Invalid type. " << std::endl;
                exit(1);
        }

        context.useReg(init_reg);
        // evaluate initialiser expression e.g. (3+4) = 7
        initialiser->compile(os, context, init_reg);

        int var_offset = context.getVarOffset(var_name);


        os << store_instruction << context.getMnemonic(init_reg) << ", " << var_offset << "(s0)" << std::endl;

        context.freeReg(init_reg);
    }
}
