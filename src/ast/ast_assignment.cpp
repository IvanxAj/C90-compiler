#include "ast/ast_assignment.hpp"

Assignment::Assignment(BaseExpression* _expr1, BaseExpression* _expr2)
    : expr1(_expr1), expr2(_expr2) {}

Assignment::~Assignment() {
    delete expr1;
    delete expr2;
}

void Assignment::compile(std::ostream& os, Context& context, int destReg) const {
    std::string var_name = expr1->getIdentifier();
    int offset = context.getVarOffset(var_name);
    Specifier type = context.getVarType(var_name);

    int right_reg;
    std::string store_instruction;

    switch(type) {
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
            std::cerr << "Assignment: Invalid type. " << std::endl;
            exit(1);
    }

    context.useReg(right_reg);
    expr2->compile(os, context, right_reg);

    os << store_instruction << context.getMnemonic(right_reg) << ", " << offset << "(s0)" << std::endl;

    context.freeReg(right_reg);

}
