#include "ast/ast_while.hpp"

While::While(BaseExpression* _condition, BaseStatement* _statements)
    : condition(_condition), statements(_statements) { };

While::~While() {
    delete condition;
    delete statements;
}

int While::getSize() const {
    // should return a size from CompoundStatements if any declarations exist
    return statements->getSize();
}

void While::compile(std::ostream& os, Context& context, int destReg) const {

    std::string start_label = context.makeLabel(".WHILE_START");
    std::string end_label = context.makeLabel(".WHILE_END");

    context.newLoop(start_label, end_label);

    os << start_label << ":" << std::endl;

    // evaluate condition
    int cond_reg = context.allocateReg();
    context.useReg(cond_reg);
    condition->compile(os, context, cond_reg);
    os << "beq " << context.getMnemonic(cond_reg) << ", " << context.getMnemonic(0) << ", " << end_label << std::endl;

    // loop body
    statements->compile(os, context, destReg);
    os << "j " << start_label << std::endl;

    // end loop
    os << end_label << ":" << std::endl;
    context.freeReg(cond_reg);

    context.exitLoop();
};
