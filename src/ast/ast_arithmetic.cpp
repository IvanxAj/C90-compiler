#include "ast/ast_arithmetic.hpp"


void Addition::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    Specifier left_type = left->getType(context);
    Specifier right_type = right->getType(context);

    // currently only support same type operations
    if (right_type != left_type) {
        exit(1);
    }

    switch (left_type) {
        case Specifier::_int:
            os << "add " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        case Specifier::_double:
            os << "fadd.d " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        case Specifier::_float:
            os << "fadd.s " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        default:
            std::cerr << "Addition: Invalid op type" << std::endl;
            exit(1);
    }

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void Subtraction::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    Specifier left_type = left->getType(context);
    Specifier right_type = right->getType(context);

    // currently only support same type operations
    if (right_type != left_type) {
        exit(1);
    }

    switch (left_type) {
        case Specifier::_int:
            os << "sub " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        case Specifier::_double:
            os << "fsub.d " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        case Specifier::_float:
            os << "fsub.s " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        default:
            std::cerr << "Subtraction: Invalid op type" << std::endl;
            exit(1);
    }

    context.freeReg(left_reg);
    context.freeReg(right_reg);
}


void Multiplication::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    Specifier left_type = left->getType(context);
    Specifier right_type = right->getType(context);

    std::cerr << "Left type: " << context.specifierToString(left_type)
                << " Right type: " << context.specifierToString(right_type)
                << "Dest reg: " << destReg << std::endl;

    // currently only support same type operations
    if (right_type != left_type) {
        std::cerr << "Right type does not equal left type" << std::endl;
        exit(1);
    }

    switch (left_type) {
        case Specifier::_int:
            os << "mul " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        case Specifier::_double:
            os << "fmul.d " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        case Specifier::_float:
            os << "fmul.s " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        default:
            std::cerr << "Multiplication: Invalid op type" << std::endl;
            exit(1);
    }

    context.freeReg(left_reg);
    context.freeReg(right_reg);

}


void Division::compile(std::ostream& os, Context& context, int destReg) const {

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    Specifier left_type = left->getType(context);
    Specifier right_type = right->getType(context);

    // currently only support same type operations
    if (right_type != left_type) {
        exit(1);
    }

    switch (left_type) {
        case Specifier::_int:
            os << "div " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        case Specifier::_double:
            os << "fdiv.d " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        case Specifier::_float:
            os << "fdiv.s " << context.getMnemonic(destReg) << ", " << context.getMnemonic(left_reg) << ", " << context.getMnemonic(right_reg) << std::endl;
            break;
        default:
            std::cerr << "Division: Invalid op type" << std::endl;
            exit(1);
    }

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
