#include "ast/ast_declaration.hpp"

// two constructors - with + without initialisation
Declaration::Declaration(Specifier _type, BaseDeclaration* _declarator)
    : type(_type), init_declarator(_declarator) {}

Declaration::~Declaration() {
    delete init_declarator;
}

int Declaration::getSize() const {

    int array_size = init_declarator->getArraySize();
    if (array_size == -1) {
        // is a variable declaration (not an array)
        return typeSizes.at(type);
    }

    // is an array with size > 0
    return typeSizes.at(type) * array_size;

}

void Declaration::compile(std::ostream& os, Context& context, int destReg) const {

    std::string var_name = init_declarator->getIdentifier();
    if (init_declarator->getArraySize() != -1) {
        // array declaration
        int array_size = init_declarator->getArraySize();
        // add to bindings with array name storing offset of a[0]
        context.addArray(var_name, type, array_size);

    } else if (init_declarator->isPointer() == true) {
        // pointer declaration
        // - type of the pointer = type of what its pointing to + sets isPointer to true
        context.addVar(var_name, type, true);
    }

    else {
        // variable declaration
        context.addVar(var_name, type);
    }

    // sort out initialisation
    init_declarator->compile(os, context, destReg);
};
