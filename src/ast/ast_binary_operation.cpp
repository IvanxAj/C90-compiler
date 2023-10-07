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

int BinaryOperation::allocateRegBasedOnType(Context& context, Specifier type, bool isFunctionCall) const {

    int reg = context.allocateReg(type);

    if (isFunctionCall) {
        reg = (type == Specifier::_int) ? 9 : 41; // s1 or fs1 callee saved reg
    }

    context.useReg(reg);
    return reg;
}

int BinaryOperation::prepLeft(std::ostream& os, Context& context, int destReg) const {
    Specifier type = left->getType(context);

    // only need to check if left is a function call - as left recursive ((1+2)+3)..
    int leftReg = allocateRegBasedOnType(context, type, right->isFuncCall());

    left->compile(os, context, leftReg);
    return leftReg;
}

int BinaryOperation::prepRight(std::ostream& os, Context& context, int destReg) const {
    Specifier type = right->getType(context);
    int var_size = typeSizes.at(type);
    // if left was a pointer var, then right operand should be an integer - for pointer arithemetic
    // int *p = &a, p = p + 2 (p + size(pointed by p) * 2)
    bool is_pointer_var = !left->isDerefPointer() && context.getIsPointer(left->getIdentifier());

    int rightReg = allocateRegBasedOnType(context, type, false);
    right->compile(os, context, rightReg);

    if (is_pointer_var) {
        os << "slli " << context.getMnemonic(rightReg) << ", " << context.getMnemonic(rightReg) << ", " << log2(var_size) << std::endl;
    }

    return rightReg;
}

void BinaryOperation::compile(std::ostream& os, Context& context, int destReg) const {
    std::cerr << "Error: compile() should not be called on a BinaryOperation object" << std::endl;
};
