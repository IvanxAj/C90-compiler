#include "ast/ast_selection.hpp"

IfElse::IfElse(BaseExpression* _condition, BaseStatement* _statements1, BaseStatement* _statements2)
    : condition(_condition), statements1(_statements1), statements2(_statements2)
    {}

IfElse::IfElse(BaseExpression* _condition, BaseStatement* _statements1)
    : condition(_condition), statements1(_statements1), statements2(nullptr)
    {}

IfElse::~IfElse() {
    delete condition;
    delete statements1;
    if (statements2) {
        delete statements2;
    }
}

int IfElse::getSize() const {
    int size1 = 0, size2 = 0;

    if (statements1) {
        size1 = statements1->getSize();
    }

    if (statements2) {
        size2 = statements2->getSize();
    }

    return std::max(size1, size2);
}

void IfElse::compile(std::ostream& os, Context& context, int destReg) const {

        std::string label1 = context.makeLabel(".L");

        // evaluate condition
        int cond_reg = context.allocateReg();
        context.useReg(destReg);
        condition->compile(os, context, cond_reg);
        os << "beq "  <<  context.getMnemonic(cond_reg) << "," << context.getMnemonic(0) << "," << label1 << std::endl;

        statements1->compile(os, context, destReg);

        //  no else
        if (statements2 == nullptr) {
             os << label1 << ":" << std::endl;
             context.freeReg(cond_reg);
             return;
        }

        // else body
        std::string label2 = context.makeLabel(".L");
        os << "j " << label2 << std::endl;

        os << label1 << ":" << std::endl;
        statements2->compile(os,context,destReg);
        os << label2 << ":" << std::endl;

        context.freeReg(cond_reg);
};


Switch::Switch(BaseExpression* _expression, BaseStatement* _statements)
    : expression(_expression), statements(_statements) {};

Switch::~Switch() {
    delete expression;
    delete statements;
}

int Switch::getSize() const {
    return statements->getSize();
}

void Switch::compile(std::ostream& os, Context& context, int destReg) const {

    std::string start_label = context.makeLabel(".SWITCH_START");
    std::string end_label = context.makeLabel(".SWITCH_END");
    context.newLoop(start_label, end_label);

    // evaluate expression into a reg
    int reg = context.allocateReg();
    context.useReg(reg);
    expression->compile(os, context, reg);

    statements->compile(os, context, reg);
    os << end_label << ":" << std::endl;
    context.freeReg(reg);
    context.exitLoop();

}


Case::Case(BaseExpression* _expression, BaseStatement* _statements)
    : expression(_expression), statements(_statements) {}

Case::Case(BaseStatement* _statements)
    : expression(nullptr), statements(_statements) {}

Case::~Case() {
    delete expression;
    delete statements;
}

int Case::getSize() const {
    return statements->getSize();
}

void Case::compile(std::ostream& os, Context& context, int destReg) const {

    std::string case_end = context.makeLabel(".CASE_END");
    int reg = context.allocateReg();
    context.useReg(reg);

    if(expression) {
        // evaluate expression into a reg
        expression->compile(os, context, reg);
        // check if the case matches switch expression in destReg
        os << "bne " << context.getMnemonic(destReg) << ", " << context.getMnemonic(reg) << ", " << case_end << std::endl;
    }
    // should be fine to re-use reg
    statements->compile(os, context, reg);
    os << case_end << ":" << std::endl;
    context.freeReg(reg);

}
