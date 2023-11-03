#include "ast/ast_init_declarator.hpp"

// Constructors

InitDeclarator::InitDeclarator(BaseDeclaration* _declarator, BaseExpression* _initialiser)
    : declarator(_declarator), initialiser(_initialiser) {}

InitDeclarator::~InitDeclarator() {
    delete declarator;
    delete initialiser;
}

bool InitDeclarator::isPointer() const {
    return declarator->isPointer();
}

int InitDeclarator::getArraySize() const {
    return declarator->getArraySize();
}

const std::string& InitDeclarator::getIdentifier() const {
    return declarator->getIdentifier();
}

void InitDeclarator::compile(std::ostream& os, Context& context, int destReg) const {

    if (getArraySize() != -1) {
        // array initialisation
        std::string array_name = declarator->getIdentifier();
        int array_address_reg = context.allocateReg(Specifier::_int);
        context.useReg(array_address_reg);

        // load the memory address of element 0 into the reg
        int base_offset = context.getVarOffset(array_name);
        os << "addi " << context.getMnemonic(array_address_reg) << ", " << "s0, " << base_offset << std::endl;
        initialiser->compile(os, context, array_address_reg);

        context.freeReg(array_address_reg);

        return;

    }

    // local + global variable initialisation
    std::string var_name = declarator->getIdentifier();
    Variable var = context.getVar(var_name);

    // Pointers themselves use standard int regs + operations
    if(isPointer()) var.type = Specifier::_int;

    // TODO: Check if global variable - store initialiser
    if (var.offset == 0) {
        int data = initialiser->getValue();
        // only for int right now
        auto& properties = context.heapMemory.at(var_name).properties;
        properties.push_back(".globl " + var_name);
        std::string size_field = ".size " + var_name + ", " + std::to_string(typeSizes.at(var.type));
        properties.push_back(size_field);
        properties.push_back(var_name + ":");
        properties.push_back(".word " + std::to_string(data));

        return;
    }

    int init_reg = context.allocateReg(var.type);
    context.useReg(init_reg);
    // evaluate initialiser expression e.g. (3+4) = 7
    initialiser->compile(os, context, init_reg);

    context.storeInstruction(os, var.type, init_reg, var.offset);

    context.freeReg(init_reg);

}
