#include "ast/ast_statements.hpp"


CompoundStatement::CompoundStatement(List_Ptr _statement_list) : statement_list(_statement_list) {};

CompoundStatement::~CompoundStatement() {
    for (auto node: *statement_list) {
        delete node;
    }
    delete statement_list;
}

// handle scope stuff as well
void CompoundStatement::compile(std::ostream& os, Context& context, int destReg) const {
    for (auto node: *statement_list) {
        node->compile(os, context, destReg);
    }
}

// Return will contain a single node
Return::Return(Node_Ptr _expression): expression(_expression) {}

Return::~Return() { delete expression; }
// compile value of expression into correct register AO = 10
void Return::compile(std::ostream& os, Context& context, int destReg) const {

    int expr_reg = context.allocate();
    expression->compile(os,context, 15);

    // os << "mv a0, x" << expr_reg << std::endl;

    // TODO maybe not hard code a5
    os << "mv a0, a5" << std::endl;
    // os << "jr ra" << std::endl;

};
