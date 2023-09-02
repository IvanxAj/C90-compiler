#include "ast/ast_init_declarator.hpp"

// Constructors

Init_Declarator::Init_Declarator(Declarator* _declarator, BaseExpression* _initialiser)
    : declarator(_declarator), initialiser(_initialiser) {}

Init_Declarator::Init_Declarator(Declarator* _declarator)
    : declarator(_declarator), initialiser(nullptr) {}

Init_Declarator::~Init_Declarator() {
    delete declarator;
    delete initialiser;
}

void Init_Declarator::compile(std::ostream& os, Context& context, int destReg) const {



}


