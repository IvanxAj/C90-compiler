#include "ast/ast_primitives.hpp"


Integer::Integer(int _value): value(_value) {}

void Integer::compile(std::ostream& os, Context& context, int destReg) const {
    os << "li "<< context.getMnemonic(destReg) << "," << value << std::endl;
}

// void Integer::compile(std::ostream& os, Context& context, int destReg) const {
//     if (value >= -4096 && value <= 4096) {
//         os << "li " << context.getMnemonic(destReg) << "," << value << std::endl;
//     } else {
//         int base = value / 4096 * 4096;
//         int offset = value - base;
//         os << "li " << context.getMnemonic(destReg) << "," << base << std::endl;
//         if (offset != 0) {
//             os << "addi " << context.getMnemonic(destReg) << "," << context.getMnemonic(destReg) << "," << offset << std::endl;
//         }
//     }
// }

Identifier::Identifier(const std::string& _name): name(_name) {}

void Identifier::compile(std::ostream& os, Context& context, int destReg) const { }

const std::string& Identifier::getIdentifier() {
    return name;
}
