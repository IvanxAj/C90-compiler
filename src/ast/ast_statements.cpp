#include "ast/ast_statements.hpp"


CompoundStatement::CompoundStatement(List_Ptr _statement_list, List_Ptr _declaration_list)
    : statement_list(_statement_list), declaration_list(_declaration_list) {}

CompoundStatement::~CompoundStatement() {
    if (statement_list) {
        for (auto node: *statement_list) {
            delete node;
        }
        delete statement_list;
    }
    if (declaration_list) {
        for (auto node: *declaration_list) {
            delete node;
        }
        delete declaration_list;
    }
}
// handle scope stuff as well
void CompoundStatement::compile(std::ostream& os, Context& context, int destReg) const {

    context.newScope();

    std::cerr << "Made new scope" << std::endl;

    if (declaration_list) {
        for (auto node: *declaration_list) {
            node->compile(os, context, destReg);
        }
    }

    // Then handle statements if they exist
    if (statement_list) {
        for (auto node: *statement_list) {
            node->compile(os, context, destReg);
        }
    }

    context.debugScope();

    context.popScope();
}

// Return will contain a single node
Return::Return(Node_Ptr _expression): expression(_expression) {}

Return::~Return() {
    delete expression;
}
// compile value of expression into correct register AO = 10
void Return::compile(std::ostream& os, Context& context, int destReg) const {

    int expr_reg = context.allocateReg();
    expression->compile(os,context, 15);

    // os << "mv a0, x" << expr_reg << std::endl;

    // TODO maybe not hard code a5
    os << "mv a0, a5" << std::endl;
    // os << "jr ra" << std::endl;
};
