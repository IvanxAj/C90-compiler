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

    int reg = context.allocateReg();
    context.useReg(reg);
    expr2->compile(os, context, reg);

    os << "sw " << context.getMnemonic(reg) << ", " << offset << "(s0)" << std::endl;

    context.freeReg(reg);

}
