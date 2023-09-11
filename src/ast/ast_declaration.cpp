#include "ast/ast_declaration.hpp"

// two constructors - with + without initialisation
Declaration::Declaration(Specifier _type, BaseDeclaration* _declarator)
    : type(_type), init_declarator(_declarator) {}

Declaration::~Declaration() {
    delete init_declarator;
}

int Declaration::getSize() const {
    return typeSizes[static_cast<int>(type)];
}

void Declaration::compile(std::ostream& os, Context& context, int destReg) const {

    std::string var_name = init_declarator->getIdentifier();
    context.addVar(var_name, type);

    init_declarator->compile(os, context, destReg);

};
