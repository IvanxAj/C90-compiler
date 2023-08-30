#include "ast/ast_primitives.hpp"


Integer::Integer(int _value): value(_value) {}

void Integer::compile(std::ostream& os, Context& context, int destReg) const {
    os << "li x"<< destReg << "," << value << std::endl;
}

Identifier::Identifier(std::string _name): name(_name) {}

void Identifier::compile(std::ostream& os, Context& context, int destReg) const { }

std::string Identifier::getIdentifier() {
    return name;
}
