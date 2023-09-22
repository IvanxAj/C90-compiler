#include "ast/ast_primitives.hpp"


Number::Number(double _value, Specifier _type)
    : value(_value), type(_type) {}

Specifier Number::getType(Context& context) const {
    return type;
}

void Number::compile(std::ostream& os, Context& context, int destReg) const {

    union {
        float f;
        uint32_t i;
    } float_cast;

    int reg;

    switch(type) {
        case Specifier::_int:
            os << "li "<< context.getMnemonic(destReg) << "," << value << std::endl;
            break;
        case Specifier::_float:
            float_cast.f = value;

            reg = context.allocateReg();
            os << "li " << context.getMnemonic(reg) << ", " << float_cast.i << std::endl;
            os << "sw " << context.getMnemonic(reg) << ", 0(sp)" << std::endl;      // store on stack temporarily
            os << "flw " <<  context.getMnemonic(destReg) << ", 0(sp)" << std::endl;

            context.freeReg(reg);
            break;
        default:
            std::cerr << "Invalid data type for a number" << std::endl;
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

const std::string& Identifier::getIdentifier() {
    return name;
}

void Identifier::compile(std::ostream& os, Context& context, int destReg) const {

    Specifier type = context.getVarType(name);

    switch(type) {
        case Specifier::_int:
            os << "lw " << context.getMnemonic(destReg) << ", " << context.getVarOffset(name) << "(s0)" << std::endl;
            break;
        case Specifier::_double:
            os << "fld " << context.getMnemonic(destReg) << ", " << context.getVarOffset(name) << "(s0)" << std::endl;
            break;
        case Specifier::_float:
            os << "flw " << context.getMnemonic(destReg) << ", " << context.getVarOffset(name) << "(s0)" << std::endl;
            break;

    }


}
