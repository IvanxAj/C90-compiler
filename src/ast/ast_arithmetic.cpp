#include "ast/ast_arithmetic.hpp"


void Addition::compile(std::ostream& os, Context& context, int destReg) const {

    Specifier left_type = left->getType(context);
    Specifier right_type = right->getType(context);

    int left_reg = prepLeft(os, context, destReg);
    int right_reg = prepRight(os, context, destReg);

    // if left contains a pointer, left_type will be type of the object the pointer is pointing to
    // e.g. for example if left_type is a float pointer -> it will return float
    // however pointers require int operation
    // this is why we switch on right_type which will always be the correct type for pointer arithmetic
    switch (right_type) {
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

    switch (right_type) {
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

    switch (right_type) {
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

    switch (right_type) {
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
