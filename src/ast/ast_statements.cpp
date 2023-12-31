#include "ast/ast_statements.hpp"


CompoundStatement::CompoundStatement(List_Ptr _statement_list, List_Ptr _declaration_list)
    : statement_list(_statement_list), declaration_list(_declaration_list) {

    // If both lists null, set isEmpty flag
    isEmpty = (statement_list == nullptr && declaration_list == nullptr);
}

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

int CompoundStatement::getSize() const {
    if (isEmpty) {
        return 0;
    }
    int total_size = 0;

    if (declaration_list) {
        for (auto decl : *declaration_list) {
            total_size += dynamic_cast<const BaseDeclaration*>(decl)->getSize();
        }
    }

    if (statement_list) {
        for (auto statement : *statement_list) {
            total_size += dynamic_cast<const BaseStatement*>(statement)->getSize();
        }
    }

    return total_size;
}


// handle scope stuff as well
void CompoundStatement::compile(std::ostream& os, Context& context, int destReg) const {

    if (isEmpty) {
        return;
    }

    if (context.is_function_def) {
        context.is_function_def = 0;
    } else {
        context.newScope();
    }

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

ExpressionStatement::ExpressionStatement(BaseExpression* _expression) : expression(_expression) {}

ExpressionStatement::~ExpressionStatement() {
    delete expression;
}

void ExpressionStatement::compile(std::ostream& os, Context& context, int destReg) const {
    expression->compile(os, context, destReg);
}
