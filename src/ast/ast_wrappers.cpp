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


const std::string& BaseExpression::getIdentifier() {
    static const std::string empty_string = "";
    return empty_string;
}

bool BaseExpression::getFuncCall() const {
    return false;  // default implementation
}

Specifier BaseExpression::getType(Context& context) const {
    // currently defaulting the type of a BaseExpression to int
    // as we can't yet guarantee getType returns correct derived type for all BaseExpressions
    return Specifier::_int;
}

const std::string& BaseDeclaration::getIdentifier() {
    static const std::string empty_string = "";
    return empty_string;
}
int BaseDeclaration::getSize() const {
    return 0;
}

Specifier BaseDeclaration::getType(Context& context) const {
    return Specifier::_int;
}

int BaseStatement::getSize() const {
    return 0;
}


