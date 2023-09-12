#include "ast/ast_logical.hpp"


void Equal::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "sub " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
    os << "seqz " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << std::endl;
    os << "andi " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << ", 0xff" << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void NotEqual::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "sub " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
    os << "snez " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << std::endl;
    os << "andi " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << ", 0xff" << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void LessThan::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "slt " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
    os << "andi " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << ", 0xff" << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void GreaterThan::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "sgt " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
    os << "andi " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << ", 0xff" << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void LessThanEqual::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "sgt " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
    os << "xori " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << ", " << "1" << std::endl;
    os << "andi " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << ", 0xff" << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void GreaterThanEqual::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "slt " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
    os << "xori " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << ", " << "1" << std::endl;
    os << "andi " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << ", 0xff" << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void BitwiseAnd::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "and " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void BitwiseXor::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "xor " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void BitwiseOr::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    os << "or " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}
