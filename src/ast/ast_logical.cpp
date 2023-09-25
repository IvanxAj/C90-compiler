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


void LogicalAnd::compile(std::ostream& os, Context& context, int destReg) const {

    std::string false_label = context.makeLabel(".AND_FALSE");
    std::string end_label = context.makeLabel(".AND_END");

    int left_reg = context.allocateReg();
    if(right->isFuncCall()) {
        left_reg = 9;
    }
    context.useReg(left_reg);

    left->compile(os, context, left_reg);
    os << "beq " << context.getMnemonic(left_reg) << ", zero, " << false_label << std::endl;

    // would short circuit, so can use same reg
    right->compile(os, context, left_reg);
    os << "beq " << context.getMnemonic(left_reg) << ", zero, " << false_label << std::endl;

    // If both are true, set destination register to 1 (true)
    os << "li " << context.getMnemonic(destReg) << ", 1" << std::endl;
    os << "j " << end_label << std::endl;

    // If either expression is false, set destReg to 0
    os << false_label << ":" << std::endl;
    os << "li " << context.getMnemonic(destReg) << ", 0" << std::endl;
    os << end_label << ":" << std::endl;

    context.freeReg(left_reg);
}


void LogicalOr::compile(std::ostream& os, Context& context, int destReg) const {

    std::string true_label = context.makeLabel(".OR_TRUE");
    std::string end_label = context.makeLabel(".OR_END");

    int left_reg = context.allocateReg();
    if(right->isFuncCall()) {
        left_reg = 9;
    }
    context.useReg(left_reg);

    left->compile(os, context, left_reg);
    os << "bne " << context.getMnemonic(left_reg) << ", zero, " << true_label << std::endl;

    right->compile(os, context, left_reg);
    os << "bne " << context.getMnemonic(left_reg) << ", zero, " << true_label << std::endl;

    // If both are false, set destination register to 0 (false)
    os << "li " << context.getMnemonic(destReg) << ", 0" << std::endl;
    os << "j " << end_label << std::endl;

    // If either expression is true, set destReg to 1 (true)
    os << true_label << ":" << std::endl;
    os << "li " << context.getMnemonic(destReg) << ", 1" << std::endl;

    // End label
    os << end_label << ":" << std::endl;
    context.freeReg(left_reg);
}
