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

int Init_Declarator::getArraySize() const {
    return declarator->getArraySize();
}

const std::string& Init_Declarator::getIdentifier() const {
    return declarator->getIdentifier();
}

void Init_Declarator::compile(std::ostream& os, Context& context, int destReg) const {

    if (initialiser == nullptr) return;

    if (getArraySize() != -1) {
        // array initialisation
        std::string array_name = declarator->getIdentifier();
        int array_address_reg = context.allocateReg();
        context.useReg(array_address_reg);

        // load the memory address of element 0 into the reg
        int base_offset = context.getVarOffset(array_name);
        os << "addi " << context.getMnemonic(array_address_reg) << ", " << "s0, " << base_offset << std::endl;
        initialiser->compile(os, context, array_address_reg);

        context.freeReg(array_address_reg);

    } else {
        // variable initialisation

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
