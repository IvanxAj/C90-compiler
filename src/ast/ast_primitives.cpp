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

    int reg = -1;

    switch(type) {
        case Specifier::_int:
            os << "li "<< context.getMnemonic(destReg) << "," << value << std::endl;
            break;
        case Specifier::_float: {
            // static_cast to float used for a more explicit and safer type conversion
            uint32_t int_bits = std::bit_cast<uint32_t>(static_cast<float>(value));

            reg = context.allocateReg(Specifier::_int);
            os << "li " << context.getMnemonic(reg) << ", " << int_bits << std::endl;
            // os << "fcvt.s.w " <<  context.getMnemonic(destReg) << ", " << context.getMnemonic(reg) << std::endl;

            context.storeInstruction(os, Specifier::_int, reg, 0);          // store on stack temporarily
            context.loadInstruction(os, Specifier::_float, destReg, 0);

            context.freeReg(reg);
            break;
        }
        default:
            std::cerr << "Number: Invalid literal data type" << std::endl;
            exit(1);
    }

}


Identifier::Identifier(const std::string& _name): name(_name) {}

Specifier Identifier::getType(Context& context) const {

    if (context.isEnum(name)) {
        return Specifier::_int;
    }

    Specifier type = context.getVarType(name);
    if (type == Specifier::INVALID_TYPE) {
        std::cerr << "Error: Identifier '" << name << "' has an invalid type." << std::endl;
    }
    return type;
}

const std::string& Identifier::getIdentifier() const {
    return name;
}

void Identifier::compile(std::ostream& os, Context& context, int destReg) const {

    if (context.isEnum(name)) {
        os << "li " << context.getMnemonic(destReg) << ", " << context.getEnumValue(name) << std::endl;
        return;
    } else {
        Variable var = context.getVar(name);
        context.loadInstruction(os, var.type, destReg, var.offset);
    }
}
