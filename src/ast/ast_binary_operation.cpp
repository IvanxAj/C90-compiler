#include "ast/ast_binary_operation.hpp"

BinaryOperation::BinaryOperation(BaseExpression* _left, BaseExpression* _right, bool _owns_operands)
        : left(_left), right(_right), owns_operands(_owns_operands) {}

BinaryOperation::~BinaryOperation() {
    if (owns_operands) delete left;
    delete right;
};

Specifier BinaryOperation::getType(Context& context) const {
    return right->getType(context);
}

int BinaryOperation::prepLeft(std::ostream& os, Context& context, int destReg) const {
    Specifier type = left->getType(context);

    int left_reg = -1;

    if (type == Specifier::_int) {
        left_reg = context.allocateReg();
        if(right->isFuncCall()) {
            left_reg = 9; // s1 callee saved reg
        }
        std::cerr << "Left: Int type" << std::endl;
    } else if (type == Specifier::_double || type == Specifier::_float) {
        left_reg = context.allocateFloatReg();
        if(right->isFuncCall()) {
            left_reg = 41; // fs1 callee saved reg
        }
        std::cerr << "Left: double/float type" << std::endl;
    } else {
        std::cerr << "Error: Invalid type" << std::endl;
    }
    // check if right is a function call - then allocate a callee saved reg eg s1
    context.useReg(left_reg);

    left->compile(os, context, left_reg);
    return left_reg;

};
int BinaryOperation::prepRight(std::ostream& os, Context& context, int destReg) const {

    Specifier type = right->getType(context);
    int right_reg = -1;

    if (type == Specifier::_int) {
        std::cerr << "Right: Int type" << std::endl;
        right_reg = context.allocateReg();
    } else if (type == Specifier::_double || type == Specifier::_float) {
        std::cerr << "Right: double/float type type" << std::endl;
        right_reg = context.allocateFloatReg();
    }

    context.useReg(right_reg);
    right->compile(os, context, right_reg);

    return right_reg;

};

void BinaryOperation::compile(std::ostream& os, Context& context, int destReg) const {
    std::cerr << "Error: compile() should not be called on a BinaryOperation object" << std::endl;
};
