#include "ast/ast_arithmetic.hpp"

Addition::Addition(BaseExpression* _left, BaseExpression* _right)
    : left(_left), right(_right) {}

Addition::~Addition() {
    delete left;
    delete right;
}

void Addition::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = context.allocateReg();
    context.useReg(left_reg);
    left->compile(os, context, left_reg);

    context.freeReg(destReg);

    int right_reg = context.allocateReg();
    context.useReg(right_reg);
    right->compile(os, context, right_reg);

    os << "add " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}



Subtraction::Subtraction(BaseExpression* _left, BaseExpression* _right)
    : left(_left), right(_right) {}

Subtraction::~Subtraction() {
    delete left;
    delete right;
}

void Subtraction::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = context.allocateReg();
    context.useReg(left_reg);
    left->compile(os, context, left_reg);

    context.freeReg(destReg);

    int right_reg = context.allocateReg();
    context.useReg(right_reg);
    right->compile(os, context, right_reg);

    os << "sub " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);
}
