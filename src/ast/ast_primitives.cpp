#include "ast/ast_primitives.hpp"


Number::Number(double _value, Specifier _type)
    : value(_value), type(_type) {}

int Number::getValue() const {
    if (type != Specifier::_int) {
        std::cerr << "Number: Invalid type for getValue" << std::endl;
        exit(1);
    }
    return value;
}

Specifier Number::getType(Context& context) const {
    return type;
}

void Number::compile(std::ostream& os, Context& context, int destReg) const {

    union {
        float f;
        uint32_t i;
    } float_cast;

    int reg = -1;

    switch(type) {
        case Specifier::_int:
            os << "li "<< context.getMnemonic(destReg) << "," << value << std::endl;
            break;
        case Specifier::_float:
            float_cast.f = value;

            reg = context.allocateReg(Specifier::_int);
            os << "li " << context.getMnemonic(reg) << ", " << float_cast.i << std::endl;
            // os << "fcvt.s.w " <<  context.getMnemonic(destReg) << ", " << context.getMnemonic(reg) << std::endl;

            os << "sw " << context.getMnemonic(reg) << ", 0(sp)" << std::endl;      // store on stack temporarily
            os << "flw " <<  context.getMnemonic(destReg) << ", 0(sp)" << std::endl;

            context.freeReg(reg);
            break;
        default:
            std::cerr << "Number: Invalid literal data type" << std::endl;
            exit(1);
    }

}


Identifier::Identifier(const std::string& _name): name(_name) {}

Specifier Identifier::getType(Context& context) const {
    Specifier type = context.getVarType(name);

    if (type == Specifier::INVALID_TYPE) {
        std::cerr << "Invalid type" << std::endl;
    }
    return context.getVarType(name);
}

const std::string& Identifier::getIdentifier() const {
    return name;
}

void Identifier::compile(std::ostream& os, Context& context, int destReg) const {

    Variable var = context.getVar(name);

    context.loadInstruction(os, var.type, destReg, var.offset);

}
