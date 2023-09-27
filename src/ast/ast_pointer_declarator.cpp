#include "ast/ast_pointer_declarator.hpp"


PointerDeclarator::PointerDeclarator(BaseDeclaration* _identifier)
    : identifier(_identifier) {}

PointerDeclarator::~PointerDeclarator() {
    delete identifier;
}

bool PointerDeclarator::isPointer() const {
    return true;
}

const std::string& PointerDeclarator::getIdentifier() const {
    return identifier->getIdentifier();
}

void PointerDeclarator::compile(std::ostream& os, Context& context, int destReg) const {

}

