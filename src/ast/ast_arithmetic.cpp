#include "ast/ast_arithmetic.hpp"


void Addition::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "add " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void Subtraction::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "sub " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);
}


void Multiplication::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "mul " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void Division::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "div " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void Modulus::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "rem " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void LeftShift::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "sll " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void RightShift::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "sra " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}
