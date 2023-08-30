#include "ast/ast_wrappers.hpp"

ProgramRoot::ProgramRoot(List* _list ) : list(_list) {};

void ProgramRoot::compile(std::ostream& os, Context& context, int destReg) const {
    for (auto node: *list) {
        node->compile(os, context, destReg);
    }
}


BaseExpression::~BaseExpression() {};
std::string BaseExpression::getIdentifier() { return ""; }
