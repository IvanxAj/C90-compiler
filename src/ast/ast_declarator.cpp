#include "ast/ast_declarator.hpp"

Declarator::Declarator(const std::string& _identifier) : identifier(_identifier) {}

void Declarator::compile(std::ostream& os, Context& context, int destReg) const { }

// only declarator have identifier so getIdentifier method only for declarators
const std::string& Declarator::getIdentifier() const {
    return identifier;
}
