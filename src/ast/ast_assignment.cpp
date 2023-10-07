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
    Specifier var_type = var.type;
    int var_size = typeSizes.at(var_type);

    int right_reg = context.allocateReg(var_type);

    context.useReg(right_reg);
    expr2->compile(os, context, right_reg);

    // at this point r-value is in right_reg
    if(expr1->isDerefPointer()) {
        // l value is a dereferenced pointer eg int *p = &a; *p = 20;
        int address_reg = context.allocateReg(Specifier::_int);
        context.useReg(address_reg);

        context.loadInstruction(os, Specifier::_int, address_reg, var.offset);
        context.storeInstruction(os, var_type, right_reg, 0, address_reg);

        context.freeReg(address_reg);

    } else if (var.is_pointer) {
        // l value variable is pointer eg int *p; p = &a;
        // cant use normal variable one as pointer variables store type of what they are pointing to
        context.storeInstruction(os, Specifier::_int, right_reg, var.offset);

    } else if (!expr1->isArray()) {
        // l-value is a variable
        context.storeInstruction(os, var_type, right_reg, var.offset);

    } else {
        // l-value is an array element/index a[i] = 5;
        // have a check to see if array in global or local scope
        // local assignment

        int index_reg = context.allocateReg(Specifier::_int);
        context.useReg(index_reg);
        auto index_expression = dynamic_cast<const ArrayIndex*>(expr1);
        index_expression->getIndex()->compile(os, context, index_reg);

        os << "slli " << context.getMnemonic(index_reg) << ", " << context.getMnemonic(index_reg) << ", " << log2(var_size) << std::endl;
        os << "sub " << context.getMnemonic(index_reg) << ", s0, " << context.getMnemonic(index_reg) << std::endl;
        os << "addi " << context.getMnemonic(index_reg) << ", " << context.getMnemonic(index_reg) << ", " << var.offset << std::endl;

        context.storeInstruction(os, var_type, right_reg, 0, index_reg);

        context.freeReg(index_reg);
    }

    context.freeReg(right_reg);

}
