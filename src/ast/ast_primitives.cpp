#include "ast/ast_primitives.hpp"


Integer::Integer(int _value): value(_value) {}

Specifier Integer::getType(Context& context) const {
    return Specifier::_int;
}

void Integer::compile(std::ostream& os, Context& context, int destReg) const {
    os << "li "<< context.getMnemonic(destReg) << "," << value << std::endl;
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
