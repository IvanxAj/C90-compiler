#include "ast/ast_primitives.hpp"


Number::Number(double _value, Specifier _type)
    : value(_value), type(_type) {}

double Number::getValue() const {
    return value;
}

Specifier Number::getType(Context& context) const {
    return type;
}

void Number::compile(std::ostream& os, Context& context, int destReg) const {

    switch(type) {
        case Specifier::_int:
            os << "li "<< context.getMnemonic(destReg) << "," << value << std::endl;
            break;
        case Specifier::_float: {
            std::string label = context.makeLabel(".flt");
            context.addHeapObject(HeapObjectTypes::Temp, label, Specifier::_float, typeSizes.at(Specifier::_float), {value});

            int temp_reg = context.allocateReg(Specifier::_int);
            os << "lui " << context.getMnemonic(temp_reg) << ", %hi(" << label << ")" << std::endl;
            os << "flw " << context.getMnemonic(destReg) << ", %lo(" << label << ")(" << context.getMnemonic(temp_reg) << ")" << std::endl;
            context.freeReg(temp_reg);
            break;
        }
        case Specifier::_double: {
            std::string label = context.makeLabel(".dbl");
            context.addHeapObject(HeapObjectTypes::Temp, label, Specifier::_double, typeSizes.at(Specifier::_double), {value});

            int temp_reg = context.allocateReg(Specifier::_int);
            os << "lui " << context.getMnemonic(temp_reg) << ", %hi(" << label << ")" << std::endl;
            os << "fld " << context.getMnemonic(destReg) << ", %lo(" << label << ")(" << context.getMnemonic(temp_reg) << ")" << std::endl;
            context.freeReg(temp_reg);
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

    // handle enum values
    if (context.isEnum(name)) {
        os << "li " << context.getMnemonic(destReg) << ", " << context.getEnumValue(name) << std::endl;
        return;
    }

    Variable var = context.getVar(name);

    // handle global variables
    if (var.offset == 0) {
        int temp_reg = context.allocateReg(Specifier::_int);
        context.useReg(temp_reg);

        os << "lui " << context.getMnemonic(temp_reg) << ", %hi(" << name << ")" << std::endl;
        switch(var.type) {
            case Specifier::_int:
                os << "lw " << context.getMnemonic(destReg) << ", %lo(" << name << ")(" << context.getMnemonic(temp_reg) << ")" << std::endl;
                break;
            case Specifier::_float:
                os << "flw " << context.getMnemonic(destReg) << ", %lo(" << name << ")(" << context.getMnemonic(temp_reg) << ")" << std::endl;
                break;
            case Specifier::_double:
                os << "fld " << context.getMnemonic(destReg) << ", %lo(" << name << ")(" << context.getMnemonic(temp_reg) << ")" << std::endl;
                break;
            default:
                std::cerr << "Identifier: Invalid type for global variable" << std::endl;
                exit(1);
        }

        context.freeReg(temp_reg);

    } else {
        // Not a global variable - load from stack
        context.loadInstruction(os, var.type, destReg, var.offset);
    }
}
