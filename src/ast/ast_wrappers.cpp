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


BaseExpression::~BaseExpression() {};
const std::string& BaseExpression::getIdentifier() {
    static const std::string empty_string = "";
    return empty_string;
}
