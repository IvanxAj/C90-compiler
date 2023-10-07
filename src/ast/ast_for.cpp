#include "ast/ast_for.hpp"

// Constructor with initializer
For::For(BaseStatement* _initialise, BaseStatement* _condition, BaseExpression* _increment, BaseStatement* _statements)
    : initialise(_initialise), condition(_condition), increment(_increment), statements(_statements) {}

// Constructor without initializer
For::For(BaseStatement* _initalise, BaseStatement* _condition, BaseStatement* _statements)
    : initialise(nullptr), condition(_condition), increment(nullptr), statements(_statements) {}

For::~For() {
    if (initialise) {
        delete initialise;
    }
    delete condition;
    if (increment) {
        delete increment;
    }
    delete statements;
}

int For::getSize() const {
    // should return a size from CompoundStatements if any declarations exist
    return statements->getSize();
}

void For::compile(std::ostream& os, Context& context, int destReg) const {

    std::string start_label = context.makeLabel(".FOR_START");
    std::string end_label = context.makeLabel(".FOR_END");
    std::string cont_label = context.makeLabel(".FOR_INCR");

    context.addLabels(cont_label, end_label);

    if (initialise) {
        initialise->compile(os, context, destReg);
    }

    os << start_label << ": " << std::endl;

    // evaluate condition
    int cond_reg = context.allocateReg(Specifier::_int);
    context.useReg(cond_reg);
    condition->compile(os, context, cond_reg);
    os << "beq " << context.getMnemonic(cond_reg) << ", zero, " << end_label << std::endl;

    // compile body
    statements->compile(os, context, destReg);

    // increment + continue
    os << cont_label << ": " << std::endl;
    if (increment) {
        increment->compile(os, context, destReg);
    }
    os << "beq zero, zero, " << start_label << std::endl;

    // end loop
    os << end_label << ": " << std::endl;
    context.popLabels();
    context.freeReg(cond_reg);
}
