#include "ast/ast_binary_operation.hpp"

BinaryOperation::BinaryOperation(BaseExpression* _left, BaseExpression* _right, bool _owns_operands)
        : left(_left), right(_right), owns_operands(_owns_operands) {}

BinaryOperation::~BinaryOperation() {
    if (owns_operands) delete left;
    delete right;
};

int BinaryOperation::prepLeft(std::ostream& os, Context& context, int destReg) const {

    int left_reg = context.allocateReg();
    // check if right is a function call - then allocate a callee saved reg eg s1
    if(right->getFuncCall()) {
        left_reg = 9; // s1 callee saved reg - should be fine to hardcode idk?
    }
    context.useReg(left_reg);
    left->compile(os, context, left_reg);

    return left_reg;

};
int BinaryOperation::prepRight(std::ostream& os, Context& context, int destReg) const{

    int right_reg = context.allocateReg();
    context.useReg(right_reg);
    right->compile(os, context, right_reg);

    return right_reg;

};

void BinaryOperation::compile(std::ostream& os, Context& context, int destReg) const {
    std::cerr << "Error: compile() should not be called on a BinaryOperation object" << std::endl;
};
