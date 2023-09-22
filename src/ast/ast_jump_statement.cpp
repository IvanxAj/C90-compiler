#include "ast/ast_jump_statement.hpp"

// Return will contain a single node
JumpStatement::JumpStatement(JumpType _statement_type, BaseExpression* _expression)
    : statement_type(_statement_type), expression(_expression) {}

JumpStatement::~JumpStatement() {
    if (expression){
        delete expression;
    }
}

// compile value of expression into correct register AO = 10
void JumpStatement::compile(std::ostream& os, Context& context, int destReg) const {

    switch (statement_type) {
        case JumpType::Return:
            if (expression) {
                if (context.current_func_type == Specifier::_float || context.current_func_type == Specifier::_double) {
                    // compile expression into reg fa5
                    expression->compile(os, context, 47);
                } else {
                    // compile expression into reg a5
                    expression->compile(os, context, 15);
                }
            }
            os << "j " << context.ret_label << std::endl;
            break;
        case JumpType::Continue:
            os << "j " << context.getCurrentLoopStart() << std::endl;
            break;
        case JumpType::Break:
            os << "j " << context.getCurrentLoopEnd() << std::endl;
            break;
    }
}
