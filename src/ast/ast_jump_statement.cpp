#include "ast/ast_jump_statement.hpp"

// Return will contain a single node
JumpStatement::JumpStatement(const std::string& _statement_type, BaseExpression* _expression)
    : statement_type(_statement_type), expression(_expression) {}

JumpStatement::~JumpStatement() {
    if (expression){
        delete expression;
    }
}
// compile value of expression into correct register AO = 10
void JumpStatement::compile(std::ostream& os, Context& context, int destReg) const {

    if(statement_type == "return") {
        if(expression) {
            // evaluate into reg a5 - hopefully not used
            expression->compile(os,context, 15);
        }
        os << "j " << context.ret_label << std::endl;
    } else if (statement_type == "continue") {
        os << "j " << context.getCurrentLoopStart() << std::endl;
    } else if (statement_type == "break") {
            os << "j " << context.getCurrentLoopEnd() << std::endl;
    }

    // int expr_reg = context.allocateReg();
    // expression->compile(os,context, 15);
    // os << "mv a0, a5" << std::endl;
};
