#include "ast/ast_unary.hpp"


UnaryOp::UnaryOp(UnaryOperator _op, BaseExpression* _expr)
    : op(_op), expr(_expr) {}

UnaryOp::~UnaryOp() {
    delete expr;
}

void UnaryOp::compile(std::ostream& os, Context& context, int destReg) const {

    switch(op)
    {
        case UnaryOperator::Minus:
            expr->compile(os, context, destReg);
            os << "neg " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << std::endl;
            break;
        case UnaryOperator::Plus:
            expr->compile(os, context, destReg);
            break;
        case UnaryOperator::LogicalNot:
            expr->compile(os, context, destReg);
            os << "seqz " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << std::endl;
            os << "andi " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << ", 0xff" << std::endl;
            break;
        case UnaryOperator::BitwiseNot:
            expr->compile(os, context, destReg);
            os << "not " << context.getMnemonic(destReg) << ", " << context.getMnemonic(destReg) << std::endl;
            break;
        case UnaryOperator::Deref:
            // Implement dereference logic
            std::cerr << "Deref operator not supported yet" << std::endl;
            break;
        case UnaryOperator::AddressOf:
            // Implement address-of logic
            std::string var_name = expr->getIdentifier();
            int offset = context.getVarOffset(var_name);
            os << "addi " << context.getMnemonic(destReg) << ", s0, " << offset << std::endl;
            break;
    }

};




Increment::Increment(BaseExpression* _expr) : expr(_expr) {};

Increment::~Increment() {
    delete expr;
}

void Increment::compile(std::ostream& os, Context& context, int destReg) const {

    std::string var_name = expr->getIdentifier();
    int reg = context.allocateReg();

    int var_offset = context.getVarOffset(var_name);
    context.useReg(reg);

    expr->compile(os, context, reg);

    os << "addi " << context.getMnemonic(reg) << ", " << context.getMnemonic(reg) << ", 1" << std::endl;
    os << "sw " << context.getMnemonic(reg) << ", " << var_offset << "(s0)" << std::endl;

    context.freeReg(reg);

}


Decrement::Decrement(BaseExpression* _expr) : expr(_expr) {};

Decrement::~Decrement() {
    delete expr;
}

void Decrement::compile(std::ostream& os, Context& context, int destReg) const {

    std::string var_name = expr->getIdentifier();
    int reg = context.allocateReg();

    int var_offset = context.getVarOffset(var_name);
    context.useReg(reg);

    expr->compile(os, context, reg);

    os << "addi " << context.getMnemonic(reg) << ", " << context.getMnemonic(reg) << ", -1" << std::endl;
    os << "sw " << context.getMnemonic(reg) << ", " << var_offset << "(s0)" << std::endl;

    context.freeReg(reg);

}


SizeOf::SizeOf(Specifier _type)
    : type(_type), expr(nullptr) {}

SizeOf::SizeOf(BaseExpression* _expr)
    : type(Specifier::INVALID_TYPE), expr(_expr) {}

SizeOf::~SizeOf() {
    if (expr) {
        delete expr;
    }
}

void SizeOf::compile(std::ostream& os, Context& context, int destReg) const {

    // instead of mutating will define a type inside the method anyway
    Specifier var_type = type;
    if (expr != nullptr) {
        std::string var_name = expr->getIdentifier();  // Assuming you have such a method.
        var_type = context.getVarType(var_name);
    }

    // At this point, 'type' should be valid either from the constructor or from the expression.

    int size = typeSizes.at(var_type);
    // Generate the load immediate (li) instruction to put the size in the destination register.
    os << "li " << context.getMnemonic(destReg) << ", " << size << std::endl;
}
