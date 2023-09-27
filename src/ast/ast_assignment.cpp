#include "ast/ast_assignment.hpp"

Assignment::Assignment(BaseExpression* _expr1, BaseExpression* _expr2)
    : expr1(_expr1), expr2(_expr2) {}

Assignment::~Assignment() {
    delete expr1;
    delete expr2;
}

void Assignment::compile(std::ostream& os, Context& context, int destReg) const {
    std::string var_name = expr1->getIdentifier();
    Variable var = context.getVar(var_name);
    // context.printVariableInfo(var_name, var);
    int var_size = typeSizes.at(var.type);

    int right_reg = -1;
    std::string store_instruction;

    switch(var.type) {
        case Specifier::_int:
            right_reg = context.allocateReg();
            store_instruction = "sw ";
            break;
        case Specifier::_float:
            right_reg = context.allocateFloatReg();
            store_instruction = "fsw ";
            break;
        case Specifier::_double:
            right_reg = context.allocateFloatReg();
            store_instruction = "fsd ";
            break;
        default:
            std::cerr << "Assignment: Invalid type" << std::endl;
            exit(1);
    }

    context.useReg(right_reg);
    expr2->compile(os, context, right_reg);

    // at this point r-value is in right_reg
    if(expr1->isPointer()) {
        // l value is a dereferenced pointer eg int *p = &a; *p = 20;
        int address_reg = context.allocateReg();
        context.useReg(address_reg);

        os << "lw " << context.getMnemonic(address_reg) << ", " << var.offset << "(s0)" << std::endl;
        os << store_instruction << context.getMnemonic(right_reg) << ", " << "0(" << context.getMnemonic(address_reg) << ")" << std::endl;

        context.freeReg(address_reg);

    } else if ( var.isPointer == true) {
        // l value variable is pointer eg int *p; p = &a;
        // cant use normal variable one as pointer variables store type of what they are pointing to
        os << "lw " << context.getMnemonic(right_reg) << ", " << var.offset << "(s0)" << std::endl;

    } else if (!expr1->isArray()) {
        // l-value is a variable
        os << store_instruction << context.getMnemonic(right_reg) << ", " << var.offset << "(s0)" << std::endl;

    } else {
        // l-value is an array element/index a[i] = 5;
        // have a check to see if array in global or local scope
        // local assignment

        int index_reg = context.allocateReg();
        context.useReg(index_reg);
        auto index_expression = dynamic_cast<const ArrayIndex*>(expr1);
        index_expression->getIndex()->compile(os, context, index_reg);

        os << "slli " << context.getMnemonic(index_reg) << ", " << context.getMnemonic(index_reg) << ", " << log2(var_size) << std::endl;
        os << "sub " << context.getMnemonic(index_reg) << ", s0, " << context.getMnemonic(index_reg) << std::endl;
        os << "addi " << context.getMnemonic(index_reg) << ", " << context.getMnemonic(index_reg) << ", " << var.offset << std::endl;

        os << store_instruction << context.getMnemonic(right_reg) << ", " << "0(" << context.getMnemonic(index_reg) << ")" << std::endl;

        context.freeReg(index_reg);
    }

    context.freeReg(right_reg);

}
