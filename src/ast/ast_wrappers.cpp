#include "ast/ast_wrappers.hpp"

ProgramRoot::ProgramRoot(List_Ptr _list ) : list(_list) {};

ProgramRoot::~ProgramRoot() {
    // delete list;
    for (auto node: *list) {
        delete node;
    }
    delete list;
}

void ProgramRoot::compile(std::ostream& os, Context& context, int destReg) const {
    for (auto node: *list) {
        node->compile(os, context, destReg);
    }
}


bool BaseExpression::isArray() const {
    return false;
}

bool BaseExpression::isDerefPointer() const {
    return false;
}

bool BaseExpression::isFuncCall() const {
    return false;
}

int BaseExpression::getValue() const {
    return -1;
}

const std::string& BaseExpression::getIdentifier() const {
    static const std::string empty_string = "";
    return empty_string;
}

Specifier BaseExpression::getType(Context& context) const {
    // currently defaulting the type of a BaseExpression to int
    // as we can't yet guarantee getType returns correct derived type for all BaseExpressions
    return Specifier::_int;
}


bool BaseDeclaration::isPointer() const {
    return false;
}

int BaseDeclaration::getSize() const {
    return 0;
}

int BaseDeclaration::getArraySize() const {
    return -1;
}

const std::string& BaseDeclaration::getIdentifier() const {
    static const std::string empty_string = "";
    return empty_string;
}

Specifier BaseDeclaration::getType(Context& context) const {
    return Specifier::_int;
}


int BaseStatement::getSize() const {
    return 0;
}
